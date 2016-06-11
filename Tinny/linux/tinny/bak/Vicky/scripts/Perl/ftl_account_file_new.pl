#! /usr/local/bin/perl 

use Getopt::Std;
use strict;
use Time::Local;
use Time::localtime;

my $usage ="ftl_account_file.pl -f <file_location>
send result to standard output
";

##Defining Options###
my %opts =();
&getopts('f:d:t:h',\%opts);
die "$usage" if ($opts{'h'});
my $ADDNL= "/ana/mds_files5/MetaDataWarehouse/bin/addnl";
##Binning
my %frd_typ_hash =(

"M00" => "L",
"V00" => "L",
"M01" => "S",
"V01" => "S",
"M02" => "N",
"V02" => "N",
"M03" => "A",
"V03" => "A",
"M04" => "C",
"V04" => "C",
"M05" => "O",
"V05" => "O",
"M06" => "M",
"V06" => "M",
"M07" => "O",
"V07" => "O",
"M08" => "T",
"V08" => "O",
"V09" => "O"
);

my ($cmd,$accnt_num,$frd_typ,$ffd,$fbd,$ffm,$fbt,$tmp,%HOH,$str);
my $ext;

if ($opts{'f'}=~/\.gz$/){
	$ext="zcat";
}
else{
	$ext ="cat";
}

$cmd="$ext $opts{'f'}|dd conv=ascii 2>/dev/null|$ADDNL -n 125 -q|bmsort -c22-47 2>/dev/null|";
#print "$cmd\n";
open (FIN, $cmd);
while (defined ($_=<FIN>)){
		$accnt_num= substr($_,21,19);
		$frd_typ= $frd_typ_hash{substr($_,18,3)};
		$ffd= &jumdy (substr($_,40,7));
		#$fbd= &jumdy(substr($_,4,7));
		#$fbt= substr($_,11,6);
		if (defined $accnt_num){
		    if (defined $HOH{$accnt_num}{'ffd'}) {
			$HOH{$accnt_num}{'ffd'}=$ffd if($HOH{$accnt_num}{'ffd'} >$ffd);
		    }
		    else {
			$HOH{$accnt_num}{'ffd'}=$ffd;
		    }
		    if (defined $HOH{$accnt_num}{'fbd'}) {
			$HOH{$accnt_num}{'fbd'}=$ffd if($HOH{$accnt_num}{'fbd'} <$ffd) ;
			$HOH{$accnt_num}{'frd_typ'}=$frd_typ;
		    }
		    else {
			$HOH{$accnt_num}{'fbd'}=$ffd;
			$HOH{$accnt_num}{'frd_typ'}=$frd_typ;
		    }
#								if (defined $HOH{$accnt_num}{'fbt'}) {
#											$HOH{$accnt_num}{'fbt'}=$fbt if ($HOH{$accnt_num}{'fbt'} <$fbt);
#								}
#								else {
#											$HOH{$accnt_num}{'fbt'}=$fbt;
#								}
		}
	    }

my $parse;
for $parse( sort keys %HOH){
    $HOH{$parse}{'ffm'}=" ";
    $HOH{$parse}{'ffm'}=pack "A7" , "$HOH{$parse}{'ffm'}";
    $str=$parse.$HOH{$parse}{'frd_typ'}.$HOH{$parse}{'ffd'}.$HOH{$parse}{'ffm'}.$HOH{$parse}{'fbd'};
    $str=pack "A91", "$str";
    print "$str\n";
}

sub jumdy {
my ($i,$julday,$leap,$timeSecs);
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);
$year=substr($_[0],0,4);
$julday=substr($_[0],4,3);

#daytab holds number of days per month for regular and leap years
my (@daytab) =(0,31,28,31,30,31,30,31,31,30,31,30,31,
0,31,29,31,30,31,30,31,31,30,31,30,31);

$leap=0;
#see if the year is a leap year
$leap = 1 if (($year%4 == 0 && $year%100 != 0) || $year%400 == 0);
for($i=1,$mday = $julday ; $mday > $daytab[$i + 13 * $leap] ; $i++) {
$mday -= $daytab[$i + 13 * $leap];
}
$mon=$i-1;

$timeSecs=timegm(0,0,0,$mday,$mon,$year-1900);
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)= gmtime($timeSecs);
$year=$year+1900;
$mon=$mon+1;
my $date_tag= sprintf "%4d%02d%02d",$year,$mon,$mday;
return ($date_tag);
}











