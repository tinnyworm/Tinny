#! /usr/local/bin/bash

#quickly build scale file and train a 4.0 model
#assumes only 1 model is specified in the MODEL_SET part in the *.cnf file
#Should be run from directory where model resides.
#Usage:
#  quicktrain_yxc.sh [--scale] [--nnet] [--eval] <cnf file>

# Set some command line input for simple modified

selectmgen=$1
selectmuse=$2
dataroot=$3 
logdir=`echo "LOG"`
outdir=`echo "Evaluate"`
modname=`echo "Home2002"`

# Set the cfg2nn parameters
cfg2nn_s=$4
cfg2nn_n=$5
cfg2nn_r=$6

echo "Command line:"
echo $0 " " $selectmgen " " $selectmuse " "$dataroot " " $cfg2nn_s " " $cfg2nn_n " " $cfg2nn_r 
echo " "

# Set the utilites to use
CPModel=/work/price/falcon/bin/model_copy40.sh # copy model 
FALCONER4=/work/gold/projects/bin/falconer4 # Driver
prep_model=/work/gold/projects/bin/prep_model # prepare a Rule Engine 3.0 model for scaling
cfg2nn=/work/price/falcon/bin/cfg2nn        # change FalconStyle (.cfg/.mode) to nnet style (.newcfg) for traing
nnet=/work/price/falcon/bin/nnet-2.3.1      # NN driver (check new version)
nn2wta=/work/price/falcon/bin/nn2wta        # based on nnet model file to generate .wta/.cta file
REASBIN=/work/price/falcon/bin/reasbin3     # Generage the Reason code 
CAPCFG=/work/price/falcon/bin/capcfg40      # Get rid of the outlier effect for the model 
rmd=/home/yxc/scripts/Perl/rmdir.pl

# set the enviorment parameter
setenv PYTHONPATH "/work/gold/projects/lib"
setenv LD_LIBRARY_PATH "/usr/local/lib:/usr/openwin/lib:/usr/lib:/usr/lib/X11"

#Step1:  Link all the data of interest and copy the model from the model directory
echo "===================================="
echo "Copy the current model files...     "
$CPModel /work/price/falcon/models/HomeImprov_2.5/rel4.0/$modname.cnf
cp  ../test1/$modname.mod .
cp  ../test2/run/*sed sed/
echo "===================================="
echo $logdir " " $outdir

#Step2:   Scaling 
echo "===================================="
echo "Start part1 (SCALE) at `date`"
cp $modname $modname.hold
cat $modname.old | sed -f sed/noreason.sed > $modname  # remove the reason code temporally
echo "~~~~ Running prep_model $modname~~~"
$prep_model --scale $modname >& $logdir/log_part1.prep_model
echo "prep_model is done. Starting scaling..."
  scl_opts=`echo "-z scl,nozerocalc,binary -d std_kchk -R silent"`
  drv_filter=`echo "-f noposts"`
$FALCONER4 -i dir=$dataroot,select=$selectmgen $scl_opts $drv_filter \
$modname.cnf >& $logdir/log_part1.scl
echo "Done part1 at `date`"
echo "===================================="
echo " "

#Step3:   Training 
echo "===================================="
echo "Start part2 (TRAIN) at `date`"
echo "~~~~ Running cfg2nn now..."
$cfg2nn $modname -s=$cfg2nn_s -n=$cfg2nn_n -r=$cfg2nn_r > $logdir/log_part2.train
cat $modname.nncfg | sed 's/momentum .*/momentum .8/' > nncfg
mv nncfg $modname.nncfg
echo "~~~~ Running retrain model..."
echo "~~~~ Running retrain model..." >> $logdir/log_part2.train
$nnet -c $modname.nncfg -o $modname.nnw -v -t backprop $modname.scl > $logdir/log_part2_main.train
echo "~~~~ Running nn2wta..."
$nn2wta $modname.nnw $modname >> $logdir/log_part2_main.train
echo "Done part2 at `date`"
echo "===================================="
echo " "

#Step4: Preparation for evaluation    
echo "===================================="
echo "Start part3 (Preparation) at `date`"
cp $modname $modname.hold
cat $modname.hold | sed -f sed/addreason.sed > $modname
echo "~~~~  Running reasbin3..."
$nnet -v -c $modname.nnw -o $modname.log $modname.scl > $logdir/log_part3.prepare
$REASBIN -m$modname >> $logdir/log_part3.prepare
echo "~~~~  Running capcfg40..."
cp $modname.rrcd $modname.orig.rrcd
cp $modname.cfg $modname.orig.cfg
$CAPCFG -i$modname.orig -o$modname -m3 -s6 >> $logdir/log_part3.prepare
echo "~~~~  Running prep_eval..."
$prep_model --score --reason $modname >> $logdir/log_part3.prepare

### Change net variable list without reason code
mv nets/usd176n4 nets/usd176n4.orig
cat nets/usd176n4.orig | sed -f sed/noreasonsinnets.sed > nets/usd176n4
mv nets/usd176n4 nets/usd176n4.orig2
cat nets/usd176n4.orig2 |sed -f sed/agree_mxs.sed > nets/usd176n4
echo "Done part3 at `date`"
echo "===================================="
echo " "

#Step5:  Calibration
#echo "Start part4 (CALIBRATION) at `date`"
#$run_part4.calib >&! log.run_part4.calib
#echo "Done part4 at `date`"
#echo " "

#Step6:   Evalutaion  
echo "===================================="
echo "Start part5 (Evaluation) at `date`"
eval_opts=`echo  "-d std_kchk -R silent -t test"`
drv_filter=`echo  "-f noposts"`

for dataset in `echo $dataroot`
do
  #ls -l $dataset/*gz
  filenm=`echo $dataset | $rmd`
  $FALCONER4 -i dir=$dataset,select=$selectmuse \
     $eval_opts $drv_filter \
    -r file=./$outdir/res.$modname.$filenm,full \
    -b ./$outdir/bigscr.$modname.$filenm.gz \
    -s ./$outdir/shortscr.$modname.$filenm.gz \
    $modname.cnf > $logdir/log_part5.evaluate.$filenm
done
echo "Done part5 at `date`"
echo "===================================="
echo " "

#echo "$modname based on $dataroot is done the retrain" | mail yaqingchen@fairisaac.com
