#! /usr/local/bin/perl -w -I/home/krh/bin/PM

$USAGE = << "xxxEOFxxx"; 
#####################################################
# This script is for setting up the working directories and
# scripts to conduct continuity test from old model to new model.
# 
# Requirements before using this script:
#	1. requires a switch model in a subdirectory switch_model 
#     with cnf file switch.cnf with combining both old model 
#     and new model with proper naming convention.
#	2. switch.rul should have the date tag ___SWITCH_DATE___
#	3. requires a Makefile script in the switch_model subdirectory 
#     that creates big score files in ./out/bScr.{name}
# 
# Options:
# 	-d: start date for evaluation window
#	-s: sampling rate
#	-w: working directory
#	-c: coverage of evaluation window, eg 30 -> 30 days. Default is 7 days. 
#	-h: help
#
# Usage:  
#	$0 -d 20050901 -s 0.05 -w `pwd` -c 30
#
# History:
#	1) Created by yxc on 2007
#	2) Modify by yxc on 2009 on usage. 
#####################################################
xxxEOFxxx
use Getopt::Std;
use D90;
#####################################################
# To process the options	
#####################################################
 
die "$USAGE" if ($#ARGV == -1); 
&getopts('d:s:w:c:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

die "ERROR: Not specify Fix start date. Quiting...\n\n" unless (defined $opts{'d'}); 
$fixStart = $opts{'d'}; 
$day90 = D90::today90($fixStart); 

if (defined $opts{'c'})
{
	$days = $opts{'c'}; 
}
else
{
	$days = 7; 
}

$new90fe=$day90 + $days; 
$fixEnd = D90::day90to($new90fe); 

if (defined $opts{'s'}){
	$sRt= $opts{'s'};
} else {
	warn "WARN: Not specify sampling rate. Use Default 0.01. \n\n"; 
	$sRt= 0.01; 
}
if (defined $opts{'w'}){
	$workdir = $opts{'w'}; 
} else {
	warn "WARN: Not specify working directory. Use current dir as default. \n\n"; 
	$workdir = "./"; 
}

foreach $iii (0 .. 10){
	$new90 = $day90 - $iii*$days + $days ; 
	$newd = D90::day90to($new90); 
	push (@dateSeq, $newd); 
}

$caseRecon = 30; 

open (OUT, "> $workdir/wrapup.sh"); 

foreach $switchDate (@dateSeq){
	$replace = "s/___SWITCH_DATE___/$switchDate/g"; 
	if (!(-d "$workdir/switch_$switchDate")){
		mkdir ("$workdir/switch_$switchDate", 0755) or die "Could not mkdir $workdir/switch_$switchDate\n"; 
}
	`cp switch_model/Makefile $workdir/switch_$switchDate;`; 
	`model_copy40.sh switch_model/switch.cnf $workdir/switch_$switchDate;`;
	`cat switch_model/switch.rul | perl -wpe $replace > $workdir/switch_$switchDate/switch.rul;`;
	$out = "echo $switchDate | cat - cases.$switchDate |" .' perl -e ' . "\'" . ' $outline = <>;chomp $outline;while(<>){@data = split /\s+/;if($data[0] eq "" and ($data[1] eq "0" or $data[1] == 500 or $data[1] == 600 or $data[1] == 700 or $data[1] == 800 or $data[1] == 900 or $data[1] == 950)){$outline = $outline . "," . $data[9];}}print $outline,"\n"; ' . "\'" . '>>  case_summary.csv ';
	print OUT $out . "\n"; 

}

#########################################################
# Compose the Makefile for all the files chosn
#########################################################
$make = ""; 
$make = "#" x 20;
$make.= "\n# Makefile for continuity test in paralel";
$make.= "\n# make -j<njobs> -l<maxload> [-f <Makefile of interest>]\n";
$make.= "#" x 20;
$make.= "\nSHELL = /bin/sh\n\n";
$make.= "WORK_DIR =" . $workdir . "\n\n";
$make.= "GETCASES = " . '"/work/price/falcon/bin/getCases"'; 


$data = "\nDATA = ";
for ($i=0; $i<=@dateSeq-1; $i++) {
	if ($i < @dateSeq-1 ) {
		$data .= "\t" . $dateSeq[$i] . "\\" . "\n";
	} else {
		$data .= "\t" . $dateSeq[$i] . "\n";
	}
}
	
$rule = "\nRESULT = " . '$(foreach a, ${DATA},${WORK_DIR}/cases.${a})'; 
$rule .= "\nOUT = " . '${WORK_DIR}/case_summary.csv' . "\n\n"; 
$all = "\n\nall :" . '${OUT}' . "\n\n"; 

$make .= $data . $rule . $all; 

$run1 = '${WORK_DIR}/case_summary.csv: ${RESULT} 
	cd ${WORK_DIR}; \
	sh wrapup.sh; ' . "\n\n";   

$run2 = "\n" . '${WORK_DIR}/cases.%: 
	cd ${WORK_DIR}/switch_$*; \
	make -j4 -l18; \
	gunzip -c ${WORK_DIR}/switch_$*/eval/bScr.*.gz | ${GETCASES} ' . "$fixStart $fixEnd $caseRecon $sRt " . '>$@ 2>&1;'; 

$make .= $run1 . $run2; 
print $make ."\n\n"; 
