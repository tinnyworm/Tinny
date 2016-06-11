#! /usr/local/bin/perl -w
###############################################################################
#
#   Purpose: To get the length distribution of merchant ID for FP
#
#   arguments: no arguments
#
#   input    : cut mcc code and merchant ID first and then pipe to this program
#
#   Example  : zcat auths | ufalcut -c85-88,124-139 | len_MM_freq.pl
#
#   Revision :  yxc   01/21/2004   creator  
#
################################################################################

use Getopt::Std;

$opt_h = 0 ; 

$usage = "Usage: $0 [ -h ] \n\n" . 
         "  where :\n" . 
         "  -h    :   Show this message\n\n" .
         " Example:   for auths: zcat auths | ufalcut -c85-88,124-139 | $0\n\n" .
         "            for posts: zcat posts | ufalcut -c93-96,121-136 | $0\n\n" .
         " Note   : a null.list will be generated for Null information with MCC \n\n";
         
getopts('h');

die $usage if ($opt_h);

%lengthHash=qw();
$c_blank=0;
$length_m = 0;
$count = 0;
$max=0;
$chart_length=60;
$null=0;
open (OUTNULL, ">null.list") ;
while ($_ = <STDIN>){
  chomp;
  $count ++;
  $line=substr($_,4,16);
  if ($line =~ /\S/) {
         $line =~ s/^\s+//;
         $line =~ s/\s+$//;
         $content = $line;
	   if ($content =~ /^[ 0]*$/){
	      $null ++;
 	      print OUTNULL "$_\n";     
	   } else {
          $length_m = length($line);
	 ($lengthHash{$length_m}++) or ($lengthHash{$length_m} =1);
         if ($max < $lengthHash{$length_m}) {$max=$lengthHash{$length_m};}
         }
  } else {
       $c_blank ++; 
  }           
  if ($max < $c_blank) {$max=$c_blank;}
  if ($max < $null) {$max=$null;}
}
close (OUTNULL);

print ("The frequency of the length of Merchant ID\n");
print "\n";
printf ("%8s |%-${chart_length}s  %10s  %9s %9s\n","length", " ", "frequency", "percent", "cumulative");

@l_sort = sort {$a<=> $b} keys %lengthHash ;
foreach $length (@l_sort){
   $value = $lengthHash{$length};
   $cumu += $value;
   $chart_line = "*"x(int($chart_length*($value/$max)));
   $ratio_v =  sprintf ("%.2f", 100*$value/$count);
   $ratio_cumu =  sprintf ("%.2f", 100*$cumu/$count);
   printf("%8s |%-${chart_length}s  %10d  %6.2f%%  %6.2f%%\n",
	 $length,
         $chart_line,
	 $value,
         $ratio_v,
	 $ratio_cumu);
  }

$ratio_blank= sprintf ("%.2f", 100*$c_blank/$count);
$ratio_null = sprintf ("%.2f", 100*$null/$count);
$cumu_blank = $cumu + $c_blank;
$cumu_null  = $cumu_blank + $null ;
$ratio_cblank  = sprintf ("%.2f", 100*$cumu_blank/$count);
$ratio_cnull   = sprintf ("%.2f", 100*$cumu_null/$count);
$chart_line_b  = "*"x(int($chart_length*($c_blank/$max)));
$chart_line_nn = "*"x(int($chart_length*($null/$max)));
printf "%8s |%-${chart_length}s  %10d  %6.2f%%  %6.2f%%\n", "blank", $chart_line_b, $c_blank , $ratio_blank, $ratio_cblank ;
printf "%8s |%-${chart_length}s  %10d  %6.2f%%  %6.2f%%\n", "Null", $chart_line_nn, $null , $ratio_null, $ratio_cnull ;
print "\n";
