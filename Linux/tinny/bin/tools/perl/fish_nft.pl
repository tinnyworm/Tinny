#!/usr/local/bin/perl
# 
# fish all non-fraud transactions between 
# from auths file
#
#
$usage = "\nUsage: \n\t" .
    "fish_ft.pl <auth.dat.gz> <fraud.dat.gz> <outfile> \n\n";

$auth    = shift(@ARGV);
$fraud   = shift(@ARGV);
$outfile = shift(@ARGV);

die $usage unless $auth && $fraud && $outfile;

$ACC_ST   = 0;
$ACC_LEN  = 19;
$FDATE_ST = 20; 
$DATE_LEN = 8;
$BDATE_ST = 35;
$TDATE_ST = 19;

open(FRAUD, "zcat $fraud |") or die "Can not open $fraud file \n";

while (<FRAUD>) {
    chomp;
    $fraud_acc = substr($_, $ACC_ST, $ACC_LEN);
    $fraud_acc =~ tr/ //d;
    $f_rec{$fraud_acc} = $_;
#    printf("%s \n", $f_rec{$fraud_acc});
}
close(FRAUD);

open(OUT, "| gzip > $outfile") or die "Can not open $outfile file \n";
open(AUTH, "zcat $auth |") or die "Can not open $auth file \n";
while (<AUTH>) {
    chomp;
    $tran_acc = substr($_, $ACC_ST, $ACC_LEN);
    $tran_acc =~ tr/ //d;
    if (exists($f_rec{$tran_acc})) {
	$ff_date = substr($f_rec{$tran_acc}, $FDATE_ST, $DATE_LEN);
	$t_date  = substr($_, $TDATE_ST, $DATE_LEN);
	if ($t_date < $ff_date) {
	    printf OUT "$_\n";
	}
    } else {
	printf OUT "$_\n";
    }
}
close(AUTH);
close(OUT);


