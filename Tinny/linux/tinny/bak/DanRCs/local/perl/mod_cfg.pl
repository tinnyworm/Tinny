#!/usr/bin/env perl 

use warnings;
use strict;
use Getopt::Std;

my $usage = "cat original_cfg | $0 -l varlist [-h]\n\n".
            "Disable input vars in cfg file if not in <varlist>\n".
            "Change Slab= INPUT --> Slab= NONE\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("l:h",\%opt);
die $usage if($opt{h});

open(VLIST,"< $opt{l}") or die "Can't open varlist file $opt{h}!";

my %varlist=();
while(<VLIST>){
    if(/^\s*(\S+)\s*$/) {
        $varlist{$1}=1;
    }
}
close(VLIST);

$/ = "";

my $trash=<STDIN>;
print $trash;
my $fline=<STDIN>;
print $fline;

while(<STDIN>){
  if(/Name= (\w+)\n/){
      if(exists($varlist{$1})) {
          print $_;
      } else {
          $_=~s/Slab= INPUT/Slab= NONE/;
          print $_;
      }
  } else {
      print $_;
  }
}
