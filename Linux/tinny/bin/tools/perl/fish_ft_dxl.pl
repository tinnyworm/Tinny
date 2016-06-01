#!/usr/local/bin/perl -w 
# 
# fish all fraudulent transactions between 
# first fraud date and block date from auths 
# or posts file   
#
#
$usage = "\nUsage: \n\t" .
    "fish_ft.pl <auths/posts.dat.gz> <frauds.dat.gz> <outfile> \n\n";

$aupo    = shift(@ARGV);
$fraud   = shift(@ARGV);
$outfile = shift(@ARGV);

die $usage unless $aupo && $fraud && $outfile;

$ACC_ST   = 0;
$ACC_LEN  = 19;
$FDATE_ST = 20; 
$DATE_LEN = 8;
$BDATE_ST = 35;
$TDATE_ST = 19;
$PDATE_ST = 48;

open(FRAUD, "zcat $fraud |") or die "Can not open $fraud file \n";

while (<FRAUD>) {
    $fraud_acc = substr($_, $ACC_ST, $ACC_LEN);
    $fraud_acc =~ tr/ //d;
    $f_rec{$fraud_acc} = $_;
}
close(FRAUD);

open(OUT, "| gzip > $outfile") or die "Can not open $outfile file \n";

if ($aupo eq "auths.dat.gz") {
	open(AUTH, "zcat $aupo |") or die "Can not open $auth file \n";
	while (<AUTH>) {
    	$tran_acc = substr($_, $ACC_ST, $ACC_LEN);
    	$tran_acc =~ tr/ //d;
    	if ( defined( $curr= $f_rec{$tran_acc} ) ) {
			$ff_date = substr($curr, $FDATE_ST, $DATE_LEN);
			$fb_date = substr($curr, $BDATE_ST, $DATE_LEN);
			$t_date  = substr($_, $TDATE_ST, $DATE_LEN);
			if (($ff_date <= $t_date) && ($t_date <= $fb_date)) {
	    		printf OUT "$_";
			}
		}
	}
} elsif ($aupo eq "posts.dat.gz") {
	open(AUTH, "zcat $aupo |") or die "Can not open $auth file \n";
	while (<AUTH>) {
    	$tran_acc = substr($_, $ACC_ST, $ACC_LEN);
    	$tran_acc =~ tr/ //d;
    	if ( defined( $curr= $f_rec{$tran_acc} ) ) {
			$ff_date = substr($curr, $FDATE_ST, $DATE_LEN);
			$fb_date = substr($curr, $BDATE_ST, $DATE_LEN);
			$t_date  = substr($_, $PDATE_ST, $DATE_LEN);
			if ( $t_date >= $ff_date ) {
				printf OUT "$_";
			}
		}
	}
} else {
	print "missing auths.data.gz or posts.data.gz!\n";
}	

close(AUTH);
close(OUT);


