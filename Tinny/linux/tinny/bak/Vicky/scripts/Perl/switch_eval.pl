#! /usr/local/bin/perl 

################################################################
####Usage:
#  switch_eval.pl model working_dir [-h]
#  switch_eval.pl Home2002 
### yxc on 200501
################################################################

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	Usage:	$0  working_dir [-h]

		Required:
			 working_dir
			
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
$modName="switch";
$dir=$ARGV[0];
$modelDir="$indir/$dir";
$client="homeDepotFdr";
$month="200309-200409";
$pct="10pct";
$seg="gen";

#########>>>>>>>>>> Change ends here <<<<<<<<<##############

##
$MODELCOPY="/work/price/falcon/bin/model_copy40.sh";

## Model Driver ###
$SYS=`uname`;
chop($SYS); 
if ($SYS eq "SunOS") {
	$FALCON4="/work/gold/projects/bin/falconer4";
} elsif ($SYS eq "Linux"){
	$FALCON4="/work/gold/linux/bin/falconer4";
}
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

#### Modify the switch date ####
foreach $date ("20040201", "20040301", "20040401", "20040501"){
	open (IN, " < switch.rul") || die "No switch.rul file exists!!\n";
	open (OUT, " > switch_$date.rul") || die "Could not write to switch_$date.rul"; 
	while (<IN>){
		$_ =~ s/_SWITCHDATE_/$date/;
		print OUT $_;
	}
	close(IN);
	close(OUT);
	open (IN2, " < switch.cnf") || die "No switch.cnf file exists!!\n";
	open (OUT2, " > switch_$date.cnf") || die "Could not write to switch_$date.cnf";
	while (<IN2>){
		$_ =~ s/switch.rul/switch_$date.rul/;
		print OUT2 $_;

	}

}


print "==========================================================================\n";
print "Step 6: Start evaluation at...\n\n"; system("date");
	$eval_opts= "-d std_kchk -R silent -t test";
	$drv_filter="-f noposts";

foreach $date ("20040201", "20040301", "20040401", "20040501"){

	!system("$FALCON4 -idir=$indir/data/$pct/$client.$month,select=$modelDir/select/$pct/muse/select.muse.$client.$month $eval_opts $drv_filter -r file=$modelDir/eval/res.$modName.$client_$date,full -b $modelDir/eval/bigscr.$modName.$client_$date.gz -s $modelDir/eval/shortscr.$modName.$client_date.gz switch_$date.cnf > $modelDir/Log/log_part5.evaluate_$modName_$date 2>$modelDir/Log/error_part5.eval_$modName_$date") || die "Failed on evaluation of step 6";
print "\nDone for step 6\n\n";
print "==========================================================================\n";

}
