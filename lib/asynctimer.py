#!/usr/bin/env python
"""AsyncTimer.py: Helper script which allows functions to run on a seperate 
                  thread on a specified interval

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"

"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
from threading import Timer

class AsyncTimer():
    
    def __init__(self, interval, function, *args, **kwargs):
        self._timer     = None
        self.interval   = interval
        self.function   = function
        self.args       = args
        self.kwargs     = kwargs
        self.is_running = False

    def _run(self):
        self.is_running = False
        self.start()
        self.function(*self.args, **self.kwargs)

    def start(self):
        if not self.is_running:
            self._timer = Timer(self.interval, self._run)
            self._timer.start()
            self.is_running = True

    def stop(self):
        self._timer.cancel()
        self.is_running = False


