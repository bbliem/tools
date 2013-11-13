#!/bin/bash

if [ "$1" == "vga" ]; then
	xrandr --output LVDS1 --off --output VGA1 --auto
elif [ "$1" == "lvds" ]; then
	xrandr --output LVDS1 --auto --output VGA1 --off
elif [ "$1" == "mirror" ]; then
	xrandr --output LVDS1 --mode 1024x768 --output VGA1 --mode 1024x768
elif [ "$1" == "dual" ]; then
	xrandr --output LVDS1 --auto --output VGA1 --auto --above LVDS1
else
	echo "Usage: $0 vga | lvds | mirror | dual"
	exit 1
fi
