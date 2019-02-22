#!/usr/bin/env python

"""event_handler.py: This application is responsible for reading messages coming from the 
                    server and logging them into a database. Also removing clients that have not
                    sent heart beats from the database.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""

#********System Imports************
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

#******Constants********************
UID = pwd.getpwuid(os.getuid()).pw_uid
DBNAME = "/srv/PhotoNetwork/PhotoNetwork.db"
SERVER_FIFO = "/var/run/user/%s/server_rx.fifo"%UID
LOG_PATH = "/var/log/PhotoNetwork/"

class SQL(object):
    def __init__(self, dbfile, Table):
        self.Table = Table
        self.TableCol = """id integer, type text, total_heartbeats integer, client_ip text, time text"""
        self.db_file = dbfile
        self._createDB()
        self.id = 0

    def _createDB(self):
        """ create a database connection to a SQLlite database """
        conn = sqlite3.connect(self.db_file)
        conn.cursor().execute("CREATE TABLE IF NOT EXISTS %s (%s)"%(self.Table, self.TableCol))
        conn.commit()
        conn.close()
        logger.debug("Database creation successful")
        
    def insetDB(self,  data):
        conn = sqlite3.connect(self.db_file)
        conn.cursor().execute("INSERT INTO %s VALUES(?, ?, ?, ?, ?)"%self.Table, (self.id, data[0], data[1], data[2], data[3]))
        conn.commit()
        conn.close()
        logger.debug("Writing {} into database".format(data))
        self.id += 1

    
def read_FIFO():
    while True:
        fifoData = open(SERVER_FIFO, "r")
        logger.debug("Data in FIFO, writing to Queue") 
        q.put(fifoData.read())
        fifoData.close()
        time.sleep(1)

def process_FIFO():
    while True:
        if not q.empty():
            TableData = json.loads(q.get())
            sqlstruct = TableData["type"].encode('utf-8'), TableData["total_hb"], TableData["client_ip"].encode('utf-8'), TableData["time"].encode('utf-8')
            sql.insetDB(sqlstruct)
        else:
            logger.warning("Queue is empty; no events to process")
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
    sql = SQL(DBNAME, "Incoming_Data")
    rFIFO = threading.Thread(target=read_FIFO)
    pFIFO = threading.Thread(target=process_FIFO)
    rFIFO.daemon = True
    pFIFO.daemon = True
    while not os.path.exists(SERVER_FIFO):
        #Do Nothing
        logger.warning("FIFO File Not Found")
        time.sleep(1)
    rFIFO.start()
    pFIFO.start()
    
    if raw_input("PRESS ANY KEY TO EXIT\n"):
        sys.exit()

    

