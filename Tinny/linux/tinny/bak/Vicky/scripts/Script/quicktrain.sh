#!/usr/local/bin/tcsh -f 

#quickly build scale file and train a 4.0 model
#assumes only 1 model is specified in the MODEL_SET part in the *.cnf file
#Should be run from directory where model resides.
#Usage:
#  quicktrain.sh [--scale] [--nnet] [--eval] <cnf file>

#set selectmgen = select.mgen
set selectmgen = ./select_mgen
set selectmuse = ../select_muse_101
set selectcount = ../../select_count
set dataroot = ../../data
set dataroot_eval = ../../data
set mode = mgen
#set datasetscale = `cat datalist.scale`
#set dataseteval = `cat datalist.eval`
# set datasetscale = `cat ./datalist`
# set dataseteval = `cat ./datalist`
# set datasetcount = `cat ./datalist`
set datafilescale = ./datalist
set datafileeval = ./datalist
set datafilecount = ./datalist
set outdir = out

set scl_opts = "-z scl,nozerocalc,binary -d std_kchk -R silent"
set eval_opts = "-z binary -d std_kchk -R silent -t test"
set count_opts = "-z binary -d std_kchk -R silent"
set drv_filter = "-f noposts"

#set driver = falconer4

#set driver = /work/gold/linux/bin/falconer4-1.1.3
#set driver = /work/gold/linux/bin/falconer4
#set driver = /work/price/falcon/SunOS/bin/falconer4
set driver = /work/fred/AEG/oxb/src/falconer4/falconer4

if (-d /opt/intel/compiler50/ia32/lib) then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/opt/intel/compiler50/ia32/lib
#  set nnet_engine = $UFALBIN/nnet-fast
   set nnet_engine = /work/iosdistrib/Linux/bin/nnet-2.3.1
else if (`uname` == "Linux") then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/home/mxs/tmp
#  set nnet_engine = $UFALBIN/nnet-fast
  set nnet_engine = /work/iosdistrib/Linux/bin/nnet-2.3.1
else
#  set nnet_engine = $UFALBIN/nnet
#  set nnet_engine = /work/FTDdistrib/solaris/bin/nnet-2.1.0
#  set nnet_engine = /work/FTDdistrib/solaris/bin/nnet-2.3.0
  set nnet_engine = /work/iosdistrib/SunOS/bin/nnet-2.3.1
#  set nnet_engine = /work/price/falcon/SunOS/bin/nnet-2.3.1

endif

set argc = $#argv
if ($argv[$argc] !~ *.cnf) then
  echo "Must specify .cnf file, exiting..."
  exit
endif

if ($argc == 1 && (-f $1)) then
  set scale = 1
  set nnet = 1
  set eval = 1
  set sens = 0
  set count = 0
else
  set scale = 0
  set nnet = 0
  set eval = 0
  set sens = 0
  set count = 0
  @ i = 1
  while ($i < $argc)
    switch ($argv[$i])
    case "--scale": 
      set scale = 1; @ i ++
      breaksw
    case "--nnet":
      set nnet = 1; @ i ++
      breaksw
    case "--eval":
      set eval = 1; @ i ++
      breaksw
    case "--count":
      set count = 1; @ i ++
      breaksw
    case "--sens":
      set sens = 1; @ i ++
      breaksw
    case "--hidol":
      set drv_filter = "${drv_filter},amountGE=250"; @ i ++
#      set selectmgen = ./select 
      breaksw
    case "--midol":
#      set drv_filter = "${drv_filter},amountGE=50,amountLT=150"; @ i ++
      set drv_filter = "${drv_filter},amountGE=100,amountLT=250"; @ i ++
#      set selectmgen = ./select
      breaksw
    case "--keyed":
      set drv_filter = "${drv_filter},keyed"; @ i ++
#      set selectmgen = ./select
      breaksw
    case "--swiped":
      set drv_filter = "${drv_filter},nokeyed"; @ i ++
