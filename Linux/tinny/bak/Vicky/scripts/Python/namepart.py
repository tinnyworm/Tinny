#! /usr/local/bin/python
# Grab the name portion of a full path
# Full path on stdin
# File name on stdout
import os, stat, sys

while 1:
	infile   = sys.stdin
	fullpath = infile.readline()
	if not fullpath:
		break
	filename = os.path.split(fullpath)[1]
	print filename,
