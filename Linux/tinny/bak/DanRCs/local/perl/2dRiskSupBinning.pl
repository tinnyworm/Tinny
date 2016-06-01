#!/usr/bin/env perl
use Getopt::Long;
use File::Temp qw/ tempfile /;


####################################################################################
#   Purpose:  Create two dimensional risk tables with 
#             dynamic binning of the second split variable 
#             within the groups of the first split
#
#   Authors:  bxz created sicxamt_risk.pl
#             krh modified this code to add the dynamic binning and addtional options
######################################################################################


###  Define default parameters
$DEFAULT_MIN_THRESHOLD=5;
$DEFAULT_ACCURACY=2;

$DEFAULT_MIN_FRAUDCOUNT = 50;
$DEFAULT_MAX_GROUPS = 10;
$DEFAULT_MIN_VALUE = 0.01;
$BIN_VOLUME_LIMIT = -1;

$INCLUDE_AMT_STATS = 0;
$INCLUDE_DIST_STATS = 1;
$USE_DEFAULT_AMT_STATS = 0;
$USE_DEFAULT_DIST_STATS = 0;

$IS_FRAUD_index = 0;
$AMT_index = 7;
$DIST_index = 8;
$SPLIT1_index = 3;
$SPLIT2_index = 7;
@defaultlabel = ("IS_FRAUD","IS_KEY","HOW","SIC","POSTAL","CNTRY","CURR","AMT","DIST","VEL","USER1","USER2");


$usage= "
Generate 2 dimensional risktables from input data file. 
The following input data file format is assumed:
  IS_FRAUD, IS_KEY, HOW, SIC, POSTAL, CNTRY, CURR, AMT, DIST,VEL[,USER1][,USER2]

Usage: zcat input_data.gz|$0 [options] > amttable
The typical usage is to pipe in the risk data.  
The risk files can also be listed after the options - THEY ARE ASSUMED TO BE GZIPPED
Options:
        -a <accuracy>           Print accuracy for floating point numbers.
                                Default: 2
        -M <Min_threshold>      Table records will be drooped, if number of 
                                transactions in it < Min_threshold.
                                Default: 5 
        -n                      NO stddev fixing. Otherwise, if (0<stddev<1), 
                                the stddev will be set to 0.0.
        -f <Min_fraud_count>    Min fraud count per group.  
                                Default: 50
        -g <Max_groups>         Max number of groups a table can have.
                                Default: 10
        -mv <Min_value>         Min value of the continuous variable being binned
                                Default: 0.01
        -lb <limit_bin_data>    Set the limit of the number of data points used in calculating one binning
                                Used to solve memory problems with finding bins on huge datasets
                                Default: Use all data with no limit on volume 
        -s1 <Split1_index>      Field index of the first split variable.
                                Default: 3       - For SIC
        -l1 <Label_for_Split1>  Label of the first split variable.
                                Default: string provide in file format for split1    
        -s2 <Split2_index>      Field index of the second split variable.
                                Default: 7       - For AMT
        -l2 <Label_for_Split2>  Label of the second split variable.
                                Default: string provide in file format for split2   
        -aa                     Add amount statistics.
                                Default: Does not include amount stats
        -rd                     Remove distance statistics.
                                Default: Includes distance stats
        -da                     Use default amount statistics for each group.
                                Default: Calculate separate amount stats for each group.
        -dd                     Use default distance statistics for each group.
                                Default: Calculates separate distance stats for each group.
         

Note:  Field index start at 0 (i.e. IS_FRAUD index = 0 and so on)
       Split1 should be a categorical variable (no binning is performed on this variable)
       Split2 should be a numeric variable (binning will be applied)
";



###  Get command line options
 
@optl = ("a=f","M=i","n","f=i","g=i","mv=f","lb=f","s1=i","s2=i","l1=s","l2=s","aa","rd","da","dd");
die $usage unless GetOptions(@optl);

