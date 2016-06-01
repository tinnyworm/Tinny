#!/usr/local/bin/tcsh -f 

#This script copies all and only the files necessary to run a model into the 
#current working directory. Assumes external files are included only from
#the main model file specified from the MODEL_SET line in the cnf file.
#Directory structures will be copied as well.

#Usage:
#   prep_templates.sh <model cnf file> <segment name> <new model name> <new
#   segment name> [<destination dir>]
#

set GNUcp = cp;
if (`uname` == SunOS) set GNUcp = /usr/local/bin/cp

if ($#argv < 4) then
    echo "Copies all and only the necessary files and directories of a 4.0
	 model. Change model name to <new model name> and segment name to <new
	 segment name>."
    echo "Usage:"
    echo "   $0 <model cnf file> <segment name> <new model name> <new
	 segment name> [<destination dir>]"
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

set segment = $2
set basename = $3
set modmrule = $4

if ($#argv > 4) then
  set destpath = $5
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

# --- Create basename.cnf
echo "Cnf file $cnffile found in $srcpath, create template $basename.cnf..."
set target_cnf = $basename.cnf
echo "MODEL_SET = $modmrule" > $destpath/$target_cnf
echo "TEMPLATE_FILE = $basename.tmplt" >> $destpath/$target_cnf
echo "RULE_BASE_FILE = $basename.rul" >> $destpath/$target_cnf

# --- Grab model files, convert them to valid paths from current ---
# --- working dir and store name in the string "rel_models"      ---
echo "Copying segment file $segment and rename it to $modmrule"
set target_seg = $modmrule
cat $srcpath/$segment > $destpath/$target_seg
set rel_models = "$destpath/$target_seg"

# --- Grab the template file ---
set tmplt = `gawk -F"=" '/TEMPLATE_FILE/{print $2}' $srcpath/$cnffile`
echo "Copying template files $tmplt and rename to $basename.tmplt"
set target_tmplt = $basename.tmplt
cat $srcpath/$tmplt > $destpath/$target_tmplt
set rel_tmplt = $destpath/$target_tmplt

# --- Grab the rules file ---
set rul = `gawk -F"=" '/RULE_BASE_FILE/{print $2}' $srcpath/$cnffile`
echo "Copying rule base files $rul and rename to basename.rul"
set target_rul = $basename.rul
cat $srcpath/$rul | sed "s/$segment/$modmrule/g" > $destpath/$target_rul
set rel_rul = $destpath/$target_rul

# --- Compile list of files included from model files --- 
set includes = `egrep -i '^ *INCLUDE' $rel_models $rel_tmplt $rel_rul | gawk '{print $2}' | sed 's/["\;]//g' | sort | uniq`

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

# --- Modify modmrule ---
echo "Replacing $segment to $modmrule in $modmrule ..."
set temp = temp
cat $destpath/$target_seg | sed "s/$segment/$modmrule/g" > $temp
mv $temp $destpath/$target_seg

echo "Done."