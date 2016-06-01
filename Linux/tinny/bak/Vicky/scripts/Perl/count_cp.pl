#! /usr/local/bin/perl 

################################################################
###quickly build scale file and train a model
#assumes only 1 model is specified in the MODEL_SET part in the *.cnf file
#Should be run from directory where model resides.
#Usage:
#  quicktrain.sh model.cnf general_segment_name working_dir [-h]
#  ../quicktrain.pl Home2002 
### yxc on 200501
################################################################

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	Usage:	$0 modelName working_dir clientName Month SegmentName [-h]

		Required:
			model.cnf files 
			
		Optional:
			-h shows this message
			-e<email address> default = blank

		Example: $0 HGGE2004 0_gen_oldRisk homeImproGE 200101-200112 gen
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
$dir=$ARGV[1];
$modelDir="$indir/$dir";

$client=$ARGV[2];
$month=$ARGV[3];
$seg=$ARGV[4]; 

#$client="homeImproGE";
#$month="200101-200112";
$pct="10pct";
#$seg="hi";

#########>>>>>>>>>> Change ends here <<<<<<<<<##############

##
$MODELCOPY="/work/price/falcon/bin/model_copy40.sh";

## Model Driver ###
$FALCON4="/work/gold/projects/bin/falconer4";

#########################################
# PRINT START TIME AND CURRENT DIRECTORY
#########################################

printf "STARTED: "; system ( "date" );

print "==========================================================================\n";
printf "Directory: "; system ( "pwd" );
if (!(-d "$modelDir/Log")) {
	mkdir ("$modelDir/Log", 0755) or die "cannot mkdir $modelDir/Log: $!";
}

if (!(-d "$modelDir/eval")) {
	mkdir ("$modelDir/eval", 0755) or die "cannot mkdir $modelDir/eval: $!";
}

print "==========================================================================\n";
print "Step 6: Start evaluation at...\n\n"; system("date");
	$eval_opts= "-d std_kchk -R silent -t threshold=100,test ";

	if ($seg eq "hi") {
		$drv_filter="-f noposts,amountGE=230";
	} elsif ($seg eq "gen"){
		$drv_filter="-f noposts";
	}

	$cmd=	"$FALCON4 -idir=$indir/data/$pct/$client.$month,select=$modelDir/select/$pct/select.count.$client.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.count-$seg.$modName.$client,full $modName.cnf >	$modelDir/Log/log_part5.count-$seg_$modName_$client 2>$modelDir/Log/error_part5.count-$seg_$modName_$client";
	print "$cmd\n\n";

	
	!system("$FALCON4 -idir=$indir/data/$pct/$client.$month,select=$modelDir/select/$pct/select.count.$client.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.count-$seg.$modName.$client,full $modName.cnf > $modelDir/Log/log_part5.count-$seg_$modName_$client 2>$modelDir/Log/error_part5.count-$seg_$modName_$client") || die "Failed on count of step 6";
print "\nDone for step 6\n\n";
print "==========================================================================\n";


