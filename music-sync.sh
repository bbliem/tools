#!/bin/bash

musicdir='/mnt/data/bernhard/Musik'
syncdir='/mnt/data/bernhard/Synchronisierte Musik'
converter='/home/bernhard/tools/flac2mp3.sh'
artistFile='/mnt/data/bernhard/synced_artists'

readarray -t artists < $artistFile || exit 1

while getopts dv opt; do
	case $opt in
	d)
		dry="-d"
		;;
	v)
		verbose="-v"
		;;
	\?)
		# unknown option
		exit 1
		;;
	esac
done
shift $((OPTIND - 1))

cd "$musicdir"
for artist in "${artists[@]}"; do
	# Make directory for the artist
	find "$artist" -type d -exec mkdir -p "$syncdir/{}" \;

	# Make hard link for all .mp3 and .jpg files
	find "$artist" -type f -regex '.*\.\(mp3\|jpg\)$' -exec ln "{}" "$syncdir/{}" \; 2> /dev/null # suppress errors (to silently do nothing when the target already exists)
done

# Convert FLACs to MP3
$converter $dry $verbose -t "${syncdir}" "${artists[@]}"
