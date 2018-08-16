#!/usr/bin/env python

"""server.py: This application is to be executed by the master on the network.
              Its able to detect all the clients on the network and able send commands to 
              capture images.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
import re
from threading import Thread

#********Local Imports************
import AsyncTimer

#******Costants********************
broadcast_port = 5560
multicast_port = 5570
listen_port = 5580
multicast_ip = "224.1.1.1"

class UDP(object):
    
    def __init__(self):
        self.sock_b = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_m = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_l = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.ip = ""
     
    def _broadcast(self):
        print "Sending broadcast message"
        self.sock_b.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.hostname = socket.gethostname()
        self.sock_b.sendto("HOST=%s"%self.hostname,('255.255.255.255',broadcast_port))

    def _multicast(self):
        print "Sending multicast message"
        self.sock_m.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(self.ip))
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        
        membership_request = socket.inet_aton(multicast_ip) + socket.inet_aton(self.ip)
        self.sock_m.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, membership_request)

        self.sock_m.sendto("Hello",(multicast_ip, multicast_port))
                      
def main(listen):
    listen.bind(("", listen_port))
    try:
        while True:
            print "Waiting for data"
            data, port = listen.recvfrom(4096)
            print data
            print port
    except(KeyboardInterrupt, SystemExit):
        raise
    
          
if __name__== "__main__":
    u = UDP()
    AsyncTimer.Async_Timer(15, u._broadcast).start()
    listner = Thread(target=main, args=(u.sock_l,))
    listner.start()

    
    
    
    

	








