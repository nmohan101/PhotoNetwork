#!/bin/bash

#"""pnstop: Script to stop all Photo Network Related Processes. 

#__author__      = "Nitin Mohan"
#__copyright__   = "Copy Right 2018. NM Technologies"
#"""


PROCESS="server.py client.py"

for p in $PROCESS; do
    kill -9 $p
done
