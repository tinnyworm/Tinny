#! /usr/local/bin/python
from sys import argv
from time import strftime, gmtime
import sys, getopt, os, Find

# Version: 
# 2002-10-02 jcl add casting to addition to prevent intermittent integer
#            overflow error

# warn of known bugs
def warn():
    print """
	    
WARNING: KNOWN BUGS: 1
    2-gig-file-bug:
    This script does not properly handle files larger than 2gig. 
    This is due to an error in python 2.1.1. 
    	os.path.getsize returns no value but an out of range error 
		os.path.exists  returns a 0
	The script works fine on python 2.2 on OSX, so I expect it to work 
	on python 2.2 for solaris as well. In the mean time PLEASE CHECK 
	reports.badlinks.txt. You should always check all the report.*.files 
	anyway. But the big files are listed here, since they "do not exist". 
	
        """ 
	
# built in help
def help(help_script_name):
    print """
        You are running: %s.

        Help is running because you specified a -h option, because the 
        word help appeared somewhere in your arguments, or because no args
        or options were specified and this script does not run that way. 

        This script is for building .filelist files for a multi-tape backup.

        It scans a directory structure for all non-empty files to which the 
        user (you) has read permission. It adds each filename to a .filelist
        file and adds the name, the filesize, and the cumulative amount of
        data to a .contents file. When the cumulative data reaches the limit

        of what fits on a tape, the .contents and .filelist are closed and new
        ones opened. 

        The .filelist files can then be used to drive a script that
        writes a backup (eg, dd backups such as
           /home/jcl/tools/backup.args.sh,
        or formal backups such as tar -I or gtar -T). The .contents files can 
        serve as contents listings of dd style backups. 

        usage: 
            %s 
            -m maxsize 
            -s startingtapeID 
            -d directorylist (file) 
            /directory/structure/to/back/up /another/one 
        options: 

        example:
            %s -m 32123456789 -s 900001041 /work/mo1/PRODUCTION
        """ %(help_script_name,help_script_name,help_script_name  )
    helps(help_script_name)
    helpm(help_script_name)
    helpd(help_script_name)
    sys.exit(0)
def helpm(help_script_name):
    print """

        --------------------------------
        The -m maxsize option is required.

        The -m maxsize option defines what's the largest cumulative amount
of
        data that can be written to a tape. For each file to be backed up, 
        its size is determined and a test value for the cumulative data in 
        the tape. If the test value exceeds this limit, the .filelist and 
        .contents files are closed, their names incremented, and the
cumulative
        value is reset to this filesixe while this file is added to the new
        .filelist and .contents. 

        Following are some examples 

        %s -m 12123456789 other options  # safe for a  DLT III 
        %s -m  7123456789 other options  # safe for an 8mm
        %s -m 32123456789 other options  # safe for a  DLT IV
        %s -m  1912345678 other options  # safe for a  3490E

        Note that at some point, it would be nifty to set this up so that 
        this option could be skipped if the user supplied a recognized arg
        such as DLT3, DLT4, ... and then the script looked up the safe value
        to use as the limit from an internal table. It would be nifty but 
        it's not the way it works now. 

        --------------------------------

        """ %(help_script_name,help_script_name,help_script_name,help_script_name )
def helps(help_script_name):
    print """

        --------------------------------
        The -s startingtape option is required.

        The -s startingtapeID option is used to provide the prefix for the
        names of the .filelist and .contents files. For tapes to be stored 
        in our library, the suggestion is to start with the next available
        number in the /work/gold/TAPELIST/TAPELIST.txt. For example, if 
        the last tapenumber in use is 912345678, use

        %s -s 912345679 ... other options

        The filelist and contents files will start with 912345679 and go up
        for each tape filled. Remember to note in 
        /work/gold/TAPELIST/TAPELIST.txt your plans to use those tapeid's
once
        you have determined them.

        If you do not want to store data in our library, and just want to 
        see how an archive would get split up, you can use any integer you
        like for the value for this option.  
        --------------------------------

        """ %(help_script_name)
def helpd(help_script_name):
    print """

        --------------------------------
        The -d directorylist option is optional. 

        The -d directorylist option is used to provide the name of a file
        that holds the list of the directories that are to be backed up.
        If this option is not provided, these must be provided as arguments
to 
        %s. Please do not provide both a -d directorylist and seperately 
        listed directories. Examples:

        Using -d:
            %s -m 12123456789 -s 100 -d myfiles.txt  
        Without -d:
            %s -m 12123456789 -s 100 /work/gold/projects /home/jcl
        --------------------------------

        """ %(help_script_name,help_script_name,help_script_name)

# check for any call for help
def findhelp(argv):
    import re
    for arg in argv:
        if re.search("/",arg):
            asksforhelp=0
        else: 
            if re.search("help", arg.lower()): asksforhelp=1
            else                             : asksforhelp=0
        if asksforhelp:
            help(argv[0])

# convert getopt.getopt options to dictionary
def list2dict(options):
    optsdict = {}
    for index in range(len(options)):
        optsdict[options[index][0]] = options[index][1]
    return optsdict


