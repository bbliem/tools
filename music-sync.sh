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

export syncdir
export dry
export verbose

cd "$musicdir"
for artist in "${artists[@]}"; do
	# Make directory for the artist
	find "$artist" -type d -print0 | sed 's/"/\\"/g' | sed "s/'/\'/g" | xargs -0 -I{} bash -c '
		dir="{}"
		if [ -d "$syncdir/$dir" ]; then
			[ -z $verbose ] || echo "Directory $dir already exists"
		else
			echo "Creating directory $dir"
			[ $dry ] || mkdir -p "$syncdir/$dir"
		fi
		'

	# Make hard link for all .mp3 and .jpg files
	find "$artist" -type f -regex '.*\.\(mp3\|jpg\)$' -print0 | sed 's/"/\\"/g' | sed "s/'/\'/g" | xargs -0 -I{} bash -c '
		file="{}"
		if [ -f "$syncdir/$file" ]; then
			[ -z $verbose ] || echo "File $file already exists"
		else
			echo "Linking $file"
			[ $dry ] || ln "$file" "$syncdir/$file"
		fi
		'
done

# Convert FLACs to MP3
$converter $dry $verbose -t "${syncdir}" "${artists[@]}"
