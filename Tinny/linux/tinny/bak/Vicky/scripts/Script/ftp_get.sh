#! /bin/sh
	echo "Welcome to john's handy ftp utility"
	server=$1
	user=$2
	requestfile=$3
	fromdir=$4

	echo "server         = " $server
	echo "user           = " $user
	echo "requestfile    = " $requestfile
	echo "from directory = " $fromdir

	cat $requestfile 

	ftp -n $server <<EOF
	    ascii
	    user $user
	    cd $fromdir
	    `cat $requestfile` 
	    quit
EOF