#!/bin/sh

n=5

while [ $n -ge 0 ]
	do 
	ln -s ../eval-10_$n-00/res.*
	n=`expr $n - 1`
	done
