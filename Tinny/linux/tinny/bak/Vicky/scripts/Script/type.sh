#! /bin/sh

# Count parameters and set colors
case "$#" in
	2) ;;
	*) echo "Expecting 2 parameters. Found $#"
	   exit 1    ;;
esac

type=$1
base=$2

for file in `ls *$base*`
do
	mv $file $type.$file
done	   
