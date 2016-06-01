#! /bin/sh

# jcl 2003-01-02
# Sample script to grab an aterm on another machine
# Needs for you to have a global $DISPLAY, whcich you 
#   will have if tcsh, or csh is your standard login shell.
# Usage
#  jclterm.sh box         open an aterm on box
#  jclterm.sh box c1 c2   open an aterm on box with bg=c1 fg=c2

# Count parameters and set colors
case "$#" in
	1) rfg=black
	   rbg=white ;;
	3) rfg=$3
	   rbg=$2    ;;
	*) echo "Expecting either 1 or 3 parameters. Found $#"
	   exit 1    ;;
esac	   

# Remote machine name
box=$1

# Establish display
setdisp=`echo "setenv DISPLAY $DISPLAY"`

# Open up terminal
setterm=`echo "aterm -bg $rbg -fg $rfg -title '$box'"`

# Pull it together
todo=`echo "$setdisp ; $setterm"`
# echo $todo

# Finish
rsh $box $todo &
