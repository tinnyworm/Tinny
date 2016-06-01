#!/bin/csh -f

### Executables
setenv SYS `uname -s`

if ($SYS == "Linux") then
	set driver = /work/gold/linux/bin/falconer4
	echo "Evaluate under Linux"
	echo "using driver: $driver\n"
else 
	set driver = /work/gold/bin/falconer4
	echo "Evaluate under SunOS"
	echo "using driver: $driver\n"
endif 

set MODNAME = "uk2004fp"
set MODDIR = "/work/frauda2/FP_UK/segments/14-gen-144vars-3way/datetime-test/model"
set DATA_ROOT = "/work/frauda2/FP_UK/01_data"
set SELECT = "/work/frauda2/FP_UK/segments/14-gen-144vars-3way/select"

set DATA_NAME = "cap1"

### Output directory
set OUTDIR = "$cwd"

### Driver options
set eval_opts = "-R silent -p mp=70 -t threshold=25,test -d std_kchk,buildcard30"
#set eval_opts = "-R silent -d std_kchk,buildcard30"
set misc_opts = "-f noposts"

echo ""
echo "-----------------------------------------------------------------"
echo " evaluation "
echo "-----------------------------------------------------------------"
echo ""

echo "Change directory to $MODDIR"
cd $MODDIR

$driver \
	-i dir=$DATA_ROOT/cap1,select=$SELECT/select.muse.cap1 \
	 $eval_opts \
	$misc_opts \
    -n 30000 \
    -A "$OUTDIR/api.in.30k.gz" \
    -s "$OUTDIR/SCR.in.30k.gz" \
	$MODNAME.cnf
	
#$driver \
#	-i dir=$DATA_ROOT/hsbc_03,select=$SELECT/select.muse.hsbc_03 \
#	 $eval_opts \
#	$misc_opts \
#    -n 200000 \
#    -A "$OUTDIR/api.in.200k.gz" \
#    -s "$OUTDIR/SCR.in.200k.gz" \
#	$MODNAME.cnf
#
#$driver \
#	-a "$OUTDIR/api.in.30k.gz" \
#	-A "$OUTDIR/api.out.30k.gz" \
#	-s "$OUTDIR/SCR.out.30k.gz" \
#	$MODNAME.cnf
#
#$driver \
#	-a "$OUTDIR/api.in.200k.gz" \
#	-A "$OUTDIR/api.out.200k.gz" \
#	-s "$OUTDIR/SCR.out.200k.gz" \
#	$MODNAME.cnf
#
#$driver \
#	-a "$OUTDIR/api.out.30k.gz" \
#	-A "$OUTDIR/api.30k.gz" \
#	-s "$OUTDIR/SCR.30k.gz" \
#	$MODNAME.cnf
#
#$driver \
#	-a "$OUTDIR/api.out.200k.gz" \
#	-A "$OUTDIR/api.200k.gz" \
#	-s "$OUTDIR/SCR.200k.gz" \
#	$MODNAME.cnf
