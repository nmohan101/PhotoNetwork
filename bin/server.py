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
import argparse
import os
import sys
import pwd
import time

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
SETTINGS_FILE  = "/etc/PhotoNetwork/network_settings.json"
BROADCAST_PORT = 5560
MULTICAST_PORT = 5570
LISTEN_PORT    = 5580
MULTICAST_IP   = "224.1.1.1"
UID            = pwd.getpwuid(os.getuid()).pw_uid
SERVER_FIFO    = "/var/run/user/%s/server_rx.fifo"%UID
MULTI_FIFO     = "/var/run/user/%s/multi.fifo"%UID


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
   
    def broadcast(self):
        bcast_message = {"type": "host_broadcast", "total_bc": self.bc_msg_counter, "hostname": socket.gethostname(), "time": str(datetime.datetime.now())}
        log.info(" tx %s"%(bcast_message))
        data = json.dumps(bcast_message)
        self.sock_b.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.sock_b.sendto(data, ('255.255.255.255',BROADCAST_PORT))
        self.bc_msg_counter += 1
    
    def _config_multicast(self):
        log.debug("Configuring Multicast")
        self.sock_m.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(self.ip))
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        membership_request = socket.inet_aton(MULTICAST_IP) + socket.inet_aton(self.ip)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, membership_request)

    def multicast(self, command):
        log.info("Sending multi-cast message {}".format(command))
        self.sock_m.sendto(command,(MULTICAST_IP, MULTICAST_PORT))

class FIFO(object):
    def __init__(self):
        if os.path.exists(SERVER_FIFO):
            os.system("rm %s"%SERVER_FIFO) 
        os.mkfifo(SERVER_FIFO)
        
    def read(self):
        if os.path.exists(MULTI_FIFO):
            log.debug("Checking for fifo_data")
            fifoData = open(MULTI_FIFO, "r")
            command = fifoData.read()
            log.debug("Found data in %s file"%MULTI_FIFO) 
            fifoData.close()
            return command
        else:
            log.debug("No multi_fifo file found at: %s"%MULTI_FIFO)

    def write(self, data):
        log.debug("Writing fifo data")
        fifo = open(SERVER_FIFO, "w")
        fifo.write(data)
        fifo.close()

class server(object):
    def __init__(self):
        pass

    def wait_for_multi(self):
        while True:
            command = fifo.read()
            if command:
                log.debug("Command rx - Sending to Clients {}".format(command))
                u.multicast(command)
                command = None
            time.sleep(0.5)

    def main(self, listen):
        #\Input: Socket object to listen
        #\Function: Bind to the listen port and listen for messages sent from the client.
        #           A warning is logged if no message is recived for 60 seconds. 
        #\Output: Once message is recieved call the fifo.write function to write message

        #@ToDo: In the future this method needs to be moved to the UDP class and main should just call it. 

        listen.settimeout(60)
        listen.bind(("", LISTEN_PORT))

        while True:
            try:
                data, port = listen.recvfrom(1024)
                log.info("rx %s"%(json.loads(data)))
                fifo.write(data)        
            except socket.timeout:
                log.warning("NO MESSAGE RECEIVED")

        log.warning("SHUTDOWN EXECUTED")
        listen.close()
    
if __name__== "__main__":
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    #Configure the logger
    log = Log(sys.argv[0], verbosity=args.verbosity).logger

    #Configure and run the main program
    u = UDP()
    mn = server()
    fifo = FIFO()
    brdcast = asynctimer.AsyncTimer(10, u.broadcast)
    brdcast.start()
    wt = Thread(target=mn.wait_for_multi)
    wt.daemon = True
    wt.start()
    mn.main(u.sock_l)

    #Shutdown Sequence
    brdcast.stop()
    u.sock_b.close()
    u.sock_m.close()

