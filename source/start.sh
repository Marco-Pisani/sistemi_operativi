#!/bin/bash

baudrate=19200



file=$(ls /dev | grep ACM)
file="/dev/$file"
./main $file $baudrate

status=$?
if (($status != 0))
then
	echo ERROR
	exit -1
fi

gnuplot "output_files/plot_script"
xdg-open "output_files/image.png"
