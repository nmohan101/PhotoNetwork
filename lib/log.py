#!/usr/bin/env python

"""log.py: This library faciliates the logging throughput the application. 

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import logging
import sys

#---------------------------------------------------#
#                   Constants                       #
#---------------------------------------------------#
LOG_PATH = "/var/log/PhotoNetwork/"

#---------------------------------------------------#
#                Start of Program                   #
#---------------------------------------------------#

class Log(object):

    def __init__(self, filename, verbosity=False):
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.DEBUG)
        self.filename = "%s.log"%filename.split("/")[-1].split(".")[0]
        formatter = logging.Formatter('%(asctime)s - %(funcName)s - %(levelname)s - %(message)s')
        ch = logging.StreamHandler(stream=sys.stdout)
        fh = logging.FileHandler("%s%s"%(LOG_PATH, self.filename))
        ch.setFormatter(formatter)
        fh.setFormatter(formatter)
        self._config(verbosity, ch, fh)

    def _config(self, verbosity, ch, fh):
        if verbosity:
            print "**** Running program in verbose mode *******"
            ch.setLevel(logging.DEBUG)
        else:
            ch.setLevel(logging.WARNING)
        
        self.logger.addHandler(ch)
        self.logger.addHandler(fh)

