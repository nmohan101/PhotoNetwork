#!/usr/bin/env python
"""client.py: This application is to be executed by the slaves on the network. The application
              will wait until its able to find a host and then connect to it. 


__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import socket
from threading import Thread
import datetime
import json
import struct
import sys
import logging
import argparse
import os

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
import asynctimer
import getip

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
PORT_TXRX = 5580
LISTEN_PORT = 5560
MULTICAST_PORT = 5570
MULTICAST_IP = "224.1.1.1"
LOG_PATH = "/var/log/PhotoNetwork/"


#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class client_udp(object):
    
    def __init__(self):
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)  
        self.sock_txrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_multi = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_multi.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_multi.bind((MULTICAST_IP, MULTICAST_PORT))
        self.master_hostname = ""
        self.master_ip = ""
        self.ip = str(getip.get_local_ip())
        self.package_counter = 0
        self.sock_txrx.settimeout(30)                        #Set the socket timeout to 30 seconds 
        self.sock_txrx.bind(('', LISTEN_PORT))               #Bind socket to broadcast listening port
        self.host_status = False                             #Flag - True if Master is found and False if Master is not found
    
      
    def  find_host(self, sys_exit):
        
        while sys_exit==False: 
            try:
                data, addr = self.sock_txrx.recvfrom(1024)
                message = json.loads(data)
                if message["type"] == "host_broadcast":
                    self.host_status = True
                    self.master_hostname = message["hostname"]
                    self.master_ip = addr[0]
                    logger.info("rx %s"%data)
            except socket.timeout:
                logger.warning("STATUS - MASTER NOT FOUND")
                self.master_hostname = ""
                self.master_ip = ""
                self.host_status = False
        self.sock_txrx.close()

    def send_heartbeat(self):
        heartbeat = {"type": "heartbeat", "total_hb": cu.package_counter, "client_ip": cu.ip, "time": str(datetime.datetime.now())}
        data = json.dumps(heartbeat)
        logger.info("tx %s"%heartbeat)
        cu.sock_txrx.sendto(data, (cu.master_ip, PORT_TXRX))
        cu.package_counter += 1
    
    def multi_listen(self):
        mreq = struct.pack("4sl", socket.inet_aton(MULTICAST_IP), socket.INADDR_ANY)
        self.sock_multi.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        
        while True:
            logger.info(self.sock_multi.recv(10240))

class controller(object):
    
    def __init__(self):
        self.heartbeat_active = False
        self.sys_exit = False

    def exits(self):
        inp = raw_input("PRESS ANY KEY TO EXIT\n")
        self.sys_exit = True

    def status_controller(self):
        host_finder = Thread(target=cu.find_host, args=(self.sys_exit,))
        host_finder.start()
        heartbeat = asynctimer.AsyncTimer(10, cu.send_heartbeat)
        

        while self.sys_exit == False:
            if cu.host_status == True and self.heartbeat_active == False:
                logger.info("START SENDING HEARTBEATS")
                heartbeat.start()
                self.heartbeat_active = True
            elif (cu.host_status == False and self.heartbeat_active == True):
                heartbeat.stop()                                       #Stop sending heartbeats as the host is no longer active
                self.heartbeat_active = False
                logger.warning("STATUS - LOST CONNECTION")
                
        logger.warning("SHUTDOWN EXECUTED")
        heartbeat.stop() 
        cu.sock_rx.close()
        cu.sock_txrx.close()
        cu.sock_multi.close()
  
if __name__=="__main__": 
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  help = "Enter -v for verbosity")
    args = parser.parse_args()

    #Create and configure the logger
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(funcName)s - %(levelname)s - %(message)s')
    ch = logging.StreamHandler()  
    fh = logging.FileHandler("%s%s.log"%(LOG_PATH, sys.argv[0].split(".")[0]))
    ch.setFormatter(formatter)
    fh.setFormatter(formatter)
    
    if args.verbosity:
        print "VERBOSE MODE"
        ch.setLevel(logging.DEBUG)
    else:
        ch.setLevel(logging.WARNING)
    
    logger.addHandler(ch)
    logger.addHandler(fh)
    
    #Initialize the UDP class
    cu = client_udp()
    cntrl = controller()
    Thread(target=cntrl.exits).start()

    
    #Call status controller
    cntrl.status_controller()

    
    
