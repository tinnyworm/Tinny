#! /usr/local/bin/perl 

################################################################
###quickly build scale file and train a model
#assumes only 1 model is specified in the MODEL_SET part in the *.cnf file
#Should be run from directory where model resides.
#
### yxc created on 200501
### yxc modified on 200604
### yxc modified on 200704
################################################################

$DEBUG = 0;
$VERSION = '6.0'; 
$USAGE = << "xxxEOFxxx";

###############################################################################1#################
#Product:
#Do the quick train by modifying the necessary text in some files, like
#mode or nets files. Allow the multiple scaling.  
#
#Module:
# $0 -m <modelName> -k <calibration_table> -w <workDirectory> -s
# <SegmentName> -d <data dir> -l <select dir> -g <client list for scal> -n <number
# of nodes> -e <number of epches> -v <client list for eval>
# [-h]
#
#Required:
# -m: model name
# -w: working directory
# -d: data directory (need to set up like data/usbank.200501-200610.1pct)
# -l: select directory (need to set up like
# select/{muse|mcount|mgen|rgen}/select.muse.usbank.200501-200610.1pct)
# -s: switch on hi segment train. Eg: -s 230. 230 is the threshhold. 
# -g: Client list for scaling. Format is 'usbank.200501-200610.1pct'.
# -v: Client list for evaluation. Format is 'usbank.200501-200610.1pct'.
# OR use a configuration file
# -o option.inputCfg
# 
#Optional:
# -h: shows this message
# -n: number of nodes. Default is 10.
# -e: number of epches. Default is 1000.
# -t: stepsize. Default is 10
# -i: allowable number of RMS increase epochs. Default is 20.
# -r: random seed. Default is 9167. 
# -b: mometum term (beta). Default is 0.1.
# -a: learning rate (alpha). Default is 0.001. 
# -c: learning acceleration rate. Default is 1. 
# -f: learning deceleration rate. Default is 1.
# -q: gain. Default is 4.  
# -p: switch to multiple scaling. 
# -x: down-sampling for the combined scale file.
# -j: down-sampling for the desired clients on some special concerns. Such
# as down-sample the MOTO fraud. 
#
#Example:
# $0 -m genAUS50 -w `pwd` 
# -d /work/minolta/GR_3.0/data -l /work/minolta/GR_3.0/select -g
# client_scl_input -v client_eval
#
#History:
# 1) Add a function in order to skip scaling part if it is already there.
# 2) Add an option to allow the user to adjust the number of epches. (To be
# continued... can extend to a config file for NNET)
# 3) Add an option to allow the mgen and muse have different clients.
# 4) Add a function to skip each step if the result is already there.
# 5) Fix one bug to not skip capcfg!
# 6) Add a linux tool box. 
# 7) Add an option for nnet about disable traning/test split "-s disable" 
# 8) Re-org the skip rule. Below is the tagger file for filing rule.
# Step 1  :modName.scl     - skip prepare model and actual scaling
# Step 1-2:modName.tag.org - skip 3-way splitting
# Step 2  :modName.nncfg   - skip cfg2nn part
# Step 2  :modName.nnw     - skip nnet train
# Step 2  :modName.wta     - skip nn2wta 
# Step 2  :modName.hold_1  - skip modifying model file for reasonBin
# Step 2  :modName.log     - skip nnet scoring part
# Step 2  :modName.plot    - skip reasbin3
# Step 2  :modName.orig.cfg- skip capping (capcfg)
# Step 2  :nameMap.*       - skip prepare for scoring
# Step 3  :modName.hold_2  - skip modifying model file for evaluation
# Step 3  :nets/netsNm.hold- skip modifying nets file for evaluation
# Step 3  :sens/modName.mse- skip sensitivity analysis
# Step 4  :eval/res.test...- skip evaluation on test
# Step 4  :eval/res.train..- skip evaluation on train
#		
# 9) Remove the -s disable option for this version. 
# 10) Found one bug for 3 way split for not-down-sample scale!
# 11) Rearrange the order for 3way training and add "unix part" for Linux.
# 12) Change to another script for cfg2nn.pl
# 13) Add endian_switch for Linux script
# 14) Add an option for down-sampling scale!
# 15) Remove the intermediate scale and bigScore.3wayscale files to save
# space. 
# 16) Add a function to send me an email when each step is done.
# 17) Add an option to skip the steps.  
################################################################################
#######
xxxEOFxxx
use Getopt::Std;

#####################################################
# To process the options	
#####################################################
 
