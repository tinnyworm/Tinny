#! /usr/local/bin/perl
#
#
use Getopt::Long;

$usage = "$0 [options]\n".
  "Outputs histogram (and cumulative) of things it reads from STDIN.\n".
  "Options:\n".
  "\t -n      assumes input are numeric.\n".
  "\t -b bin  uses bin size (bin) - only makes sense for numeric items\n".
  "\t -s      sort by frequency, not token\n\n"; 

@optl = ("help","n","s","b=f");
die $usage unless GetOptions(@optl);

die $usage if($opt_help);

while ($line = <STDIN>) {
  if(!($. % 10000)){
    print STDERR "$.\r";
  }
  chop ($line);
  if($opt_b){
    $line = $opt_b * int($line/$opt_b + 1);
  }
  $hist{$line}++;
  $total++;
}
print STDERR "$.\n";

$cumulative=0;
if($opt_s){
  @k_sort = sort {$hist{$b}<=>$hist{$a}} keys %hist;
} elsif($opt_n){
  @k_sort = sort {$a<=>$b} keys %hist;
} else {
  @k_sort = sort {$a cmp $b} keys %hist;
}

##foreach $val (sort {$opt_n? $a<=>$b:$a cmp $b} keys %hist){
foreach $val (@k_sort){
  $cumulative +=  $hist{$val};
  printf("%s,  %10d,  %6.2f%%,  %6.2f%%,  %6.2f%%\n",
	 $val,
	 $hist{$val},
	 100*$hist{$val}/$total,
	 100*$cumulative/$total,
	 100-100*$cumulative/$total);
}

