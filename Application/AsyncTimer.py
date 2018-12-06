#!/usr/bin/env python
"""AsyncTimer.py: Helper script which allows functions to run on a seperate 
                  thread on a specified interval

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"

"""
#********System Imports************
from threading import Timer

class Async_Timer():

    def __init__(self, t, hFunction):
        self.t = t
        self.hFunction = hFunction
        self.thread = Timer(self.t, self.handle_function)

    def handle_function(self):
        self.hFunction()
        self.thread = Timer(self.t, self.handle_function)
        self.thread.start()

    def start(self):
        self.thread.start()

    def cancel(self):
        self.thread.cancel()

