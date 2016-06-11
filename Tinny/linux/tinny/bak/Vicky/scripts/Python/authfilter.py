#! /usr/local/bin/python

import os

authdata=os.popen("gunzip -c auths.dat.gz", "r")
nodata  =os.popen("gzip > noauths.gz", "w")
yesdata =os.popen("gzip > yesauths.gz","w")
carddata=os.popen("gunzip -c cards.dat.gz", "r")
carddict={}

while 1:
   cardline=carddata.readline()
   if not cardline:break
   cardid=cardline[:19]
   if not carddict.has_key(cardid):
      carddict[cardid]=1
   
while 1:
   nextline=authdata.readline()
   if not nextline:break
   authid=nextline[:19]
   if not carddict.has_key(authid): print >> nodata, nextline[:-1]
   else:
   	print >> yesdata, nextline[:-1]
      
      
      
      
