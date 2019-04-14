#!/usr/bin/env python

"""server.py: This application is to be executed by the master on the network.
              Its able to detect all the clients on the network and able send commands to 
              capture images.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import socket
from threading import Thread
import json
import datetime
import logging
import argparse
import os
import sys
import pwd
import time

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
import asynctimer
import getip

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
SETTINGS_FILE = "/etc/PhotoNetwork/network_settings.json"
BROADCAST_PORT = 5560
MULTICAST_PORT = 5570
LISTEN_PORT = 5580
MULTICAST_IP = "224.1.1.1"
UID = pwd.getpwuid(os.getuid()).pw_uid
SERVER_FIFO = "/var/run/user/%s/server_rx.fifo"%UID
MULTI_FIFO = "/var/run/user/%s/multi.fifo"%UID
LOG_PATH = "/var/log/PhotoNetwork/"


#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class UDP(object):
    
    def __init__(self):
        self.sock_b = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_m = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_l = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.ip = str(getip.get_local_ip())
        self.bc_msg_counter = 0
        self._config_multicast()
    
    def _broadcast(self):
        bcast_message = {"type": "host_broadcast", "total_bc": self.bc_msg_counter, "hostname": socket.gethostname(), "time": str(datetime.datetime.now())}
        logger.info(" tx %s"%(bcast_message))
        data = json.dumps(bcast_message)
        self.sock_b.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.sock_b.sendto(data, ('255.255.255.255',BROADCAST_PORT))
        self.bc_msg_counter += 1
    
    def _config_multicast(self):
        logger.debug("Configuring Multicast")
        self.sock_m.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(self.ip))
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        membership_request = socket.inet_aton(MULTICAST_IP) + socket.inet_aton(self.ip)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, membership_request)

    def multicast(self, command):
        logger.info("Sending multi-cast message {}".format(command))
        self.sock_m.sendto(command,(MULTICAST_IP, MULTICAST_PORT))

class FIFO(object):
    def __init__(self):
        if os.path.exists(SERVER_FIFO):
            os.system("rm %s"%SERVER_FIFO) 
        os.mkfifo(SERVER_FIFO)
        
    def read(self):
        logger.debug("Checking for fifo_data")
        fifoData = open(MULTI_FIFO, "r")
        command = fifoData.read()
        logger.debug("Found data in %s file"%MULTI_FIFO) 
        fifoData.close()
        return command

    def write(self, data):
        fifo = open(SERVER_FIFO, "w")
        fifo.write(data)
        fifo.close()

class server(object):
    def __init__(self):
        self.sys_exit = False

    def exits(self):
        while True:
            default_message = ['python', "/opt/PhotoNetwork/Application/capture.py", "-v", "-c"]
            inp = raw_input("Enter exit to close or message for multicast\n")
            logger.debug("User input is: {}".format(inp))
            if inp == 'exit':
                self.sys_exit = True
            else: 
                default_message.append(inp)
                u.multicast(str(default_message))

    def wait_for_multi(self):
        while True:
            command = fifo.read()
            if command:
                logger.debug("Command rx - Sending to Clients {}".format(command))
                u.multicast(command)
                command = None
            time.sleep(0.5)

    def main(self,listen):
        listen.settimeout(20)
        listen.bind(("", LISTEN_PORT))

        while self.sys_exit == False:
            try:
                data, port = listen.recvfrom(1024)
                logger.info("rx %s"%(json.loads(data)))
                fifo.write(data)        
            except socket.timeout:
                logger.warning("NO MESSAGE RECEIVED")

        logger.warning("SHUTDOWN EXECUTED")
        listen.close()
    
if __name__== "__main__":
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    #Create and configure the logger
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(funcName)s - %(levelname)s - %(message)s')
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
    
    #Configure and run the main program
    u = UDP()
    mn = server()
    fifo = FIFO()
    brdcast = asynctimer.AsyncTimer(10, u._broadcast)
    brdcast.start()
    et = Thread(target=mn.exits)
    wt = Thread(target=mn.wait_for_multi)
    et.daemon = True 
    wt.daemon = True
    et.start()
    wt.start()
    mn.main(u.sock_l)

    #Shutdown Sequence
    brdcast.stop()
    u.sock_b.close()
    u.sock_m.close()