die "$USAGE" if ($#ARGV == -1); 
&getopts('a:b:c:d:e:f:g:i:j:k:l:m:n:o:q:r:s:t:v:w:z:x:hp', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

if (defined $opts{'o'})
{
	 open (CFG, "< $opts{'o'}") || die "Could not open the config file $opts{'o'}\n"; 
	while (defined($line=<CFG>))
	{
		chomp; 
		$modName=$1 if ($line =~ /model_name\=(\w*)/); 
		$seg=$1 if ($line =~ /segment_genHi\=(\w*)/); 
		$amt_t = $1 if ($line =~ /seg_hi_thrhold\=(\d*)/ && $seg eq "hi"); 
		$seg1=$1 if ($line =~ /other_seg_opt\=(\w*)/);
		@tmp0=split "\=", $line if ($line =~ /work_dir\=/);
		$work=$tmp0[1];
	   chop($work); 	
		@tmp1=split "\=", $line if ($line =~ /inputdata_dir\=/);
		$data=$tmp1[1]; 
		chop($data); 
		@tmp2=split "\=", $line if ($line =~ /select_dir\=/);
		$select=$tmp2[1];
		chop($select); 
		@tmp3=split "\=", $line if ($line =~ /scalingclt_list\=/);
		$scalIN=$tmp3[1]; 
		chop($scalIN); 		
		@tmp4=split "\=", $line if ($line =~ /evalclt_list\=/);
		$evalIN=$tmp4[1];
		chop($evalIN); 
		$seg1=$seg if ($seg1 =~ /^\s*$/); 
		$node=$1 if ($line =~ /numnode\=(\d*)/);
		$epch=$1 if ($line =~ /numepch\=(\d*)/);
		$step=$1 if ($line =~ /stepsize\=(\d*)/);
		$incr=$1 if ($line =~ /numRMS\=(\d*)/);
		$seed=$1 if ($line =~ /rd_seed\=(\d*)/);
		$mome=$1 if ($line =~ /mom_beta\=(\d*\.\d*)/);
		$learnRt=$1 if ($line =~ /lnrate_alpha\=(\d*\.\d*)/);
		$acceRt=$1 if ($line =~ /lnaccrate\=(\d*\.\d*)/);
		$deceRt=$1 if ($line =~ /lndecrate\=(\d*\.\d*)/);
		$gain=$1 if ($line =~ /gain\=(\d*)/);
		
		$node=10 if ($node=~/^\s*$/);  
		$epch=1000 if ($epch=~/^\s*$/);
		$step=10 if ($step=~/^\s*$/);
		$incr=20 if ($incr=~/^\s*$/);
		$seed=9167 if ($seed=~/^\s*$/);
		$mome=0.1 if ($mome=~/^\s*$/);
		$learnRt=0.001 if ($learnRt=~/^\s*$/);
		$acceRt=1 if ($acceRt=~/^\s*$/);
		$deceRt=1 if ($deceRt=~/^\s*$/);
		$gain=4 if ($gain=~/^\s*$/); 
		@tmp5=split "\=", $line if ($line =~ /downsample\=/);
		$downsample=$tmp5[1];
		chop($downsample);
		@tmp6=split "\=", $line if ($line =~ /skip\=/); 
		chop($tmp6[1]);
		if (defined($tmp6[1]))
		{
			$lenStep= length $tmp6[1]; 
			for ($i = 0; $i<$lenStep; $i++)
			{
				$skipStep[$i]=substr($tmp6[1],$i,1); 
			}
		}
		
  }
	 
}
else 
{
	 die "ERROR: No model name specified. Quiting..." unless (defined $opts{'m'});
	 $modName = $opts{'m'}; 

	if (defined $opts{'s'}){
		 $seg = "hi"; 
		 $amt_t = $opts{'s'}; 
		warn "NOTE: -s specified: $amt_t. Switch to hi segment.\n";
	} else {
		$seg = "gen"; 
		warn "NOTE: -s not specified. Switch to gen segment.\n"; 
	}

	if (defined $opts{'z'}){
		$seg1 = $opts{'z'}; 
	} else {
		$seg1 = $seg; 
	}

	if (defined $opts{'j'}){
		$downsample = $opts{'j'}; 
	}
		
	if (defined("$opts{'w'}")){
		die "ERROR: Please specify the working directory.\n" unless (defined $opts{'w'});
		$work = $opts{'w'}; 
	} else {
		$work = "./"; 
		warn "NOTE: -w not specified. Use the default value of current directory.\n"; 
	}

	if (defined("$opts{'d'}")){
		die "ERROR: Please specify the data directory.\n" unless (defined $opts{'d'});
		$data = $opts{'d'}; 
	} else {
		$data = "./"; 
		warn "NOTE: -d not specified. Use the default value of current directory.\n"; 
	}

	if (defined("$opts{'l'}")){
		die "ERROR: Please specify the select directory.\n" unless (defined $opts{'l'});
		$select = $opts{'l'}; 
	} else {
		$select = "./"; 
		warn "NOTE: -l not specified. Use the default value of current directory.\n"; 
	}

	die "ERROR: Please specify the client name, coverage and sample rate text
file.\n" unless (defined $opts{'g'}); 
	$scalIN = $opts{'g'}; 

	if(defined $opts{'v'}){
		warn "NOTE: The $opts{'v'} is considered for evaluation.\n"; 
		$evalIN= $opts{'v'};
	} else {
		$evalIN= $opts{'g'}; 
		warn "NOTE: The $evalIN is considered for evaluation as same as trained
data\n";
	}

	$node=$opts{'n'} ? $opts{'n'} : 10 ;
	$epch=$opts{'e'} ? $opts{'e'} : 1000 ;
	$step= ($opts{'t'}) ? $opts{'t'} : 10; 
	$incr= ($opts{'i'}) ? $opts{'i'} : 20;
	$seed= ($opts{'r'}) ? $opts{'r'} : 9167;
	$mome= ($opts{'b'}) ? $opts{'b'} : 0.1;
	$learnRt= ($opts{'a'}) ? $opts{'a'} : 0.001;
	$acceRt= ($opts{'c'}) ? $opts{'c'} : 1;
	$deceRt= ($opts{'f'}) ? $opts{'f'} : 1;
	$gain= ($opts{'q'}) ? $opts{'q'} : 4;
}

warn "NOTE: model name is $modName. 
	NOTE: specified seg name is $seg.
	NOTE: specified seg amount for hi-dollar is $amt_t.
	NOTE: specified other seg name is $seg1.
	NOTE: working directory is $work.
	NOTE: input data dir is $data.
	NOTE: select file dir is $select.
	NOTE: client list for scaling is $scalIN.
	NOTE: client list for evaluation is $evalIN. 
	NOTE: number of nodes is $node.
	NOTE: number of epches is $epch.
	NOTE: stepsize is $step.
	NOTE: allowable number of RMS increase epochs is $incr.
	NOTE: random seed is $seed.
	NOTE: mometum term (beta) is $mome.
	NOTE: learning rate (alpha) is $learnRt.
	NOTE: learning acceleration rate is $acceRt.
	NOTE: learning deceleration rate is $deceRt.
	"; 
############################################################
# modeling tools 	
############################################################
$tooldir="/home/yxc/models";
$tmp = `uname -s`;
chomp $tmp;

##
$CREATE="$tooldir/create_dmwwts.sh";

##
$MODCONT="$tooldir/mod_content.pl";

##
$WAY3SPLIT="$tooldir/3waysplit.pl";

## 
$SCALFILTER = "/home/yxc/models/scale_filter.pl"; 

##  prepare a Rule Engine 3.0 model for scaling
$PREPMODEL="/work/gold/projects/bin/prep_model";

##  change FalconStyle (.cfg/.mode) to nnet style (.newcfg) for traing
#$CFG2NN="/work/price/falcon/bin/cfg2nn";
$CFG2NN="/home/yxc/models/cfg2nn_dxl.pl"; 
#$CFG2NN="/home/dxl/tools/perl/cfg2nn_dxl.pl"; 

## combine scal file
$COMSCAL="java -cp /home/mip/work/dev/java/projects/FalScale/build/classes com.fi.falcon.core.pcd.CombineScaleFiles "; 

## downsample the combined scale file
$SCAL_SAMP="/home/yxc/models/ds_scal.pl";

## set constant variables as NONE
$SET="/home/yxc/models/set_NONE_cfg.pl"; 

if ($tmp =~ /^Linux/){
$MODELCOPY="/work/price/falcon/Linux/bin/model_copy40.sh";

## Model Driver ###
$FALCON4="/work/gold/linux/bin/falconer4";

##  NN driver (check new version)
$NNET="/work/price/falcon/Linux/bin/nnet-2.3.3beta2";

##  based on nnet model file to generate .wta/.cta file
$NN2WTA="/work/price/falcon/Linux/bin/nn2wta";

##  return the 1:1 calibration table
$INSERT="/work/price/falcon/Linux/bin/insert_calib4.pl";

## Big2res tool
$BIG2RES = "/work/gold/linux/bin/big2res"; 

## column grep tool 
$COLGREP = "/work/price/falcon/Linux/bin/colgrep"; 

## hashtag split for bigScore
$HASHTEST = "/home/yxc/scripts/C/hashtagblind_yxc_Linux"; 

$SWITCH = "/home/yxc/models/endian_switch_4.Linux"; 

##  Generage the Reason code
$REASBIN="unix /work/price/falcon/bin/reasbin3";

##  
$CAPCFG="unix /work/price/falcon/bin/capcfg40";
} else {
## hashtag split for bigScore
$HASHTEST = "/home/yxc/scripts/C/hashtagblind_yxc_SunOS"; 

##
$MODELCOPY="/work/price/falcon/bin/model_copy40.sh";

## Big2res tool
$BIG2RES = "/work/gold/bin/big2res";

## column grep tool 
$COLGREP = "/work/price/falcon/bin/colgrep"; 

## Model Driver ###
$FALCON4="/work/gold/projects/bin/falconer4";

##  prepare a Rule Engine 3.0 model for scaling
$PREPMODEL="/work/gold/projects/bin/prep_model";

##  NN driver (check new version)
$NNET="/work/price/falcon/bin/nnet-2.3.3beta2";

##  based on nnet model file to generate .wta/.cta file
$NN2WTA="/work/price/falcon/bin/nn2wta";

##  return the 1:1 calibration table
$INSERT="/work/price/falcon/bin/insert_calib4.pl";

##  Generage the Reason code
$REASBIN="/work/price/falcon/bin/reasbin3";

##  
$CAPCFG="/work/price/falcon/bin/capcfg40";
}
#########################################
# PRINT START TIME AND CURRENT DIRECTORY
#########################################

print STDERR "STARTED: "; #system ( "date" );

print STDERR "==========================================================================\n";
print STDERR "Working directory: $work\nCurrent directory: "; #system ( "pwd" );

#`setenv PYTHONPATH "/work/gold/projects/lib"`;  ### GET ERROR on setenv####
#`setenv LD_LIBRARY_PATH "/usr/local/lib:/usr/openwin/lib:/usr/lib:/usr/lib/X11"`; 
############################################################
# directory setup, test and process the input options. 	
############################################################

open (OPTC, "< $scalIN") || die "Could not open file $scalIN ";
print STDERR "\nFor scaling only ...\n";
while (defined($line=<OPTC>)){
	 chop($line); 
	 $client_h{$line} = 1; 
	 print STDERR "SCAL: $line\n"; 
}
close(OPTC); 

open (OPTE, "< $evalIN") || die "Could not open file $evalIN";
print STDERR "\nFor evaluation only ...\n";
while (defined($line=<OPTE>)){
	 chop($line); 
	 $client_e{$line} = 1; 
	 print STDERR "EVAL: $line\n"; 
}
close(OPTE); 

# test data and select directory #
if (!(-d "$data")) {
	print STDERR "\nERROR:  No data directory. Will create one and link the data please!\n";
	mkdir ("$data", 0755) or die "cannot mkdir $data: $!";
	exit; 
} 

if (!(-d "$select")) {
	print STDERR "\nERROR: NO select directory!! Will create the select directory. Please create the select file into it.\n";
	mkdir ("$select", 0755) or die "cannot mkdir $select: $!";
}

if (!(-d "$work/log")) {
	mkdir ("$work/log", 0755) or die "cannot mkdir $work/log: $!";
}

if (!(-d "$work/eval")) {
	mkdir ("$work/eval", 0755) or die "cannot mkdir $work/eval: $!";
}

if (!(-d "$work/sens")) {
	mkdir ("$work/sens", 0755) or die "cannot mkdir $work/sens: $!";
}

if (!(-d "$work/script")) {
	mkdir ("$work/script", 0755) or die "cannot mkdir $work/script: $!";
}

if (!(-d "$work/scale")) {
	mkdir ("$work/scale", 0755) or die "cannot mkdir $work/scale: $!";
}

######### Check for each client for scaling ###########
print STDERR "\n"; 

foreach $clt (keys %client_h){
	 
	 ######### DATA directory check ###########
	 @dataFile=("auths.dat.gz", "cards.dat.gz", "frauds.dat.gz");

print STDERR "ACTUAL SCAL: $clt\n";
 
	if (!(-d "$data/$clt")) {
		  die "\nERROR: Scal Data directory do not exist. Please create one $data/$clt and link the data please!\n"; 
	 }
   foreach $file (@dataFile){
		 if (!(-e "$data/$clt/$file")) {
	    	die "\nERROR: NO actual data in scal data directory $data/$clt !! Please link the data into it.\n";
    	  }
	 }
	  ######### Select directory check ###########
	$sele="mgen";
	if (!(-d "$select/$sele")){
	  die "\nERROR: Select directories do not exist. Please create the directory: $select/$sele and copy the select files please!\n";
	}	  
	if (!(-e "$select/$sele/select.$sele.$clt.$seg1")) {
	  die "\nERROR: NO actual select file in $select/$sele/select.$sele.$clt.$seg1!! Please create the select file into it.\n";
   }
}
 
print STDERR "\n"; 



#######################################################################
## Start now ####

print STDERR "\n=====================================================================\n";

open (MAKE, "> $work/Makefile"); 

print MAKE "#
# Makefile for Falcon modeling
#
# Can be used to run jobs in parallel
#	make -j<njobs> -l<maxload> -f<Makefile of interest>
#		njobs: Allow N jobs at once; infinite jobs with no arg.
#		maxload: Don't start multiple jobs unless load is below N.
#
# See the help of make for detail information (using make --help)
#
# This Makefile is used to multiple scaling, down-sample, NNet training,
# reasbin generation, sensitivity analysis and final evaluation on both test
# and train set. 
#
#
\nSHELL = /bin/sh\n\n
"; 
$All = "\n" . 'all : ${ALL} ${SEN_SEN}' . "\n"; 

$i=0; 
$s1_sclDt_bfAll = "SCAL = "; 

foreach $clt (keys %client_h)
{
	@num = keys %client_h; 		  
	if ($i <	$#num)
	{
		$s1_sclDt_bfAll .= "\t" . $clt . "\\\n"; 
  	}
	else
	{
		$s1_sclDt_bfAll .= "\t" . $clt . "\n"; 
  }

  $i ++; 
}

### write individual sh for scaling	
if (defined($skipStep[0]))
{
## Skip the first step
	warn "NOTE: The step 1 is skipped!!\n\n"; 
}
else
{
$scl_opts="-z scl,nozerocalc,binary -R silent -t train";
if ($seg eq "hi"){	
  	$drv_filter="-f noposts,amountGE=$amt_t";
} else
{
  	$drv_filter="-f noposts";
}
   		
$option1 = "";
foreach $clt (keys %client_h){
  	$dataLink="-idir=$data/$clt,";
  	$selectFile="select=$select/mgen/select.mgen.$clt.$seg1 ";
  	$option1 = $dataLink . $selectFile;
 	
  	open (SCAL, ">$work/script/step1.$clt.sh"); 
	$out_scal = "#! /usr/local/bin/bash\n"; 
	$nclt_cnf = $clt; 
	$nclt_cnf =~ s/\./\_/g; 
	$out_scal .= "cp $modName.cnf $nclt_cnf.cnf;\n"; 
	$out_scal .= "$FALCON4 $option1 $scl_opts $drv_filter -b	$work/eval/bigScore.$clt.scale.3way.gz $nclt_cnf.cnf >$work/log/log_part1_2.scl_$clt 2>$work/log/error_part1.scl_$clt;\n";
	
	if ($tmp =~ /^Linux/)
	{
		 $out_scal .= "cat $nclt_cnf.scl | $SWITCH > $nclt_cnf.scl.new;\n"; 
		 $out_scal .= "/bin/rm $nclt_cnf.scl;\n";
		 $out_scal .= "/bin/mv $nclt_cnf.scl.new $nclt_cnf.scl;\n"; 	 
	}	 
	
	$out_scal .= "/bin/mv $nclt_cnf.cnf $nclt_cnf.scl $nclt_cnf.cfg $nclt_cnf.tag $work/scale;\n";

	print SCAL $out_scal; 
	close(SCAL); 
}

print STDERR "\nStep 1: Running prep_model $modName after modifying model file ";
	
$s1_scl_bfAll = "\n" . 'SCALALL = $(foreach a, ${SCAL}, ' . "$work/eval/bigScore.\$\{a\}.scale.3way.gz)\n"; 

$s1_scle0_afAll = "\n$work/log/log_part1.prep_$modName :\n";
$s1_scle0_afAll .="\tcd $work;\\\n\tcp $modName $modName.hold_0;\\\n";
$s1_scle0_afAll .="\t$MODCONT -m $modName -w $work -s;\\\n"; 
$s1_scle0_afAll .="\t$PREPMODEL --scale $modName >\$\@ 2>$work/log/error_part1.prep_$modName;\\\n";
$s1_scle0_afAll .="\t" . 'echo "Step1: Running prep_model' . $work . ' is done" | mail yaqingchen@fairisaac.com;' . "\n"; 
	
print STDERR "\nStep 1:Running scaling $modName\n"; 
$s1_scle1_afAll  = "\n$work/eval/bigScore.\%.scale.3way.gz : $work/log/log_part1.prep_$modName\n"; 	
$s1_scle1_afAll .= "\tsh $work/script/step1.\$\*.sh;\\\n";

$s1_scle2_afAll  ="\n" . '${FINALSCAL} : ${SCALALL} ';
print STDERR "\nStep 1: Check whether needs to be downsampled?\\\n";

$combine=""; 
$combineB=""; 
if ($downsample !~ /^\s*$/ && (-s "$downsample"))
{
	 print STDERR "\nYes. We need to downsample based on $downsample\n";
	$s1_scle2_afAll  .= ' ${DOWNSCALE}'; 
	
	open (DS, "< $downsample"); 

	while (<DS>)
	{
		chomp;
		@temp = split ":", $_; 
		$clt = $temp[0]; 
		$clt_ds{$clt}[0]=$temp[2]; 
		
		if ($temp[1] =~ /all/)
		{
			 $clt_ds{$clt}[1]=1; 
		}
		else 
		{
			$clt_ds{$clt}[1]=$temp[1]*1/100; 
		}
				
	}

	foreach $cl (keys %clt_ds)
	{
				$nclt_cnf = $cl; 
				$nclt_cnf =~ s/\./\_/g; 
				$combine = $combine . " $nclt_cnf";
				if ($clt_ds{$cl}[1] <1) 
		 {
				
				print STDERR "$cl  $clt_ds{$cl}[0]  $clt_ds{$cl}[1]\n";
				$downclt{$nclt_cnf}[0]=$clt_ds{$cl}[0]; 
				$downclt{$nclt_cnf}[1]=$clt_ds{$cl}[1];
				$downclt{$nclt_cnf}[2]=$cl; 
		  }
	}
	
	foreach $dclt (keys %downclt)
	{
		open (OUTD, "> $work/script/step1-2.$dclt.sh");  
		$cmd = "#! /usr/local/bin/bash\n";
		$cmd = "cd $work/scale;\n"; 
		$cmd .= "$WAY3SPLIT -t $dclt -b $work/eval/bigScore.$downclt{$dclt}[2].scale.3way.gz -p 25;\n"; 
		$cmd .= "$SCALFILTER -i $dclt -o sampled.$dclt -s $downclt{$dclt}[1] -f $downclt{$dclt}[0] -b $work/eval/bigScore.$downclt{$dclt}[2].scale.3way.gz 2> $work/scale/out.down.$downclt{$dclt}[2].stat;\n";
		$cmd .= "/bin/mv $dclt.scl $dclt.scl.orig;\n";
		$cmd .= "/bin/mv $dclt.tag $dclt.tag.orig;\n";
		$cmd .= "/bin/ln -s sampled.$dclt.scl $dclt.scl;\n";
		$cmd .= "/bin/ln -s sampled.$dclt.tag $dclt.tag;\n";
		print OUTD $cmd; 
		close(OUTD); 	
	}

	foreach $clns (keys %client_h)
	{
		  
		if (!defined($clt_ds{$clns}))
		{
			$clnst = $clns; 
			$clnst =~ s/\./\_/g;  
         $combine = $combine . " $clnst";
			open (OUTNS, ">$work/script/step1-2.$clt.sh"); 
			$out_3way = "#! /usr/local/bin/bash\n"; 
			$out_3way.= "cd $work/scale;\n"; 
			$out_3way.= "$WAY3SPLIT -t $clnst -b $work/eval/bigScore.$clns.scale.3way.gz -p 25;\n"; 
			print OUTNS $out_3way; 
			close(OUTNS); 
		}
	}
}
else 
{
	print STDERR "Step 1-2: No such downsample $downsample file exists. Skip this step!\n"; 
	print STDERR "\nStep 1-3: Start the 3-way splitting ...\n";
	
	foreach $cc (keys %client_h)
	{
		
		$nclt = $cc; 
		$nclt =~ s/\./\_/g; 
		$combine = $combine . " $nclt";
		$combineS= $combineS. " $work/scale/$nclt.scl"; 
		$combineB= $combineB. " $work/eval/bigScore.$cc.scale.3way.gz" ; 
	}
}

$s1_scl_bfAll .= "FINALSCAL = $work/$modName.tag\n";

$s1_scle2_afAll .="\n\tcd $work/scale;\\\n"; 
$s1_scle2_afAll .= "\t$COMSCAL --noKey -o $modName $combine > $work/log/log_part1_combine 2>$work/log/error_part1_combine;\\\n";
$s1_scle2_afAll .= "\tgzip -dc $combineB | gzip -c > $work/eval/bigScore.combined.scale.3way.gz;\\\n"; 
$s1_scle2_afAll .= "\t$WAY3SPLIT -t $modName -b $work/eval/bigScore.combined.scale.3way.gz -p 19;\\\n"; 
$s1_scle2_afAll .= "\t$SCAL_SAMP $modName;\\\n"; 
$s1_scle2_afAll .= "\tmv $modName.cfg $modName.cfg.orig0;\\\n"; 
$s1_scle2_afAll .= "\tcd $work;\\\n";
$s1_scle2_afAll .= "\tcat scale/$modName.cfg.orig0 | $SET > scale/$modName.cfg 2> $work/log/error_part1_resetCfg; \\\n"; 
$s1_scle2_afAll .= "\tln -s $work/scale/sampled_$modName.scl $modName.scl;\\\n"; 
$s1_scle2_afAll .= "\tln -s $work/scale/sampled_$modName.tag $modName.tag;\\\n";
$s1_scle2_afAll .= "\t/bin/cp $work/scale/$modName.cfg $modName.cfg;\\\n";
$s1_scle2_afAll .= "\t/bin/rm $combineB;\\\n";
$s1_scle2_afAll .= "\t/bin/rm $combineS;\\\n"; 
$s1_scle2_afAll .= "\t" . 'echo "Step1: Final merging scale files ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";
print STDERR "\n==========================Done for step 1-3==========================\n\n";
}
#######################################
## STEP 2 : Train
#######################################
if (defined($skipStep[1]))
{
	## Skip step2
	warn "NOTE: Step 2 for traiing is skipped!!\n\n"; 
}
else
{
print STDERR "Step 2: Start the training ..."; 
print STDERR "\nStep 2: Running cfg2nn now...\n";
print STDERR "Step 2: # of nodes is $node and # of epches is $epch\n";
print STDERR "Step 2: Running nnet ...\n"; 
print STDERR "Step 2: Running reasbin and capping\n";

$s2_trn_bfAll = "NN_NNW = $work/$modName.nnw\n"; 

if (defined($skipStep[0]))
{
	$s2_trn0_afAll  = "\n\$\{NN_NNW\} \:\n"; 
}
else
{
	 $s2_trn0_afAll  = "\n\$\{NN_NNW\} \: \$\{FINALSCAL\}\n";
}
$s2_trn0_afAll .= "\t$CFG2NN  -s=$step -n=$node -r=$seed -i=$incr -e=$epch -m=$mome -l=$learnRt -a=$acceRt -d=$deceRt -g=$gain $modName >$work/log/log_part2.cfg2nn_$modName	2>$work/log/error_part2.cfg2nn_$modName;\\\n";
$s2_trn0_afAll .= "\t$NNET -c $modName.nncfg -o $modName.nnw -v -t backprop -s disable $modName.scl > $work/log/log_part2.train_$modName	2>$work/log/error_part2.train_$modName;\\\n";
$s2_trn0_afAll .= "\t" . 'echo "Step2: NNet training ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";

$s2_trn_bfAll  .= "NN_WTA = $work/$modName.wta\n";

$s2_trn1_afAll  = "\n" . '${NN_WTA} : ${NN_NNW}';  
$s2_trn1_afAll .= "\n\tcp $modName $modName.hold_1;\\\n";
$s2_trn1_afAll .= "\t$NN2WTA $modName.nnw $modName > $work/log/log_part2.nn2wta_$modName 2>$work/log/error_part2.nn2wta_$modName;\\\n";
$s2_trn1_afAll .= "\t" . 'echo "Step2: NN_WTA ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";

$s2_trn_bfAll  .= "NN_LOG = $work/$modName.log\n";
$s2_trn2_afAll  = "\n" .'${NN_LOG} : ${NN_WTA}'; 
$s2_trn2_afAll .= "\n\t$MODCONT -m $modName -w $work -p;\\\n";
$s2_trn2_afAll .= "\t$NNET -v -c $modName.nnw -o $modName.log -a $modName.sa $modName.scl >	$work/log/log_part3.prepare_$modName	2>$work/log/error_part3.prepare_$modName;\\\n";
$s2_trn2_afAll .= "\t" . 'echo "Step2: NNET recall for log file ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";

$s2_trn_bfAll .= "NN_REAS = $work/$modName.plot\n";
$s2_trn3_afAll = "\n" . '${NN_REAS} : ${NN_LOG}'; 
$s2_trn3_afAll.= "\n\t$REASBIN -m$modName;\\\n";
$s2_trn3_afAll.= "\t" . 'echo "Step2: ReasonBin ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";

$s2_trn_bfAll .= "NN_CAP = $work/$modName.orig.cfg\n";
$s2_trn4_afAll = "\n" . '${NN_CAP} : ${NN_REAS}';
$s2_trn4_afAll.= "\n\tcp $modName.rrcd $modName.orig.rrcd;\\\n"; 
$s2_trn4_afAll.= "\tcp $modName.cfg $modName.orig.cfg;\\\n";
$s2_trn4_afAll.= "\t$CAPCFG -i$modName -o$modName -m3 -d6;\\\n";
$s2_trn4_afAll.= "\t" . 'echo "Step2: NN capping ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";


print STDERR "\n==========================Done for step 2============================\n\n";
}

if (defined ($skipStep[2]))
{
	 ## skip step3
	 warn "NOTE: Step3 for prepare_eval, SA is skipped!!\n\n"; 
}
else
{
print STDERR "Step 3: Running prep_eval now ... \n";
$fir3 = substr($modName,0,3); 

$s3_sen_bfAll  = "SEN_PRE = $work/nameMap.$fir3\n";

if (defined($skipStep[1]))
{
	$s3_sen0_afAll = "\n" . '${SEN_PRE} :'; 
}
else
{
	$s3_sen0_afAll = "\n" . '${SEN_PRE} : ${NN_CAP}'; 
}
$s3_sen0_afAll.= "\n\t$PREPMODEL --score --reason $modName;\\\n";

print STDERR "Step 3: Starting to do the sensitivity analysis\n\n";
$modName_new = $modName . $node;
$s3_sen_bfAll .= "SEN_SEN = $work/sens/$modName.mse\n"; 
$s3_sen1_afAll = "\n" . '${SEN_SEN} : ${SEN_PRE}';
$s3_sen1_afAll.= "\n\tcd $work/sens;\\\n"; 
$s3_sen1_afAll.= "\tln -s $work/$modName.scl .;\\\n"; 
$s3_sen1_afAll.= "\tln -s $work/$modName.cfg .;\\\n";
$s3_sen1_afAll.= "\tln -s $work/$modName.tag .;\\\n";
$s3_sen1_afAll.= "\tcp $work/$modName.nnw $work/sens/$modName_new.nnw;\\\n";
$s3_sen1_afAll.= "\t$CREATE $modName $node;\\\n";
$s3_sen1_afAll.= "\t" . 'echo "Step3: Prep_model and SA ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";
print STDERR "==========================Done for step 3============================\n\n";
}

if (defined($skipStep[3]))
{
	## skip the evaluation
	warn "NOTE: Step4 for evaluation is skipped!!\n\n";
}
else
{

foreach $cltE (keys %client_e){
	 
	 ######### DATA directory check ###########
	 @dataFile=("auths.dat.gz", "cards.dat.gz", "frauds.dat.gz");
		
print STDERR "ACTUAL EVAL: $cltE\n"; 

	 if (!(-d "$data/$cltE")) {
		  die "\nERROR: Eval Data directory do not exist. please create one $data/$cltE and link the data please!\n"; 
	 }
   foreach $file (@dataFile){
		 if (!(-e "$data/$cltE/$file")) {
	    	die "\nERROR: NO actual data in eval data directory $data/$cltE !! Please link the data into it.\n";
    	  }
	 }
	  ######### Select directory check ###########
	$sele="muse";
	if (!(-d "$select/$sele")){
	  die "\nERROR: Select directories do not exist. Please create the directory: $select/$sele and copy the select files please!\n";
	}	  
	if (!(-e "$select/$sele/select.$sele.$cltE")) {
	  die "\nERROR: NO actual select file in $select/$sele/select.$sele.$cltE!! Please create the select file into it.\n";
   }
}

print STDERR "Step 4: Evaluation for both test and train. Also for combined
all and non-M combined all\n";

$eval_opts = "-R silent -t test";
$eval_optsT= "-R silent -t threshold=25,train";
$option1 = ""; 

$s4_evalDt_bfAll = "\nEVAL = ";
$j=0; 
foreach $cle (keys %client_e)
{
	@num = keys %client_e; 		  
	if ($j <	$#num)
	{
		$s4_evalDt_bfAll .= "\t" . $cle . "\\\n"; 
  	}
	else
	{
		$s4_evalDt_bfAll .= "\t" . $cle . "\n\n\n"; 
   }
	  $j ++; 
	
	$dataLink="-idir=$data/$cle,";
	$selectFile="select=$select/muse/select.muse.$cle ";
	$option1 = $dataLink . $selectFile;

	open (EVAL, ">$work/script/step4.$cle.sh");
	$out_eval  = "#! /usr/local/bin/bash\n"; 
	$out_eval .= "$FALCON4 $option1 $eval_opts $drv_filter -r file=$work/eval/res.hold.$modName.$cle,full -b $work/eval/bigscr.hold.$modName.$cle.gz -s $work/eval/shortscr.hold.$modName.$cle.gz $modName.cnf > $work/log/log_part4.evaluate_hold.$modName.$cle 2>$work/log/error_part4.eval_hold.$modName.$cle;\n";
	print EVAL $out_eval; 
	close(EVAL); 
}

$s4_eval_bfAll = 'EVALALL = $(foreach a, ${EVAL}, ' . "$work/eval/res.hold.$modName.\$\{a\})"; 

if (defined($skipStep[2]))
{
	$s4_eval1_afAll  = "\n$work/eval/res.hold.$modName.\% :\n"; 
}
else
{
	$s4_eval1_afAll  = "\n$work/eval/res.hold.$modName.\% : \$\{SEN_PRE\}\n"; 	
}
$s4_eval1_afAll .= "\tsh $work/script/step4.\$\*.sh;\n";

$s4_final_bfAll = "\nALL = $work/eval/res.test.combineAll\n"; 
$s4_final_afAll = "\n" . '${ALL} : ${EVALALL}';
$s4_final_afAll.= "\n\tcd $work/eval/;\\\n"; 
$s4_final_afAll.= "\t/usr/local/bin/gzip -dc bigscr.hold.\*.gz | $BIG2RES -s 0.1 - $work/eval/res.hold.combineAll >$work/log/log_part4.evaluate.hold 2>$work/log/error_part4.eval.hold;\\\n"; 
$s4_final_afAll.= "\t" . 'echo "Step4: Final evaluation ' . $work . 'is done" | mail yaqingchen@fairisaac.com;' . "\n";
print STDERR "==========================Done for step 4============================\n\n";
}

###### Write the command in Makefile
print MAKE $s1_sclDt_bfAll if (!defined ($skipStep[0])); 
print MAKE $s4_evalDt_bfAll if (!defined ($skipStep[3]));
print MAKE $s1_scl_bfAll if (!defined ($skipStep[0])); 
print MAKE $s1_downO3_bfAll if (!defined ($skipStep[0])); 
print MAKE $s2_trn_bfAll if (!defined ($skipStep[1])); 
print MAKE $s3_sen_bfAll if (!defined ($skipStep[2])); 
print MAKE $s4_eval_bfAll if (!defined ($skipStep[3])); 
print MAKE $s4_final_bfAll if (!defined ($skipStep[3])); 
print MAKE $All if (!defined ($skipStep[3])); 
print MAKE $s1_scle0_afAll if (!defined ($skipStep[0])); 
print MAKE $s1_scle1_afAll if (!defined ($skipStep[0])) ; 
print MAKE $s1_downO3_afAll if (!defined ($skipStep[0])); 
print MAKE $s1_scle2_afAll if (!defined ($skipStep[0]));
print MAKE $s2_trn0_afAll if (!defined ($skipStep[1])); 
print MAKE $s2_trn1_afAll if (!defined ($skipStep[1])); 
print MAKE $s2_trn2_afAll if (!defined ($skipStep[1])); 
print MAKE $s2_trn3_afAll if (!defined ($skipStep[1])); 
print MAKE $s2_trn4_afAll if (!defined ($skipStep[1])); 
print MAKE $s3_sen0_afAll if (!defined ($skipStep[2])); 
print MAKE $s3_sen1_afAll if (!defined ($skipStep[2])); 
print MAKE $s4_eval1_afAll if (!defined ($skipStep[3])); 
print MAKE $s4_final_afAll if (!defined ($skipStep[3])); 

