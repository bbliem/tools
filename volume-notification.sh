#!/bin/sh

VOLUME=$(amixer sget Master | sed '/^ *Mono: /{s/^.*\[\(.*\)%\].*/\1/;p;};d;')
if [ $VOLUME -eq 0 ]; then
	ICON=notification-audio-volume-off
elif [ $VOLUME -lt 33 ]; then
	ICON=notification-audio-volume-low
elif [ $VOLUME -lt 66 ]; then
	ICON=notification-audio-volume-medium
else
	ICON=notification-audio-volume-high
fi
VOLUME="${VOLUME}%"

notify-send "Volume" -i $ICON -h int:value:$VOLUME -h string:synchronous:volume
