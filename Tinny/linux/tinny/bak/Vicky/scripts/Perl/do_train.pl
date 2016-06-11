#! /usr/local/bin/perl 

################################################################
###quickly build scale file and train a model
#assumes only 1 model is specified in the MODEL_SET part in the *.cnf file
#Should be run from directory where model resides.
#Usage:
#  quicktrain.sh model.cnf general_segment_name calibration_table [-h]
#  ../quicktrain.pl Home2002 HI02_genSCal
### yxc on 200501
################################################################

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	Usage:	$0 modelName calibration_table [-h]

		Required:
			model.cnf files 
			
		Optional:
			-h shows this message
			-e<email address> default = blank
\n\n";

die "$USAGE" if ($#ARGV == -1);
#&Getopts('f:hF');
die "$USAGE" if ($opt_h);

############################################################
#	Initialize executable paths and setup
############################################################
#########>>>>>>>>>> Change starts here <<<<<<<<<############
### Working directory
#$indir="/work/ffp6/HomeImprove3.0/yxc/000_baseline/001_HI3.0_gen";
$indir="/work/ffp6/HomeImprove3.0/yxc/homeImpro3.0";
$modelDir="$indir/0_gen_oldRisk";
### previous model directory
$modName=$ARGV[0];
$calTab=$ARGV[1];
$client="homeDepotFdr";
$month="200309-200409";
$pct="10pct";
$seg="gen";

#########>>>>>>>>>> Change ends here <<<<<<<<<##############

##
$MODELCOPY="/work/price/falcon/bin/model_copy40.sh";

## Model Driver ###
$FALCON4="/work/gold/projects/bin/falconer4";

##  prepare a Rule Engine 3.0 model for scaling
$PREPMODEL="/work/gold/projects/bin/prep_model";

##  change FalconStyle (.cfg/.mode) to nnet style (.newcfg) for traing
$CFG2NN="/work/price/falcon/bin/cfg2nn";

##  NN driver (check new version)
$NNET="/work/price/falcon/bin/nnet-2.3.1";

##  based on nnet model file to generate .wta/.cta file
$NN2WTA="/work/price/falcon/bin/nn2wta";

##  Generage the Reason code
$REASBIN="/work/price/falcon/bin/reasbin3";

##  
$CAPCFG="/work/price/falcon/bin/capcfg40";

##  return the 1:1 calibration table
$INSERT="/work/price/falcon/bin/insert_calib4.pl";

#########################################
# PRINT START TIME AND CURRENT DIRECTORY
#########################################

printf "STARTED: "; system ( "date" );

print "==========================================================================\n";
printf "Directory: "; system ( "pwd" );

`setenv PYTHONPATH "/work/gold/projects/lib"`;  ### GET ERROR on setenv####
`setenv LD_LIBRARY_PATH "/usr/local/lib:/usr/openwin/lib:/usr/lib:/usr/lib/X11"`; 

##  Test the requirements###
## Test data directory
if (!(-d "$indir/data")) {
	print "ERROR:  No data directory. Will create one and link the data please!\n";
	mkdir ("$indir/data", 0755) or die "cannot mkdir $indir/data: $!";
	exit; 
} else {
   @dataFile=("auths.dat.gz", "cards.dat.gz", "frauds.dat.gz");
   foreach $file (@dataFile){
	    if (!(-e "$indir/data/$file")) {
	    	print "ERROR: NO actual data in data directory!! Please link the data into it.\n";
    	    }
    }
}
## Test select file directory
if (!(-d "$modelDir/select")) {
	print "ERROR: NO select directory!! Will create the select directory. Please create the select file into it.\n";
	mkdir ("$modelDir/select", 0755) or die "cannot mkdir $modelDir/select: $!";
}  else {
    @selFile=("muse/select.muse.$client.$month", "mgen_gen/select.mgen.$client.$month", "mgen_hi/select.mgen.$client.$month", );
    foreach $sele (@selFile){
	    if (!(-e "$modelDir/select/$pct/$sele")) {
		    print "ERROR: NO actual select file in select directory!! Please create the select file into it.\n";
    	     }
    }
}

if (!(-d "$modelDir/Log")) {
	mkdir ("$modelDir/Log", 0755) or die "cannot mkdir $modelDir/Log: $!";
}

if (!(-d "$modelDir/eval")) {
	mkdir ("$modelDir/eval", 0755) or die "cannot mkdir $modelDir/eval: $!";
}

#######################################################################
## Start now ####

#print "==========================================================================\n";
#print "Step 1: Copy the previous model files...\n\n"; 
#	system ("$MODELCOPY $moddir/$modName.cnf 2>$indir/error_all");
#print "\nDone for step 1\n\n";

print "==========================================================================\n";
print "Step 1: Start the scaling...\n\n"; 
	system ("cp $modName $modName.hold");
	system ("cp /home/yxc/models/Home2002.mod $modName.mod");

	## Comment off the reason code in this step ### 
	open (IN, "< $modName.hold ") || die "Could not open $modName.hold";
	open (OUT, " > $modName") || die "Could not write to $modName"; 
	  while (<IN>){
		$_ =~ s/^\s+IF\s+\(SCOR.SCORE/\/\/  IF (SCOR.SCORE/; 
		$_ =~ s/^\s+Get_Reasons/\/\/  Get_Reasons/;
		$_ =~ s/^\s+SCOR.REASON1/\/\/   SCOR.REASON1/;
		print OUT $_; 
	  }
	close(IN);
	close(OUT);

	print "Running prep_model $modName  "; system ( "date" );
	!system ("$PREPMODEL --scale $modName > $modelDir/Log/log_part1.prep_$modName 2>$modelDir/Log/error_part1.prep_$modName") || die "Failed on prep_$modName of step 2" ;
	$scl_opts="-z scl,nozerocalc,binary -d std_kchk,buildcard30 -R silent -t threshold=25";
	$drv_filter="-f noposts";
	print "Running scaling $modName "; system ( "date" );
	$selectFile="select.mgen.$client.$month";
	!system ( "$FALCON4 -idir=$indir/data/$pct/$client.$month,select=$modelDir/select/$pct/mgen_$seg/$selectFile $scl_opts $drv_filter $modName.cnf >$modelDir/Log/log_part1_2.scl_$modName 2>$modelDir/Log/error_part1.scl_$modName" ) || die "Failed on scaling 
of step 2";
print "\n Done for step 1\n\n";

print "==========================================================================\n";
print "Step 2: Start the training ..."; system ( "date" );
	print "\nRunning cfg2nn now...\n\n";
	####<<<<<<<<<<<< change parameters here  >>>>>>>>>>>#######
	!system ("$CFG2NN  -s 10 -n 8 -r 9167 -i 50 -e 1200 -m 0.8 $modName > $modelDir/Log/log_part2.cfg2nn_$modName 2>$modelDir/Log/error_part2.cfg2nn_$modName") || die "Failed on cfg2nn of step 3" ;
	system ("cp $modName.nncfg $modName.nncfg.orig");
	open (NNCFGI, "< $modName.nncfg.orig ") || die "Could not open $modName.nncfg.orig";
	open (NNCFGO, " > $modName.nncfg ") || die "Could not open $modName.nncfg";
	while (<NNCFGI>){
	   $_ =~ s/^\s+momentum\s+.*/    momentum .8/;
	   print NNCFGO $_#;
	}
	close(NNCFGI);
	close(NNCFGO);
	print "Running retrain model ...\n\n";
	system ("cp $modName $modName.hold_2");
	!system ("$NNET -c $modName.nncfg -o $modName.nnw -v -t backprop $modName.scl > $modelDir/Log/log_part2.train_$modName 2>$modelDir/Log/error_part2.train_$modName") || die "Failed on training of step 3" ;
	print "Running nn2wta now ...\n\n";
	!system ("$NN2WTA $modName.nnw $modName > $modelDir/Log/log_part2.nn2wta_$modName 2>$modelDir/Log/error_part2.nn2wta_$modName") || die "Failed on nn2wta of step 3" ;
print "\n Done for step 2\n\n";

print "==========================================================================\n";
print "Step 4: Prepare for evaluation new model...\n\n"; 
	system ("cp $modName $modName.hold");
	
	## Add back the reason code
	open (IN, "< $modName.hold ") || die "Could not open $modName.hold";
	open (OUT, " > $modName") || die "Could not write to $modName"; 
	  while (<IN>){
		$_ =~ s/\s+\/\/\s+IF\s+\(SCOR.SCORE/  IF (SCOR.SCORE/; 
		$_ =~ s/\s+\/\/\s+Get_Reasons/  Get_Reasons/;
		$_ =~ s/\s+\/\/\s+SCOR.REASON1/    SCOR.REASON1/;
		print OUT $_; 
	  }
	close(IN);
	close(OUT);
	print "Running reason3 now ...\n";
	!system ("$NNET -v -c $modName.nnw -o $modName.log $modName.scl > $modelDir/Log/log_part3.prepare_$modName 2>$modelDir/Log/error_part3.prepare_$modName") || die "Failed on Reasonbin3 / nnet of step 3" ;
	!system ("$REASBIN -m$modName > $modelDir/Log/log_part3.prepare.reasbin4_$modName 2>$modelDir/Log/error_part3.prepare_$modName") || die "Failed on reasonbin3 of step 3" ; 
	!system ("cp $modName.rrcd $modName.orig.rrcd");
	!system ("cp $modName.cfg $modName.orig.cfg");
	!system ("$CAPCFG -i$modName -o$modName -m3 -s6 > $modelDir/Log/log_part3.prepare_capcfg.$modName 2>$modelDir/Log/error_part3.capcfg_$modName") || die "Failed on reasonbin3 / capcfg40 of step 3" ;
	print "Running prep_eval now ... \n";
	!system ("$PREPMODEL --score --reason $modName > $modelDir/Log/log_part3.prepModel.prepare_$modName 2>$modelDir/Log/error_part3.prepModel_$modName") || die "Failed on prep_eval of step 3" ;
	
	### change varible list without reason code ###
	system ("cp $modelDir/nets/$calTab $modelDir/nets/$calTab.orig");
	!system ("$INSERT $modelDir/nets/$calTab") || die "Failed to modify the calibration file";
	
print "\nDone for step 4\n\n";


