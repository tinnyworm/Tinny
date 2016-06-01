#! /usr/local/bin/perl 
#
#
use Getopt::Long;

$n = 0;
$num_blank = 0;
$variance_n = 0;

while (<>) {
  chomp;
  if ($_ eq " "x(length $_)){$num_blank++;}
  else {
    $hist{$_}++;
    $n++;
    $sum += $_;
    $mean += ($_ - $mean) / $n;
    if ($n > 1) {
      $variance_n += ((($_ - $mean) * ($_ - $mean)) / ($n-1)) - ($variance_n / $n);
    }

  }
}
## print STDERR "$.\n";

@k_sort = sort {$a<=>$b} keys %hist;

$min = "NO_VALUE";
$median_n_low = sprintf("%d",($n + 1)/2);
$median_n_high = sprintf("%d",($n + 2)/2);
$n_Q1 = $median_n_low;
if ($median_n_low == $median_n_high){$n_Q1--;}
$Q1_n_low = sprintf("%d",($n_Q1+1)/2);
$Q1_n_high = sprintf("%d",($n_Q1+2)/2);
$Q3_n_low = $n + 1 - $Q1_n_high;
$Q3_n_high = $n + 1 - $Q1_n_low;





$Q1_low = "NO_VALUE";
$Q1_high = "NO_VALUE";
$median_low = "NO_VALUE";
$median_high = "NO_VALUE";
$Q3_low = "NO_VALUE";
$Q3_high = "NO_VALUE";

foreach $val (@k_sort){
  $cumulative +=  $hist{$val};
  if ($min eq  "NO_VALUE"){$min = $val;}
  if ($Q1_low eq "NO_VALUE" && $cumulative >= $Q1_n_low){$Q1_low = $val;}
  if ($Q1_high eq "NO_VALUE" && $cumulative >= $Q1_n_high){$Q1_high = $val;}
  if ($median_low eq "NO_VALUE" && $cumulative >= $median_n_low){$median_low = $val;}
  if ($median_high eq "NO_VALUE" &&  $cumulative >= $median_n_low){$median_high = $val;}
  if ($Q3_low eq "NO_VALUE" && $cumulative >= $Q3_n_low){$Q3_low = $val;}
  if ($Q3_high eq "NO_VALUE" && $cumulative >= $Q3_n_high){$Q3_high = $val;}
  $max = $val;
}
$Q1 = ($Q1_low + $Q1_high)/2;
$median = ($median_low + $median_high)/2;
$Q3 = ($Q3_low + $Q3_high)/2;

$variance = $variance_n*($n/($n-1)); 
$stdev =  sqrt($variance);
print "count =\t ", $n , "\n";
if ($n >= 1) {
  print "sum =\t " ,$sum, "\n";
  print "mean =\t ", $mean,"\n";
  if ($n >= 2) { 
    print "standard deviation =\t ", $stdev,"\n";
    print "variance =\t ", $variance,"\n";
  }
  print "min =\t ", $min,"\n";
  print "Q1 =\t ", $Q1,"\n";
  print "medin =\t ",$median,"\n";
  print "Q3 =\t ", $Q3, "\n";
  print "max =\t ", $max,"\n";
  print "range =\t ", $max - $min, "\n"; 
}  
print "number blank =\t ", $num_blank,"   ";
printf("(%6.2f%%)\n", ($num_blank/($num_blank + $n))*100);


