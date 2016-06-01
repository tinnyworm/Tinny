#!/usr/local/bin/perl
# Purpose
# Give the list of client, for whom you would like to get the missing rrport file.
## Date: 25-Oct-2005
# Last modified:
#Author: Partha

################################################################################################
#          Product      : MSG / DQR
#          Module       : missing_report_wrapper.pl
#          Module       : missing_report_wrapper.pl # wrapper script to run report_missing_data.pl
#          Purpose      : Wrapper script to run report_missing_data.pl, run missing reports for a list of clients
#          Arguments    :
#                      arg1 . -f  file containing the details for the clients the report is run. THe line in this file should 
#                        	look like anz|credit|AUTH
#			arg2 . -p period
#                      arg3 . -w Password file.
#          Options:
#                      opt1 . -h to show the usage of the script.
#          Input:
#          Output       : Missing data report for the list of clients mentioned in the file above
#          Return:
#          Usage        : missing_report_wrapper.pl  -f <file_name> -p 200406:200508 -w <password_file>
#          Example      : missing_report_wrapper.pl -f <file_name> -p 200406:200508 -w <password_file>
#          Revision History:
#                      Authors             Date                 Involvement
#                      Partha              10/25/05               Creator
#
#          Notes:
#                     This is a wrapper program that calls the program report_missing_data.pl for a list of clients 
#
#######################################################################################

#######################
use strict;
use Getopt::Std;
##########  Program usage ###############################
my $USAGE = "
        Usage:  -f file -w password file 
        Example: -f <file_name> -p 200406:200506 -w <password_fiile>
        Version 1.0
        Required:
                -f      file containing the details for the clients the report is run. THe line in this file should look like
			anz|credit|AUTH 
                -w      Password file
        Optional:
                -h shows this message
";

die "$USAGE" if ($#ARGV == -1) ;

#------
my %opts = ();
&getopts('f:p:w:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
die "Client list file required \n" unless (defined $opts{'f'});
die "Period of the report required\n" unless (defined $opts{'p'});
die "Password file required \n" unless (defined $opts{'w'});

open (CLIENT_FILE, "< $opts{'f'}") || die "Couldn't open $opts{'f'}   :$!";
while(<CLIENT_FILE>){

chomp;

my ($client, $portfolio, $file_type ) = split (/\|/,$_);

system ("echo report_missing_data_yxc.pl -c $client -p $opts{'p'} -f
$portfolio -t $file_type -w $opts{'w'}");
#system ("/ana/mds_files5/MetaDataWarehouse/scripts/dbi/report_missing_data.pl -c $client -p $opts{'p'} -f $portfolio -t $file_type -w $opts{'w'}");

}

