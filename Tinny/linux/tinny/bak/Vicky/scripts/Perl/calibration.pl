#! /usr/local/bin/perl -w

################################################################
### To calibration the segment with the standard curve or desired one.
### Then copy back to the real calibration tables into nets directory.
#
#Usage:
#  calibration.pl workdir referenceRes TargetRes finalTablename
#  netsCabliTable [-h]
#   
### yxc on 200501
################################################################

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	Usage:	$0 workdir referenceRes TargetRes finalTablename netsCabliTable [-h]

		Required:
			workdir referenceRes TargetRes finalTablename netsCabliTable 
			
		Optional:
			-h shows this message

\n\n";

die "$USAGE" if ($#ARGV == -1);
#&Getopts('f:hF');
die "$USAGE" if ($opt_h);


#$refer="/work/empowerment/GECapital/winner_segments/res.reference.credit";
$upperDir="/work/ffp6/HomeImprove3.0/yxc/homeImpro3.0";
$workDir=$ARGV[0];
$resRef=$ARGV[1];
$targetRes=$ARGV[2];
$finalTable=$ARGV[3];
$netsCabliTable=$ARGV[4];

$CNV= "/work/price/falcon/bin/cnvscore";
$CNSMOT= "/work/price/falcon/bin//calibSmooth";
$CALIB="/work/price/falcon/bin/calib_p.pl";


#	system ("cp $refer $upperDir/$workDir");
#	!system ("$CNV -g1 $upperDir/$workDir/$resRef $upperDir/$workDir/$targetRes | $CNSMOT 1 > $upperDir/$workDir/new_hiCal ")|| die "Failed on the calibration";
	
#	!system ("$CALIB_K -k -g1 -f30 $upperDir/$workDir/$resRef $upperDir/$workDir/$targetRes | $CNSMOT 1 >$upperDir/$workDir/$finalTable ")|| die "Failed on the calibration";

	$cmd = "cp $netsCabliTable $netsCabliTable.cp;\n";
   $cmd.= "head -15 $netsCabliTable.cp >  $netsCabliTable;\n";
	$cmd.= "$CALIB $research >> $netsCaliTable\n";

	
