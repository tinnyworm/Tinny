#!/usr/local/bin/bash

function usage
{
	 echo "Usage: $0 [-n N] [-d printer] [-h] psfiles"
	 echo ""
	 echo "       -n number of pages per sheet (defualt 4)"
	 echo "       -d printer (defualt printer if not specified)"
	 echo "       -h help"
}

# default values
pps=4
printer=$PRINTER

while getopts ":n:d:h" opt; do
	case $opt in 
		n ) pps=$OPTARG ;;
		d ) printer=$OPTARG ;;
		h ) usage
  			  exit 0 ;;
	   \? ) usage
			  exit 1 ;;
	esac
done

#echo "pps=$pps"
#echo "printer=$printer"

shift $(($OPTIND -1))

if [ -z "$*" ]; then
	usage
	exit 1
fi

echo "print the following ps files into $pps pages per sheet"
for ps in $@
do
	echo $ps
done

cat $* > tmp_0.ps
ps2ps tmp_0.ps tmp_1.ps
psnup -$pps tmp_1.ps tmp_2.ps

lp -d $printer tmp_2.ps
