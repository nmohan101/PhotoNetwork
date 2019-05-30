#!/usr/bin/env python

"""unittest_server.py: Test all functions of the server.py


__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2019. NM Technologies" 
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import unittest
import datetime
import sys
import os
import logging
import pwd
#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/Application")
import server
import client
import getip
import socket

class TestServerClient(unittest.TestCase):

    def setUp(self):
        self.uid = pwd.getpwuid(os.getuid()).pw_uid

    def test_server_constants(self):
        self.assertEqual(server.SETTINGS_FILE, "/etc/PhotoNetwork/network_settings.json")
        self.assertEqual(server.BROADCAST_PORT, 5560)
        self.assertEqual(server.MULTICAST_PORT, 5570)
        self.assertEqual(server.LISTEN_PORT, 5580)
        self.assertEqual(server.MULTICAST_IP, "224.1.1.1")
        self.assertEqual(server.SERVER_FIFO, "/var/run/user/%s/server_rx.fifo"%self.uid)
        self.assertEqual(server.MULTI_FIFO, "/var/run/user/%s/multi.fifo"%self.uid)
        self.assertEqual(server.LOG_PATH, "/var/log/PhotoNetwork/")
    
    def test_client_constants(self):
        self.assertEqual(client.PORT_TXRX, 5580)
        self.assertEqual(client.LISTEN_PORT, 5560)
        self.assertEqual(client.MULTICAST_PORT, 5570)
        self.assertEqual(client.MULTICAST_IP, "224.1.1.1")
        self.assertEqual(client.LOG_PATH, "/var/log/PhotoNetwork/")

    def test_broadcast(self):
        #\Description: Test the broadcast functions for server and client
        #\Step: Initialize server and client
        #\Step: Send a broadcast message
        #\Step: Read the broadcast message and confrim accuracy

        #Get Values
        my_ip = str(getip.get_local_ip())
        my_hostname = socket.gethostname()


        #Initialize server and client
        srv = server.UDP()
        clnt = client.client_udp()

        #Send the broadcast message
        srv.broadcast()

        #Receive the boradcast message
        clnt.find_host()

        #Compare Results
        self.assertEqual(my_ip, clnt.master_ip)
        self.assertEqual(my_hostname, clnt.master_hostname)

        #Close Sockets
        srv.sock_b.close()
        clnt.sock_txrx.close()


if __name__ == '__main__':
    unittest.main()
