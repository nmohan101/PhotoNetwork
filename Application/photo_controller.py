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
import argparse
import sys
import pwd
import os

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
SETTINGS_FILE = "/etc/PhotoNetwork/network_settings.json"
LOG_PATH = "/var/log/PhotoNetwork/"
UID = pwd.getpwuid(os.getuid()).pw_uid
MULTI_FIFO = "/var/run/user/%s/multi.fifo"%UID


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

class FIFO(object):
    def __init__(self):
        if os.path.exists(MULTI_FIFO):
            os.system("rm %s"%MULTI_FIFO) 
        os.mkfifo(MULTI_FIFO)
        
    def write(self, data):
        logger.debug("Writing data to fifo {}".format(data))
        fifo = open(MULTI_FIFO, "w")
        fifo.write(data)
        fifo.close()
        return
            
class controller(object):
    
    def __init__(self):
        self.terminate = False
        self.active_process = []
        self.event_handler = False
        self.server_mode = False
        self.sim_mode = False

    def user_input(self):
        exit = False
        while exit == False:
            default_message = ['python', config.app_path + "capture.py", "-v", "-c"]
            inp = raw_input("Enter number of captures to be taken or enter exit to close program\n")
            logger.debug("User input is: {}".format(inp))
            if inp == 'exit':
                exit = True
            else: 
                default_message.append(inp)
                logger.info("Sending multicast message to clients {}".format(default_message))
                fifo.write(str(default_message))

    def monitor(self):
        if config.mode == 'server' and config.sim:
            try:
                logger.info("Starting: event_simulator.py")
                events = subprocess.Popen(["python", config.test_path + "event_simulator.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
                stdout, stderr = events.communicate()
                self.active_process.append(events)
                self.event_handler = True
                self.sim_mode = True
            except:
                raise
                sys.exit(1)
        elif config.mode == 'server' and config.sim == False:
            try:
                logger.info("Starting: server.py")
                server = subprocess.Popen(["python", config.app_path + "server.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
                self.active_process.append(server)
                self.event_handler = True
                self.server_mode = True
            except:
                raise
                sys.exit(1)
        elif config.mode == 'client':
            try:
                logger.info("Starting: client.py")
                client = subprocess.Popen(["python", config.app_path + "client.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
                self.active_process.append(client)
                self.event_handler = False
            except:
                raise
                sys.exit(1)
        else:
            logger.warning("%s file not configured correctly. System could not start"%SETTINGS_FILE)
            sys.exit(1)

        if self.event_handler:
            time.sleep(1)
            try:
                logger.info("Starting: event_handler.py")
                handler = subprocess.Popen(["python", config.app_path + "event_handler.py", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
                self.active_process.append(handler)
            except:
                raise
                sys.exit(1)

        uit = None
        if self.server_mode:
            uit = Thread(target=self.user_input)
            uit.daemon = True 
            uit.start()

        while True:
            for proc in self.active_process:
                if  proc.poll() != None:
                    logger.warning("{} is no longer running".format(proc.pid))
                    self.terminate = True
            if uit:
                if not uit.is_alive():
                    self.terminate = True
            if self.terminate:
                logger.warning("Process died or user is terminating application. Exiting")
                for proc in self.active_process:
                    if proc.poll() == None:
                        proc.terminate()
                sys.exit(1)
            time.sleep(1)

if __name__== "__main__":

    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbosity', action = "store_true",  
                        help = "Enter -v for verbosity")
    args = parser.parse_args()

    #Create and configure the logger
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(funcName)s - %(levelname)s - %(message)s')
    ch = logging.StreamHandler()
    fh = logging.FileHandler("%s%s.log"%(LOG_PATH, sys.argv[0].split("/")[-1].split(".")[0]))
    ch.setFormatter(formatter)
    fh.setFormatter(formatter)
    
    if args.verbosity:
        print "VERBOSE MODE"
        ch.setLevel(logging.DEBUG)
    else:
        ch.setLevel(logging.WARNING)
    
    logger.addHandler(ch)
    logger.addHandler(fh)

    fifo = FIFO()
    config = config()
    control = controller()
    control.monitor()

