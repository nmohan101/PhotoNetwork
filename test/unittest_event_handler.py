#!/usr/bin/env python

"""unittest_event_handler.py: Test all functions of the event_handler.py

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
#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/Application")
import event_handler as eh

#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#

class TestEventHandler(unittest.TestCase):
    
    def setUp(self):
        table = [{"TableName": "unitTestTable", "TableCol": """id integer PRIMARY KEY, type text NOT NULL, 
                   total_heartbeats integer NOT NULL, client_ip text NOT NULL, time text NOT NULL"""}]

        self.sql = eh.SQL("/tmp/unittest_eh.db", table)

    def test_insert_input_func(self):

        #Description - Test inset_input_structure function. Ensure function output equals actual output
        time = datetime.datetime.now()
        table = "All_Messages"
        tb_columns = ["type", "total_heartbeats", "client_ip", "time"]
        expected_struct = "INSERT INTO %s(type, total_heartbeats, client_ip, time) VALUES(?, ?, ?, ?)"%(table)

        sqlstruct = ("heartbeat", 10, "193.168.2.1", time)
        actual_struct = self.sql._insert_input_struct(sqlstruct, table, tb_columns)
        self.assertEqual(expected_struct, actual_struct)

    def test_update_input_func(self):

        #Description - Test update_input_struct function. Ensure function output equals actual output
        time = datetime.datetime.now()
        table = "active_clients"
        struct = {"total_hb": 10, "client_ip": "192.168.2.1", "time": time}
        expected_struct = ("UPDATE %s set total_hb = ?, time = ? WHERE client_ip = ?"%table, (struct["client_ip"], struct["time"], struct["total_hb"]))

        actual_struct = self.sql._update_input_struct(struct, table, ("client_ip", "192.168.2.1"))
        self.assertEqual(expected_struct, actual_struct)

    def test_create_table_func(self):
        #Description - Test that the format to create the input structure is correct for creating tables
        self.tb_config = eh.TableConfig()

if __name__ == '__main__':
    unittest.main()
