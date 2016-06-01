#!/usr/local/bin/perl
use Getopt::Long;

$usage= "
Usage: $0 [options] nnw1 nnw2 [...]
        Print training curves by using \"nnw\" files.

Options:
        -t  (test curves only)
        -r  (train curves only)
        -p  Generate printable postscript file.
";

@optl = ("t","r","p");
die $usage unless GetOptions(@optl);

if($opt_p){
  open PLOT, "| /home/bxz/bin/perl/x_plot -c Fig_plotnnw_$$.ps" or die "Cannot open x_plot!\n";
}else{
  open PLOT, "| /home/bxz/bin/perl/x_plot" or die "Cannot open x_plot!\n";
}

foreach $f (@ARGV){
  open(FI,"<$f") or die $usage;
  if (!$opt_t){
    print PLOT "\"$f Training rms:\n";
    while(<FI>){
      last if (/stats_train/);
    }
    $trash=<FI>;
    while(<FI>){
      last if (/\}/);
      chomp;
      split;
      print PLOT $_[0], " ", $_[5],"\n";
    }
    print PLOT "\n\n";
  }
  if (!$opt_r){
    print PLOT "\"$f Test rms:\n";
    while(<FI>){
      last if (/stats_tests/);
    }
    $trash=<FI>;
    while(<FI>){
      last if (/\}/);
      chomp;
      split;
      print PLOT $_[0], " ", $_[5],"\n";
    }
    print PLOT "\n\n";
  }
  close FI;
}
