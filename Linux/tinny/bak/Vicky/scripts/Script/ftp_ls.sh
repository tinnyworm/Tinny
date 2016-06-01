#! /bin/sh
	echo "Welcome to john's handy ftp utility"
	server=$1
	user=$2
	listingfile=$3
	directory=$4

	echo "server      = " $server
	echo "user        = " $user
	echo "listingfile = " $listingfile
	echo "directory   = " $directory
	echo "  (that's the one we list)"

	ftp -n $server <<EOF
	    ascii
	    user $user
	    ls $directory $listingfile
	    quit
EOF