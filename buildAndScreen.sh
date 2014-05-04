#!/bin/bash

sudo make program3
success=$?

if [ $success -eq 0 ]; then
   sudo screen /dev/ttyACM0 115200
else
   echo "Make failed. Exiting"
   exit $success
fi

