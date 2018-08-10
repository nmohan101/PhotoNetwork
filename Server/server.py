#!/usr/bin/env python

"""server.py: This application is to be executed by the master on the network.
              Its able to detect all the clients on the network and able send commands to 
              capture images.

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
import time


#**************Constants***********
host = ""
port = 5560



def broadcast():
    bsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    bsocket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    hostname = socket.gethostname()
    bsocket.sendto("HOST=%s"%hostname,('255.255.255.255',5560))
    
def set_server():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	print('Socket Created')

	try:
		s.bind((host, port))
	except socket.error as msg:
		print(msg)

	print('Socket bind complete')

	return s

def set_con():

	s.listen(1)
	print('Waiting for Client................................')
	conn, address = s.accept()
	print('Connected to:' + address[0] + ':' + str(address[1]))
	return conn

def send(conn, message):
	conn.sendall(str.encode(str(message)))
	print('Data sent')
	#conn.close()

def recv():

	cm = None
	
	while not cm:
		cm = conn.recv(1024)
		cm = cm.decode('utf-8')

	print(cm)

if __name__== "__main__":
    
    broadcast()

	

	#s = set_server()

 	#try: 
	#	conn = set_con()
	#except:
	#	raise

	#while True:
	#	message = int(input('Enter Number of Captures to Take:'))
	#	send(conn, message)
	##	print('{} captures taken successfully!'.format(message))






