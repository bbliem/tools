#!/bin/bash

if [ "$1" == "vga" ]; then
	xrandr --output LVDS1 --off --output VGA1 --auto
elif [ "$1" == "lvds" ]; then
	xrandr --output LVDS1 --auto --output VGA1 --off
elif [ "$1" == "mirror" ]; then
	xrandr --output LVDS1 --mode 1024x768 --output VGA1 --mode 1024x768
else
	echo "Usage: $0 vga | lvds | mirror"
	exit 1
fi
