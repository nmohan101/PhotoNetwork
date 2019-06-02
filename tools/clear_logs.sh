#!/bin/bash

#clear_logs.sh: Remove all logs and database from system (Reset)
#__author__      = Nitin Mohan
#__copyright__   = Copy Right 2018. NM Technologies


#----------------------------------#
#           Constants              #
#----------------------------------#
LOG_PATH=/var/log/PhotoNetwork
DB_PATH=/srv/PhotoNetwork


#-----------------------------------#
#      Start of Program             #   
#-----------------------------------#

remove () {
    echo "Removing all files in ..... $1"
    sudo rm $1/*
}

#Check if script is being run as root
if ! id |grep -q root; then
    echo "Must be root user to install application"
    exit
fi

remove $LOG_PATH
remove $DB_PATH

