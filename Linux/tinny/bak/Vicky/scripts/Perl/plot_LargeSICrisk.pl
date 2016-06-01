#!/usr/local/bin/perl

use warnings;
use Getopt::Std;

my $usage = "\nUsage:\n".
            " $0 \n".
            "     -t the RiskTable \n". 
            "     -p Generate printable postscript file \n".
            "     -h: show this message \n".
            " $0 plot the large SIC risk curve.\n\n";

my %opt = ();

die $usage unless ($#ARGV>0);
die $usage unless getopts("t:p h", \%opt);
die $usage if($opt{h});

if($opt{t}) {
  open(RKTAB, "cat $opt{t} |")
      or die "Can NOT open risk table $opt{c}!\n";
}

if($opt{p}) {
         open PLOT, "| /home/jaz/tools/x_plot -c plot_$$.ps" or die "Cannot opt x_plot! $! 
\n";
     }else{
         open PLOT, "| /home/jaz/tools/x_plot" or die "Cannot open x_plot! $! \n";
     }

while(<RKTAB>){
    if (/TABLE (.*SIC*\w*)/){
      print PLOT "\"$opt{t} $1\n";
		print "\"$opt{t} $1\n";
		while(<RKTAB>){last if (/^DATA/);}
      while(<RKTAB>){
		  @AR = split(/,/,$_);
			
		  if (1*$AR[1] > 50000){
			print $AR[0]," ",$AR[1]," ",$AR[2]," ",$AR[2]/$AR[1],"\n";
	        print PLOT $AR[0]," ",$AR[2]/$AR[1],"\n";
			}
        last if (/^99999/);
      }
    }
}
