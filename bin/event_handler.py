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
import datetime as dt

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/lib")
import asynctimer
from log import Log
from sql import SQL

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
UID = pwd.getpwuid(os.getuid()).pw_uid
SERVER_fifo = "/var/run/user/%s/server_rx.fifo"%UID

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
        
        log.debug("Read db_settings file %s"%DB_SETTINGS)
        db_settings_file = open(DB_SETTINGS, "r")
        db_settings = db_settings_file.read()
        db_settings_file.close()
        return json.loads(db_settings)
    
    def _create_struct(self):
        log.debug("Initializing and creating database")
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
                tables.append(table_struct)
                db_data += [{"db_name": db_name, "tb_name": tb_name, "tb_columns": column_names, "tb_update_mode": tb_update_mode}] #Show Database name for each table. 
                tb_index_data += [{"tb_index": tb_index, "tb_name": tb_name}]
            log.debug("Requesting Database creation with the following: db_name = {}, tables = {}".format(db_name, tables))
            sql = SQL(db_path, tables, log)
            db_index_data = [{"db_index":db_index, "db_name":db_name, "db_path": db_path, "tables": tb_index_data}]
            all_index += db_index_data
        return db_data, all_index, sql

class InputProcessor(object):
    def __init__(self):
        self.clients_list = []
        self.tb_db_map = table_config.db_tb_index_map
        self.db_data = table_config.db_data
        self.db_info = filter(lambda tb_db: tb_db["db_index"] == 0, self.tb_db_map)[0] 
        self.db_name = self.db_info["db_name"]
        self.db_path = self.db_info["db_path"]

    def check_client_present(self, client_data):
        #\Input: client_data (client_ip, total_heartbeats, time)
        #\Function: Check if clients list has the input ip. Add if not.
        #\Output: True if ip was found in list and false if ip was not found
        log.debug("Check if client present {}".format(client_data)) 
        client = filter(lambda c_ip: c_ip["client_ip"] == client_data['client_ip'], self.clients_list) 
        if client:    
            client_index = self.clients_list.index(client[0])
            self.clients_list[client_index] = {"client_ip": client_data['client_ip'], "total_hb": client_data['total_hb'], "time": client_data['time']}
            log.debug("Client present {}".format(client_data["client_ip"])) 
            return ("client_ip", client_data['client_ip'])

        else:
            self.clients_list.append({"client_ip": client_data['client_ip'], "total_hb": client_data['total_hb'], "time": client_data['time']})
            log.debug("Client NOT present {}".format(client_data["client_ip"])) 
            return False

    def inactive_detection(self):
        #\Input: N/A
        #\Function: Check if any client has been inactive for longer than 120s
        #\Output: If client has been active for longer than 120s remove from db, if not do nothing

        current_time = dt.datetime.now()
        inactive_clients = filter(lambda client: 
                                  (current_time - dt.datetime.strptime(client["time"], 
                                  "%Y-%m-%d %H:%M:%S.%f")).total_seconds() > 120.00, 
                                  self.clients_list)

        if inactive_clients:
            log.warning("Inactive clients found {}".format(inactive_clients))
            for inact in inactive_clients:
                client_index = self.clients_list.index(inact)
                del self.clients_list[client_index]
                return inactive_clients
        else:
            return False

        
    def _all_messages(self, db_info, db_name, db_path, db_data, incoming_data, indata_keys, sql):
        #\Input: db_info, db_name, and sql object 
        #\Function: Update the all messages table
        #\Output: N/A

        struct = tuple([incoming_data[key] for key in indata_keys])
        tb_name = filter(lambda tb: tb["tb_index"] == 0, db_info["tables"])[0]["tb_name"]
        tb_db_data = filter(lambda tb_db: tb_db["tb_name"] == tb_name and tb_db["db_name"] == db_name, db_data)[0]
        tb_columns = [column["cl_name"].encode('utf-8') for column in filter(lambda col: col["cl_parm"] != "PRIMARY KEY", tb_db_data["tb_columns"])]
        log.debug("insertDB with {} {} {}".format(db_name, tb_name, tb_columns))
        sql.insertDB(struct, db_path, tb_name, tb_columns)
    
    def _active_clients(self, db_info, db_name, db_path, db_data, incoming_data, sql):
        #\Input: db_info, db_name, and sql object 
        #\Function: Update the all clients table
        #\Output: N/A

        #Active Clients setup
        tb_name = filter(lambda tb: tb["tb_index"] == 1, db_info["tables"])[0]["tb_name"]
        tb_db_data = filter(lambda tb_db: tb_db["tb_name"] == tb_name and tb_db["db_name"] == db_name, db_data)[0]
        tb_update_mode = tb_db_data["tb_update_mode"]
        tb_columns = [column["cl_name"].encode('utf-8') for column in filter(lambda col: col["cl_parm"] != "PRIMARY KEY", tb_db_data["tb_columns"])]

        result = self.check_client_present(incoming_data)
        if result: 
            log.debug("IP {} found in records. Updateing table with new information".format(result))
            struct = [(column, incoming_data[column]) for column in tb_update_mode]
            sql.updateDB(struct, result, db_path, tb_name)
        else:
            log.debug("IP {} NOT found in records. Inserting as new record".format(incoming_data["client_ip"]))
            struct = tuple([incoming_data[column] for column in tb_columns])
            sql.insertDB(struct, db_path, tb_name, tb_columns)

        #Check if there are any inactive clients
        result = self.inactive_detection()
        if result:
            for client in result:
                sql.removeDB("client_ip", (client["client_ip"], ), db_path, tb_name)

    def process_fifo_data(self):
        #\Input: Queue input from read_fifo function
        #\Function: Main function responsible for deciding what is to be added/removed from database\
        #           Update All_Messages & Active_Clients table
        #\Output: N/A

        sql = table_config.sql
        counter = 0
        while True:
            if not q.empty():
                incoming_data = json.loads(q.get())
                log.debug("Data from read_Fifo found {}".format(incoming_data))

                #Decode all incomnig data
                for key in incoming_data.keys():
                    if isinstance(incoming_data[key], unicode):
                        incoming_data[key] = incoming_data[key].encode('utf-8')
                indata_keys = incoming_data.keys()
                
                message_type = incoming_data["type"]
                
                #Update All messages table
                self._all_messages(self.db_info, self.db_name, self.db_path, self.db_data, incoming_data, indata_keys, sql)

                #Update Active clients table
                if message_type == "heartbeat":
                    self._active_clients(self.db_info, self.db_name, self.db_path, self.db_data, incoming_data, sql)
                
            else:
                if counter > 30:
                    log.warning("Queue is empty; no events to process")
                    counter = 0
            time.sleep(1)

def read_fifo():
    #\Input: N/A
    #\Function: Read data from fifo file and put into queue
    #\Output: N/A

    while True:
        fifoData = open(SERVER_fifo, "r")
        log.debug("Data in fifo, putting to Queue") 
        q.put(fifoData.read())
        fifoData.close()
        time.sleep(1)


if __name__ == '__main__':
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()

    #Configure the logger
    log = Log(sys.argv[0], verbosity=args.verbosity).logger
    
    #Initialize and the start of the program
    q = Queue.Queue()
    table_config = TableConfig()
    in_proc = InputProcessor()
    rfifo = threading.Thread(target=read_fifo)
    pfifo = threading.Thread(target=in_proc.process_fifo_data)
    rfifo.daemon = True
    pfifo.daemon = True
    while not os.path.exists(SERVER_fifo):
        #Do Nothing
        log.warning("fifo File Not Found")
        time.sleep(1)
    rfifo.start()
    pfifo.start()

    #Ensure program doesnt terminate
    rfifo.join()
    pfifo.join()
    
    
