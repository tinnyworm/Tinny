#!/usr/local/bin/perl 

$DEBUG = 0;

warn "DO NOT PUT THIS PROCESS IN BACKGROUND\n\n";



$client_list = $ARGV[0];
$machine = $ARGV[1];
$Ctype = $ARGV[2];
$port = "41570";

$maNum=substr($machine,7,2);
#chop($maNum); 

$usage="$0 clientList MB_Server DateType(Crt/Dbt)\n\n ";
$arg = shift(@ARGV);
die $usage if(!$arg or $arg eq "-h" or $arg eq "--help");

$xml = "/work/aisle07/yxc/dqrProjects_mp$maNum/dqr310$Ctype/projects/projectConfig.DQR.xml";
#$xml = "/work/aisle09/jfa/mb/1/dqr310Debit/projects/projectConfig.DQR.xml";

if ($Ctype eq "Credit"){
	$type = "credit";
} else {
	$type = "debit";
}

$work_dir = "/work/aisle09/client/FALCON/FDR/200307_200409_1pct/CLIENTS";

$mb_priceclub = "/work/aisle01/modelbuilder/mb220_ga/products/modelbuilder/2.2.0/bin/mb.pl";
$mb_ux58 = "/ana/mds_files2/modelbuilder/mb220_ga/products/modelbuilder/2.2.0/bin/mb.pl";
$mb_mnappmb = "/work/mb_apps/fi/mb.220.258/products/modelbuilder/2.2.0/bin/mb.pl";

open(CLIST, $client_list) || die "Unable to open file $client_list\n";
while (<CLIST>) {
	chomp;
	next unless (-e "$work_dir/$_/$type/dqr/mb.source");

	#$cmd = "$mb_ux58 -u jfa -d default -r default -P $xml -s $machine:$port $work_dir/$_/$type/dqr/mb.source >& $work_dir/$_/$type/log/mb.log";

	$cmd ="$mb_mnappmb -u yxc -d default -r default -P $xml -s $machine:$port $work_dir/$_/$type/dqr/mb.source >& $work_dir/$_/$type/log/mb.log";

	print "$cmd\n";
	unless ($DEBUG) {
		`$cmd`;
	}
	print "done\n";
}

close (CLIST);
