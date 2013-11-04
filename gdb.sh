#!/bin/sh
binary=$1
shift
arguments=$@
#input=$(</dev/stdin)
inputfile=`mktemp`
cat /dev/stdin > $inputfile
commands=`mktemp`
#echo "set args $arguments <<< '$input'" > $commands
echo "set args $arguments < $inputfile" > $commands
# open /dev/tty as stdin
exec 0</dev/tty
gdb -x $commands $binary
rm $commands
rm $inputFile
