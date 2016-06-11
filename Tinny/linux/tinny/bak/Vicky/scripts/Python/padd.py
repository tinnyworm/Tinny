#! /usr/bin/env python
from sys import argv
import sys, string

padto = long(argv[1])

while 1:
	line = sys.stdin.readline() 
	if not line: break
	linelength=len(line)
	newline=line[:-1]
#	print linelength, line
	for i in range(linelength, padto):
		newline = newline + " "	
	print newline
	
sys.exit(0)
