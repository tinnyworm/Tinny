#! /usr/local/bin/perl

# Includes
use Getopt::Std;
getopts('hvzasl:d:m:n:c:b:');

# Combined header/help text
if ($opt_h) {
    print <<"EOF";

 NAME
     new_split.pl - eFalcon supervised binning script

 DESCRIPTION
     Imports two columns data (space separated) with "tag variable_value".
     and outputs the optimal supervised binning. 

     The script starts by creating an initial histogram which is a set of small initial bins.
     The boundaries of these initial bins are considered later as posible boundaries for
     the final binning of the variable and the script will determine which is
     the best subset of boundaries to keep by minimizing the entropy.

     Minimum size of bins in initial histogram is <hist_min_pct> measured as a percentage 
     of input counts (default is hist_min_pct=0.1 which means 0.1% = 1000 bins). 
     Minimum size of output bins is <bin_min_pct> measured as a percentage 
     of input counts (bin_min_pct=1 means output bins must at least have 1% of data).
     The initial set of histogram bins is created using equal population, but is adjusted if
     needed so that a given value of the variable is not split across two contiguous bins.  

     Program creates 1 split per iteration by minimizing entropy. 
     Splitting stops when chi2 of next split is smaller than requested <chi2_min> 
     (default is 5.0) or the number of desired bins <number_bins> is reached. 
     Tags = "1", "Y", "F" and "S" are considered fraud, and any other is non fraud.
     Variable_value is numerical (negatives values allowed).
 
 OPTIONS
  -h                Prints this help
  -d <data>         OPTIONAL input file, two columns with "tag variable_value"
  -m <hist_min_pct> OPTIONAL input number (0-100) is the size for bins 
                             to use in initial histogram measured in % of total 
			     input counts. Defaults to 0.1 meaning create an initial 
                             histogram with 0.1% data in each bin equivalent to 1000 initial bins 
  -b <bin_min_pct>  OPTIONAL input number (0-100) is the minimum allowed size 
                             for output bins measured in % of total input counts. Defaults to 
                             values provided in -m option and if missing then to 0.1% 
  -n <number_bins>  OPTIONAL input number, is the number of bins to be created
  -c <chi2_min>     OPTIONAL input number, minimum value of chi2 to accept an split (smaller -c more bins)
  -v                OPTIONAL will print more detailed information on the split process
  -s                OPTIONAL special fraud_rate=(#Y_bin+0.5)/(#Y_bin+#N_bin+1/(2*tot_fraud_rate)) 
                             used instead of fraud_rate=#Y_bin/(#Y_bin+#N_bin)
  -z                OPTIONAL do not accept bins with 0 frauds 						 
  -a                OPTIONAL outputs the raw variable average and standard deviation in each bin   
                             for frauds and non-frauds 
 
 SIMPLEST EXAMPLE

input_data_file = two columns format with "tag" and "var_value" 

This will use an initial partition histogram with 1000 bins, output bins will have at least 
0.1% of data, splitting will stop when chi2 > 5. 

  cat input_data_file | perl new_split.pl 

Verbose added

  cat input_data_file | perl new_split.pl -v

						 
 EXAMPLES WITH OPTIONS

i) request 10 output bins

  cat input_data_file | perl new_split.pl -n 10       

ii) request 10 output bins and that initial histogram and output bins have at least 1% data

  cat input_data_file | perl new_split.pl -n 10 -m 1  

iii) request 10 output bins and that initial histogram has 1% data (100 bins) and output 
bins have at least 10% of data

  cat input_data_file | perl new_split.pl -n 10 -m 1 -b 10

iv) request 10 output bins and fraud rate reported uses the corrected formula

  cat input_data_file | perl new_split.pl -n 10 -s

v) request 10 output bins and sooner stopping criteria at chi2 = 10 instead of the default value 5.

  cat input_data_file | perl new_split.pl -n 10 -c 10


 BUGS CORRECTIONS
     2/1/2005 a \$ was missing for variable \$left_countsT (search bug and 2/1/2005 below)
     3/29/2005 < replaced by <= (search bug and 3/29/2005 below) 
     3/29/2005 the output table reports now one bin for cases of constant input var (search bug and 3/29/2005 below) 
     4/21/2006 sort logic of var values rewritten (bug first var value was considered blank before)
     5/10/2006 option s was being defined with argument so was not active

 NEWS
     4/1/2005 introduced option -s to use special formula to evaluate fraud rate  
     5/26/2005 now -m and -b are different options
     5/26/2005 -z option added to request all bins have at least 1 fraud as optional
     4/21/2006 -a option added to output avg and stdev in each bins   
 AUTHOR
     Gabriela Surpi, July 2004
EOF
    exit;
}
;


open (TEMP1, "> temp1");
open (TEMP2, "> temp2");

$start_time = localtime(time);
$start_sec = time;

#*************************************************************************#
#=========================================================================#
# Identify names of input and output files
#=========================================================================#
#*************************************************************************#

#Initial histogram bin size:
if ( $opt_m > 0 and $opt_m <= 100 ) {
    $minimum_pct = $opt_m;
} else {
    $minimum_pct = 0.1;
}

#Output final bins minimum size: 
if ( $opt_b > 0 and $opt_b <= 100 ) {
    $bin_min_pct = $opt_b;
} else {
    $bin_min_pct = $minimum_pct;
}

$file_data = $opt_d;
$number_bins_requested = $opt_n;
$chi2_requested = $opt_c;

#*************************************************************************#
#=========================================================================#
# Input data file with "tag variable_values"
#=========================================================================#
#*************************************************************************#

