#!/bin/bash
#for f in "$@"; do
##	flac -d -c "$f" | lame -V 4 - "${f%%.flac}.mp3"
#	ffmpeg -i "$f" -qscale:a 0 "${f[@]/%flac/mp3}"
#done

while getopts t:dv opt; do
	case $opt in
	t)
		# Option -t lets you specify a target directory in which the MP3 files shall be put.
		# The names of all files ending in .flac in the directory tree under any of this script's arguments will be prefixed with this target directory, and the .flac suffix will be changed to .mp3.
		# $targetdir will have a trailing slash if used!
		targetdir="$OPTARG/"
		;;
	d)
		dry=1
		;;
	v)
		verbose=1
		;;
	\?)
		# unknown option
		exit 1
		;;
	esac
done
shift $((OPTIND - 1))

export targetdir
export dry
export verbose
numProcesses=$(nproc)
#find "$@" -name "*.flac" | xargs -P $numProcesses -n 1 -I{} bash -c 'f="{}"; ffmpeg -i "$f" -qscale:a 0 "${f[@]/%flac/mp3}"'
find "$@" -name "*.flac" -print0 | sed 's/"/\\"/g' | sed "s/'/\'/g" | xargs -P $numProcesses -n 1 -I{} -0 bash -c '
	f="{}"
	outfile="$targetdir${f[@]/%flac/mp3}"
	if [ -f "$outfile" ]; then
		[ -z $verbose ] || echo "Not converting $f (MP3 already exists)"
	else
		echo Converting $f
		[ $dry ] || ffmpeg -v warning -i "$f" -qscale:a 0 "$outfile" > /dev/null
	fi
	'
