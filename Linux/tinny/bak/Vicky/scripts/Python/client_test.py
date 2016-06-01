#! /usr/local/bin/python

from sys import argv
import os, sys, string

rawdata=os.popen("gunzip -c " +sys.argv[13], "r")
HK     =os.popen("gzip > " + sys.argv[1], "w")
CHINA  =os.popen("gzip > " + sys.argv[2], "w")
THAI   =os.popen("gzip > " + sys.argv[3], "w")
BRUNEI =os.popen("gzip > " + sys.argv[4], "w")
SRILAN =os.popen("gzip > " + sys.argv[5], "w")
INDON  =os.popen("gzip > " + sys.argv[6], "w")
INDIA  =os.popen("gzip > " + sys.argv[7], "w")
MACAU  =os.popen("gzip > " + sys.argv[8], "w")
PHILI  =os.popen("gzip > " + sys.argv[9], "w")
MALAY  =os.popen("gzip > " + sys.argv[10], "w")
SINGA  =os.popen("gzip > " + sys.argv[11], "w")
OTHER  =os.popen("gzip > " + sys.argv[12], "w")

while 1:
 #  nextline=sys.stdin.readline()
   nextline=rawdata.readline()
   if not nextline:break
   bin=nextline[:6]
   if  bin=="541180" or bin=="543122" or bin=="518542" or bin=="496604" or bin=="492111" or bin=="492184" or bin=="485600" or bin=="448404" or bin=="456079" or bin=="356200" or bin=="420184" or bin=="475514" or bin=="988388":  
           print >> HK, nextline[:-1]
   elif bin=="486401" or bin=="486402" or bin=="356210" or bin=="442314" or bin=="442315" or bin=="453993" or bin=="454889" or bin=="496613" or bin=="540018" or bin=="540806" or bin=="541939" or bin=="552037" or bin=="552268" or bin=="558427" or bin=="558803": 
           print >> HK, nextline[:-1]
   elif bin=="438669" or bin=="421180" or bin=="518831" or bin=="518847" or bin=="522216" or bin=="522217":
           print >> CHINA, nextline[:-1]    
   elif bin=="543673" or bin=="541690" or bin=="499959" or bin=="499960":
           print >> THAI,  nextline[:-1]	                              
   elif bin=="545245" or bin=="545244" or bin=="512049" or bin=="400214" or bin=="492294" or bin=="400213" or bin=="400215":
           print >> BRUNEI, nextline[:-1]
   elif bin=="542937" or bin=="542949" or bin=="517964" or bin=="491013" or bin=="491014":
	   print >> SRILAN, nextline[:-1]
   elif bin=="518494" or bin=="518535" or bin=="518323" or bin=="447211" or bin=="454493" or bin=="400934" or bin=="409675":
	   print >> INDON, nextline[:-1]
   elif bin=="542207" or bin=="540259" or bin=="554837" or bin=="554851" or bin=="512042" or bin=="517782" or bin=="541089" or bin=="517798" or bin=="517797" or bin=="517755" or bin=="456342" or bin=="447692" or  bin=="438459" or bin=="456719" or bin=="490900" or bin=="467191" or bin=="467190":
	   print >> INDIA,  nextline[:-1]
   elif bin=="544594" or bin=="544517" or bin=="518305" or bin=="518325" or bin=="490867" or bin=="490866" or bin=="475515" or bin=="475516":
	   print >> MACAU,  nextline[:-1]
   elif bin=="517790" or bin=="544757" or bin=="544758" or bin=="402892" or bin=="402893":
	   print >> PHILI,  nextline[:-1]
   elif bin=="510426" or bin=="512089" or bin=="518332" or bin=="540041" or bin=="541288" or bin=="541901" or bin=="453966" or bin=="492159" or bin=="438675":
	   print >> MALAY,  nextline[:-1]
   elif bin=="541287" or bin=="544234" or bin=="512043" or bin=="411902" or bin=="411903" or bin=="411908" or bin=="492160" or bin=="496645" or bin=="455227" or bin=="455228":
	   print >> SINGA,  nextline[:-1]
   else :  print >> OTHER,  nextline[:-1]	   	   
  
      
sys.exit(0)      
 
