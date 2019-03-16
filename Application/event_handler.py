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

#Database Table Names
ALL_MESSAGES = "All_Messages"
ACTIVE_CLIENTS = "Active_Clients"


#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class TableConfig(object):
    def __init__(self):
        self.tables = [{"TableName": ALL_MESSAGES, "TableCol": """id integer PRIMARY KEY, type text NOT NULL, 
                        total_heartbeats integer NOT NULL, client_ip text NOT NULL, time text NOT NULL"""},
                       {"TableName": "Active_Clients", "TableCol": """id integer PRIMARY KEY, client_ip text NOT NULL, 
                        total_heartbeats integer NOT NULL, last message, text NOT NULL"""}]

class SQL(object):
    def __init__(self, dbfile):
        self.db_file = dbfile
        self._createDB()

    def _createDB(self):
        """ create a database connection to a SQLlite database """
        conn = sqlite3.connect(self.db_file)
        for table in TableConfig().tables:
            logger.debug("Creating Table {}".format(table))
            conn.cursor().execute("CREATE TABLE IF NOT EXISTS %s (%s)"%(table["TableName"], table["TableCol"]))
            conn.commit()
            logger.debug("Created Table: {}".format(table["TableCol"]))
        conn.close()
        logger.debug("Database creation successful")
        
    def _create_input_struct(self, sqlstruct, table):
        struct = "INSERT INTO %s(type, total_heartbeats, client_ip, time) VALUES("%(table)

        for item in sqlstruct:
            struct += "?, "
         
        return struct[:len(struct)-2] + ")"
                 
    def insetDB(self, sqlstruct, table):
        conn = sqlite3.connect(self.db_file)
        input_struct = self._create_input_struct(sqlstruct, table)
        conn.cursor().execute(input_struct, sqlstruct)
        conn.commit()
        conn.close()
        logger.debug("Data: {} Written to Table: {} into database".format(input_struct, table))

    def removeDB(self, column, arg, table):
        #\Input column name, value , table name
        #\Function: Remove line from database given input
        #\Output: N/A
        
        conn = sqlite3.connect(self.db_file)
        input_struct = "DELETE FROM %s WHERE %s = ?"%(table, column)
        conn.cursor().execute(input_struct, arg)

class InputProcessor(object):
    def __init__(self):
        self.clients_list = []

    def check_client_present(self, client_data):

        #\Input: client_data (client_ip, total_heartbeats, time)
        #\Function: Check if clients list has the input ip. Add if not.
        #\Output: True if ip was found in list and false if ip was not found
        
        client = filter(lambda c_ip: c_ip["client_ip"] == client_data[0], self.clients_list) 
        if client:    
            client_index = self.clients_list.index(client[0])
            self.clients_list[client_index] = {"client_ip": client_data[0], "total_hb": client_data[1], "time": client_data[2]}
            return True
        else:
            self.clients_list.append({"client_ip": client_data[0], "total_hb": client_data[1], "time": client_data[2]})
            return False

    def inactive_detection(self):

        #\Input: N/A
        #\Function: Check if any client has been inactive for longer than 120s
        #\Output: If client has been active for longer than 120s remove from db, if not do nothing

        current_time = dt.datetime.now()
        inactive_clients = filter(lambda client: current_time - dt.strptime(client["time"]) > 120, self.client_list)

        if inactive_clients:
                for inact in inactive_clients:
                    client_index = self.clients_list.index(inact)
                    del self.clients_list[client_index]
                    sql.removeDB("client_ip", inact["client_ip"], ACTIVE_CLIENTS)

        
    def process_fifo_data(self):

        #\Input: Queue input from read_fifo function
        #\Function: `
        #\Output: True if ip was found in list and false if ip was not found

        while True:
            if not q.empty():
                TableData = json.loads(q.get())
                logger.debug("Data from read_Fifo found {}".format(TableData))
                sqlstruct = (TableData["type"].encode('utf-8'), TableData["total_hb"], TableData["client_ip"].encode('utf-8'), TableData["time"].encode('utf-8'))
                sql.insetDB(sqlstruct, ALL_MESSAGES)
            else:
                logger.warning("Queue is empty; no events to process")
            time.sleep(1)

def read_fifo():
    while True:
        fifoData = open(SERVER_fifo, "r")
        logger.debug("Data in fifo, writing to Queue") 
        q.put(fifoData.read())
        fifoData.close()
        time.sleep(1)



if __name__ == '__main__':
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    #Create and configure the logger
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
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
    
    logger.addHandler(ch)
    logger.addHandler(fh)

    #Initialize and the start of the program
    q = Queue.Queue()
    sql = SQL(DBNAME)
    in_proc = InputProcessor()
    rfifo = threading.Thread(target=read_fifo)
    pfifo = threading.Thread(target=in_proc.process_fifo_data)
    inactive_alg = threading.Thread(target=in_proc.inactive_detection)
    rfifo.daemon = True
    pfifo.daemon = True
    inactive_alg.daemon = True
    while not os.path.exists(SERVER_fifo):
        #Do Nothing
        logger.warning("fifo File Not Found")
        time.sleep(1)
    rfifo.start()
    pfifo.start()
    inactive_alg.start()
    
    if raw_input("PRESS ANY KEY TO EXIT\n"):
        sys.exit()

    

