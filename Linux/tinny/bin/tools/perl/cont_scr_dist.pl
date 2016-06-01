#!/usr/local/bin/perl

use Getopt::Std;
use strict;
use warnings;

my $usage = "\nUsage:\n".
            "\t zcat <bigScr> | $0\n".
            "\t   -s <sample rate>     Default 0.01 \n".
            "\t   -b <score bin>       Default 50 \n".
            "\t   -t <threshold>       Default 500 \n".
            "\t   [-h]\n\n";
            
my %opt = ();
die $usage unless getopts("s:b:t:h", \%opt);
die $usage if($opt{h});

my $rate = $opt{s}?$opt{s}:0.01;
my $bin = $opt{b}?$opt{b}:50;
my $threshold = $opt{t}?$opt{t}:500;

my $all_nf=0;
my $all_f=0;
my $all_pref=0;

my $tb=int((1000-$threshold)/$bin);
my (@c_nf, @c_f, @c_pref);
for(my $i=0; $i<$tb; $i++) {
    $c_nf[$i]=0; $c_f[$i]=0; $c_pref[$i]=0;
}

while(<>) {
    my ($fn,$scr)=unpack("\@59 a \@83 a4",$_);
    if($scr>0) {
        my $index=int(($scr-$threshold)/$bin);
        #print STDERR "$fn, $scr, $index\n";
        if($fn==0) {
            $all_nf++;
            if($index>=0) {$c_nf[$index]++;}
        }
        elsif($fn==1) {
            $all_f++;
            if($index>=0) {$c_f[$index]++;}
        }
        elsif($fn==2) {
            $all_pref++;
            if($index>=0) {$c_pref[$index]++;}
        }
        else {
            die "Unkown fraud tag in big score file\n";
        }
    }
}

my (@ac_nf, @ac_f, @ac_pref);
for(my $i=0; $i<$tb; $i++) {
    $ac_nf[$i]=0; $ac_f[$i]=0; $ac_pref[$i]=0;
}
for(my $i=0; $i<$tb; $i++) {
    #print STDERR "$i: $c_nf[$i], $c_f[$i], $c_pref[$i]\n";
    for(my $j=$i; $j<$tb; $j++) {
        $ac_nf[$i] += $c_nf[$j];
        $ac_f[$i] += $c_f[$j];
        $ac_pref[$i] += $c_pref[$j];
    }
}

printf STDOUT "Score,     Frauds,  Nonfrauds\n";
printf STDOUT "  %3d, %10d, %10d\n",
               0,$all_f,int($all_nf/$rate)+$all_pref;
for (my $i=0; $i<$tb; $i++) {
    printf STDOUT "  %3d, %10d, %10d\n",
    $threshold+$i*$bin,$ac_f[$i],int($ac_nf[$i]/$rate)+$ac_pref[$i];
}
        
    
