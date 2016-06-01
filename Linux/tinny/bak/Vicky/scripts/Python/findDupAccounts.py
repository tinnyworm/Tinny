#! /usr/bin/env python
"""
Read one or more cardholder files to count and  identify account IDs that 
are potential duplicates. Check for TYPENOTE for notes on adding new types
of duplicates.  

Usage
   cat data | this-script prefix types (A, B, C)
   If the prefix isn't included, only the report will be generate.
   If the prefix is included, one or more types must be included to
   indicate which types of duplicates will be saved. 
Examples 
   counts only          : zcat cards.gz | findDupAccounts.py 
   save type A or B dups: zcat cards.gz | findDupAccounts.py saveprefix ab
   
Type A - More than one Open Date ([31:39]) for an account
Type B - More than one entry with a given record date ([90:98])
Type C - More than one entry with a given record month ([90:96])

Reads from stdin. Expects sorted data (needed for types B,C counts)

Writes
   Counts report of # of type A, B, C, A or B, and A or C found
   Optionally, a prefix.duplist.pickle file is saved
   
Revision 1.00 2004/06/21 12:00:00 jcl
"""
import os, sys, pickle
# First we'll parse the arg string and provide help if needed. 
# Look for the string BEGIN MAIN to see the body of the loop.
# Case out the sys.argv
#     len(sys.argv) = 1 -> No arguments. Running counts only.
#     len(sys.argv) = 2 -> One argument. Error. 
#     len(sys.argv) = 3 -> Saving results
#        sys.argv[1] is savefile prefix
#        sys.argv[2] is list of known dup types to save
# TYPENOTE must list all "known" type of duplicates here
knownDupTypes=("A","B","C")
saveFlag=0
if len(sys.argv) == 1:
   print >> sys.stderr, "%s Gathering counts only. No save file." %(sys.argv[0])
   dupTypes=()
elif len(sys.argv) == 2:
   print >> sys.stderr, __doc__
   print >> sys.stderr, "**********************"
   print >> sys.stderr, """%s running with one argument, %s. If %s is to 
save a list of duplicates to %s.duplicates.pickle, a list of which 
type of duplicates is to be saved (such as a or ab or ac).
      """ %(sys.argv[0], sys.argv[1], sys.argv[0], sys.argv[1])
   sys.exit(1)
elif len(sys.argv) == 3:
   print >> sys.stderr, "%s Saving type %s duplicates to %s" %(sys.argv[0], sys.argv[2], sys.argv[1])
   try: 
      saveFile=open(sys.argv[1] + ".duplicates.pickle", "w")   
      saveFlag=1
   except IOError:
      print >> sys.stderr, "Unable to open to write to a regular file %s.duplicates.pickle" %(sys.argv[1])
      sys.exit(2)
   # Check dups against known list
   # TYPENOTE Type names should not be lowercase.
   dupTypes=sys.argv[2].upper()
   for x in dupTypes:
      if not x in knownDupTypes:
         print >> sys.stderr, "Trying to save type %s duplicates, which are unknown. The known dupplicate types are: " %(x), knownDupTypes
         sys.exit(3)
# BEGIN MAIN processing
# Initialize counters, and a dictionary for each type of duplicate            
numAccts=0
openDateDict={}
typeADict={}
typeBDict={}
typeCDict={}
lastID=""
lastRecordDate=""   
# Read the card data from stdin.
for line in sys.stdin:
# Pull the accountID, openDate, and recordDate from the card data.
   account=line[:19]
   openDate=line[31:39]
   recordDate=line[90:98]
# TYPENOTE - Code to identify duplicates is in this loop, not
# in functions, to avoid overhead. Add any new duplicate types 
# here. 
   try:
      oldOpenDate=openDateDict[account]
      # Type A duplicates == change in open date.
      if not oldOpenDate == openDate:
         typeADict[account] = 1
   except KeyError:
      numAccts += 1
      openDateDict[account] = openDate

   if lastID == account:
      # Type B duplicate == 2 records with same record date
      if lastRecordDate == recordDate:
         typeBDict[account] = 1
      # Type C duplicate == 2 records with same record month
      if lastRecordDate[:-2] == recordDate[:-2]:
         typeCDict[account] = 1
#   TYPENOTE - A print like the following can help test a new dup-type
#   Remember to send output through more. 
#   print account, openDate, recordDate, lastRecordDate
   lastID = account
   lastRecordDate = recordDate
   
# TYPENOTE - For any new types, dictionaries to hold the union or intersection
# with other types for reporting should be added and updated here. Since 
# typeC is a subset of typeB, we never need to count the union or 
# intersection of B and C. B v C == C. B n C == B. 
#
# We use a union rule, that if an account is flagged as a dup by any of the 
# rules indicated by sys.argv[2], it's a dup. 
AorBDict={}
AorCDict={}
ABCDict ={}
saveDict={}
for x in typeADict:
   AorBDict[x] = 1
   AorCDict[x] = 1
   ABCDict [x] = 1
   if "A" in dupTypes:
      saveDict[x] = 1
for x in typeBDict:
   AorBDict[x] = 1
   ABCDict [x] = 1
   if "B" in dupTypes:
      saveDict[x] = 1
for x in typeCDict:
   AorCDict[x] = 1
   ABCDict [x] = 1
   if "C" in dupTypes:
      saveDict[x] = 1
   
   
print "Number of type A dups is %d" %(len(typeADict))
print "Number of type B dups is %d" %(len(typeBDict))
print "Number of type C dups is %d" %(len(typeCDict))
print "Number of type A or B is %d" %(len(AorBDict))
print "Number of type A or C is %d" %(len(AorCDict))
#print "Number of type A,B, or C is %d" %(len(ABCDict))
print "Number of duplicates saved is %d" %(len(saveDict))
print "Number of accounts       %d" %(numAccts)

pickle.dump(saveDict,saveFile)