$accuracy=$opt_a?$opt_a:$DEFAULT_ACCURACY;
$min_threshold=$opt_M?$opt_M:$DEFAULT_MIN_THRESHOLD;
$fix_stddev=$opt_n?0:1;
$DEFAULT_MIN_FRAUDCOUNT=$opt_f?$opt_f:$DEFAULT_MIN_FRAUDCOUNT;
$DEFAULT_MAX_GROUPS = $opt_g?$opt_g:$DEFAULT_MAX_GROUPS;
$MIN_VALUE = (defined $opt_mv)?$opt_mv:$DEFAULT_MIN_VALUE;
$BIN_VOLUME_LIMIT = (defined $opt_lb and $opt_lb > 0)?$opt_lb:$BIN_VOLUME_LIMIT;
$SPLIT1_index = $opt_s1?$opt_s1:$SPLIT1_index;
$SPLIT2_index = $opt_s2?$opt_s2:$SPLIT2_index;
$LABEL1 = $opt_l1?$opt_l1:$defaultlabel[$SPLIT1_index]; 
$LABEL2 = $opt_l2?$opt_l2:$defaultlabel[$SPLIT2_index]; 
$INCLUDE_AMT_STATS = $opt_aa?1:$INCLUDE_AMT_STATS;
$INCLUDE_DIST_STATS = $opt_rd?0:$INCLUDE_DIST_STATS;
$USE_DEFAULT_AMT_STATS = $opt_da?1:$USE_DEFAULT_AMT_STATS;
$USE_DEFAULT_DIST_STATS = $opt_dd?1:$USE_DEFAULT_DIST_STATS;
foreach $file (@ARGV){$filestr .= "$file ";}
$stdin = $filestr?0:1;

#######################################################################
#  Make first pass of the data to store split2 values for fraud records
#######################################################################
if($stdin){
 (undef,$tempfile) = tempfile("tempfile_d2dr.XXXXXXXXXXXX",DIR => ".",SUFFIX => ".dat.gz");
 open(TEMPOUT,"| gzip -c > $tempfile")  || die "cannot open $tempfile";
 $in = STDIN;
}else{
  open($in,"gunzip -c $filestr |") || die "cannot read $filestr";
}

while(<$in>){
  print TEMPOUT $_ if $stdin;
  chomp;
  @data = split /,/;
  ($IS_FRAUD,$AMT,$DIST,$SPLIT1,$SPLIT2) = @data[$IS_FRAUD_index,$AMT_index,$DIST_index,$SPLIT1_index,$SPLIT2_index];
  if($IS_FRAUD > 0.5){push @{$fsplit{$SPLIT1}}, $SPLIT2 if $BIN_VOLUME_LIMIT < 0 or scalar @{$fsplit{$SPLIT1}} < $BIN_VOLUME_LIMIT;}
}
close($in);
close(TEMPOUT) if $stdin;


####################################################################
#  Foreach split 1 value create the binning of the split 2 variable
####################################################################
foreach $split (keys %fsplit){
  $fraudtrans = scalar @{$fsplit{$split}};
  $groups = $fraudtrans / $DEFAULT_MIN_FRAUDCOUNT > $DEFAULT_MAX_GROUPS ? $DEFAULT_MAX_GROUPS : int($fraudtrans / $DEFAULT_MIN_FRAUDCOUNT);
  $step = $groups > 0 ? int($fraudtrans/$groups): $DEFAULT_MIN_FRAUDCOUNT;
  $n=1;
  @temp = sort {$a<=>$b} @{$fsplit{$split}};
  while(defined($s2 = shift @temp)){
     if($n % $step == 0 and scalar @temp >= $DEFAULT_MIN_FRAUDCOUNT and $s2 > (defined(${$amt_BDR{$split}}[-1])?${$amt_BDR{$split}}[-1]:$MIN_VALUE))
        {push @{$amt_BDR{$split}},$s2;}
     push @defsplit,$s2;
     $n++; 
  }
  unshift @{$amt_BDR{$split}},$MIN_VALUE;
}  

###  Create binning of the split 2 variable for the default entry
$fraudtrans = scalar @defsplit;
$groups = $fraudtrans / $DEFAULT_MIN_FRAUDCOUNT > $DEFAULT_MAX_GROUPS ? $DEFAULT_MAX_GROUPS : int($fraudtrans / $DEFAULT_MIN_FRAUDCOUNT);
$step = $groups > 0 ? int($fraudtrans/$groups): $DEFAULT_MIN_FRAUDCOUNT;
$n=1;
@temp = sort {$a<=>$b} @defsplit;
while(defined($s2 = shift @temp)){
  if($n % $step == 0 and scalar @temp >= $DEFAULT_MIN_FRAUDCOUNT and $s2 > (defined(${$amt_BDR{"def"}}[-1])?${$amt_BDR{"def"}}[-1]:$MIN_VALUE)){push @{$amt_BDR{"def"}},$s2;}
  $n++;
}
unshift @{$amt_BDR{"def"}},$MIN_VALUE;



