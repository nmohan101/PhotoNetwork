#!/usr/bin/env python
"""client.py: This application is to be executed by the slaves on the network. The application
              will wait until its able to find a host and then connect to it. 


__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
import struct
import time
from threading import Thread, Timer, Event
import re

#********Local Imports************
import AsyncTimer

#**************Constants***********
port_txrx = 5580
listen_port = 5560
multicast_ip = "224.1.1.1"
package_counter = 0
heartbeat = {"type": "heartbeat", "total_hb": 0, "clent_ip": "", "time": ""}

class client_udp(object):
    
    def __init__(self):
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_txrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.master_hostname = ""
        self.master_ip = ""
        self.ip = ""
        
    def  find_host(self):
        print "Looking for Master......"
        self.sock_txrx.bind(('', listen_port))
        
        while self.master_ip == "":
            data, addr = self.sock_txrx.recvfrom(1024)
            if data.startswith("HOST="):
                self.master_hostname = data.split("=")[1]
                self.master_ip = addr[0] 
                print "Found master, Hostname: %s, IP: %s"%(self.master_hostname, self.master_ip)
                
def send_heartbeat():
    print "Sending heartbeat.........."
    cu.sock_txrx.sendto("I am client", (cu.master_ip, port_txrx))
    

    
#    s.close()
#    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
#    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#    s.bind(("224.1.1.1", 5570))
#    
#    mreq = struct.pack("4sl", socket.inet_aton("224.1.1.1"), socket.INADDR_ANY)
#    s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
#    
#    while True:
#        print s.recv(10240)
#    
    

if __name__=="__main__": 
    cu = client_udp()
    cu.find_host()
    AsyncTimer.Async_Timer(15, send_heartbeat).start()
	


	
	

