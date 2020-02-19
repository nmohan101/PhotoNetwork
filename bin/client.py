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
import time
import json
import struct
import sys
import argparse
import subprocess
import os

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/lib")
import asynctimer
import getip
from log import Log

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
PORT_TXRX      = 5580
LISTEN_PORT    = 5560
MULTICAST_PORT = 5570
MULTICAST_IP   = "224.1.1.1"

    


#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class client_udp(object):
    
    def __init__(self):
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)  
        self.sock_txrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_multi = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_multi.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_txrx.settimeout(0.0)                        #Set socket to non-blocknig. 
        self.sock_multi.bind((MULTICAST_IP, MULTICAST_PORT))
        self.master_hostname = ""
        self.master_ip = ""
        self.ip = str(getip.get_local_ip())
        self.package_counter = 0
        self.sock_txrx.settimeout(30)                        #Set the socket timeout to 30 seconds 
        self.sock_txrx.bind(('', LISTEN_PORT))               #Bind socket to broadcast listening port
        self.host_status = False                             #Flag - True if Master is found and False if Master is not found
        self._config_multicast()
    
      
    def find_host(self):
        
        try:
            data, addr = self.sock_txrx.recvfrom(1024)
            message = json.loads(data)
            if message["type"] == "host_broadcast":
                self.host_status = True
                self.master_hostname = message["hostname"]
                self.master_ip = addr[0]
                log.info("rx %s"%data)
        except socket.timeout:
            log.warning("STATUS - MASTER NOT FOUND")
            self.master_hostname = ""
            self.master_ip = ""
            self.host_status = False

    def send_heartbeat(self):
        heartbeat = {"type": "heartbeat", "total_hb": cu.package_counter, "client_ip": cu.ip, "time": str(datetime.datetime.now())}
        data = json.dumps(heartbeat)
        log.info("tx %s"%heartbeat)
        cu.sock_txrx.sendto(data, (cu.master_ip, PORT_TXRX))
        cu.package_counter += 1
    
    def _config_multicast(self):
        log.debug("Configuring multicast")
        mreq = struct.pack("4sl", socket.inet_aton(MULTICAST_IP), socket.INADDR_ANY)
        self.sock_multi.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    def multi_listen(self):
        self.rx_data = self.sock_multi.recv(10240).split()
        log.info("Action message rx from host - {}".format(self.rx_data))

        try:
            proc = subprocess.Popen(self.rx_data, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
        except subprocess.CalledProcessError:
            log.error("Unable to execute command %s"%self.rx_data)
            raise


class controller(object):
    
    def __init__(self):
        self.heartbeat_active = False

    def status_controller(self):
        host_finder = asynctimer.AsyncTimer(20, cu.find_host)
        multi_listen = asynctimer.AsyncTimer(0.5, cu.multi_listen)
        heartbeat = asynctimer.AsyncTimer(10, cu.send_heartbeat)
        host_finder.start()
        
        while True:
            time.sleep(0.1)
            if cu.host_status == True and self.heartbeat_active == False:
                log.info("STATUS - Master found start sending heartbeats")
                log.info("STATUS - Master found listen for multi-cast message")
                heartbeat.start()
                multi_listen.start()
                self.heartbeat_active = True
            elif (cu.host_status == False and self.heartbeat_active == True):
                heartbeat.stop()                                       #Stop sending heartbeats as the host is no longer active
                #multi_listen.stop()                                    #Stop listening for multi-cast message as host not active
                self.heartbeat_active = False
                log.warning("STATUS - LOST CONNECTION TO HOST")
                
        log.warning("SHUTDOWN EXECUTED")
        heartbeat.stop() 
        host_finder.stop()
        #multi_listen.stop()
        cu.sock_rx.close()
        cu.sock_txrx.close()
        cu.sock_multi.close()
  
if __name__=="__main__": 
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  help = "Enter -v for verbosity")
    args = parser.parse_args()

    #Configure the logger
    log = Log(sys.argv[0], verbosity=args.verbosity).logger

    #Initialize the UDP class
    cu = client_udp()
    cntrl = controller()

    #Call status controller
    cntrl.status_controller()

    
    
