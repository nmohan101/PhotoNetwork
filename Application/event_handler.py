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

#******Constants********************
dbname = "/tmp/ClientList.db"
fifoname = "/tmp/server_rx.fifo"

class SQL(object):
    def __init__(self, dbfile):
        self.Table = "Clients"
        self.TableCol = """Type text, Total_Heartbeats integer, 
                         Client_IP text, Time text"""
        self.db_file = dbfile
        

    def _createDB(self):
        """ create a database connection to a SQLite database """
        conn = sqlite3.connect(self.db_file)
        conn.cursor().execute("CREATE TABLE IF NOT EXISTS %s (%s)"%(self.Table, self.TableCol))
        conn.commit()
        conn.close()
        
    def _insertDB(self,  data):
        conn = sqlite3.connect(self.db_file)
        conn.cursor().execute("INSERT INTO %s VALUES(%s)"%(self.Table, data))
        conn.commit()
        conn.close()

    
def read_FIFO():
    while True:
        fifoData = open(fifoname, "r")
        q.put(fifoData.read())
        fifoData.close()

def process_FIFO():
    while True:
        if not q.empty():
            TableData = json.loads(q.get())
            sqlstruct = TableData["type"].encode('utf-8'), TableData["total_hb"], TableData["clent_ip"].encode('utf-8'), TableData["time"].encode('utf-8')
            print sqlstruct
            sql._insertDB(sqlstruct)



      
if __name__ == '__main__':
    q = Queue.Queue()
    sql = SQL(dbname)
    sql._createDB()
    rFIFO = threading.Thread(target=read_FIFO)
    pFIFO = threading.Thread(target=process_FIFO)
    rFIFO.daemon = True
    pFIFO.daemon = True
    rFIFO.start()
    pFIFO.start()
    
    if raw_input("PRESS ANY KEY TO EXIT\n"):
        sys.exit()

    

