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
#**************Constants***********
port_txrx = 5580
listen_port = 5560
multicast_ip = "224.1.1.1"

class Async_Timer():

    def __init__(self, t, hFunction):
        self.t = t
        self.hFunction = hFunction
        self.thread = Timer(self.t, self.handle_function)

    def handle_function(self):
        self.hFunction()
        self.thread = Timer(self.t, self.handle_function)
        self.thread.start()

    def start(self):
        self.thread.start()

    def cancel(self):
        self.thread.cancel()

class client_udp(object):
    
    def __init__(self):
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_txrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.master_hostname = ""
        self.master_ip = ""
        self.ip = ""
        
    def ip_is_local(self, ip_string):
        
        combined_regex = "(^10\.)|(^172\.1[6-9]\.)|(^172\.2[0-9]\.)|(^172\.3[0-1]\.)|(^192\.168\.)"
        return re.match(combined_regex, ip_string) is not None   
        
    def get_local_ip(self):
        
        local_ips = [ x[4][0] for x in socket.getaddrinfo(socket.gethostname(), 80) if self.ip_is_local(x[4][0]) ]
        local_ip = local_ips[0] if len(local_ips) > 0 else None

        if not local_ip:
            temp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            try:
                temp_socket.connect(('8.8.8.8', 9))
                self.ip = temp_socket.getsockname()[0]
            except socket.error:
                self.ip = "127.0.0.1"
            finally:
                temp_socket.close()
        
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
    Async_Timer(15, send_heartbeat).start()
	


	
	