$n=0;
if ( $opt_d ) {
    open (STDIN, "cat $file_data |") or die "Couldn't open data file\n";
}
while ($stats = <STDIN>) {
    @fld = split(" ",$stats);
    $fld[0] =~ s/ //g;
    $fld[1] =~ s/ //g;

    $n +=1;

    #------------------------------------------------------------------------------------------------------------
    # Define tag
    #------------------------------------------------------------------------------------------------------------

    if ($fld[0] =~ /Y|F|S/ || int($fld[0]) == "1") {
	$tag_not_sorted[$n]=1;
    } else {
	$tag_not_sorted[$n]=0;
    }

    #------------------------------------------------------------------------------------------------------------
    # Define variable value and check that is numeric
    #------------------------------------------------------------------------------------------------------------

    #    CHECK THAT INPUT VAR IS NUMERICAL
    #    $temp_only_num = $fld[1]; $temp_only_num =~ s/\.//g; 
    #    if ( substr($temp_only_num,0,1) eq "-" ) { $temp_only_num = substr($temp_only_num,1,length($temp_only_num));}
    #    $temp_only_num =~ s/ //g;
    #    if ( $temp_only_num eq "" || $temp_only_num =~ /\D/) {
    #	die "Variable value \"$fld[1]\" in record number $n is non-numeric. Process terminated\n";
    #    }
    #    END CHECK THAT INPUT VAR IS NUMERICAL

    $var_value_not_sorted[$n] = $fld[1];
}
close DAT;

$num_counts = $n;
if ( $num_counts < 10 ) {
    die "Less that 10 input values. Process terminated\n";
}

#*************************************************************************#
#=========================================================================#
# Define initial histogram with intervals $i
#=========================================================================#
#*************************************************************************#

#------------------------------------------------------------------------------------------------------------
# Each interval will have a $minimum_pct number of counts or 10 counts (whatever is larger)
#------------------------------------------------------------------------------------------------------------

$num_intervals = int(100.0/$minimum_pct);
$min_counts_per_interval = int($num_counts/$num_intervals);

if ( $min_counts_per_interval < 10 ) {
    $min_counts_per_interval = 10;
    $num_intervals = int($num_counts/10);
    $minimum_pct =  $min_counts_per_interval * 100/$num_counts;
}

if ($opt_v) {
    print  "Input number of counts = $num_counts\n";
    print  "Preliminar number of intervals = $num_intervals\n";
    print  "Preliminar counts per interval = $min_counts_per_interval\n\n";
}

#------------------------------------------------------------------------------------------------------------
# sort variable values and tags
#------------------------------------------------------------------------------------------------------------

#for ( $j=1;$j<=$num_counts;$j++){
#   $arr_not_sorted[$j]=$var_value_not_sorted[$j]._.$tag_not_sorted[$j];   
#}
#@arr = sort {$a<=>$b}(@arr_not_sorted) ;
#for ( $j=1;$j<=$num_counts;$j++){
#    @fld = split("_",$arr[$j]);
#    $var_value[$j] = $fld[0];
#    $tag[$j] = $fld[1];
#}

for ( $j=1;$j<=$num_counts;$j++) {
    $var_value_not_sorted{$j} = $var_value_not_sorted[$j];
    $tag_not_sorted{$j} = $tag_not_sorted[$j];
    print TEMP1 "$tag_not_sorted[$j] $var_value_not_sorted[$j]\n";
}

@index_sorted = sort {$var_value_not_sorted{$a} <=> $var_value_not_sorted{$b}} keys %var_value_not_sorted;

for ( $j=1;$j<=$num_counts;$j++) {
    $var_value[$j] = $var_value_not_sorted{$index_sorted[$j-1]};
    $tag[$j] = $tag_not_sorted{$index_sorted[$j-1]};   
    print TEMP2 "$tag[$j] $var_value[$j]\n";
}

#------------------------------------------------------------------------------------------------------------
# define intervals $i
#------------------------------------------------------------------------------------------------------------

# $varvalue_at_interval[$i] is the value of the input variable that devides segment $i-1 from segment $i 
# $cum_counts_at_interval[$i] is the cumulative number of counts in intervals 1 to $i
# note that a given input variable value can only be in one interval, so intervals might be larger than
# requested in $min_counts_per_interval

$varvalue_at_interval[0] = $var_value[1];
$cum_counts_at_interval[0] = 0;

