#!/bin/bash

if [ -z $1 ]; then
   echo ""
   echo "Usage: ./buildAndScreen.sh <device serial port path>"
   echo "Example: ./buildAndScreen.sh /dev/ttyACM0"
   exit 1
fi

DEV=$1
make program5 DEV=$1
success=$?

if [ $success -eq 0 ]; then
   sudo screen $DEV 115200
else
   echo "Make failed. Exiting with value $success"
   exit $success
fi