#      set selectmgen = ../select_swiped
      breaksw
    case "--buck":
      set drv_filter = "${drv_filter},amountLT=1.01"; @ i ++
#      set selectmgen = ../select_buck
      breaksw
    case "--fp":
      set drv_filter = "${drv_filter} -p mp=70"; @ i ++
      breaksw
    case "--datascale":
      @ i ++
      set datafilescale = $argv[$i]; @ i ++
      breaksw
    case "--dataeval":
      @ i ++
      set datafileeval = $argv[$i]; @ i ++
      breaksw
    case "--datacount":
      @ i ++
      set datafilecount = $argv[$i]; @ i ++
      breaksw
    default:
      break
    endsw
  end
endif

#set datasetscale = `cat datalist.scale`
#set dataseteval = `cat datalist.eval`
if ($scale == 1) then
    set datasetscale = `cat ${datafilescale}`
endif
if ($eval == 1) then
    set dataseteval = `cat ${datafileeval}`
endif
if ($count == 1) then
    set datasetcount = `cat ${datafilecount}`
endif

set cnffile = $argv[$argc]

set model = `gawk -F"=" '/MODEL_SET/{print $2}' $cnffile`

# set drv_filter = "${drv_filter} -p mp=70"


if ($scale == 1) then

    set datasets
    foreach jjj ($datasetscale)
	set datadir = $dataroot/$jjj/
	set thisdat = "-idir=$datadir,select=$selectmgen/select.$mode.$jjj:t "
	set datasets = `echo $datasets $thisdat`
    end


  setenv PYTHONPATH /work/gold/projects/lib
  $MSGBIN/prep_model --scale $model

  echo === Scaling starting at `date` ===
  $driver $datasets $scl_opts $drv_filter -b $outdir/bScr.scale.gz $cnffile >&! $outdir/logs/log.scale
  #$driver $datasets $scl_opts $drv_filter $cnffile >&! $outdir/logs/log.scale
  echo === Scaling completed at `date` ===

  #Create nncfg file
#  set_mod.pl -n 12 -a 0.001 -m 0.9 -g 4 -e 100 >! $cnffile:r.mod
#  cfg2nn -s 10 -r 23 -i 20 $cnffile:r
#  patch -i lr.patch -o $cnffile:r.nncfg.lr $cnffile:r.nncfg
#  if ($status == 0) then
#     mv -f $cnffile:r.nncfg.lr $cnffile:r.nncfg
#  else
#     echo "WARNING: PATCH WAS NOT SUCCESSFULL\!\!\!"
#  endif


endif

if ($nnet == 1) then

  echo === Starting cfg2nn at `date` ===
#  cfg2nn -s 10 -r 35345342 -i 200 -e 200 -n 22 -l 0.0001 -m 0.98 -g 4 $cnffile:r
#  cfg2nn -s 10 -r 4556342 -i 200 -e 200 -n 14 -l 0.001 -m 0.8 -g 4 $cnffile:r
#  cfg2nn -s 10 -r 3325342 -i 200 -e 200 -n 18 -l 0.001 -m 0.8 -g 4 $cnffile:r
#  cfg2nn -s 10 -r 23 -i 200 -e 200 -n 18 -l 0.0001 -m 0.8 -g 4 $cnffile:r
#  cfg2nn -s 10 -r 324534523 -i 200 -e 200 -n 18 -l 0.0001 -m 0.9 -g 4 $cnffile:r

#  cfg2nn -s 10 -r 31253 -i 250 -e 250 -n 18 -l 0.0001 -m 0.9 -g 4 $cnffile:r
#  cfg2nn -s 10 -r 987433 -i 150 -e 150 -n 18 -l 0.0001 -m 0.9 -g 4 $cnffile:r

#  cfg2nn -s 10 -r 22333 -i 200 -e 200 -n 10 -l 0.00005 -m 0.9 -g 4 $cnffile:r