###################################################################
#  Second pass of the data, track necessary statistics for each bin 
###################################################################
$def_SICXAMT={};
for ($bin=0;$bin<scalar(@{ $amt_BDR{"def"} });$bin++){
  $def_SICXAMT->{$bin}=[0,0,0,0,0,0,0,0,0,0];
}

if($stdin){
  open(TEMP,"gunzip -c $tempfile |")  || die "cannot open $tempfile";
}else{
  open(TEMP,"gunzip -c $filestr |") || die "cannot read $filestr";
}
while(<TEMP>){
  chomp;
  ### read record and find bin numbers
  @data = split /,/;
  ($IS_FRAUD,$AMT,$DIST,$SPLIT1,$SPLIT2) = @data[$IS_FRAUD_index,$AMT_index,$DIST_index,$SPLIT1_index,$SPLIT2_index];
  $IS_FRAUD=($IS_FRAUD > 0.5);
  $bin=grep( {$_ <= $SPLIT2} @{ $amt_BDR{$SPLIT1} })-1;
  $bindef=grep( {$_ <= $SPLIT2} @{ $amt_BDR{"def"} })-1;
  next if ($bin < 0);


  ###  Update bin statistics 
  updateSICXAMT($def_SICXAMT->{$bindef});

#  next if ($SIC<0 || $SIC>9999);
  $SICXAMT_table{$SPLIT1}={} unless (defined $SICXAMT_table{$SPLIT1});
  $SICXAMT_table{$SPLIT1}->{$bin}=[0,0,0,0,0,0,0,0,0,0] 
        unless (defined $SICXAMT_table{$SPLIT1}->{$bin});
  updateSICXAMT($SICXAMT_table{$SPLIT1}->{$bin});
}
close(TEMP);
system("rm $tempfile") if $stdin;




###########################################################################
#  Summarize and clean the bin statistics to calculate final values for risk tables
#  Print the two dimintional risk table rule engine code
############################################################################

print "
//
//************************************************
//* Auth ${LABEL1} x ${LABEL2} risk tables (4.0)
//************************************************
//
";

foreach $k (sort {$a<=>$b} keys %SICXAMT_table){
  $SICXAMT_def{$k}=summarizeSICXAMT($SICXAMT_table{$k});
  if ($SICXAMT_def{$k}->[0]<$min_threshold or scalar @{ $amt_BDR{$k} } < 2){
    delete $SICXAMT_table{$k};
    delete $SICXAMT_def{$k};
  }else{
    cleanup("A_SICxAMT_$k",$SICXAMT_def{$k},$SICXAMT_table{$k});
    SICXAMT_print("A_${LABEL1}x${LABEL2}_$k","${LABEL2}_RISK_DATA",$SICXAMT_def{$k},$SICXAMT_table{$k},@{ $amt_BDR{$k} });
  }
}
$def_SICXAMT_def=summarizeSICXAMT($def_SICXAMT);
SICXAMT_print("DEF_A_${LABEL1}x${LABEL2}","${LABEL2}_RISK_DATA",$def_SICXAMT_def,$def_SICXAMT,@{ $amt_BDR{"def"}});

print "
TABLE A_${LABEL1}x${LABEL2}_RISK_DATA OF A_${LABEL1}x${LABEL2}_RISK_DATA
DEFAULT =
12345, DEF_A_${LABEL1}x${LABEL2}
";

print "NUM_ROWS = ", scalar(keys %SICXAMT_table),"\n";
print "DATA =\n";
foreach $k (sort {$a<=>$b} keys %SICXAMT_table){
  print $k,", A_${LABEL1}x${LABEL2}_",$k,"\n";
}





############################################################################
#  Define necessary subroutines to track, summarize, clean, and print stats
############################################################################ 



sub updateSICXAMT {
  my $ref = shift;
  $ref->[0]+=1;
  $ref->[1]+=($IS_FRAUD)?1:0;
  if($IS_FRAUD){
    $ref->[3]+=$AMT;
    $ref->[5]+=$AMT*$AMT;
    $ref->[7]+=$DIST;
    $ref->[9]+=$DIST*$DIST;
  }else{
    $ref->[2]+=$AMT;
    $ref->[4]+=$AMT*$AMT;
    $ref->[6]+=$DIST;
    $ref->[8]+=$DIST*$DIST;
  }
}

