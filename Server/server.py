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
from threading import Thread, Timer, Event

#******Costants********************
broadcast_port = 5560
multicast_port = 5570
listen_port = 5580
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
    Async_Timer(15, u._broadcast).start()
    listner = Thread(target=main, args=(u.sock_l,))
    listner.start()

    
    
    
    

	








