#! /usr/local/bin/perl -w

print "\n";
print "Command line: $0 @ARGV\n";
print "Started on " . scalar localtime() . "\n\n";

################################################################################
$usage = << "xxxEOFxxx";
\nUsage: $0 -c <cards> -a <auths>\n
xxxEOFxxx

use Getopt::Std;

$opt_c = "";
$opt_a = "";

getopts('hc:a:');
$err = "";

if (!$opt_c) {
    $err .= "Must specify file containing cardholder data\n";
    $opt_h = 1;
}
if (!$opt_a) {
    $err .= "Must specify file containing authorization data\n";
    $opt_h = 1;
}

if ($opt_h) {
    print STDERR $err;
    print STDERR $usage;
    exit (1);
}

################################################################################
###############################################
# 
# Read in and write out the data
#
###############################################
if ($opt_c =~ m/\.gz$/ || $opt_c =~ m/\.Z$/) {
    $CPROF = "gzip -dc $opt_c |";
} else {
    $CPROF = "< $opt_c";
}

if ($opt_a =~ m/\.gz$/ || $opt_a =~ m/\.Z$/) {
    $APROF = "gzip -dc $opt_a |";
} else {
    $APROF = "< $opt_a";
}

open (OTHER, "| gzip -c > otherThan_sameOpen.auths.dat.gz ") || die "Couldn't open the file for not same\n";
open (YES, "| gzip -c > sameOpen.auths.dat.gz ") || die "Couldn't open the file for same\n";
#open (ALL, "| gzip -c > all_combinedOpen.auths.dat.gz ") || die "Couldn't open the file for all\n";

###############################################
#
# Read cardholder file to get the open_date info. 
#
###############################################
use lib "/home/yxc/scripts/Perl/PM";
use Date::Simple (':all');

open CPROF or die "Can't open cardholder file: $CPROF";
while (defined($line = <CPROF>)) {
	chomp($line);
	$cardacct=substr($line,0,19);
	$cardopen=substr($line,31,8);
	$cardAcct{$cardacct}=1;
	$cardOpenHash{$cardacct}=$cardopen;
}

$lastAcct="";

open APROF or die "Can't open auths file: $APROF";
while (defined($line = <APROF>)) {
	chomp($line);

	$authacct=substr($line,0,19);
	$trans   =substr($line,19,8);

	if (!defined($cardAcct{$authacct})){
		next;
	} else {

	if ($trans =~ m/^[ 0]*$/ && $cardOpenHash{$authacct} =~ m/^[ 0]*$/){
		 next;
	} else {	 
		$runday=d8($trans) - d8($cardOpenHash{$authacct});

				 if ($runday <=1 && $runday >=-2){
							if ($authacct ne $lastAcct) {
							    print YES "$line $cardOpenHash{$authacct} $runday\n";
							} else {
						  		print OTHER "$line $cardOpenHash{$authacct} $runday\n";
					  		}
				 } else {
				 		if ($cardOpenHash{$authacct} >= 20030901){
					 		print OTHER "$line $cardOpenHash{$authacct} $runday\n";
						}
				 }

	 $lastAcct=$authacct;   
	}
  }
}	  


