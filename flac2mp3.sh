#!/bin/bash
for f in "$@"; do
#	flac -d -c "$f" | lame -V 4 - "${f%%.flac}.mp3"
	ffmpeg -i "$f" -qscale:a 0 "${f[@]/%flac/mp3}"
done
