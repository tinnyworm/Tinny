#!/usr/local/bin/perl

use warnings;
use strict;
use Getopt::Std;

# $Log: fishUnsorted_new.pl,v $
# Revision 1.1  2006/02/09 23:36:38  dxl
# Initial revision
#

my $usage = "\nUsage:\n".
            " $0 \n".
            "     -b <bait_file> \n".
            "     -c <bait_col_spec> (start from 1)\n".
            "     -s <pond_col_spec> (start from 1)\n".
            "     [options] \n".
            "     <pond_file> [ > fishfile ]\n".
            " options:\n".
            "    -h: show this message\n".
            "    -v: write complement (all records in pond file NOT in bait file)\n".
            "\n".
            " Uses the length of <bait_col_spec> determine how many columns to use from the pond if only one column number is specified in <pond_col_spec>, otherwise, the number of columns should match between bait and pond\n".
            " You may use gzipped or ungzipped files at will.\n".
            "\n".
            " NOTE: to use STDIN for the pond, you must give the <pond_file> name \"-\"\n"
          . "\n";

my %opt = ();

die $usage unless ($#ARGV>0);
die $usage unless getopts("b:c:s:vh", \%opt);
die $usage if($opt{h});

my $bait_fmt="";
my $pond_fmt="";
my $bait_length=0;
my $pond_length=0;

# parse <bait_col_spec>
if($opt{c} =~ /((\d+)(-\d+)?,?)+/) {
   my @cols = split /,/, $opt{c};
   my ($start,$length);
   foreach my $seg (@cols) {
      if ($seg =~ /^(\d+)$/) {
         die "The column number is started from 1\n" if $1<1;
         $bait_fmt .= "\@".($1-1)." a1 ";
         $bait_length += 1;
      } elsif ($seg =~ /(\d+)-(\d+)/) {
         $length = $2-$1+1;
         die "Bad column specification\n" if ($1<1 || $length<0);
         $bait_fmt .= "\@".($1-1)." a".$length." ";
         $bait_length += $length;
      } else {
         die "Bad column specification\n";
      }
   }
} else {
   die "Bad column specification\n";
}
#print "$bait_fmt \t $bait_length\n";

# parse <pond_col_spec>
if($opt{s} =~ /^(\d+)$/) {
   # only 1 column number is specified, use it as the start col number
   die "The column number is started from 1\n" if $1<1;
   $pond_length = $bait_length;
   $pond_fmt .= "\@".($1-1)." a".$pond_length;
} elsif($opt{s} =~ /((\d+)(-\d+)?,?)+/) {
   my @cols = split /,/, $opt{s};
   my ($start,$length);
   foreach my $seg (@cols) {
      if ($seg =~ /^(\d+)$/) {
         die "The column number is started from 1\n" if $1<1;
         $pond_fmt .= "\@".($1-1)." a1 ";
         $pond_length += 1;
      } elsif ($seg =~ /(\d+)-(\d+)/) {
         $length = $2-$1+1;
         die "Bad column specification\n" if ($1<1 || $length<0);
         $pond_fmt .= "\@".($1-1)." a".$length." ";
         $pond_length += $length;
      } else {
         die "Bad column specification\n";
      }
   }
} else {
   die "Bad column specification\n";
}
#print "$pond_fmt \t $pond_length\n";

# check if the length of bait and pond match
die "The key length of the bait and pond are different!" 
   unless ($bait_length == $pond_length);

# read in the bait
my %keys=();
if (substr($opt{b}, -3) eq ".gz" || substr($opt{b},-2) eq ".Z") {
   open(BAIT, "zcat $opt{b} |")
      or die "Can NOT open bait $opt{b}!\n";
} else {
   open(BAIT, "cat $opt{b} |")
      or die "Can NOT open bait $opt{b}!\n";
}

while(<BAIT>) {
   my @t=unpack("$bait_fmt",$_);
   my $key="";
   foreach my $k (@t) {
      $key .= $k;
   }
   $keys{$key}=1;
}
close(BAIT);

# process the pond
while(my $pond_file=shift(@ARGV)) {
   if(substr($pond_file, -3) eq ".gz" || substr($pond_file,-2) eq ".Z") {
       open(POND, "zcat $pond_file |")
         or die "Can NOT open bait $pond_file!\n";
   } else {
       open(POND, "cat $pond_file |")
          or die "Can NOT open bait $pond_file!\n";
   }
   print STDERR "Processing pond: $pond_file\n";

   while(<POND>) {
      my @t=unpack($pond_fmt,$_);
      my $key="";
      foreach my $k (@t) {
          $key .= $k;
      }
      if($opt{v}) {
         print if (!defined ($keys{$key}));
      } else {
         print if (defined ($keys{$key}));
      }
   }
   close(POND);

   $pond_file="";
}

   


