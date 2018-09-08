#!/usr/bin/env python

"""server.py: This application is to be executed by the master on the network.
              Its able to detect all the clients on the network and able send commands to 
              capture images.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
from threading import Thread
import json
import datetime
import logging
import argparse
import os
import sys
#********Local Imports************
import AsyncTimer

#******Costants********************
BROADCAST_PORT = 5560
MULTICAST_PORT = 5570
LISTEN_PORT = 5580
MULTICAST_IP = "224.1.1.1"

class UDP(object):
    
    def __init__(self):
        self.sock_b = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_m = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_l = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.ip = ""
        self.bc_msg_counter = 0
    
    def _broadcast(self):
        bcast_message = {"type": "host_broadcast", "total_bc": self.bc_msg_counter, "hostname": socket.gethostname(), "time": str(datetime.datetime.now())}
        logging.info("%s tx %s"%(datetime.datetime.now(), bcast_message))
        data = json.dumps(bcast_message)
        with open("bcmessage.json", "w") as f:
            json.dump(bcast_message, f)
        try:
            self.sock_b.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            self.sock_b.sendto(data, ('255.255.255.255',BROADCAST_PORT))
            self.bc_msg_counter += 1
        except(KeyboardInterrupt, SystemExit):
            self.sock_b.shutdown
            self.sock_m.shutdown
            self.sock_l.shutdown
            raise

    def _multicast(self):
        logging.info("Sending multicast message")
        self.sock_m.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(self.ip))
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        
        membership_request = socket.inet_aton(MULTICAST_IP) + socket.inet_aton(self.ip)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, membership_request)

        self.sock_m.sendto("Hello",(MULTICAST_IP, MULTICAST_PORT))

class FIFO(object):
    def __init__(self):
        self.fifo_path = ("/tmp/server_rx.fifo")
        os.mkfifo(self.fifo_path)
    
    def write(self, data):
        fifo = open(self.fifo_path, "w")
        fifo.write(data)
        fifo.close()

def main(listen):
    listen.bind(("", LISTEN_PORT))
    try:
        while True:
            data, port = listen.recvfrom(4096)
            logging.info("%s rx %s"%(datetime.datetime.now(), json.loads(data)))
            fifo.write(data)         
    except(KeyboardInterrupt, SystemExit):
        raise
    
if __name__== "__main__":
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  help = "Enter -v for verbosity")
    args = parser.parse_args()
    
    logging.basicConfig(filename='server.log', level=logging.INFO)
    if args.verbosity:
        logging.getLogger().setLevel(logging.DEBUG)

    u = UDP()
    fifo = FIFO()
    brdcast = AsyncTimer.Async_Timer(10, u._broadcast)
    brdcast.start()
    listner = Thread(target=main, args=(u.sock_l,))
    listner.start()    
    
    inp = raw_input("PRESS ANY KEY TO EXIT")
    
    if inp:
        print "SHUTDOWN EXECUTED"
        brdcast.cancel()
        u.sock_b.shutdown
        u.sock_m.shutdown
        u.sock_l.shutdown
        sys.exit(1)

    
    

    

    
    

    
    
    
    

	








