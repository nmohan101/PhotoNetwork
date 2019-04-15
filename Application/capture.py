#!/usr/bin/env python

"""capture.py: This application is on the client. It is the main process that applies
               camera settings and takes captures. 

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
from picamera import PiCamera
import datetime
import argparse
import logging
import sys

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
LOG_PATH = "/var/log/PhotoNetwork/"
CAM_SETTINGS = "/etc/PhotoNetwork/cam_settings.config"

class Camera(object):
    
    def __init__(self):
        #Initialize all camera inputs to 0
        self.camera = PiCamera()
        self.capture_path = ""
        self.res_width = 0
        self.res_height = 0
        self.sharpness = 0
        self.contrast = 0
        self.brightness = 0
        self.saturation = 0
        self.iso = 0
        self.shutter = 0
        self.opacity = 0 
        self.flash = ""
        
    def read_cam_settings(self):
        #Read and save camera settings
        logger.debug("Reading camera settings")
        settings_list = []
        
        with open(CAM_SETTINGS, "r") as f:
            for line in f:
                settings_list.append(line.rstrip("\n").split("="))
                
        sd = dict(settings_list)
        self.capture_path = sd["capture_path"]
        self.res_width = int(sd["resolutionw"])
        self.res_height = int(sd["resolutionh"])
        self.sharpness = int(sd["sharpness"])
        self.contrast = int(sd["contrast"])
        self.brightness = int(sd["brightness"])
        self.saturation = int(sd["saturation"])
        self.iso = int(sd["iso"])
        self.shutter = int(sd["shutter"])
        self.flash = str(sd["flash"])

            
            
    def camera_settings(self):
        #Apply Camera settings 
        logger.debug("Applying camera settings")
        self.camera.resolution = (self.res_width, self.res_height)
        self.camera.sharpness = self.sharpness
        self.camera.contrast = self.contrast
        self.camera.brightness = self.brightness
        self.camera.saturation = self.saturation
        self.camera.iso = self.iso
        self.camera.shutter_speed = self.shutter
        self.camera.flash_mode = self.flash
        

    def capture(self, captures):
        
        #Capture an image and store it in specified location
        for c in range(0, captures):
            time_stamp = str(datetime.datetime.now().strftime("%y-%m-%d_%H_%M_%S"))
            self.camera.capture("{}/image_{}.jpg".format(self.capture_path, time_stamp))
            
            logger.info("Imaged saved to: %s/%s"%(self.capture_path, time_stamp))

if __name__=="__main__":
    
    #Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--capture', default=1, type = int, help = "Enter number of captures to be taken")
    parser.add_argument('-v', '--verbosity', action = "store_true",  help = "Enter -v for verbosity")
    args = parser.parse_args()
    captures = args.capture
    
    #Create and configure the LOGGER
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(funcName)s - %(levelname)s - %(message)s')
    ch = logging.StreamHandler(sys.stdout)
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
        
    #Initialize the camera object
    c = Camera()
    
    #Read file located in /etc/PhotoNetwork/cam_settings
    c.read_cam_settings()
    
    #Apply camera settings to the camera
    c.camera_settings()
    
    #Send command to capture
    c.capture(captures)
    
