package  Binning;
require  Exporter;

our @ISA    = qw(Exporter);
our @EXPORT = qw(binning);

# call
# @boundary = binning(\@var,     : variable array
#                     \@tag,     : variable array
#                      $v,       : verbose
#                      $min_pct, : minimum percentage for each bin,
#                                      (0,100])

sub binning {
    my($var, $tagUnSorted, $verbose, $minimum_pct) = @_;

    my $nVars=@$var;
    my $nTags=@$tagUnSorted;
    die "The number of samples mismatch between variable($nVars) and tag($nTags)!"
    if ($nVars!=$nTags);
    die "Less than 10 samples. Process terminated"
    if ($nVars<10);
    
    my $num_counts=$nVars;

#*************************************************************************#
# Define initial histogram with intervals $i
#*************************************************************************#

#--------------------------------------------------------------------------
# Each interval will have a $minimum_pct number of counts or 10 counts 
# (whatever is larger)
#--------------------------------------------------------------------------

    my $num_intervals = int(100.0/$minimum_pct);
    my $min_counts_per_interval = int($num_counts/$num_intervals);

    if ( $min_counts_per_interval < 10 ) {
        $min_counts_per_interval = 10;
        $num_intervals = int($num_counts/10);
        $minimum_pct =  $min_counts_per_interval * 100/$num_counts;
    }

    if ($verbose){
    print  STDERR "Input number of counts = $num_counts\n";
    print  STDERR "Preliminar number of intervals = $num_intervals\n";
    print  STDERR "Preliminar counts per interval = $min_counts_per_interval\n\n";
    }

#--------------------------------------------------------------------------
# sort variable values and tags
#--------------------------------------------------------------------------

    my (%var_value_not_sorted, %tag_not_sorted);
    for (my $j=1;$j<=$num_counts;$j++){
        $var_value_not_sorted{$j} = $var->[$j];
        $tag_not_sorted{$j} = $tagUnSorted->[$j];
    }

    my @index_sorted = sort {$var_value_not_sorted{$a} <=> $var_value_not_sorted{$b}} keys %var_value_not_sorted;

    my (@var_value, @tag);
    for (my $j=1;$j<=$num_counts;$j++){
        $var_value[$j] = $var_value_not_sorted{$index_sorted[$j-1]};
        $tag[$j] = $tag_not_sorted{$index_sorted[$j-1]};   
    }

#--------------------------------------------------------------------------
# define intervals $i
#
# $varvalue_at_interval[$i] : the value of the input variable that devides 
#                             segment $i-1 from segment $i 
# $cum_counts_at_interval[$i] : the cumulative number of counts in intervals 
#                               1 to $i
# note that a given input variable value can only be in one interval, so 
# intervals might be larger than requested in $min_counts_per_interval
#--------------------------------------------------------------------------

    my (@varvalue_at_interval, @cum_counts_at_interval);
    my (@counts_at_interval, @added, @added_min);
    $varvalue_at_interval[0] = $var_value[1];
    $cum_counts_at_interval[0] = 0;

    my $i=1;
    for (my $n=1;$n<=$num_counts;$n++) {
        $counts_at_interval[$i] = 0;
        $added[$i] = 0;
        $added_min[$i] = 0;
        for ( my $j=$n;$j<=$n+$min_counts_per_interval-1;$j++ ) {
            if ( $j > $num_counts ) { last; }
            $counts_at_interval[$i] +=1;
            $added_min[$i] +=1;
        }
        # go to next $n that has not been used 
        $n = $n + $added_min[$i];
        for ( $j=$n;$j<=$num_counts;$j++ ) {
            if ( $var_value[$j] != $var_value[$j-1] ) {
           last;
            } else {
           $counts_at_interval[$i] +=1;
           $added[$i] +=1;
            }	
        }    
        # go to last $n that has been used 
        $n = $n + $added[$i] - 1;
        $cum_counts_at_interval[$i] = $n;
        $varvalue_at_interval[$i] = $var_value[$n]; 
        $i += 1;
    }

    my $num_intervals_i = $i-1;

# if last interval has less than 90% of the required $min_counts_per_interval, 
# merge with previous one
       
    my $temp_counts_last = $cum_counts_at_interval[$num_intervals_i] 
                           - $cum_counts_at_interval[$num_intervals_i-1];
    if ( $temp_counts_last < 0.9*$min_counts_per_interval ){
        $cum_counts_at_interval[$num_intervals_i-1] = 
               $cum_counts_at_interval[$num_intervals_i];
        $varvalue_at_interval[$num_intervals_i-1] = 
               $varvalue_at_interval[$num_intervals_i];
        $num_intervals_i = $num_intervals_i-1;
    }

    if ($verbose){
    print STDERR "number of intervals i = $num_intervals_i (variable value not split among intervals) \n";
    }

#------------------------------------------------------------------------
# Calculate fraud (Y) and non-fraud (N) counts in each interval $i
#------------------------------------------------------------------------

    my (@num_Y, @num_N, @num_T);
    for (my $i=1;$i<=$num_intervals_i;$i++) { 
       $num_Y[$i] = 0; 
       $num_N[$i] = 0; 
       $num_T[$i] = 0; 
       for (my $n=$cum_counts_at_interval[$i-1]+1;
            $n<=$cum_counts_at_interval[$i];$n++ ) {
           $num_Y[$i] += $tag[$n];
       }
       $num_T[$i] = $cum_counts_at_interval[$i]-$cum_counts_at_interval[$i-1]; 
       $num_N[$i] = $num_T[$i] - $num_Y[$i]; 
    }


}

1;

