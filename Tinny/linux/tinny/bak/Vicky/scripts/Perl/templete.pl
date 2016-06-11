#! /usr/local/bin/perl -w 

my $VERSION = '1.0';
my $DEBUG = 1;
my $USAGE =<< "xxxEOFxxx";

################################################################################
# Product:
#
#          
# Module:
#			$0 
#
# Purpose:
#
#                     
# Arguments:
#
#
# Options:
#
#
# Input:
#
#
# Output:
#
#
# Examples:
#
#
# Revision History:
#			  yxc						9/16/05					 
#
#
# Notes: 
#                
################################################################################
xxxEOFxxx

use strict;
use Getopt::Std;
use File::stat;

use Date::Calc ("check_date");

############################################################
#       Write start time, machine name and version
############################################################
my $tmp = `date`;
chomp $tmp;
my $sttime=$tmp;
print STDOUT "Command line: $0 @ARGV\n";
print STDOUT ("Starting on $tmp\n");
$tmp = `uname -ns`;
chomp $tmp;
print STDOUT ("Host is $tmp\n");
print STDOUT ("Version $VERSION\n\n");


############################################################
#	Process options
############################################################
die "$USAGE" if ($#ARGV == -1 );
my %opts = ();
&getopts('f:p:t:l:v:d:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
die "ERROR: Portfolio type required\n" unless (defined $opts{'p'});

if (defined $opts{'t'}) {
	  		die "ERROR: Tsys client Hash table is required since -t is chose\n" unless (defined $opts{'l'});
			die "ERROR: Data specification version is required since -t is	chose\n" unless (defined $opts{'v'});
			die "ERROR: Wrong value \'$opts{'v'}\' for -v. Only actual length is valid\n" unless ($opts{'v'} =~ /^\d+$/ );
}

