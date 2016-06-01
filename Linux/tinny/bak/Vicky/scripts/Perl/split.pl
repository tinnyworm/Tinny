#!/usr/local/bin/perl  -w

#%DayCUTOFF = ('20040403' => 'day1_pre',
#	'20040404' => 'day1_post',
#	'20040411' => 'day7_post',
#	'20040425' => 'day21_post',
#	'20040501' => 'day28_post'
#	);

#%MonthCUTOFF = ('200403' => 'mon1_pre',
#	'200404' => 'mon1_post',
#	'200405' => 'mon2_post',
#	'200406' => 'mon3_post',
#	'200407' => 'mon4_post',
#	'200408' => 'mon5_post',
#	'200409' => 'mon6_post'
#	);


%DayCUTOFF = ('20040501' => 'week1_post',
	'20040508' => 'week2_post',
	'20040515' => 'week3_post',
	'20040522' => 'week4_post',
	);



$indir="/work/ffp6/HomeImprove3.0/yxc/homeImpro3.0/";
$son=$ARGV[0];

if (!(-d "$indir/$son/split")){
	mkdir ("$indir/$son/split", 0755) or die "cannot mkdir $indir/$son/split: $!";
}

while (<STDIN>) {
	$date=substr($_,34,8);
	$mon=substr($date,0,6);
	
	$month = $MonthCUTOFF{$mon};
	$day = $DayCUTOFF{$date};

	if (!defined($month)) {
		next;
	}
	
	if (!defined($fh{$month})){
		$out = "f$month";
		open $out, "| gzip -c > $indir/$son/split/$month.bigScore.gz";
		$fh{$month} = $out;	

	} 
	
	$fh = "f$month";
	print $fh $_; 

	if (!defined($day)) {
 		next;
	}
	if (!defined($fhd{$day})){
		$outd = "f$day";
		open $outd, "| big2res -s 0.1 - $indir/$son/split/res.$day";
		$fhd{$day} = $outd;	
	} 
	
	$fhd = "f$day";
	print $fhd $_; 
	
}


