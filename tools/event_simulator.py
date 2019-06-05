#!/usr/bin/env python
"""event_simulator.py: This application simulates the client sending messages to the fifo file.  



__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import datetime
import json
import logging
import argparse
import random 
import os
import time
import sys
import pwd


#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/lib")
import asynctimer
from log import Log

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
IP_ADDRESS = "192.168.0"
IP_LIST = {}
UID = pwd.getpwuid(os.getuid()).pw_uid
SERVER_FIFO = "/var/run/user/%s/server_rx.fifo"%UID


#---------------------------------------------------#
#                 Start of Program                  #
#---------------------------------------------------#
def run(upper_value):
    while True:
        log.debug("Executing simulation")
        net_value = random.randint(0, upper_value)
        ip_address = "%s.%d"%(IP_ADDRESS, net_value)

        if not ip_address in IP_LIST:
            IP_LIST.update({ip_address: 0})
        
        heartbeat = {"type": "heartbeat", "total_hb": IP_LIST[ip_address], "client_ip": ip_address, "time": str(datetime.datetime.now())}
        IP_LIST[ip_address] += 1
        data = json.dumps(heartbeat)
        fi.write(data)
        log.debug(heartbeat)
        time.sleep(1)

class FIFO(object):
    def __init__(self):
        if os.path.exists(SERVER_FIFO):
            os.system("rm %s"%SERVER_FIFO)
        os.mkfifo(SERVER_FIFO)
        
    def write(self, data):
        fifo = open(SERVER_FIFO, "w")
        fifo.write(data)
        fifo.close()

if __name__=="__main__": 
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true", help = "Enter -v for verbosity")
    parser.add_argument('-r', '--random', default = 10, required = False, help = 'Enter the number of unique ips to be broadcasted') 
    args = parser.parse_args()
    
    #Configure the logger
    log = Log(sys.argv[0], verbosity=args.verbosity).logger

    fi = FIFO()
    run(int(args.random))

