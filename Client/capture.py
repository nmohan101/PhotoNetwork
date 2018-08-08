# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 20:37:40 2018

@author: nmohan
"""

#*********SYSTEM IMPORTS*********
from picamera import PiCamera
import datetime

class Camera(object):
    
    def __init__(self):
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
        settings_list = []
        
        with open("/var/pn/cam_settings", "r") as f:
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
        self.camera.resolution = (self.res_width, self.res_height)
        self.camera.sharpness = self.sharpness
        self.camera.contrast = self.contrast
        self.camera.brightness = self.brightness
        self.camera.saturation = self.saturation
        self.camera.iso = self.iso
        self.camera.shutter_speed = self.shutter
        self.camera.flash_mode = self.flash
        

    def capture(self):
        time_stamp = datetime.datetime.now().strftime("%y-%m-%d_%H_%M_%S")
        self.camera.capture("{}/image_{}.jpg".format(self.capture_path, time_stamp))
        
        print "Imaged saved to %s"%self.capture_path

if __name__=="__main__":
    
    
    c = Camera()
    c.read_cam_settings()
    c.camera_settings()
    c.capture()
    