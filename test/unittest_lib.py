#!/usr/bin/env python

"""unittest_lib.py: Test all library functions

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2019. NM Technologies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import unittest
import sys
import os
import StringIO

#---------------------------------------------------#
#                   Local Imports                   #
#---------------------------------------------------#
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)) + "/lib")
from log import Log
import log
#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#

class TestLib(unittest.TestCase):

    def test_log(self):
        log_object = Log(sys.argv[0], verbosity=True)
        l = log_object.logger
        filename = log_object.filename

        l.debug("Testing the logging libirary")
        l.warning("Testing the logging libirary")
        l.error("Testing the logging libirary")

        with open(log.LOG_PATH + filename, "r") as f:
            data = f.read()
            self.assertIn("DEBUG", data)
            self.assertIn("WARNING", data)
            self.assertIn("ERROR", data)

if __name__ == '__main__':
    unittest.main()
