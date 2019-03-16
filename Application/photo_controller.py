#!/usr/bin/env python

"""photo_controller.py: This applcation is the main program that controls all child applications. 
			The main purpose of this application is to monitor all the child proocess and
			and execute them appropriatly. 
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""


#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import subprocess
from threading import Thread
import logging
import json
import time

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
SETTINGS_FILE = "/etc/PhotoNetwork/network_settings.json"


#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class config(object):
	def __init__(self):
		self.settings_data = self._getsettings()
		self.mode = self.settings_data["Network_Mode"]
                self.sim = self.settings_data["Sim_Mode"]
                self.test_path = self.settings_data["test_path"]
                self.app_path = self.settings_data["app_path"]
	
	def _getsettings(self):
		settings_file = open(SETTINGS_FILE, "r")
		settings = settings_file.read()
		settings_file.close()
		return json.loads(settings)
            
class controller(object):
    
    def __init__(self):
        pass

    def monitor(self):
        
        if config.sim == True:
            events = subprocess.Popen(["python", config.test_path + "event_simulator.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            time.sleep(1)
            handler = subprocess.Popen(["python", config.app_path + "event_handler.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            while True:
                if  events.poll() != None:
                    print "event_handler.py is no longer running"
                if handler.poll() != None:
                    print "event_simulator.py is no longer running"
                time.sleep(1)

if __name__== "__main__":

	config = config()
        control = controller()
        control.monitor()
