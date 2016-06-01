#! /usr/local/bin/perl
#
#
use Getopt::Long;
$chart_length = 60;
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
##  if(!($. % 1000)){
##    print STDERR "$.\r";
##  }
  chomp ($line);
  if($opt_b){
    $line = $opt_b * int($line/$opt_b + 1);
  }
  $hist{$line}++;
  $total++;
  if ($hist{$line}>$max) {$max++;}
  if ((length $line)>$val_length){$val_length = length $line;}
  if ((length $hist{$line})>$count_length){$count_length = length $hist{$line};} 
}
## print STDERR "$.\n";

$cumulative=0;
$cumulative_length = length $total;
if($opt_s){
  @k_sort = sort {$hist{$b}<=>$hist{$a}} keys %hist;
} elsif($opt_n || $opt_b){
  @k_sort = sort {$a<=>$b} keys %hist;
} else {
  @k_sort = sort {$a cmp $b} keys %hist;
}

##foreach $val (sort {$opt_n? $a<=>$b:$a cmp $b} keys %hist){
foreach $val (@k_sort){
  $cumulative +=  $hist{$val};
  $chart_line = "*"x(int($chart_length*($hist{$val}/$max)));
  printf("%-${val_length}s |%-${chart_length}s  %${count_length}d  %${cumulative_length}d  %6.2f%%  %6.2f%%\n",
	 $val,
         $chart_line,
	 $hist{$val},
         $cumulative,
	 100*$hist{$val}/$total,
	 100*$cumulative/$total);
}



