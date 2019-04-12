#!/bin/bash

#install.sh: Script to install the PhotoNetwork/Configure Application
#__author__      = Nitin Mohan
#__copyright__   = Copy Right 2018. NM Technologies


#----------------------------------#
#           Constants              #
#----------------------------------#
DEST_BASE_DIR=/opt/PhotoNetwork
DEST_APP_PATH=$DEST_BASE_DIR/Application
DEST_WEB_PATH=$DEST_BASE_DIR/FrontEnd
DEST_CONFIG_PATH=/etc/PhotoNetwork
SOURCE_APP_PATH=~/Documents/PhotoNetwork/Application
SOURCE_WEB_PATH=~/Documents/PhotoNetwork/FrontEnd
SOURCE_CONFIG_PATH=~/Documents/PhotoNetwork/Config
DEFAULT_OUT_DIR=/usr/PhotoNetwork


#-----------------------------------#
#      Start of Program             #   
#-----------------------------------#
USAGE="install.sh [-m MODE] [-h HELP]"

#Check if user passed in arguments
if [ ! $1 ]; then
    echo "NO Options provided. Must provide option"
    echo "USAGE: $USAGE"
    exit 1
fi


#Check if script is being run as root
if ! id |grep -q root; then
    echo "Must be root user to install application"
    exit
fi

while getopts ":m:h:" opt; do
    case $opt in
        m)
            MODE="$OPTARG"
            ;;
        h)
            echo "Enter server to setup device as server; Enter client to setup device as client"
            echo "USAGE: $USAGE"
            ;;
        \?)
            echo "Invalid option: $OPTARG"
            echo "USAGE: $USAGE"
            exit 1
            ;;
    esac
done

copy () {
    for file in $1/*.*; do
        filename=${file##*/}
        if [ -f $file ]; then
            if ! cmp -s $file $2/$filename; then
                echo "Copying ..... $filename"
                rsync -a $file $2/
            else
                echo "$2/$filename ........ upto date"
            fi
        fi
    done
}


dir_copy () {
    for dir in $(find $SOURCE_WEB_PATH -type d); do
        folder=${dir#"$SOURCE_WEB_PATH"}
        dest_path=$DEST_WEB_PATH$folder
        if  ! [ -d $dest_path  ]; then
            echo "Creating Directory $dest_path"
            mkdir $dest_path
            copy $dir $dest_path 
        else
            copy $dir $dest_path
        fi
    done
}

settings () {
    echo "Updating system mode to: $MODE"
    sed -i -e "s/"\"Network_Mode"\":.*/"\"Network_Mode"\":"\"$MODE"\",/g" $DEST_CONFIG_PATH/network_settings.json
}

#Copy/Update Application Files
if [ -d $DEST_BASE_DIR ]; then
    copy $SOURCE_APP_PATH $DEST_APP_PATH
else
    echo "Making directory: $DEST_BASE_DIR"
    mkdir $DEST_BASE_DIR
    copy $SOURCE_APP_PATH $DEST_APP_PATH
fi


#Copy/Update Front end files
if [ -d $DEST_WEB_PATH ]; then
    dir_copy
else
    echo "Making directory $DEST_WEB_PATH"
    mkdir $DEST_WEB_PATH
    dir_copy
fi


#Copy/Update Config Files
if [ -d $DEST_CONFIG_PATH ]; then
    copy $SOURCE_CONFIG_PATH $DEST_CONFIG_PATH
    settings
    chmod -R 777 $DEST_CONFIG_PATH
else
    echo "Making Directory $DEST_CONFIG_PATH"
    mkdir $DEST_CONFIG_PATH
    copy $SOURCE_CONFIG_PATH $DEST_CONFIG_PATH
    settings
    chmod -R 777 $DEST_CONFIG_PATH
fi

#Ensure output directory for captures is present (If not create it)
if ! [ -d $DEFAULT_OUT_DIR ]; then
    echo "Making Directory..... $DEFAULT_OUT_DIR"
    mkdir $DEFAULT_OUT_DIR
    chmod 777 $DEFAULT_OUT_DIR
fi
    