sub summarizeSICXAMT {
  my $hashref=shift;
  my @def=();

  foreach my $k (keys %{ $hashref }){
    my $ref=$hashref->{$k};

    my $i=0;
    while(defined ($ref->[$i])){
      $def[$i]+=$ref->[$i];
      $i++;
    }
    
    my $f=$ref->[1];
    my $nf=$ref->[0]-$f;

    $ref->[2]=eval{$ref->[2]/$nf;} || 0;
    $ref->[4]=eval{($ref->[4]-$ref->[2]*$ref->[2]*$nf)/($nf-1);} || 0;
    $ref->[4]=eval{sqrt($ref->[4]);} || 0;

    $ref->[3]=eval{$ref->[3]/$f;} || 0;
    $ref->[5]=eval{($ref->[5]-$ref->[3]*$ref->[3]*$f)/($f-1);} || 0;
    $ref->[5]=eval{sqrt($ref->[5]);} || 0;

    $ref->[6]=eval{$ref->[6]/$nf;} || 0;
    $ref->[8]=eval{($ref->[8]-$ref->[6]*$ref->[6]*$nf)/($nf-1);} || 0;
    $ref->[8]=eval{sqrt($ref->[8]);} || 0;

    $ref->[7]=eval{$ref->[7]/$f;} || 0;
    $ref->[9]=eval{($ref->[9]-$ref->[7]*$ref->[7]*$f)/($f-1);} || 0;
    $ref->[9]=eval{sqrt($ref->[9]);} || 0;
  }

  $f=$def[1];
  $nf=$def[0]-$f;

  $def[2]=eval{$def[2]/$nf;} || 0;
  $def[4]=eval{($def[4]-$def[2]*$def[2]*$nf)/($nf-1);} || 0;
  $def[4]=eval{sqrt($def[4]);} || 0;

  $def[3]=eval{$def[3]/$f;} || 0;
  $def[5]=eval{($def[5]-$def[3]*$def[3]*$f)/($f-1);} || 0;
  $def[5]=eval{sqrt($def[5]);} || 0;

  $def[6]=eval{$def[6]/$nf;} || 0;
  $def[8]=eval{($def[8]-$def[6]*$def[6]*$nf)/($nf-1);} || 0;
  $def[8]=eval{sqrt($def[8]);} || 0;

  $def[7]=eval{$def[7]/$f;} || 0;
  $def[9]=eval{($def[9]-$def[7]*$def[7]*$f)/($f-1);} || 0;
  $def[9]=eval{sqrt($def[9]);} || 0;

  return \@def;
}

sub cleanup {
  my ($name,$def,$ref)=@_;

  if($fix_stddev){
    foreach my $i (qw(4 5)){
      if($def->[$i]>0 && $def->[$i]<1){
        print STDERR "WARNING: $name, DEFAULT, stddev$i=",$def->[$i]," is set to 0.0\n";
        $def->[$i]=0;
      }
    }
  }

  foreach my $k (keys %{ $ref }){
    if($fix_stddev){
      foreach my $i (qw(4 5)){
        if($ref->{$k}->[$i]>0 && $ref->{$k}->[$i]<1){
          print STDERR "WARNING: $name, KEY=$k, stddev$i=",$ref->{$k}->[$i]," is set to 0.0\n";
          $ref->{$k}->[$i]=0;
        }
      }
    }
  }
}


sub SICXAMT_print{
  my ($title,$type,$def,$ref,@amt_BDR)=@_;

  print "TABLE $title OF $type\n";
  print "DEFAULT =\n";

  my $avg_ac=$accuracy+2;
  printf("%.${avg_ac}f,%d,%d",$def->[0]>0?$def->[1]/$def->[0]:0,@{$def}[0,1]);
  if($INCLUDE_AMT_STATS == 1){printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$def}[2..5]);}
  if($INCLUDE_DIST_STATS == 1){printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$def}[6..9]);}
  print "\n";
  print "NUM_ROWS = ",scalar(@amt_BDR),"\n";
  print "DATA =\n";
  for (my $i=0; $i<scalar(@amt_BDR);$i++){
    printf("%.2f,%d,%d",$amt_BDR[$i],@{$ref->{$i}}[0,1]);
  if($INCLUDE_AMT_STATS == 1){
    if($USE_DEFAULT_AMT_STATS == 1){printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$def}[2..5]);}
    else{printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$ref->{$i}}[2..5]);}
  }
  if($INCLUDE_DIST_STATS == 1){
    if($USE_DEFAULT_DIST_STATS == 1){printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$def}[6..9]);}
    else{printf(",%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f",@{$ref->{$i}}[6..9]);}
  }
  print "\n";
  }
}