#  cfg2nn -s 10 -r 22333 -i 400 -e 400 -n 18 -l 0.00005 -m 0.9 -g 4 $cnffile:r

#  cfg2nn -s 10 -r  31253 -i 250 -e 250 -n 18 -l 0.0005 -m 0.9 -g 4 $cnffile:r
 
#used for hidollar and keyed
# cfg2nn -s 10 -r   987433 -i 296 -e 296 -n 18 -l 0.0005 -m 0.9 -g 4 $cnffile:r

#young general
#cfg2nn -s 10 -r   643532 -i 300 -e 300 -n 16 -l 0.001 -m 0.9 -g 4 $cnffile:r
#cfg2nn -s 10 -r   345532 -i 300 -e 300 -n 18 -l 0.001 -m 0.9 -g 4 $cnffile:r

cfg2nn -s 10 -r   345532 -i 600 -e 600 -n 18 -l 0.0005 -m 0.9 -g 4 $cnffile:r
#cfg2nn -s 10 -r   345532 -i 600 -e 600 -n 17 -l 0.0005 -m 0.9 -g 4 $cnffile:r

#  cfg2nn -s 10 -r 23 -i 30 -e 120 -n 14 -l 0.001 -m 0.8 -g 4 $cnffile:r
##  patch -i lr.patch -o $cnffile:r.nncfg.lr $cnffile:r.nncfg

  lr_patch.sh $cnffile:r.nncfg lrbak
  if ($status != 0) then
     mv -f $cnffile:r.nncfg.lrbak $cnffile:r.nncfg
     echo "WARNING: PATCH WAS NOT SUCCESSFULL\!\!\! reverting to original"
  endif 


  echo === cfg2nn complete at `date` ===


  echo === Starting Nnet at `date` ===
  $nnet_engine -v -t backprop -c $cnffile:r.nncfg -o $cnffile:r.nnw $cnffile:r.scl >&! $outdir/logs/log.nnet
  echo === Nnet completed at `date` ===

  nn2wta $cnffile:r.nnw $cnffile:r
  #cp $cnffile:r.wta $cnffile:r.wta.tmp
  $MSGBIN/prep_model --score $cnffile:r
endif

if ($eval == 1) then

  set mode = muse
  foreach jjj ($dataseteval)
    set resfile = $outdir/res.$mode.$jjj:t
    set bscrfile = $outdir/bScr.$mode.$jjj:t.gz
#    set datadir = $dataroot_eval/$jjj/
    set datadir = $dataroot/$jjj/
  
    echo "=== Starting $jjj at "`date` ===
    $driver \
         -i dir=$datadir,select=$selectmuse/select.$mode.$jjj:t\
       $eval_opts $drv_filter \
       -b $bscrfile -r file=$resfile $cnffile \
       >&! $outdir/logs/log.$mode.$jjj:t
    echo "=== Finished $jjj at "`date` ===
  end 
endif


if ($sens == 1) then
  echo === Starting Sensitivity analysis at `date` ===
  $nnet_engine -z $cnffile:r.sa -v -c $cnffile:r.nnw $cnffile:r.scl \
     >&! $outdir/logs/log.sense
  echo === Sensitivity analysis completed at `date` ===
endif



if ($count == 1) then

  set mode = count
  foreach jjj ($datasetcount)
    set resfile = $outdir/res.$mode.$jjj:t
    set bscrfile = $outdir/bScr.$mode.$jjj:t.gz
#    set datadir = $dataroot_eval/$jjj/
    set datadir = $dataroot/$jjj/
  
    echo "=== Starting $jjj at "`date` ===
    $driver \
         -i dir=$datadir,select=$selectcount/select.$mode.$jjj:t\
       $count_opts $drv_filter \
       -b $bscrfile -r file=$resfile $cnffile \
       >&! $outdir/logs/log.$mode.$jjj:t
    echo "=== Finished $jjj at "`date` ===
  end 
endif


