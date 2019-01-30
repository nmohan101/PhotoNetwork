#!/usr/bin/env python
"""event_simulator.py: This application simulates the client sending messages to the fifo file.  



__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import datetime
import json
import logging
import argparse
import random 
import os
import time
#*******Constants*****************
IP_ADDRESS = "192.168.0"
IP_LIST = {}

def run():
    while True:
        net_value = random.randint(0,10)
        ip_address = "%s.%d"%(IP_ADDRESS, net_value)

        if not ip_address in IP_LIST:
            IP_LIST.update({ip_address: 0})
        
        heartbeat = {"type": "heartbeat", "total_hb": IP_LIST[ip_address], "client_ip": ip_address, "time": str(datetime.datetime.now())}
        IP_LIST[ip_address] += 1
        data = json.dumps(heartbeat)
        print data
        fi.write(data)
        time.sleep(1)

class FIFO(object):
    def __init__(self):
        self.fifo_path = ("/tmp/server_rx.fifo")
        if os.path.exists("/tmp/server_rx.fifo"):
            os.system("rm /tmp/server_rx.fifo")
        os.mkfifo(self.fifo_path)
        
    def write(self, data):
        fifo = open(self.fifo_path, "w")
        fifo.write(data)
        fifo.close()

if __name__=="__main__": 
    fi = FIFO()
    run()
