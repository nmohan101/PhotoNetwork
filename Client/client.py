#!/usr/bin/env python
"""client.py: This application is to be executed by the slaves on the network. The application
              will wait until its able to find a host and then connect to it. 


__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
import datetime

#**************Constants***********
port = 5560

def connect():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	try: 
		s.connect((host, port))
		print('Connected to: {}'.format(host))
	except:
		s.close()
		raise
	
	return s
	
def  find_host():
    
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('',5560))
    
    while True: 
        print "Waiting for Host Address.."
        data, addr = s.recvfrom(1024)
        
        if data.startswith("HOST="):
            server_hostname = data.split("=")[1]
            server_ip = addr[0] 
            print (server_hostname, server_ip)
            break
        
    return server_hostname, server_ip


if __name__=="__main__":
    
	server_hostname, server_ip = find_host()
	
	# = connect()
	
	#while True:
	#	cn = s.recv(1024)
	#	if cn:
	#		print(cn)
	#		capture(int(cn))
	#		s.send(str.encode('Capture Successful!'))
	#		print('Capture {} image(s)'.format(cn)) 

	
	

