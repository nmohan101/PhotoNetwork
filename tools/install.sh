#!/bin/bash

#install.sh: Script to install the PhotoNetwork/Configure Application
#__author__      = Nitin Mohan
#__copyright__   = Copy Right 2018. NM Technologies


#----------------------------------#
#           Constants              #
#----------------------------------#
DEST_BASE_DIR=/opt/PhotoNetwork
DEST_APP_PATH=$DEST_BASE_DIR/bin
DEST_WEB_PATH=$DEST_BASE_DIR/FrontEnd
DEST_LIB_PATH=$DEST_BASE_DIR/lib
DEST_CONFIG_PATH=/etc/PhotoNetwork
SOURCE_APP_PATH=../bin
SOURCE_WEB_PATH=../FrontEnd
SOURCE_LIB_PATH=../lib
SOURCE_CONFIG_PATH=../config
DEFAULT_OUT_DIR=/usr/PhotoNetwork
DEST_DB_PATH=/srv/PhotoNetwork
DEST_LOG_PATH=/var/log/PhotoNetwork
GREEN='\e[32m'
NC='\e[0m'


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
                echo -e "${GREEN}Copying ..... $filename${NC}"
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


mkdir_photo (){

    if ! [ -d $1 ]; then
        echo "Making Directory....$1"
        mkdir $1
        chmod 777 $1
    fi
}


settings () {
    echo "Updating system mode to: $MODE"
    sed -i -e "s/"\"Network_Mode"\":.*/"\"Network_Mode"\":"\"$MODE"\",/g" $DEST_CONFIG_PATH/network_settings.json
}

#Copy/Update bin Files
if [ -d $DEST_BASE_DIR ]; then
    copy $SOURCE_APP_PATH $DEST_APP_PATH
else
    echo "Making directory: $DEST_BASE_DIR"
    mkdir $DEST_BASE_DIR
    copy $SOURCE_APP_PATH $DEST_APP_PATH
fi

#Copy/Update lib Files
if [ -d $DEST_BASE_DIR ]; then
    copy $SOURCE_LIB_PATH $DEST_LIB_PATH
else
    echo "Making directory: $DEST_BASE_DIR"
    mkdir $DEST_BASE_DIR
    copy $SOURCE_LIB_PATH $DEST_LIB_PATH
fi

#Copy/Update Front end files
if [ -d $DEST_WEB_PATH ]; then
    dir_copy
else
    echo "Making directory $DEST_WEB_PATH"
    mkdir $DEST_WEB_PATH
    dir_copy
fi


#Copy/Update config Files
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

#Ensure required output directories for logs, db and capture are present
mkdir_photo $DEFAULT_OUT_DIR
mkdir_photo $DEST_DB_PATH
mkdir_photo $DEST_LOG_PATH
 
echo "**************** PhotoNetwork Install Complete*************************"
