package Statistics;


require 5.00404;  
                
# KRH
$VERSION = '1.0';


use POSIX qw/ceil/;

package Statistics::Statistics; 
use Utils;
# use strict;
use vars qw($VERSION $AUTOLOAD %fields);
use Carp;

##Define the fields to be used as methods
%fields = (
  n                     => 0,
  variance_n            => 0,
  sum                   => 0,
  mean                  => 0,
  num_blank             => 0,
  not_num               => 0,
  );

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;

  my $self = {
    %fields,
    _permitted => \%fields,
  };

  $self->{hist} = ();
  bless ($self, $class);
  return $self;
}


sub add_data {
  my $self = shift;  ##Myself
  my $aref;

  if (ref $_[0] eq 'ARRAY') {
    $aref = $_[0];
  }
  else {
    $aref = \@_;
  }
  foreach ( @{ $aref } ) {
    if($_ eq " "x(length $_)){$self->{num_blank}++;}
    elsif(!defined(str2num($_))){$self->{not_num}++;}
    else{
    $self->{hist}{$_}++;
    $self->{n}++;
    $self->{sum} += $_;
    $self->{mean} += ($_ - $self->{mean}) / $self->{n};
    if ($self->{n} > 1) {
      $self->{variance_n} += ((($_ - $self->{mean}) * ($_ - $self->{mean})) / ($self->{n}-1)) - ($self->{variance_n} / $self->{n});
    }
    }

  }
}


sub print {
  my $self = shift;
  my $opt = shift(@_);
  my @k_sort;
  @k_sort = sort {$a<=>$b} keys %{$self->{hist}};
  my $min;
  my $median_n_low;
  my $median_n_high;
  my $n_Q1;
  my $Q1_n_low;
  my $Q1_n_high; 
  my $Q3_n_low;
  my $Q3_n_high;
  my $Q1_low;
  my $Q1_high;
  my $median_low;
  my $median_high;
  my $Q3_low;
  my $Q3_high;
  my $max;
  my $Q1;
  my $median;
  my $Q3;
  my $cumulative;
  my $val;
  my $variance;
  my $stdev;

$min = "NO_VALUE";
$median_n_low = sprintf("%d",($self->{n} + 1)/2);
$median_n_high = sprintf("%d",($self->{n} + 2)/2);
$n_Q1 = $median_n_low;
if ($median_n_low == $median_n_high){$n_Q1--;}
$Q1_n_low = sprintf("%d",($n_Q1+1)/2);
$Q1_n_high = sprintf("%d",($n_Q1+2)/2);
$Q3_n_low = $self->{n} + 1 - $Q1_n_high;
$Q3_n_high = $self->{n} + 1 - $Q1_n_low;





$Q1_low = "NO_VALUE";
$Q1_high = "NO_VALUE";
$median_low = "NO_VALUE";
$median_high = "NO_VALUE";
$Q3_low = "NO_VALUE";
$Q3_high = "NO_VALUE";


$cumulative = 0;
foreach $val (@k_sort){
  $cumulative +=  $self->{hist}{$val};
  if ($min eq  "NO_VALUE"){$min = $val;}
  if ($Q1_low eq "NO_VALUE" && $cumulative >= $Q1_n_low){$Q1_low = $val;}
  if ($Q1_high eq "NO_VALUE" && $cumulative >= $Q1_n_high){$Q1_high = $val;}
  if ($median_low eq "NO_VALUE" && $cumulative >= $median_n_low){$median_low = $val;}
  if ($median_high eq "NO_VALUE" &&  $cumulative >= $median_n_low){$median_high = $val;}
  if ($Q3_low eq "NO_VALUE" && $cumulative >= $Q3_n_low){$Q3_low = $val;}
  if ($Q3_high eq "NO_VALUE" && $cumulative >= $Q3_n_high){$Q3_high = $val;}
  $max = $val;
}
if($Q3_high eq "NO_VALUE"){$Q3_high = $Q3_low;}


$Q1 = ($Q1_low + $Q1_high)/2;
$median = ($median_low + $median_high)/2;
$Q3 = ($Q3_low + $Q3_high)/2;

if ($self->{n} > 1) {$variance = $self->{variance_n}*($self->{n}/($self->{n}-1));} 
$stdev =  sqrt($variance);

print "Overall Stats:\n";
print "Number of records seen = ", $self->{num_blank} + $self->{not_num} + $self->{n},"\n";
print "number blank = ", $self->{num_blank},"   ";
printf("(%6.2f%%)\n", ($self->{num_blank}/($self->{num_blank} + $self->{not_num} +$self->{n}))*100);
print "number not numeric = ", $self->{not_num},"   ";
printf("(%6.2f%%)\n", ($self->{not_num}/($self->{num_blank} + $self->{not_num} + $self->{n}))*100);


print "\nNumeric Stats:\n";
print "count = ", $self->{n} , "\n";
if ($self->{n} >= 1) {
  print "sum = " ,$self->{sum}, "\n";
  print "mean = ", $self->{mean},"\n";
  if ($self->{n} >= 2) { 
    print "standard deviation = ", $stdev,"\n";
    print "variance = ", $variance,"\n";
  }
  print "min = ", $min,"\n";
  print "Q1 = ", $Q1,"\n";
  print "median = ",$median,"\n";
  print "Q3 = ", $Q3, "\n";
  print "max = ", $max,"\n";
  print "range = ", $max - $min, "\n"; 

}  
}

