#!/bin/bash
#for f in "$@"; do
##	flac -d -c "$f" | lame -V 4 - "${f%%.flac}.mp3"
#	ffmpeg -i "$f" -qscale:a 0 "${f[@]/%flac/mp3}"
#done

numProcesses=$(nproc)
find "$@" -name "*.flac" | xargs -P $numProcesses -n 1 -I{} bash -c 'f="{}"; ffmpeg -i "$f" -qscale:a 0 "${f[@]/%flac/mp3}"'
