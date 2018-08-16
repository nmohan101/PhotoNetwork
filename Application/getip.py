#!/usr/bin/env python

"""server.py: Application which local systems IP address

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technlogies"
"""

#********System Imports************
import socket
import re


def ip_is_local(self, ip_string):
    
    combined_regex = "(^10\.)|(^172\.1[6-9]\.)|(^172\.2[0-9]\.)|(^172\.3[0-1]\.)|(^192\.168\.)"
    return re.match(combined_regex, ip_string) is not None   
    
def get_local_ip(self):
    
    local_ips = [ x[4][0] for x in socket.getaddrinfo(socket.gethostname(), 80) if self.ip_is_local(x[4][0]) ]
    local_ip = local_ips[0] if len(local_ips) > 0 else None

    if not local_ip:
        temp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            temp_socket.connect(('8.8.8.8', 9))
            return temp_socket.getsockname()[0]
        except socket.error:
            return "127.0.0.1"
        finally:
            temp_socket.close()
