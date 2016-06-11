package Statistics;


require 5.00404;  

# KRH
$VERSION = '1.1';


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

sub reset {
    my $self = shift; 
    $self->{hist} = ();
    $self->{n}                     = 0;
    $self->{variance_n}            = 0;
    $self->{sum}                   = 0;
    $self->{mean}                  = 0;
    $self->{num_blank}             = 0;
    $self->{not_num}               = 0;
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

sub printQuantiles {
    my $self = shift;  ##Myself
    my $aref;
    my $cumulative;
    my @k_sort;
    my $avg;
    my $last_num;
    @k_sort = sort {$a<=>$b} keys %{$self->{hist}}; 
    if (ref $_[0] eq 'ARRAY') {
        $aref = $_[0];
    }
    else {
        $aref = \@_;
    }
    # Should only have to go through the hist once, revise if necessary
    QUANT:  foreach ( @{ $aref } ) {
      $cumulative = 0;
      $avg = "false";
      if(0 <= $_ && $_ <= 100 && $self->{n} >= 1){
       printf("%3.2f%% = ",$_);
       foreach $val (@k_sort){
         $cumulative +=  $self->{hist}{$val};
         if ($_ == 0){print $val,"\n";next QUANT;}
         if ($_/100 < $cumulative/$self->{n}){
           if($avg eq "true"){print (($val + $last_num) / 2,"\n");next QUANT;}
           if($avg eq "false"){print $val,"\n";next QUANT;} 
         }
         if ($_/100 == $cumulative/$self->{n}){
           if($_ == 100){print $val,"\n";next QUANT;}
           else{$avg = "true";$last_num = $val;}
         }
       }
      }
   }
}
    

sub getQuantile {
    my $self = shift;  ##Myself
    my $aref;
    my $cumulative;
    my @k_sort;
    my $avg;
    my $last_num;
    @k_sort = sort {$a<=>$b} keys %{$self->{hist}};
    if (ref $_[0] eq 'ARRAY') {
        $aref = $_[0];
    }
    else {
        $aref = \@_;
    }
    # Return the quantile for the FIRST VALID quantile percentage
    foreach ( @{ $aref } ) {
      $cumulative = 0;
      $avg = "false";
      if(0 <= $_ && $_ <= 100 && $self->{n} >= 1){
      foreach $val (@k_sort){
         $cumulative +=  $self->{hist}{$val};
         if ($_ == 0){return $val;}
         if ($_/100 < $cumulative/$self->{n}){
           if($avg eq "true"){return (($val + $last_num) / 2);}
           if($avg eq "false"){return $val;}
         }
         if ($_/100 == $cumulative/$self->{n}){
           if($_ == 100){return $val;}
           else{$avg = "true";$last_num = $val;}
         }
       }
      }
   }
}  
     
sub getRecordsSeen {
   my $self = shift;
   return ($self->{num_blank} + $self->{not_num} + $self->{n});
}
sub getNumBlank {
   my $self = shift;
   return $self->{num_blank};
}
sub getNumNonNumeric{
   my $self = shift;
   return $self->{not_num};
}
sub getCount{
   my $self = shift;
   return $self->{n};
}
sub getSum{
   my $self = shift;
   return $self->{sum};
}
sub getMean{
   my $self = shift;
   return $self->{mean};
}
sub getStdDev{
   my $self = shift;
   return (sqrt( $self->{variance_n}*($self->{n}/($self->{n}-1))) );
}
sub getVariance{
   my $self = shift;
   return $self->{variance_n}*($self->{n}/($self->{n}-1));
}

sub print {
    my $self = shift;
    my $opt = shift(@_);
    my $variance;
    my $stdev;


    if ($self->{n} > 1) {$variance = $self->{variance_n}*($self->{n}/($self->{n}-1));} 
    $stdev =  sqrt($variance);

    print "Overall Stats:\n";
    print "number of records seen = ", $self->{num_blank} + $self->{not_num} + $self->{n},"\n";
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
        print "range = ",getQuantile($self,100) - getQuantile($self,0),"\n"; 
        print "Quartiles:\n";
        printQuantiles $self,0,25,50,75,100;
        
    }  
}

