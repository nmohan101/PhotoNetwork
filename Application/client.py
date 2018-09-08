#!/usr/bin/env python
"""client.py: This application is to be executed by the slaves on the network. The application
              will wait until its able to find a host and then connect to it. 


__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
from threading import Thread
import datetime
import json
import struct
import sys
import logging
import argparse
#********Local Imports************
import AsyncTimer
import getip

#**************Constants***********
PORT_TXRX = 5580
LISTEN_PORT = 6005
MULTICAST_PORT = 5570
MULTICAST_IP = "224.1.1.1"


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
        self.host_status = False
    
      
    def  find_host(self):
        
        while self.sock_txrx: 
            try:
                try:
                    data, addr = self.sock_txrx.recvfrom(1024)
                    message = json.loads(data)
                    if message["type"] == "host_broadcast":
                        self.host_status = True
                        self.master_hostname = message["hostname"]
                        self.master_ip = addr[0]
                        logging.info("%s rx %s"%(datetime.datetime.now(), data))
                except socket.timeout:
                    logging.warning("%s STATUS - MASTER NOT FOUND"%datetime.datetime.now())
                    self.master_hostname = ""
                    self.master_ip = ""
                    self.host_status = False
            except(KeyboardInterrupt, SystemExit):
                sys.exit()
                raise
                             
    def send_heartbeat(self):
        heartbeat = {"type": "heartbeat", "total_hb": cu.package_counter, "clent_ip": cu.ip, "time": str(datetime.datetime.now())}
        data = json.dumps(heartbeat)
        logging.info("%s tx %s"%(datetime.datetime.now(), heartbeat))
        cu.sock_txrx.sendto(data, (cu.master_ip, PORT_TXRX))
        cu.package_counter += 1
    
    def multi_listen(self):
        mreq = struct.pack("4sl", socket.inet_aton(MULTICAST_IP), socket.INADDR_ANY)
        self.sock_multi.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        
        while True:
            logging.info(self.sock_multi.recv(10240))

class controller(object):
    
    def __init__(self):
        self.heartbeat_active = False
        self.system_exit = False

    def status_controller(self):
        host_finder = Thread(target=cu.find_host)
        host_finder.start()
        heartbeat = AsyncTimer.Async_Timer(10, cu.send_heartbeat)
        
        try:
            while self.system_exit == False:
                if cu.host_status == True and self.heartbeat_active == False:
                   heartbeat.start()
                   self.heartbeat_active = True
                elif (cu.host_status == False and self.heartbeat_active == True) or self.system_exit == True:
                    heartbeat.cancel()                                       #Stop sending heartbeats as the host is no longer active
                    self.heartbeat_active = False
                    logging.warning("%s STATUS - LOST CONNECTION"%datetime.datetime.now())
        except(KeyboardInterrupt, SystemExit):
            sys.exit(1)
            raise

    
if __name__=="__main__": 
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    #logging.basicConfig(filename='client.log', level=logging.INFO)
    if args.verbosity:
        logging.getLogger().setLevel(logging.INFO)
    
    #Intialize the UDP class
    cu = client_udp()
    cntrl = controller()

    
    #Call status controllershutdown
    controller = Thread(target=cntrl.status_controller)
    controller.start()
    
    inp = raw_input("PRESS ANY KEY TO EXIT")
    
    if inp:
        logging.warning("SHUTDOWN EXECUTED")
        cntrl.system_exit = True
        cu.sock_rx.close()
        cu.sock_txrx.close()
        cu.sock_multi.close()
        sys.exit(1)

    

    
    
    
	


	
	

