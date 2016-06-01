#!/usr/local/bin/tcsh -f 

#This script copies all and only the files necessary to run a model into the 
#current working directory. Assumes external files are included only from
#the main model file specified from the MODEL_SET line in the cnf file.
#Directory structures will be copied as well.

#It based on model_copy40.sh, added copy ISAM config files

#Usage:
#   model_copy40.sh <model cnf file> [<destination dir>]
#

set GNUcp = cp;
if (`uname` == SunOS) set GNUcp = /usr/local/bin/cp

if (0 == $#argv) then
    echo "Copies all and only the necessary files and directories of a 4.0 model."
    echo "Usage:"
    echo "   model_copy40.sh <model cnf file> [<destination dir>]"
    exit
endif

# --- set up source & destination paths --- 
if ($1 =~ */*) then 
  set srcpath = $1:h
else 
  set srcpath = .
endif

set cnffile = $1:t
if ($cnffile !~ *.cnf) set cnffile = $cnffile.cnf

if !(-f $srcpath/$cnffile) then
    echo "Can't find cnf file : $srcpath/$cnffile, exiting..."
    exit
endif

if (1 < $#argv) then
  set destpath = $2
  if !(-d $destpath && -w $destpath) then
    echo "Destination directory $destpath not found or unwriteable, exiting..."
    exit
  endif
else
  set destpath = .
endif

if ($srcpath == $destpath) then
  echo "Please explicitly specify another destination directory, exiting..."
  exit
endif

echo "Cnf file $cnffile found in $srcpath, copying..."
$GNUcp -fb $srcpath/$cnffile $destpath

# --- Grab model files, convert them to valid paths from current ---
# --- working dir and store name in the string "rel_models"      ---
set models = `gawk -F"=" '/MODEL_SET/{print $2}' $srcpath/$cnffile | sed 's/,/ /g'`
set rel_models
foreach jjj ($models)
  echo "Copying model file $jjj..."
  $GNUcp -fb $srcpath/$jjj $destpath
  set rel_models = "$rel_models $destpath/$jjj"
end

# --- Grab the template file and rules file ---
set tmplt_rul = `gawk -F"=" '/(TEMPLATE|RULE_BASE)_FILE/{print $2}' $srcpath/$cnffile`
echo "Copying template & rule base files $tmplt_rul..."
set rel_tmplt_rul
foreach jjj ($tmplt_rul)
    $GNUcp -fb $srcpath/$jjj $destpath
    set rel_tmplt_rul = "$rel_tmplt_rul $destpath/$jjj"
end

# --- Grab the ISAM config files ---
set isam_config = `gawk -F"=" '/ISAMCONFIGFILE/{print $2}' $srcpath/$cnffile`
echo "Copying ISAM config files $isam_config..."
foreach jjj ($isam_config)
    $GNUcp -fb $srcpath/$jjj $destpath
end

# --- Compile list of files included from model files --- 
set includes = `egrep -i '^ *INCLUDE' $rel_models $rel_tmplt_rul | gawk '{print $2}' | sed 's/["\;]//g' | sort | uniq`

# --- Copy include files creating subdirectories as necessary, ---
# --- assumes depth of the subdirectory structure is 1 at most ---
foreach jjj ($includes)
  if (($jjj =~ */*) && !(-d $destpath/$jjj:h)) then
    echo "Creating subdirectory $jjj:h..."
    mkdir $destpath/$jjj:h
  endif
  echo "Copying $jjj..."
  $GNUcp -fb $srcpath/$jjj $destpath/$jjj:h
end

echo "Done."
