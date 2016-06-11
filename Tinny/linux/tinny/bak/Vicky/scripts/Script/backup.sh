#! /bin/sh
# Script to write dd list backups. 
# Expects 
#    $1 - Required dlt device number (1 for Ndlt.1, 2 for Ndlt.2)
#    $2 - Required tape number prefix for local .filename and .contents
#    $3 - Optional boxname identifies expected machone to run job. 
#         Script errors out if running on a different box. This check
#         is not run if the parameter is not provided. 
# johnlichtenstein@fairisaac.com 2002-07-30
#
	
	drive=/dev/Ndlt.$1
	tapenum=$2
	user=`whoami`
		
	case "$#" in 
	2)  : ;;                     # Two parameters provided - no box check
	3)  rightbox=$3              # Three parms - run box check
		boxname=`uname -n`
        case "$boxname" in
        $rightbox) 
            echo "Running on right box: $rightbox" ;;
        *)  echo "Script is not running on intended box."
            echo "   Running on: $boxname"
            echo "   Expecting : $rightbox"
            echo "   Exiting now before any damage is done to wrong tape"
            exit 2 ;;
        esac       ;;
	*)	echo "Invalid # of parameters. 2 or 3 required and $# provided."
		echo "head $0 for usage" 
		exit 3     ;;
	esac 

	errors=0
	writetotal=0
	readtotal=0
	starttime=`date`
	echo ""
	echo "$starttime   Starting  $0 for tape $2"
	echo "rewinding" 
	date
	echo "starting write"
	mt -t $drive rewind
	sleep 3
	/bin/dos2unix $tapenum.contents | dd bs=32768 of=$drive
	status=`echo "$?"`
	case "$status" in
	0)  : ;;                 # Normal status from dd
	*)  echo "Error writing $tapenum.contents" 
		echo "Status is $status."
		echo "Time is `date`." 
		echo "Exiting now."
		echo "This means that most likely:"
		echo "   1 - You mistyped the tapenum number or forgot to create"
		echo "       a .contents file"
		echo "   2 - The write lock is on on your tape. Or you specified a"
		echo "       different tape drive than you wanted." 
		echo "Write error $writetotal bytes into tape $tapenum on $boxname \
		with status of $status" | mail $user
		exit 3 ;;
	esac
	sleep 1  
	date
	
	for file in `/bin/dos2unix $tapenum.filelist`
	do
	    list=`/bin/ls -lnL $file`
	    disksize=`echo "$list" | awk '{print $5}`
     	writetotal=`echo "$writetotal $disksize +p" | dc`
	    stmp=`date`
	    echo "$stmp $writetotal $list"
		dd if=$file bs=32768 of=$drive
		status=`echo "$?"`
		case "$status" in
		0)  : ;;                 # Normal status from dd
		*)  echo "Error writing $file." 
			echo "Status is $status."
			echo "Time is `date`." 
			echo "Exiting now."
			echo "Write error $writetotal bytes into tape $tapenum on $boxname \
			with status of $status" | mail $user
			exit 4 ;;
		esac
	    sleep 2
    done

	echo "finishing write"
	echo "rewinding" 
	date
	mt -t $drive rewind
	sleep 10

	echo "starting verify"
	echo "verify toc"
	finalstat=0
	dd if=$drive bs=32768 of=delete.me.$tapenum
	sleep 1
	if diff -bl delete.me.$tapenum $tapenum.contents > /dev/null
	then
	    echo "TOC on tape same as TOC onfile for tape $tapenum"
	else
	    echo "TOC on tape differs from TOC onfile for tape $tapenum"
	    echo "Likely tape mixup"
	    echo "exiting now"
		echo "Error comparing contents to 1st dd for tape $tapenum" | mail $user
	    exit 5
	fi
	
	echo "verifying data"

	for file in `/bin/dos2unix $tapenum.filelist`
	do
	    archivesize=`dd bs=32768 if=$drive | wc -c`
	    listing=`/bin/ls -lnL $file`
	    disksize=`echo "$listing" | awk '{print $5}'`
	    delta=`expr $disksize = $archivesize`
     	readtotal=`echo "$archivesize $readtotal +p" | dc`
	    case "$delta" in
	    1) echo "cum: $readtotal - $file: disk size = archive size = $disksize" ;;
	    *) echo "cum: $readtotal - $file: sizes $disksize and $archivesize differ ERROR" 
	       finalstat=1
	       errors=`expr $errors + 1`
	       ;;
	    esac
	    sleep 2
	    done
	date
	mt -t $drive offl
	echo "number of verify errors $errors" 
	exit $finalstat




	exit

