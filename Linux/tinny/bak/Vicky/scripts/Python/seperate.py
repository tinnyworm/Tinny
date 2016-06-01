#! /usr/local/bin/python

import os, sys

authdata=os.popen("gunzip -c " +sys.argv[1], "r")
nodata  =os.popen("gzip > " + sys.argv[3], "w")
yesdata =os.popen("gzip > " + sys.argv[4],"w")
carddata=os.popen("gunzip -c "+sys.argv[2], "r")
carddict={}

while 1:
   cardline=carddata.readline()
   if not cardline:break
   cardid=cardline[:6]
   if not carddict.has_key(cardid):
      carddict[cardid]=1
   
while 1:
   nextline=authdata.readline()
   if not nextline:break
   authid=nextline[:6]
   if not carddict.has_key(authid): print >> nodata, nextline[:-1]
   else:
   	print >> yesdata, nextline[:-1]
      
      
      
      
