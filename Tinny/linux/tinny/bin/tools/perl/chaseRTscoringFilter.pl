#!/usr/local/bin/perl

# Chase Real Time Scoring Filter
#
# Apply on the bigScr file to get Chase real time score. 

use strict;
use warnings;

my $usage = "\n Usage:\n".
            "\t zcat <bigScr> | $0\n";

# lists
my @midList= qw/000000000652990 322131000784231 322164000784231/;
my @sicList=(5441,5937,5975,5977,7276,7623,7829,8041,8049,8220,
             8911,0004,0005,0006,0007,4119,5949,5973,5976,7261,
             8021,8031,8042,8050,8071,8398,8661,7296,9311,0742,
             1740,1761,8011,8043,8062,8099);
my @cashList=(6011,6051,6010,4829,7995,6012,6050);

my %midHash=();
my %sicHash=();
my %cashHash=();
foreach my $a (@midList) {
    $midHash{$a}=1;
}
foreach my $a (@sicList) {
    $sicHash{$a}=1;
}
foreach my $a (@cashList) {
    $cashHash{$a}=1;
}

my $pre_acct="";
my $last_date="00000000";
my $last_dollarV=0;
my $this_dollarV=0;
my $counter24=0;
my $pre_scr=0;

# main loop
while(<>) {
    my ($acct,$amt,$date,$scr,$dec,$entry,$sic,$mcntry,$mid) = 
      unpack("a19 \@19 a14 \@34 a8 \@83 a4 \@88 a \@92 a \@94 a4 \@138 a3 \@142 a15", $_);
    #print STDERR "$acct,$amt,$date,$scr,$dec,$entry,$sic,$mcntry,$mid ";

    # new account?
    if($acct ne $pre_acct) {
        $last_date = "00000000";
        $last_dollarV = 0;
        $this_dollarV = 0;
        $counter24 = 0;
        $pre_scr = 0;

        $pre_acct = $acct;
    }
    # new day?
    if($date != $last_date) {
        $last_dollarV = $this_dollarV;
        $this_dollarV = 0;

        $counter24 = 0;

        $last_date = $date;
    }
    # Approved?
    if($dec eq "A") {
        $this_dollarV += $amt;
        $counter24++;
    }
    # 48 hr dollar velocity
    my $dollarV48 = $this_dollarV + $last_dollarV;
   
    #printf STDERR "%14.2f %14.2f %14.2f %4d ", $dollarV48, $this_dollarV,
    #     $last_dollarV, $counter24; 
    if( $acct =~ /^6/ || exists($midHash{$mid}) || exists($sicHash{$sic}) ) {
    # exclusions
        if($scr > 0) {
         substr($_,83,4)="0000";
        }
        #print STDERR "Exclusions\n";
        print $_;
    }
    elsif( $dollarV48 > 300 ||
           $counter24 > 6 ||
           $mcntry != 840 ||
           exists($cashHash{$sic}) ||
           $entry eq " " ||
           $amt < 1 ||
           $pre_scr > 300 ) {
    # inclusions
        #print STDERR "Inclusions\n";
        print $_;
    } else {
    # everything else is also not real time
        if($scr > 0) {
            substr($_,83,4)="0000";
        }
        #print STDERR "Others\n";
        print $_;
    }

    $pre_scr = $scr;
}