$i=1;
for ($n=1;$n<=$num_counts;$n++) {
    $counts_at_interval[$i] = 0;
    $added[$i] = 0;
    $added_min[$i] = 0;
    for ( $j=$n;$j<=$n+$min_counts_per_interval-1;$j++ ) {
	if ( $j > $num_counts ) {
	    last;
	}
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

$num_intervals_i = $i-1;

# if last interval has less than 90% of the required $min_counts_per_interval, merge with previous one
	
$temp_counts_last = $cum_counts_at_interval[$num_intervals_i] - $cum_counts_at_interval[$num_intervals_i-1];
if ( $temp_counts_last < 0.9*$min_counts_per_interval ) {
    $cum_counts_at_interval[$num_intervals_i-1] = $cum_counts_at_interval[$num_intervals_i];
    $varvalue_at_interval[$num_intervals_i-1] = $varvalue_at_interval[$num_intervals_i];
    $num_intervals_i = $num_intervals_i-1;
}

if ($opt_v) {
    print  "number of intervals i = $num_intervals_i (variable value not split among intervals) \n";
}

#------------------------------------------------------------------------------------------------------------
# Calculate fraud (Y) and non-fraud (N) counts in each interval $i
#------------------------------------------------------------------------------------------------------------

for ($i=1;$i<=$num_intervals_i;$i++) { 
    $num_Y[$i] = 0; 
    $num_N[$i] = 0; 
    $num_T[$i] = 0; 
    for ( $n=$cum_counts_at_interval[$i-1]+1;$n<=$cum_counts_at_interval[$i];$n++ ) {
	$num_Y[$i] += $tag[$n];
    }
    $num_T[$i] = $cum_counts_at_interval[$i] - $cum_counts_at_interval[$i-1]; 
    $num_N[$i] = $num_T[$i] - $num_Y[$i]; 
}

#*************************************************************************#
#=========================================================================#
# If -z used require that intervals have at least 1 fraud and 1 non-fraud: 
# merge intervals $i with entropy 0 and create a new set of intervals $ii
#=========================================================================#
#*************************************************************************#

# Entropy is zero if $num_N[$i] == 0 || $num_Y[$i] == 0

if ($opt_z) {

    #------------------------------------------------------------------------------------------------------------
    # Find the last interval with entropy > 0 and merge together any interval on the right to that one
    #------------------------------------------------------------------------------------------------------------

    for ($i=1;$i<=$num_intervals_i;$i++) { 
	if ( $num_N[$i] > 0 && $num_Y[$i] > 0 ) {
	    $interval_last_entropy_gt_0 = $i;
	}
    }

    $i = $interval_last_entropy_gt_0;
    for ($j=$interval_last_entropy_gt_0+1;$j<=$num_intervals_i;$j++) { 
	$num_Y[$i] += $num_Y[$j]; 
	$num_N[$i] += $num_N[$j]; 
	$num_T[$i] += $num_T[$j];
    } 

    if ( $num_N[$i] == 0 || $num_Y[$i] == 0 ) {
	$entropy[$i] = 0;
    } else {
	$entropy[$i] = -($num_N[$i]/$num_T[$i])*log10($num_N[$i]/$num_T[$i])
	    -($num_Y[$i]/$num_T[$i])*log10($num_Y[$i]/$num_T[$i]);
    }

    $varvalue_at_interval[$i] = $varvalue_at_interval[$num_intervals_i];
    $cum_counts_at_interval[$i] = $cum_counts_at_interval[$num_intervals_i];

    $num_intervals_i = $interval_last_entropy_gt_0;

    if ($opt_v) {
	print  "number of intervals i = $num_intervals_i (1 time intervals with entropy 0 merged to the left)\n";
    }

    #------------------------------------------------------------------------------------------------------------
    # Multiple merge of intervals $i with entropy 0 to intervals to the right and create a new set of intervals $ii
    #------------------------------------------------------------------------------------------------------------

    $ii =0;
    for ($i=1;$i<=$num_intervals_i;$i++) { 
	if ( $num_N[$i] > 0 && $num_Y[$i] > 0 ) {
	    $ii += 1;
	    $num_Y[$ii] = $num_Y[$i]; 
	    $num_N[$ii] = $num_N[$i]; 
	    $num_T[$ii] = $num_T[$i]; 

	    if ( $num_N[$i] == 0 || $num_Y[$i] == 0 ) {
		$entropy[$ii] = 0;
	    } else {
		$entropy[$ii] = -($num_N[$i]/$num_T[$i])*log10($num_N[$i]/$num_T[$i])
		    -($num_Y[$i]/$num_T[$i])*log10($num_Y[$i]/$num_T[$i]);
	    }

	    $varvalue_at_interval[$ii] = $varvalue_at_interval[$i];
	    $cum_counts_at_interval[$ii] = $cum_counts_at_interval[$i];
	} else {
	    $ii += 1;
	    $num_Y[$ii] = $num_Y[$i]; 
	    $num_N[$ii] = $num_N[$i]; 
	    for ( $j=$i+1;$j<=$num_intervals_i;$j++ ) {
		$num_Y[$ii] += $num_Y[$j];
		$num_N[$ii] += $num_N[$j];
		if ( $num_Y[$ii] > 0 && $num_N[$ii] > 0 ) {
		    $num_T[$ii] = $num_Y[$ii] + $num_N[$ii];

		    if ( $num_N[$ii] == 0 || $num_Y[$ii] == 0 ) {
			$entropy[$ii] = 0;
		    } else {
			$entropy[$ii] = -($num_N[$ii]/$num_T[$ii])*log10($num_N[$ii]/$num_T[$ii])
			    -($num_Y[$ii]/$num_T[$ii])*log10($num_Y[$ii]/$num_T[$ii]);
		    }
		    $varvalue_at_interval[$ii] = $varvalue_at_interval[$j];
		    $cum_counts_at_interval[$ii] = $cum_counts_at_interval[$j];
		    last;
		}
	    }
	    $i = $j;
	}
    }
    $num_intervals_ii = $ii;

} else {
    $num_intervals_ii = $num_intervals_i;
} 

if ($opt_v) {
    print  "number of intervals ii = $num_intervals_ii (multiple intervals with entropy 0 merged to the right)\n\n";
}

#------------------------------------------------------------------------------------------------------------
# Calculate initial entropy and fraud rate
#------------------------------------------------------------------------------------------------------------

$countsT = 0; $countsY = 0; $countsN = 0;
 
for ( $ii=1; $ii<=$num_intervals_ii;$ii++ ) {
    $countsY += $num_Y[$ii];
    $countsN += $num_N[$ii];
}
$countsT = $countsY + $countsN;

if ( $countsY == 0 || $countsN == 0 ) {
    $entropy_initial = 0;
} else {
    $entropy_initial = -($countsY/$countsT)*log10($countsY/$countsT)
	-($countsN/$countsT)*log10($countsN/$countsT);
}

$tot_fraud_rate = $countsY/$countsT;
$tot_Y = $countsY;
$tot_N = $countsN;
$tot_T = $countsT;

#*************************************************************************#
#=========================================================================#
# Subroutine to evaluate best cut and entropy gain when an split is done
#=========================================================================#
#*************************************************************************#

# intial segment of data to be split in two is ($start,$end] where $start and $end are $ii interval values
#
# inputs are: $start and $end 
# outputs are: $best_cut $entropy_gain
#

# USE
#
# @output = best_cut($start,$end);
# $best_cut[$start][$end] = $output[0]; 
# $entropy_gain[$start][$end] = $output[1]; 
#
# $entropy_no_cut = $output[2];
# $entropy_with_cut = $output[3];
#
# $chi2_min_entropy = $output[4];
# $chi2_pct_data = $output[5];
#

#------------------------------------------------------------------------------------------------------------
# DEFINITION
#------------------------------------------------------------------------------------------------------------

sub best_cut {
    my $start = $_[0];
    my $end = $_[1];

    #------------------------------------------------------------------------------------------------------------
    # Entropy with no cut - 1 segment 
    #------------------------------------------------------------------------------------------------------------

    $countsT = 0; $countsY = 0; $countsN = 0;

    for ( $j=$start+1; $j<=$end;$j++ ) {
	$countsY += $num_Y[$j];
	$countsN += $num_N[$j];
    }
    $countsT = $countsY + $countsN;

    if ( $countsY == 0 || $countsN == 0 ) {
	$entropy_no_cut = 0;
    } else {
	$entropy_no_cut = -($countsY/$countsT)*log10($countsY/$countsT)
	    -($countsN/$countsT)*log10($countsN/$countsT); 
    }

    #------------------------------------------------------------------------------------------------------------
    # Min entropy with cut - 1 segment (cut creates two bins "left" and "right")
    #------------------------------------------------------------------------------------------------------------

    $entropy_gain = 1;
    $chi2_pct_data = $countsT * 100.0/$tot_T;

    for ($cut=$start+1; $cut<=$end-1;$cut++) {
	$left_countsT = 0; $left_countsY = 0; $left_countsN = 0; $left_entropy = 0;
	$right_countsT = 0; $right_countsY = 0; $right_countsN = 0; $right_entropy = 0;

	for ( $j=$start+1; $j<=$cut;$j++ ) {
	    $left_countsY += $num_Y[$j];
	    $left_countsN += $num_N[$j];
	} 
	$left_countsT = $left_countsY + $left_countsN;

	if ( $left_countsY == 0 || $left_countsN == 0 ) {
	    $left_entropy = 0;
	} else {
	    $left_entropy = -($left_countsY/$left_countsT)*log10($left_countsY/$left_countsT)
		-($left_countsN/$left_countsT)*log10($left_countsN/$left_countsT);
	}

	for ( $j=$cut+1; $j<=$end;$j++ ) {
	    $right_countsY += $num_Y[$j];
	    $right_countsN += $num_N[$j];
	}
	$right_countsT = $right_countsY + $right_countsN;

	if ( $right_countsY == 0 || $right_countsN == 0 ) {
	    $right_entropy = 0;
	} else {
	    $right_entropy = -($right_countsY/$right_countsT)*log10($right_countsY/$right_countsT)
		-($right_countsN/$right_countsT)*log10($right_countsN/$right_countsT);
	}
    
	$temp_entropy_with_cut = ($left_countsT*$left_entropy+$right_countsT*$right_entropy)/
	    ($left_countsT+$right_countsT);

	#$temp_entropy_gain should always be negative, the more negative the better the split
	$temp_entropy_gain = (($right_countsT+$left_countsT)/$tot_T) * ($temp_entropy_with_cut - $entropy_no_cut);

	if ( ($left_countsT*100.0/$tot_T >= $bin_min_pct) && ($right_countsT*100.0/$tot_T >= $bin_min_pct) && 
		 (($entropy_gain > 0) || ($temp_entropy_gain < $entropy_gain )) ) {

	    $cut_min_entropy = $cut;

	    #next line bug fixed 2/1/2005 $ sign added in front of left_countsT

	    $tmp_fraud_rate = ($right_countsY+$left_countsY)/($right_countsT+$left_countsT);

	    if ( $tmp_fraud_rate == 0 ) { 
		$chi2_min_entropy = ($right_countsN - (1-$tmp_fraud_rate)*$right_countsT)**2/((1-$tmp_fraud_rate)*$right_countsT)
		    + ($left_countsN - (1-$tmp_fraud_rate)*$left_countsT)**2/((1-$tmp_fraud_rate)*$left_countsT);
	    } elsif ( $tmp_fraud_rate == 1 ) { 
		$chi2_min_entropy = ($right_countsY - $tmp_fraud_rate*$right_countsT)**2/($tmp_fraud_rate*$right_countsT)
		    + ($left_countsY - $tmp_fraud_rate*$left_countsT)**2/($tmp_fraud_rate*$left_countsT);
	    } else {
		$chi2_min_entropy = ($right_countsN - (1-$tmp_fraud_rate)*$right_countsT)**2/((1-$tmp_fraud_rate)*$right_countsT)
		    + ($right_countsY - $tmp_fraud_rate*$right_countsT)**2/($tmp_fraud_rate*$right_countsT)
			+ ($left_countsN - (1-$tmp_fraud_rate)*$left_countsT)**2/((1-$tmp_fraud_rate)*$left_countsT)
			    + ($left_countsY - $tmp_fraud_rate*$left_countsT)**2/($tmp_fraud_rate*$left_countsT);
	    }

	    $chi2_pct_data = ($right_countsT+$left_countsT) * 100.0/$tot_T;
	    $chi2_pct_data_right = ($right_countsT) * 100.0/$tot_T;
	    $chi2_pct_data_left = ($left_countsT) * 100.0/$tot_T;
	    $entropy_gain = $temp_entropy_gain;
	    $entropy_with_cut = $temp_entropy_with_cut;

	}
    }

    # it can be at this point that none of the posible cuts would have the minimum requested % of 
    # counts $bin_min_pct in the new 2 bins to be created. In that case not cut was found, and 
    # so far $entropy_gain keeps being =1. 

    return($cut_min_entropy,$entropy_gain,$entropy_no_cut,$entropy_with_cut,$chi2_min_entropy,$chi2_pct_data,$chi2_pct_data_right,$chi2_pct_data_left);

}

#*************************************************************************#
#=========================================================================#
# Summary so far
#=========================================================================#
#*************************************************************************#

# intervals created $ii = 1, ... , $num_intervals_ii
# variables calculated at each interval:
# $num_T[$ii] = total number of counts in interval
# $num_Y[$ii] = total number of frauds in interval
# $num_N[$ii] = total number of non-frauds in interval
# $entropy[$ii] = entropy
# $cum_counts_at_interval[$ii] = cumulative counts in intervals $ii=1,...$ii
# $varvalue_at_interval[$ii] = variable value at the large end of interval $ii
# 
# subroutine best_cut to split a segment into 2 bins, segment is ($start_ii,$end_ii]
#
# @output = best_cut($start_ii,$end_ii);
# $best_cut[$start_ii][$end_ii] = $output[0]; 
# $entropy_gain[$start_ii][$end_ii] = $output[1]; 
#
# $entropy_no_cut = $output[2];
# $entropy_with_cut = $output[3];
# $chi2_min_entropy = $output[4];
# $chi2_pct_data = $output[5];
# $chi2_pct_data_right = $output[6];
# $chi2_pct_data_left = $output[7];

#*************************************************************************#
#=========================================================================#
# Doing the actual binning - splits $l 
#=========================================================================#
#*************************************************************************#

#------------------------------------------------------------------------------------------------------------
# Stopping criteria with either number of bins requested or chi2 requested
#------------------------------------------------------------------------------------------------------------

# number of cuts = number of splits that will be done in data
# number if final bins = number of cuts +1;
	
# $number_bins_requested = user's requested number of cuts   
# $bins_max = maximum number of allowed cuts at this time 

$bins_max = $num_intervals_ii ;

if ( $opt_n && $number_bins_requested < $bins_max ) {
    $bins_max = $number_bins_requested ;
} 

if ( $opt_c && $chi2_requested > 0 ) {
    $chi2_to_stop = $chi2_requested;
} else {
    $chi2_to_stop = 5.0;
}

$entropy_current = $entropy_initial;

for ($l=1;$l<=$bins_max -1;$l++) {

    #------------------------------------------------------------------------------------------------------------
    # Loop in the number of cuts, each new $l run means a new cut being processed
    #------------------------------------------------------------------------------------------------------------

    if ($l==1) {
	$cut_accepted[0] = 0;
	$cut_accepted[1] = $num_intervals_ii;
	$num_cuts = @cut_accepted;	#$num_cuts defined as 2 = the boundaries
    } 

    if ($opt_v) {
	print  "*---------------------------------*\n";
	print  "*    Processing cut number = $l    *\n";
	print  "*---------------------------------*\n";
    }

    $min_S = 1;
    $entropy_gain = 0;

    for ( $i=1; $i<= $num_cuts-1; $i++) {

	#------------------------------------------------------------------------------------------------------------
	# Loop in the different possible cuts $i from which an optimal one is choosed 
	#------------------------------------------------------------------------------------------------------------

	### calculate the entropy gain of cut $i if it hasn't been already

	if ( $cut_accepted[$i] > $cut_accepted[$i-1] + 1 ) {
	    if ( not exists $entropy_gain[$cut_accepted[$i-1]][$cut_accepted[$i]] ) {
		@output = best_cut($cut_accepted[$i-1],$cut_accepted[$i]);

		$best_cut[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[0]; 
		$entropy_gain[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[1]; 
		$chi2_min_entropy[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[4];
		$chi2_pct_data[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[5];
		$chi2_pct_data_right[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[6];
		$chi2_pct_data_left[$cut_accepted[$i-1]][$cut_accepted[$i]] = $output[7];
	    }

	    ### keep parameters of cut $i 

	    $best_cut = $best_cut[$cut_accepted[$i-1]][$cut_accepted[$i]]; 
	    $entropy_gain = $entropy_gain[$cut_accepted[$i-1]][$cut_accepted[$i]]; 
	    $chi2_min_entropy = $chi2_min_entropy[$cut_accepted[$i-1]][$cut_accepted[$i]];
	    $chi2_pct_data = $chi2_pct_data[$cut_accepted[$i-1]][$cut_accepted[$i]];
	    $chi2_pct_data_right = $chi2_pct_data_right[$cut_accepted[$i-1]][$cut_accepted[$i]];
	    $chi2_pct_data_left = $chi2_pct_data_left[$cut_accepted[$i-1]][$cut_accepted[$i]];

	    # here if the best cut has entropy_gain=1 means that there was not good cut found 	
	    # as all posible cuts would produce an smaller data% in the bins than the required

	    if ($opt_v) {
		if ( $entropy_gain != 1) {	    
		    print  "segment($cut_accepted[$i-1], $cut_accepted[$i]\] 
           best_cut=$best_cut pct_of_data=$chi2_pct_data pct_of_data_right=$chi2_pct_data_right pct_of_data_left=$chi2_pct_data_left  
           entropy_gain=$entropy_gain
           chi2_min_entropy=$chi2_min_entropy\n";
		} else {
		    print  "segment($cut_accepted[$i-1], $cut_accepted[$i]\] 
           best_cut= not found, because pct_of_data=$chi2_pct_data to be cut produces bins smaller than bin_min_pct = $bin_min_pct\n";
		}
	    }
    
  
	    ### accept first cut $i or any posterior one that minimizes even more the entropy

	    # next line bug fixed 3/29/2005 uses "<=" instead of "<"

	    if ( (not exists $cut_accepted[$num_cuts]) || ( $entropy_gain <= $min_S )) {	

		#	print "\n entropy_gain = $entropy_gain    min_S = $min_S\n";

		$cut_accepted[$num_cuts] = $best_cut;
		$min_S = $entropy_gain;
		$chi2_min_S=$chi2_min_entropy;
		$chi2_pct_data_min_S=$chi2_pct_data;
		$chi2_pct_data_min_S_left=$chi2_pct_data_left;
		$chi2_pct_data_min_S_right=$chi2_pct_data_right;

		#	print "entropy_gain = $entropy_gain    min_S = $min_S\n\n";

	    }
	}
    }

    ### accept cut being evaluated if satisfies the following requirements 
    ### 1) a cut was found ( $entropy_gain != 1)
    ### 2) produce a chi2 < $chi2_to_stop
    ### 3) create bins with the min% required $bin_min_pct
    ### 4) do not exceed maximum number of bins required

    if ( $min_S != 1 &&
	     ($chi2_min_S > $chi2_to_stop) &&
		 ($chi2_pct_data_min_S_right >= $bin_min_pct && $chi2_pct_data_min_S_left >= $bin_min_pct) &&
		     $num_cuts <= $bins_max + 1 ) {

	@cuts_sorted = sort {$a<=>$b}(@cut_accepted) ;
	@cut_accepted = @cuts_sorted;

	$num_cuts = @cut_accepted;
	$entropy_current = $entropy_current + $min_S;

	if ($opt_v) {
	    print  "\n\nnum_boundaries_so_far=$num_cuts cut_boundaries_locations=@cut_accepted\n";
	    print  "entropy_gain=$entropy_gain entropy_current=$entropy_current entropy_initial=$entropy_initial\n"; 
	    print  "chi2_pct_data_min_S=$chi2_pct_data_min_S chi2_min_S=$chi2_min_S\n\n";
	}

	if ($num_cuts == $bins_max + 1) {
	    $stop_criteria = "\n\nStopping criteria: maximum allowed number of bins = $bins_max was reached";
	} 
    } else {
	if ( $chi2_min_S <= $chi2_to_stop ) { 	
	    $stop_criteria = "\n\nStopping criteria: X2 = $chi2_to_stop reached. Next cut with niminum entropy would have X2 < $chi2_to_stop";
	}
	if ( $min_S == 1 || $chi2_pct_data_min_S_right < $bin_min_pct || $chi2_pct_data_min_S_left < $bin_min_pct ) {
	    $stop_criteria = "\n\nStopping criteria: no further split with more than $bin_min_pct\% data in new bins are found";
	}
	last;
    }

}

#*************************************************************************#
#=========================================================================#
# Calculate average var value and standard deviation for each bin Y & N separetely
#=========================================================================#
#*************************************************************************#

if ($opt_a && ($num_cuts-1>1)) {

    for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	$thres[$cut] = $varvalue_at_interval[$cut_accepted[$cut]];
    }

    for ( $j=1;$j<=$num_counts;$j++) {
	for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	    if ($var_value[$j] <= $thres[$cut]) {
		if ( $tag[$j] == 1 ) {
		    $counts_Y{$thres[$cut]} += 1;
		    $var_sum_Y{$thres[$cut]} += $var_value[$j];
		    $var_sq_sum_Y{$thres[$cut]} += $var_value[$j]*$var_value[$j];
		} else {
		    $counts_N{$thres[$cut]} += 1;
		    $var_sum_N{$thres[$cut]} += $var_value[$j];
		    $var_sq_sum_N{$thres[$cut]} += $var_value[$j]*$var_value[$j];
		}            
		last;
	    }
	}
    }
    for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	$counts_tot_Y += $counts_Y{$thres[$cut]};
	$var_sum_tot_Y += $var_sum_Y{$thres[$cut]};
	$var_sq_sum_tot_Y += $var_sq_sum_Y{$thres[$cut]};
	$counts_tot_N += $counts_N{$thres[$cut]};
	$var_sum_tot_N += $var_sum_N{$thres[$cut]};
	$var_sq_sum_tot_N += $var_sq_sum_N{$thres[$cut]};
    }
    if ($counts_tot_Y-1 >0) {
	$factor_tot_Y = 1/($counts_tot_Y-1);
    } else {
	$factor_tot_Y =0;
    }
    if ( $counts_tot_Y >0) {
	$var_avg_tot_Y = $var_sum_tot_Y/$counts_tot_Y;
    } else {
	$var_avg_tot_Y =0;
    }
    $var_std_tot_Y = $factor_tot_Y * ( $counts_tot_Y * $var_avg_tot_Y**2 - 2 * $var_avg_tot_Y * $var_sum_tot_Y + $var_sq_sum_tot_Y );
    if ($var_std_tot_Y<0) {
	$var_std_tot_Y=0;
    }
    $var_std_tot_Y = sqrt($var_std_tot_Y);
    if ($counts_tot_N-1 >0) {
	$factor_tot_N = 1/($counts_tot_N-1);
    } else {
	$factor_tot_N =0;
    }
    if ( $counts_tot_N >0) {
	$var_avg_tot_N = $var_sum_tot_N/$counts_tot_N;
    } else {
	$var_avg_tot_N =0;
    }
    $var_std_tot_N = $factor_tot_N * ( $counts_tot_N * $var_avg_tot_N**2 - 2 * $var_avg_tot_N * $var_sum_tot_N + $var_sq_sum_tot_N );
    if ($var_std_tot_N<0) {
	$var_std_tot_N=0;
    }
    $var_std_tot_N = sqrt($var_std_tot_N);

    for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	if ($counts_Y{$thres[$cut]}-1 >0) {
	    $factor_Y = 1/($counts_Y{$thres[$cut]}-1);
	} else {
	    $factor_Y =0;
	}
	if ($counts_Y{$thres[$cut]} >0) {
	    $var_avg_Y{$thres[$cut]} = $var_sum_Y{$thres[$cut]}/$counts_Y{$thres[$cut]};
	} else {
	    $var_avg_Y{$thres[$cut]} = 0;
	}
	$var_std_Y{$thres[$cut]} = $factor_Y * ( $counts_Y{$thres[$cut]} * $var_avg_Y{$thres[$cut]}**2 
						     - 2 * $var_avg_Y{$thres[$cut]} * $var_sum_Y{$thres[$cut]} + $var_sq_sum_Y{$thres[$cut]} );
	if ($var_std_Y{$thres[$cut]}<0) {
	    $var_std_Y{$thres[$cut]}=0;
	}
	$var_std_Y{$thres[$cut]} = sqrt($var_std_Y{$thres[$cut]});
	#    print "$thres[$cut] $counts_Y{$thres[$cut]} $var_sum_Y{$thres[$cut]} $var_sq_sum_Y{$thres[$cut]} $var_avg_Y{$thres[$cut]} $var_std_Y{$thres[$cut]}\n";
	if ($counts_N{$thres[$cut]}-1 >0) {
	    $factor_N = 1/($counts_N{$thres[$cut]}-1);
	} else {
	    $factor_N =0;
	}
	if ($counts_N{$thres[$cut]} >0) {
	    $var_avg_N{$thres[$cut]} = $var_sum_N{$thres[$cut]}/$counts_N{$thres[$cut]};
	} else {
	    $var_avg_N{$thres[$cut]} = 0;
	}
	$var_std_N{$thres[$cut]} = $factor_N * ( $counts_N{$thres[$cut]} * $var_avg_N{$thres[$cut]}**2 
						     - 2 * $var_avg_N{$thres[$cut]} * $var_sum_N{$thres[$cut]} + $var_sq_sum_N{$thres[$cut]} );
	if ($var_std_N{$thres[$cut]}<0) {
	    $var_std_N{$thres[$cut]}=0;
	}
	$var_std_N{$thres[$cut]} = sqrt($var_std_N{$thres[$cut]});
	#    print "$thres[$cut] $counts_N{$thres[$cut]} $var_sum_N{$thres[$cut]} $var_sq_sum_N{$thres[$cut]} $var_avg_N{$thres[$cut]} $var_std_N{$thres[$cut]}\n";
    }

}

#*************************************************************************#
#=========================================================================#
# Print Summary Output
#=========================================================================#
#*************************************************************************#

print  "\n*--------------------------------------------------------------------------------------------*\n";
print  "*                                     Output Summary                                         *\n";
print  "*--------------------------------------------------------------------------------------------*\n\n";

# BINS 

printf   ("%40s %10s%10s%10s   %7s  %7s\n",
	  "        Variable Value Interval       ","    #(Y)","    #(N)","  #(Y+N)","  %(Y+N)","Fraud_rate");
printf   ("%40s %10s%10s%10s   %7s  %7s\n",
	  "--------------------------------------","--------","--------","--------","--------","----------");

# special case the input var was constant

# Restrict bin boundaries values to at most 14 char

for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
    if (length($varvalue_at_interval[$cut_accepted[$cut]])>14) {
	$varvalue_at_interval[$cut_accepted[$cut]]=substr($varvalue_at_interval[$cut_accepted[$cut]],0,14);
    }
}

# bug fixed 3/29/2005 now it reports output for case of constant var

if ( $num_intervals_ii == 1 ) {
    printf   ("%1s%14s%9s%15s%2s%10d%10d%10d   %6.2f%1s    %7f\n",
	      "(",$varvalue_at_interval[0]," <= x <= ",$varvalue_at_interval[1],")", 
	      $tot_Y, $tot_N, $tot_T,100.0,"%", $tot_fraud_rate);
}

# input var not constant 

for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
    $countsY = 0;
    $countsN = 0;
    for ( $ii=$cut_accepted[$cut-1]+1;$ii<=$cut_accepted[$cut];$ii++ ) {
	$countsY += $num_Y[$ii];
	$countsN += $num_N[$ii];
    }
    $cut_min_var_value = $varvalue_at_interval[$cut_accepted[$cut-1]];
    $cut_max_var_value = $varvalue_at_interval[$cut_accepted[$cut]];
    $countsYN=$countsY+$countsN;
    if ($opt_s) {
	$cut_fraud_rate = ($countsY+0.5)/($countsYN+1/(2*$tot_fraud_rate));
    } else {
	$cut_fraud_rate = $countsY/$countsYN;
    }

    $cut_percentage = $countsYN*100/$tot_T;

    if ($varvalue_at_interval[$cut_accepted[$cut-1]] == $varvalue_at_interval[$cut_accepted[$cut]]) {
	printf   ("%1s%14s%9s%15s%2s%10d%10d%10d   %6.2f%1s    %7f\n",
		  "(",$varvalue_at_interval[$cut_accepted[$cut-1]]," <= x <= ",$varvalue_at_interval[$cut_accepted[$cut]],")", 
		  $countsY, $countsN, $countsYN,$cut_percentage,"%", $cut_fraud_rate);
    } else {
	printf   ("%1s%15s%8s%15s%2s%10d%10d%10d   %6.2f%1s    %7f\n",
		  "(",$varvalue_at_interval[$cut_accepted[$cut-1]]," < x <= ",$varvalue_at_interval[$cut_accepted[$cut]],")", 
		  $countsY, $countsN, $countsYN,$cut_percentage,"%", $cut_fraud_rate);
    }

    $counts_Y[$cut] = $countsY;
    $counts_N[$cut] = $countsN;
    $fraud_rate[$cut] = $cut_fraud_rate;

}

printf   ("%40s %10s%10s%10s   %7s  %7s\n",
	  "--------------------------------------","--------","--------","--------","--------","----------");
printf   ("%40s %10d%10d%10d   %6.2f%1s    %7f\n",
	  "               All Data               ", 
	  $tot_Y, $tot_N, $tot_T,100.0,"%", $tot_fraud_rate);

#-------------------------------------------------------------------------#
if ($opt_a) {
    #-------------------------------------------------------------------------#

    print  "\n*==================================================================================================*\n\n";

    # BINS 

    printf   ("%36s %9s%8s  %7s  %7s  %7s  %7s\n",
	      "      Variable Value Interval     "," #(Y+N)","  #(Y)","Average(N)","Average(Y)","StdDev(N)","StdDev(Y)");
    printf   ("%36s %9s%8s  %7s  %7s  %7s  %7s\n",
	      "----------------------------------","-------","------","----------","----------","---------","---------");

    # special case the input var was constant

    # Restrict bin boundaries values to at most 14 char

    for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	if (length($varvalue_at_interval[$cut_accepted[$cut]])>12) {
	    $varvalue_at_interval[$cut_accepted[$cut]]=substr($varvalue_at_interval[$cut_accepted[$cut]],0,12);
	}
	if (length($var_avg_Y{$thres[$cut]})>9) {
	    $var_avg_Y{$thres[$cut]} = substr($var_avg_Y{$thres[$cut]},0,9);
	}
	if (length($var_std_Y{$thres[$cut]})>9) {
	    $var_std_Y{$thres[$cut]} = substr($var_std_Y{$thres[$cut]},0,9);
	}
	if (length($var_avg_tot_Y)>9) {
	    $var_avg_tot_Y = substr($var_avg_tot_Y,0,9);
	}
	if (length($var_std_tot_Y)>9) {
	    $var_std_tot_Y = substr($var_std_tot_Y,0,9);
	}
	if (length($var_avg_N{$thres[$cut]})>9) {
	    $var_avg_N{$thres[$cut]} = substr($var_avg_N{$thres[$cut]},0,9);
	}
	if (length($var_std_N{$thres[$cut]})>9) {
	    $var_std_N{$thres[$cut]} = substr($var_std_N{$thres[$cut]},0,9);
	}
	if (length($var_avg_tot_N)>9) {
	    $var_avg_tot_N = substr($var_avg_tot_N,0,9);
	}
	if (length($var_std_tot_N)>9) {
	    $var_std_tot_N = substr($var_std_tot_N,0,9);
	}
    }

    # bug fixed 3/29/2005 now it reports output for case of constant var

    if ( $num_intervals_ii == 1 ) {
	printf   ("%1s%12s%9s%13s%2s%9d%8d  %9s  %9s  %9s  %9s\n",
		  "(",$varvalue_at_interval[0]," <= x <= ",$varvalue_at_interval[1],")", 
		  $tot_T, $tot_Y, $var_avg_tot_N, $var_avg_tot_Y, $var_std_tot_N, $var_std_tot_Y);
    }

    # input var not constant 

    for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
	$countsY = 0;
	$countsN = 0;
	for ( $ii=$cut_accepted[$cut-1]+1;$ii<=$cut_accepted[$cut];$ii++ ) {
	    $countsY += $num_Y[$ii];
	    $countsN += $num_N[$ii];
	}
	$cut_min_var_value = $varvalue_at_interval[$cut_accepted[$cut-1]];
	$cut_max_var_value = $varvalue_at_interval[$cut_accepted[$cut]];
	$countsYN=$countsY+$countsN;
	if ($opt_s) {
	    $cut_fraud_rate = ($countsY+0.5)/($countsYN+1/(2*$tot_fraud_rate));
	} else {
	    $cut_fraud_rate = $countsY/$countsYN;
	}

	$cut_percentage = $countsYN*100/$tot_T;

	if ($varvalue_at_interval[$cut_accepted[$cut-1]] == $varvalue_at_interval[$cut_accepted[$cut]]) {
	    printf   ("%1s%12s%9s%13s%2s%9d%8d    %9s   %9s  %9s  %9s\n",
		      "(",$varvalue_at_interval[$cut_accepted[$cut-1]]," <= x <= ",$varvalue_at_interval[$cut_accepted[$cut]],")", 
		      $countsYN, $countsY,$var_avg_N{$thres[$cut]},$var_avg_Y{$thres[$cut]},$var_std_N{$thres[$cut]},$var_std_Y{$thres[$cut]});
	} else {
	    printf   ("%1s%13s%8s%13s%2s%9d%8d   %9s   %9s  %9s  %9s\n",
		      "(",$varvalue_at_interval[$cut_accepted[$cut-1]]," < x <= ",$varvalue_at_interval[$cut_accepted[$cut]],")", 
		      $countsYN, $countsY,$var_avg_N{$thres[$cut]},$var_avg_Y{$thres[$cut]},$var_std_N{$thres[$cut]},$var_std_Y{$thres[$cut]});
	}

	$counts_Y[$cut] = $countsY;
	$counts_N[$cut] = $countsN;
	$fraud_rate[$cut] = $cut_fraud_rate;

    }

    printf   ("%36s %9s%8s  %7s  %7s  %7s  %7s\n",
	      "----------------------------------","-------","------","----------","----------","---------","---------");
    printf   ("%36s %9d%8d   %9s   %9s  %9s  %9s\n",
	      "             All Data             ", 
	      $tot_T, $tot_Y,$var_avg_tot_N,$var_avg_tot_Y,$var_std_tot_N,$var_std_tot_Y);

    #-------------------------------------------------------------------------#
}
#-------------------------------------------------------------------------#


