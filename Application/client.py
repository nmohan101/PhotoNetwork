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

#********Local Imports************
import AsyncTimer
import getip

#**************Constants***********
port_txrx = 5580
listen_port = 5560
multicast_port = 5570
multicast_ip = "224.1.1.1"


class client_udp(object):
    
    def __init__(self):
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)  
        self.sock_txrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_multi = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock_multi.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_multi.bind((multicast_ip, multicast_port))
        self.master_hostname = ""
        self.master_ip = ""
        self.ip = str(getip.get_local_ip())
        self.package_counter = 0
        self.sock_txrx.settimeout(30)                        #Set the socket timeout to 30 seconds 
        self.sock_txrx.bind(('', listen_port))               #Bind socket to broadcast listening port
        self.host_status = False
        
    def  find_host(self):
        
        while True: 
            try:
                try:
                    data, addr = self.sock_txrx.recvfrom(1024)
                    message = json.loads(data)
                    if message["type"] == "host_broadcast":
                        self.host_status = True
                        self.master_hostname = message["hostname"]
                        self.master_ip = addr[0]
                        print data
                except socket.timeout:
                    print "Master Not found"
                    self.master_hostname = ""
                    self.master_ip = ""
                    self.host_status = False
            except(KeyboardInterrupt, SystemExit):
                raise
                             
    def send_heartbeat(self):
        heartbeat = {"type": "heartbeat", "total_hb": cu.package_counter, "clent_ip": cu.ip, "time": str(datetime.datetime.now())}
        data = json.dumps(heartbeat)
        print "Sending heartbeat"
        cu.sock_txrx.sendto(data, (cu.master_ip, port_txrx))
        cu.package_counter += 1
    
    def multi_listen(self):
        mreq = struct.pack("4sl", socket.inet_aton(multicast_ip), socket.INADDR_ANY)
        self.sock_multi.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        
        while True:
            print self.sock_multi.recv(10240)

class controller(object):
    
    def __init__(self):
        self.heartbeat_active = False

    def status_controller(self):
        host_finder = Thread(target=cu.find_host)
        host_finder.start()
        
        try:
            while True:
                if cu.host_status == True and self.heartbeat_active == False:
                    AsyncTimer.Async_Timer(10, cu.send_heartbeat).start()
                    self.heartbeat_active = True
                elif cu.host_status == False and self.heartbeat_active == True:
                    AsyncTimer.Async_Timer(10, cu.send_heartbeat).cancel()         #Stop sending heartbeats as the host is no longer active
                    self.heartbeat_active = False
        except(KeyboardInterrupt, SystemExit):
            raise
    
if __name__=="__main__": 
    
    #Intialize the UDP class
    cu = client_udp()

    
    #Call status controller
    cnt = controller().status_controller()
    

    
    
    
	


	
	

