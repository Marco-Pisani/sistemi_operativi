#!/bin/bash

baudrate=19200



file=$(ls /dev | grep ACM)
file="/dev/$file"
./main $file $baudrate