#-------------------------------------------------------------------------#
# correlation with fraud (no sense unless test/train split, otherwise
# the larger the number of bins the higher the correlation
#-------------------------------------------------------------------------#

##for ( $cut=1;$cut<=$num_cuts-1;$cut++ ) {
##    $bin_count += ( $counts_Y[$cut] + $counts_N[$cut] );
##    $bin_var_sum += ( $counts_Y[$cut] + $counts_N[$cut] ) * $fraud_rate[$cut];
##    $bin_var_sq_sum += ( $counts_Y[$cut] + $counts_N[$cut] ) * $fraud_rate[$cut] * $fraud_rate[$cut];
##    $bin_tag_sum += ( $counts_Y[$cut] * 1.0 + $counts_N[$cut] * 0.0 );
##    $bin_var_tag_sum += ( $counts_Y[$cut] * 1.0 * $fraud_rate[$cut] + $counts_N[$cut] * 0.0 * $fraud_rate[$cut]);
##    $bin_tag_sq_sum += ( $counts_Y[$cut] * 1.0 * 1.0 + $counts_N[$cut] * 0.0 * 0.0 );
##}
##$bin_var_avg = $bin_var_sum/$bin_count;
##$bin_tag_avg = $bin_tag_sum/$bin_count;

##    $factor = 1/($bin_count-1);
##    $bin_var_std = $factor * ( $bin_count * $bin_var_avg**2 - 2 * $bin_var_avg * $bin_var_sum + $bin_var_sq_sum );
##    $bin_var_std = sqrt($bin_var_std);
##    $bin_tag_std = $factor * ( $bin_count * $bin_tag_avg**2 - 2 * $bin_tag_avg * $bin_tag_sum + $bin_tag_sq_sum );
##    $bin_tag_std = sqrt($bin_tag_std);
   
##    $covariance = $factor * ($bin_count * $bin_var_avg*$bin_tag_avg 
##                                   - $bin_var_avg*$bin_tag_sum
##                                   - $bin_var_sum*$bin_tag_avg
##                                   + $bin_var_tag_sum);
    
##    $correlation = $covariance / ($bin_var_std * $bin_tag_std);

##print "\nCorrelation of binned risk variable with tag = $correlation";

#----------- Stopping criteria ----------------#

print $stop_criteria;

#----------- Running time ----------------#

$end_time = localtime(time);
$end_sec= time;
$delta_sec = $end_sec-$start_sec;
$delta_min = $delta_sec/60;
if ($delta_sec <=60 ) {
    print  "\n\nRunning time = $delta_sec seconds\n\n";
} else {
    print  "\nRunning time = $delta_min minutes\n\n";
}

#----------- Subroutine logarithm in base 10 ----------------#

sub log10 {
    my $n = shift;
    return log($n)/log(10);
}

#$log10 = log10(10);
#$log20 = log10(20);
#print "$log10 $log20\n";

#----------- End ----------------#


