#!/usr/bin/env python

"""event_handler.py: This application is responsible for reading messages coming from the 
                    server and logging them into a database. Also removing clients that have not
                    sent heart beats from the database.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import sqlite3
import os
import Queue
import time
import threading
import sys
import json
import pwd
import argparse
import logging
import datetime as dt

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
import asynctimer


#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
UID = pwd.getpwuid(os.getuid()).pw_uid
DBNAME = "/srv/PhotoNetwork/PhotoNetwork.db"
SERVER_fifo = "/var/run/user/%s/server_rx.fifo"%UID
LOG_PATH = "/var/log/PhotoNetwork/"
LOGGER = logging.getLogger(__name__)

#Database Table Names
DB_SETTINGS = "/etc/PhotoNetwork/db_settings.json" 

#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class TableConfig(object):
    def __init__(self):
        self.db_settings = self._getsettings()
        self.db_data, self.db_tb_index_map, self.sql = self._create_struct()

    def _getsettings(self):
        #\Input: N/A
        #\Function: Read the db settings json file and load as dict object
        #\Output: json file object

        LOGGER.debug("Reading db_settings file %s"%DB_SETTINGS)
        db_settings_file = open(DB_SETTINGS, "r")
        db_settings = db_settings_file.read()
        db_settings_file.close()
        return json.loads(db_settings)
    
    def _create_struct(self):
        LOGGER.debug("Initializing and creating database")
        #\Input: N/A
        #\Function: Create structures for creating database and tables and then call function to create them
        #\Output: Databae data

        db_data = []
        all_index = []
        for db in self.db_settings["Databases"]:
            tables = []
            db_index_data = []
            db_path = os.path.join(db["db_dir"], db["db_name"])
            db_name = db["db_name"]
            db_index = db["db_index"]
            tb_index_data = []
            for table in db["db_tables"]:
                table_struct = {"TableName": table["tb_name"]}
                tb_name = table["tb_name"]
                tb_columns = table["tb_columns"]
                tb_update_mode = table["tb_update_mode"]
                tb_index = table["tb_index"]
                table_string = None
                column_names = []
                for column in table["tb_columns"]: 
                    column_names.append(column)
                    if table_string == None:
                        table_string = "{} {} {},".format(column["cl_name"], column["cl_type"], column["cl_parm"])
                    else:
                        table_string = "{} {} {} {},".format(table_string, column["cl_name"], column["cl_type"], column["cl_parm"])
                table_string = table_string[:-1] 
                table_struct.update({"TableCol": table_string})
                db_data += [{"db_name": db_name, "tb_name": tb_name, "tb_columns": column_names, "tb_update_mode": tb_update_mode}]
                tb_index_data += [{"tb_index": tb_index, "tb_name": tb_name}]
            sql = SQL(db_path, tables)
            db_index_data = [{"db_index":db_index, "tb_name":tb_name, "tables": tb_index_data}]
            all_index += db_index_data
        LOGGER.debug("Databases created {}".format(db_data))
        return db_data, all_index, sql

class SQL(object):
    def __init__(self, dbfile, tables):
        self.db_file = dbfile
        self.tables = tables
        self._createDB()

    def _createDB(self):
        #\Input: N/A
        #\Function: Create a database and tables using the Table Config
        #\Output: N/A
        
        conn = sqlite3.connect(self.db_file)
        for table in self.tables:
            LOGGER.debug("Creating Table {}".format(table))
            conn.cursor().execute("CREATE TABLE IF NOT EXISTS %s (%s)"%(table["TableName"], table["TableCol"]))
            conn.commit()
            LOGGER.debug("Created Table: {}".format(table["TableCol"]))
        conn.close()
        LOGGER.debug("Database creation successful")
        
    def _insert_input_struct(self, sqlstruct, table, tb_columns):
        #\Input: input values, input columns, and table name
        #\Function: Construct input structure for inserting a line into the database
        #\Output: Input structure

        struct_0 = "INSERT INTO %s("%table
        struct_1 = ""
        struct_2 = ") VALUES("
        for column in tb_columns:
            struct_1 += "%s, "%column

        struct_1 = struct_1[:-2]
        struct = struct_0 + struct_1 + struct_2

        for item in sqlstruct:
            struct += "?, "
         
        return struct[:len(struct)-2] + ")"
                 
    def _update_input_struct(self, columns, table, update_arg):
        #\Input: columns to be updated, table name and search criteria
        #\Function: Construct the sql argument for updating a table based on inputs
        #\Output: Skeleton structure and values

        struct_1 = "UPDATE %s set "%table
        struct_2 = "WHERE %s = ?"%(update_arg[0])
        update_struct = []

        for key, item in columns.items():
            struct_1 += "%s = ?, "%key
            update_struct.append(item)

        skel_struct = struct_1 + struct_2
        return skel_struct, tuple(update_struct)
         
    def insertDB(self, sqlstruct, db_file, table, tb_columns):
        #\Input: Input Data, Table Name
        #\Function: Insert a line into the table given the input
        #\Output: N/A

        conn = sqlite3.connect(db_file)
        input_struct = self._insert_input_struct(sqlstruct, table, tb_columns)
        conn.cursor().execute(input_struct, sqlstruct)
        conn.commit()
        conn.close()
        LOGGER.debug("Data: {} Written to Table: {} into database".format(input_struct, table))

    def removeDB(self, column, arg, db_file, table):
        #\Input column name, value , table name
        #\Function: Remove line from database given input
        #\Output: N/A
        
        conn = sqlite3.connect(db_file)
        input_struct = "DELETE FROM %s WHERE %s = ?"%(table, column)
        conn.cursor().execute(input_struct, arg)

    def updateDB(self, columns, update_arg, db_file, table):
        #\Input Columns to be updated, search criteria, table name
        #\Function: Update a line in a table given the input criteria
        #\Output: N/A

        conn = sqlite3.connect(db_file)
        skel_struct, update_struct = self._update_input_struct(columns, table, update_arg)
        conn.cursor().execute(skel_struct, update_struct)
        conn.commit()
        conn.close()
        LOGGER.debug("Updated >> Table: {},  Data: {}".format(table, update_struct))

class InputProcessor(object):
    def __init__(self):
        self.clients_list = []

    def check_client_present(self, client_data):
        #\Input: client_data (client_ip, total_heartbeats, time)
        #\Function: Check if clients list has the input ip. Add if not.
        #\Output: True if ip was found in list and false if ip was not found
        
        if self.clients_list:
            client = filter(lambda c_ip: c_ip["client_ip"] == client_data['client_ip'], self.clients_list) 
        if client:    
            client_index = self.clients_list.index(client[0])
            self.clients_list[client_index] = {"client_ip": client_data['client_ip'], "total_hb": client_data['total_hb'], "time": client_data['time']}
            return True
        else:
            self.clients_list.append({"client_ip": client_data['client_ip'], "total_hb": client_data['total_hb'], "time": client_data['time']})
            return False

    def inactive_detection(self):
        #\Input: N/A
        #\Function: Check if any client has been inactive for longer than 120s
        #\Output: If client has been active for longer than 120s remove from db, if not do nothing

        current_time = dt.datetime.now()
        inactive_clients = filter(lambda client: current_time - dt.strptime(client["time"]) > 120, self.clients_list)

        if inactive_clients:
                for inact in inactive_clients:
                    client_index = self.clients_list.index(inact)
                    del self.clients_list[client_index]
                    sql.removeDB("client_ip", inact["client_ip"], ACTIVE_CLIENTS)

        
    def process_fifo_data(self):
        #\Input: Queue input from read_fifo function
        #\Function: Main function responsible for deciding what is to be added/removed from database\
        #           Update All_Messages & Active_Clients table
        #\Output: N/A

        tb_db_map = table_config.db_tb_index_map
        db_data = table_config.db_data
        db_info = filter(lambda tb_db: tb_db["db_index"] == 0, tb_db_map)[0] 
        db_name = db_data["db_name"]
        sql = table_config.sql
        while True:
            if not q.empty():
                incoming_data = json.loads(q.get())
                LOGGER.debug("Data from read_Fifo found {}".format(incoming_data))

                #Decode all incomnig data
                for key in incoming_data.keys():
                    if isinstance(incoming_data[key], unicode):
                        incoming_data[key] = incoming_data[key].encode('utf-8')
                idata_keys = incoming_data.keys()
                
                struct = [incoming_data[key] for key in indata_keys] 

                #Update table which stores all incoming messages
                tb_name = filter(lambda tb: tb["tb_index"] == 0, db_info["tables"])[0]["tb_name"]
                tb_db_data = filter(lambda tb_db: tb_db["tb_name"] == tb_name and tb_db["db_name"] == db_name, db_info)[0]
                tb_columns = tb_db_data["tb_columns"] 
                sql.insertDB(tuple(struct), db_name, tb_name, tb_columns)
                
                #Active Clients setup
                tb_name = filter(lambda tb: tb["tb_index"] == 1, db_info["tables"])[0]["tb_name"]
                tb_db_data = filter(lambda tb_db: tb_db["tb_name"] == tb_name and tb_db["db_name"] == db_name, db_info)[0]
                tb_columns = tb_db_data["tb_columns"] 

                struct = [incoming_data[columns] for column in tb_columns]

                if self.check_client_present(incoming_data):
                    sql.updateDB(struct, ("client_ip", struct["client_ip"]), db_name, tb_name)
                else:
                    sql.insertDB(sqlstruct, ACTIVE_CLIENTS)

            else:
                LOGGER.warning("Queue is empty; no events to process")
            time.sleep(1)

def read_fifo():
    #\Input: N/A
    #\Function: Read data from fifo file and put into queue
    #\Output: N/A

    while True:
        fifoData = open(SERVER_fifo, "r")
        LOGGER.debug("Data in fifo, writing to Queue") 
        q.put(fifoData.read())
        fifoData.close()
        time.sleep(1)


if __name__ == '__main__':
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    #Create and configure the LOGGER
    LOGGER.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    ch = logging.StreamHandler()
    fh = logging.FileHandler("%s%s.log"%(LOG_PATH, sys.argv[0].split("/")[-1].split(".")[0]))
    ch.setFormatter(formatter)
    fh.setFormatter(formatter)
    
    if args.verbosity:
        print "VERBOSE MODE"
        ch.setLevel(logging.DEBUG)
    else:
        ch.setLevel(logging.WARNING)
    
    LOGGER.addHandler(ch)
    LOGGER.addHandler(fh)

    #Initialize and the start of the program
    q = Queue.Queue()
    table_config = TableConfig()
    in_proc = InputProcessor()
    rfifo = threading.Thread(target=read_fifo)
    pfifo = threading.Thread(target=in_proc.process_fifo_data)
    inactive_alg = threading.Thread(target=in_proc.inactive_detection)
    rfifo.daemon = True
    pfifo.daemon = True
    inactive_alg.daemon = True
    while not os.path.exists(SERVER_fifo):
        #Do Nothing
        LOGGER.warning("fifo File Not Found")
        time.sleep(1)
    rfifo.start()
    pfifo.start()
    inactive_alg.start()
    
    if raw_input("PRESS ANY KEY TO EXIT\n"):
        sys.exit()

    