findhelp(argv)
# Look through options. Borrowed from 
# sf  /work/gold/projects/bin/prep_falcon
options = 's:hm:d:'
long_options = []
script_name = argv[0]
try:
    options_returned, args = getopt.getopt(sys.argv[1:], options,
long_options)
except getopt.error, error_values:
    print "--> %s: Invalid options/arguments: %s" %(script_name,error_values)
    print "--->    For help Issue %s" % (script_name)
    raise SystemExit    
print
myargs = list2dict(options_returned)
if myargs.has_key('-h'): help(argv[0])
if myargs.has_key('-s'):
    try:
        tapeid = long(myargs['-s'])
    except ValueError:
        helps(argv[0])
        print " "
        print "ERROR: Non integer argument %s supplied for -s option" %(myargs['-s'])
        print "       Integer argument required."
        sys.exit(2)
    print "startingtape= ", " ", tapeid
else: 
    helps(argv[0])
    sys.exit(1)
if myargs.has_key('-m'):
    try:
        maxtapesize = long(myargs['-m'])
    except ValueError:
        helps(argv[0])
        print " "
        print "ERROR: Non integer argument %s supplied for -m option" %(myargs['-m'])
        print "       Integer argument required."
        sys.exit(2)
    print "maxtapesize = ", " ", maxtapesize
else:
    helpm(argv[0])
    sys.exit(1)     
needsinputfile=0
if myargs.has_key('-d'):
#    print myargs['-d']
    if os.path.isfile(myargs['-d']):
        try: 
            directorylist=open(myargs['-d'],"r")
        except:
            helpd(argv[0])
            print " "
            print "ERROR: We were unable to open the directory list specified"
            print "      ", myargs['-d']
            sys.exit(2)         
        dirlist=[]
        while 1:
            directory=directorylist.readline()
            if not directory: break
            dirlength=len(directory) - 1
            if dirlength:
                directory=directory[:dirlength]
                dirlist=dirlist + [directory]
    else:
        helpd(argv[0])
        print " "
        print "ERROR: The directory list specified is not a file"
        print "      ", myargs['-d']
        sys.exit(2)               
else:
    if len(args) == 0:
        helpd(argv[0])
        print " "
        print "ERROR: If -d option is not supplied, then directories to"
        print "       back up must be listed on command line"
        sys.exit(2)
    else: dirlist = args
#
# Woo-woo. We made it through the options.
# Now we better check that our dirlist contains only directory names
#
for adir in dirlist:
    if os.path.isdir(adir):
        print "Preparing to list files from: ", adir
    else : 
        print """
            ERROR: One of the directories listed to backup does not appear
            to be a directory. Unable to locate %s
            """ %(adir)
#
#
tapeidname=str(tapeid)
tapeidcontents=tapeidname + ".contents"
tapeidfilelist=tapeidname + ".filelist"
badlinks=open("report.badlinks.txt","w")
lockouts=open("report.lockouts.txt","w")
mathlog=open("report.mathlog.txt","w")
tapecontents=open(tapeidcontents,"w")
tapefilelist=open(tapeidfilelist,"w")
tapesize = 0l
for directory in dirlist:
    filelist=Find.find("*",directory)
    print directory
    for filename in filelist:
        canread=os.access(filename, os.R_OK)
        isalink=os.path.islink(filename)
        isafile=os.path.isfile(filename)
        exists =os.path.exists(filename)
        if exists: filesize=os.path.getsize(filename) * canread * isafile
        if (exists ==0) :
            filesize = 0l
            print >> badlinks, filename          
        elif (canread==0) :
            print >> lockouts, filename
        elif (isafile and filesize): # we are going to add this file to archive
            nmtime  = os.path.getmtime(filename)
            gmmtime = gmtime(nmtime)
            mtime   = strftime("%Y-%m-%d", gmmtime)
            print >> mathlog, filename, tapesize, filesize, long(tapesize + long(filesize))
            testsize = long(tapesize + long(filesize))
            if testsize > maxtapesize : 
                print "testsize= ", testsize 
                print "finished with tape ", tapeid
                if filesize > maxtapesize:
                    print """
                          Oops. One of the files listed 
                              %s
                          is bigger than the max size for an archive, %s.
                          We don't split files accross tapes. Exiting now.
                          """ %(filename,maxtapesize)
                    sys.exit(4)
                tapesize = filesize
                tapeid   = tapeid + 1
                tapecontents.close()
                tapefilelist.close()
                tapeidname=str(tapeid)
                tapeidcontents=tapeidname + ".contents"
                tapeidfilelist=tapeidname + ".filelist"
                print "opening ", tapeidcontents
                tapecontents=open(tapeidcontents,"w")
                tapefilelist=open(tapeidfilelist,"w")
            else                      : tapesize = testsize
            print >> tapecontents, mtime, "\t", tapesize, "\t", filesize, "\t", filename
            print >> tapefilelist, filename
warn()
#
#print dirlist    
#print myargs
#print
#print options_returned
#print 
#print len(args)
#print
