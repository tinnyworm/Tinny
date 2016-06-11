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
	Usage:	$0 modelName calibration_table workDirectory SegmentName[-h]

		Required:
			modelName files Working_dir
			
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

### previous model directory
$modName=$ARGV[0];
$calTab=$ARGV[1];
$work=$ARGV[2];
$seg=$ARGV[3];

%client=("homeImproGE" => "200101-200112",
		   "homeDepotFdr"=> "200309-200409");
	  
$evalClient="homeDepotFdr";
$month=$client{$evalClient};

$pct="10pct";
$modelDir="$indir/$work"; 
#########>>>>>>>>>> Change ends here <<<<<<<<<##############

##
$MODELCOPY="/work/price/falcon/bin/model_copy40.sh";

##
$CREATE="$indir/script/create_dmwwts.sh";

##
$WAY3SPLIT="/home/yxc/models/3waysplit.pl";

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
	
    @selFile=("muse/select.muse", "mgen_gen/select.mgen", "mgen_hi/select.mgen");
    foreach $sele (@selFile){
    	foreach $clt (keys %client){
		$mth=$client{$clt};
	    if (!(-e "$modelDir/select/$pct/$sele.$clt.$mth")) {
		    print "ERROR: NO actual select file in select directory!! Please create the select file into it.\n";
    	     }
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

$cmd_0="cp $modName $modName.hold";
$cmd_1="cp /home/yxc/models/Home2002.mod $modName.mod";

	print "$cmd_0\n$cmd_1\n";
	unless ($DEBUG){
		`$cmd_0`;
		`$cmd_1`;
	}

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
	$cmd_2="$PREPMODEL --scale $modName > $modelDir/Log/log_part1.prep_$modName 2>$modelDir/Log/error_part1.prep_$modName";
		
	unless ($DEBUG){
		 !system ("$PREPMODEL --scale $modName > $modelDir/Log/log_part1.prep_$modName 2>$modelDir/Log/error_part1.prep_$modName") || die "Failed on prep_$modName of step 2" ;
	}
	
	$scl_opts="-z scl,nozerocalc,binary -d std_kchk,buildcard30 -R silent -t threshold=25,train";

	if ($seg eq "hi"){	
		$drv_filter="-f noposts,amountGE=230";
  	} elsif ($seg eq "gen"){
		$drv_filter="-f noposts";
  	} else {
		print "Warning!: Wrong segment Name. Please enter gen or hi. "
	}
	print "Running scaling $modName "; system ( "date" );
	
	$OPTALL="";
	foreach (keys %client){
		$clt=$_;
		$mth=$client{$_};
		$dataLink="-idir=$indir/data/$pct/$clt.$mth,";
		$selectFile="select=$modelDir/select/$pct/mgen_$seg/select.mgen.$clt.$mth ";
		$option1=$dataLink . $selectFile;
		$OPTALL=$OPTALL . $option1;
	}
	
	print "$FALCON4 $OPTALL $scl_opts $drv_filter -b eval/bigScore.scale.3way.gz $modName.cnf	>$modelDir/Log/log_part1_2.scl_$modName 2>$modelDir/Log/error_part1.scl_$modName\n" ;	
	unless ($DEBUG){
		 !system ("$FALCON4 $OPTALL $scl_opts $drv_filter -b eval/bigScore.scale.3way.gz $modName.cnf >$modelDir/Log/log_part1_2.scl_$modName 2>$modelDir/Log/error_part1.scl_$modName" ) || die "Failed on scaling of step 2";
	}
print "\n Done for step 1\n\n";


print "==========================================================================\n";
print "Step 1-2: Start the 3-way splitting ..."; system ( "date" );
	print "\nRunning 3waysplit now...\n\n";
	
	$cmdTemp="$WAY3SPLIT -t $modName -b eval/bigScore.scale.3way.gz -p 20";

	print "$cmdTemp\n\n";

	unless ($DEBUG){
	
		!system("$WAY3SPLIT -t $modName -b eval/bigScore.scale.3way.gz -p 20") || die "Failed on 3 way splitting of Step 1-2";
  }
print "\n Done for step 1-2\n\n";  

	
print "==========================================================================\n";
print "Step 2: Start the training ..."; system ( "date" );
	print "\nRunning cfg2nn now...\n\n";
	####<<<<<<<<<<<< change parameters here  >>>>>>>>>>>#######
	print "$CFG2NN  -s 10 -n 8 -r 9167 -i 50 -e 1200 -m 0.8 $modName	>$modelDir/Log/log_part2.cfg2nn_$modName	2>$modelDir/Log/error_part2.cfg2nn_$modName\n";

	unless ($DEBUG){	
		 !system ("$CFG2NN  -s 10 -n 8 -r 9167 -i 50 -e 1200 -m 0.8 $modName > $modelDir/Log/log_part2.cfg2nn_$modName 2>$modelDir/Log/error_part2.cfg2nn_$modName") || die "Failed on cfg2nn of step 3" ;
	}
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
	
	print "$NNET -c $modName.nncfg -o $modName.nnw -v -t backprop $modName.scl > $modelDir/Log/log_part2.train_$modName	2>$modelDir/Log/error_part2.train_$modName\n";

	unless ($DEBUG){
		 !system ("$NNET -c $modName.nncfg -o $modName.nnw -v -t backprop $modName.scl > $modelDir/Log/log_part2.train_$modName 2>$modelDir/Log/error_part2.train_$modName") || die "Failed on training of step 3" ;
	}
	print "Running nn2wta now ...\n\n";

	print "$NN2WTA $modName.nnw $modName >	$modelDir/Log/log_part2.nn2wta_$modName	2>$modelDir/Log/error_part2.nn2wta_$modName\n";

	unless ($DEBUG){
		 !system ("$NN2WTA $modName.nnw $modName > $modelDir/Log/log_part2.nn2wta_$modName 2>$modelDir/Log/error_part2.nn2wta_$modName") || die "Failed on nn2wta of step 3" ;
	}
print "\n Done for step 2\n\n";

print "==========================================================================\n";
print "Step 4: Prepare for evaluation new model...\n\n"; 
	system ("cp $modName $modName.hold");
	
	## Add back the reason code
	open (IN, "< $modName.hold ") || die "Could not open $modName.hold";
	open (OUT, " > $modName") || die "Could not write to $modName"; 
	  while (<IN>){
		$_ =~ s/\/\/\s+IF\s+\(SCOR.SCORE/  IF (SCOR.SCORE/; 
		$_ =~ s/\/\/\s+Get_Reasons/  Get_Reasons/;
		$_ =~ s/\/\/\s+SCOR.REASON1/    SCOR.REASON1/;
		print OUT $_; 
	  }
	close(IN);
	close(OUT);
	print "Running reason3 now ...\n";
	
	print "$NNET -v -c $modName.nnw -o $modName.log $modName.scl >	$modelDir/Log/log_part3.prepare_$modName	2>$modelDir/Log/error_part3.prepare_$modName\n";

	unless ($DEBUG){
		 !system ("$NNET -v -c $modName.nnw -o $modName.log $modName.scl > $modelDir/Log/log_part3.prepare_$modName 2>$modelDir/Log/error_part3.prepare_$modName") || die "Failed on Reasonbin3 / nnet of step 3" ;
	}

	print "$REASBIN -m$modName >	$modelDir/Log/log_part3.prepare.reasbin4_$modName	2>$modelDir/Log/error_part3.prepare_$modName\n";

	unless ($DEBUG){
		 !system ("$REASBIN -m$modName > $modelDir/Log/log_part3.prepare.reasbin4_$modName 2>$modelDir/Log/error_part3.prepare_$modName") || die "Failed on reasonbin3 of step 3" ; 
	}
	
	!system ("cp $modName.rrcd $modName.orig.rrcd");
	!system ("cp $modName.cfg $modName.orig.cfg");
	
	print "$CAPCFG -i$modName -o$modName -m3 -s6 >	$modelDir/Log/log_part3.prepare_capcfg.$modName	2>$modelDir/Log/error_part3.capcfg_$modName\n";

	unless ($DEBUG){
		 !system ("$CAPCFG -i$modName -o$modName -m3 -s6 > $modelDir/Log/log_part3.prepare_capcfg.$modName 2>$modelDir/Log/error_part3.capcfg_$modName") || die "Failed on reasonbin3 / capcfg40 of step 3" ;
	}
	
	print "Running prep_eval now ... \n";
	
	print "$PREPMODEL --score --reason $modName >	$modelDir/Log/log_part3.prepModel.prepare_$modName	2>$modelDir/Log/error_part3.prepModel_$modName\n";

	unless ($DEBUG){
		 !system ("$PREPMODEL --score --reason $modName > $modelDir/Log/log_part3.prepModel.prepare_$modName 2>$modelDir/Log/error_part3.prepModel_$modName") || die "Failed on prep_eval of step 3" ;
	}
	
	### change varible list without reason code ###
	system ("cp $modelDir/nets/$calTab $modelDir/nets/$calTab.orig");
	!system ("$INSERT $modelDir/nets/$calTab") || die "Failed to modify the calibration file";
	
print "\nDone for step 4\n\n";

print "==========================================================================\n";
print "Step 5: Starting the sensetivity analysis.\n\n"; 
print "+++++++ Starting to modify the model file and net file.\n\n";

	system ("cp $modName $modName.hold");
	
	## Add back the reason code
	open (IN, "< $modName.hold ") || die "Could not open $modName.hold";
	open (OUT, " > $modName") || die "Could not write to $modName"; 
	  while (<IN>){
			$_ =~ s/\s+NSCORE\s+:=\s+\@CALC\_/   RAW_SCORE := \@CALC\_/; 
			print OUT $_; 
			if ( /\s+RAW_SCORE\s+:=\s+\@CALC\_/) { 			
				 @temp =split('_', $_); 
				 $netsNm=$temp[2]; 
      		print STDERR "nets file name is $netsNm. Is it true??";
			}
	  }
	close(IN);
	close(OUT);
	
	system ("cp nets/$netsNm nets/$netsNm.hold");

	open (IN, "< nets/$netsNm.hold ") || die "Could not open	nets/$netsNm.hold";
	open (OUT, " > nets/$netsNm") || die "Could not write to nets/$netsNm"; 
	  while (<IN>){
			$_ =~ s/\s+NSCORE\s+:=\s+/\/\/ NSCORE :=/g;
			$_ =~ s/\s+IF\s+\(NSCORE\s+\>/\/\/  IF (NSCORE >/g;
			$_ =~ s/\s+Get_Reasons\(/\/\/Get_Reasons(/g;
			if (/\s+Calc_NN_Slab\(/ and /_OUTPUT/){
				 	@temp=split(',', $_);
					$op=$temp[2];
					$op =~ s/ //g; 
			  }

			 $_ =~ s/NSCORE/$op\[1\]/ if (/\s+RETURN\(NSCORE\)/); 
				 
		print OUT $_; 
	  }
	close(IN);
	close(OUT);

print "+++++++ Starting to do the sensitivity analysis\n\n";

	mkdir ("$modelDir/sens", 0755) or die "cannot mkdir $modelDir/eval: $!";
	system ("cd $modelDir/sens");
	system ("ln -s $modelDir/$modName.scl .");
	system ("ln -s $modelDir/$modName.cfg .");
	system ("ln -s $modelDir/$modName.tag .");
	system ("cp $modelDir/$modName.nnw $modelDir/$modName\8.nnw");

	print "$CREATE $modName 8\n";

	unless ($DEBUG){
		 !system("$CREATE $modName 8") || die "Failed on sensitivity analysis" ;
	}
print "\n Done for step 5. Need to launch DMW and actually run it...\n\n";

print "==========================================================================\n";
print "Step 6: Start evaluation at...\n\n"; system("date");
        $eval_opts= "-d std_kchk -R silent -t test";

		  print "$FALCON4 -idir=$indir/data/$pct/$evalClient.$month,select=$modelDir/select/$pct/muse/select.muse.$evalClient.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.$modName.$evalClient,full -b  $modelDir/eval/bigscr.$modName$evalClient.gz -s  $modelDir/eval/shortscr.$modName.$evalClient.gz $modName.cnf > $modelDir/Log/log_part5.evaluate_$modName  2>$modelDir/Log/error_part5.eval_$modName\n\n";

		  unless ($DEBUG){
				!system("$FALCON4	-idir=$indir/data/$pct/$evalClient.$month,select=$modelDir/select/$pct/muse/select.muse.$evalClient.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.$modName.$evalClient,full -b $modelDir/eval/bigscr.$modName$evalClient.gz -s $modelDir/eval/shortscr.$modName.$evalClient.gz $modName.cnf > $modelDir/Log/log_part5.evaluate_$modName 2>$modelDir/Log/error_part5.eval_$modName") || die "Failed on evaluation of step 6";
		  }
		  
print "\nDone for step 6\n\n";
print "==========================================================================\n";

print "==========================================================================\n";
print "Step 7: Start evaluation at Train dataset...\n\n"; system("date");
        $eval_opts= "-d std_kchk -R silent -t threshold=25,train";

		  print "$FALCON4 -idir=$indir/data/$pct/$evalClient.$month,select=$modelDir/select/$pct/muse/select.muse.$evalClient.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.$modName.$evalClient,full -b $modelDir/eval/bigscr.$modName$evalClient.gz -s $modelDir/eval/shortscr.$modName.$evalClient.gz $modName.cnf > $modelDir/Log/log_part6.evaluate_$modName 2>$modelDir/Log/error_part6.eval_$modName\n\n";

		  unless ($DEBUG){
				!system("$FALCON4	-idir=$indir/data/$pct/$evalClient.$month,select=$modelDir/select/$pct/muse/select.muse.$evalClient.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.$modName.$evalClient,full -b $modelDir/eval/bigscr.$modName$evalClient.gz -s $modelDir/eval/shortscr.$modName.$evalClient.gz $modName.cnf> $modelDir/Log/log_part6.evaluate_$modName 2>$modelDir/Log/error_part6.eval_$modName") || die "Failed  on evaluation of step 7";
		  }
		  
print "\nDone for step 7\n\n";
print "==========================================================================\n";


