#!/usr/bin/env perl
use Getopt::Long;

$DEFAULT_MIN_THRESHOLD=5;
$DEFAULT_ACCURACY=2;

$usage= "
Generate SICxAMT risktables from input data file. 
The following input data file format is assumed:
  IS_FRAUD, IS_KEY, HOW, SIC, POSTAL, CNTRY, CURR, AMT, DIST

Usage: zcat input_data.gz|$0 [options] > amttable
Options:
        -a <accuracy>           Print accuracy for floating point numbers.
                                Default: 2
        -M <Min_threshold>      Table records will be drooped, if number of 
                                transactions in it < Min_threshold.
                                Default: 5
        -n                      NO stddev fixing. Otherwise, if (0<stddev<1), 
                                the stddev will be set to 0.0.
        -b <bin boundries>      Comma separated list of boundries for amount
                                bins. Default: 50,100,150,250,500,1000,2000
";
 
@optl = ("a=f","b=s","M=i","n");
die $usage unless GetOptions(@optl);

$accuracy=$opt_a?$opt_a:$DEFAULT_ACCURACY;
$min_threshold=$opt_M?$opt_M:$DEFAULT_MIN_THRESHOLD;
$fix_stddev=$opt_n?0:1;

if($opt_b){
  $amt_BDR = [ split(/,/,$opt_b) ];
}else{
  $amt_BDR = [ 50, 100, 150, 250, 500, 1000, 2000 ];
}
unshift @{ $amt_BDR }, 0.01;

$def_SICXAMT={};
for ($bin=0;$bin<scalar(@{ $amt_BDR });$bin++){
  $def_SICXAMT->{$bin}=[0,0,0,0,0,0];
}

while(<STDIN>){
  chomp;
  ($IS_FRAUD, $IS_KEY, $HOW, $SIC, $POSTAL, $CNTRY, $CURR, $AMT, $DIST)
        =split(/,/,$_);

  $IS_FRAUD=($IS_FRAUD > 0.5);

  $bin=grep( {$_ <= $AMT} @{ $amt_BDR })-1;
  next if ($bin < 0);
  updateSICXAMT($def_SICXAMT->{$bin});

  next if ($SIC<0 || $SIC>9999);
  $SICXAMT_table{$SIC}={} unless (defined $SICXAMT_table{$SIC});
  $SICXAMT_table{$SIC}->{$bin}=[0,0,0,0,0,0] 
        unless (defined $SICXAMT_table{$SIC}->{$bin});
  updateSICXAMT($SICXAMT_table{$SIC}->{$bin});
}

print << '_END';
//
//************************************************
//* Auth SIC x AMT risk tables (4.0)
//************************************************
//
_END

foreach $k (sort {$a<=>$b} keys %SICXAMT_table){
  $SICXAMT_def{$k}=summarizeSICXAMT($SICXAMT_table{$k});
  if ($SICXAMT_def{$k}->[0]<$min_threshold){
    delete $SICXAMT_table{$k};
    delete $SICXAMT_def{$k};
  }else{
    cleanup("A_SICxAMT_$k",$SICXAMT_def{$k},$SICXAMT_table{$k});
    SICXAMT_print("A_SICxAMT_$k","AMT_RISK_DATA",$SICXAMT_def{$k},$SICXAMT_table{$k});
  }
}
$def_SICXAMT_def=summarizeSICXAMT($def_SICXAMT);
SICXAMT_print("DEF_A_SICxAMT","AMT_RISK_DATA",$def_SICXAMT_def,$def_SICXAMT);

print << '_END';
TABLE A_SICxAMT_RISK_DATA OF A_SICxAMT_RISK_DATA
DEFAULT =
12345, DEF_A_SICxAMT
_END

print "NUM_ROWS = ", scalar(keys %SICXAMT_table),"\n";
print "DATA =\n";
foreach $k (sort {$a<=>$b} keys %SICXAMT_table){
  print $k,", A_SICxAMT_",$k,"\n";
}










sub updateSICXAMT {
  my $ref = shift;
  $ref->[0]+=1;
  $ref->[1]+=($IS_FRAUD)?1:0;
  if($IS_FRAUD){
    $ref->[3]+=$DIST;
    $ref->[5]+=$DIST*$DIST;
  }else{
    $ref->[2]+=$DIST;
    $ref->[4]+=$DIST*$DIST;
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
  }

  $f=$def[1];
  $nf=$def[0]-$f;

  $def[2]=eval{$def[2]/$nf;} || 0;
  $def[4]=eval{($def[4]-$def[2]*$def[2]*$nf)/($nf-1);} || 0;
  $def[4]=eval{sqrt($def[4]);} || 0;

  $def[3]=eval{$def[3]/$f;} || 0;
  $def[5]=eval{($def[5]-$def[3]*$def[3]*$f)/($f-1);} || 0;
  $def[5]=eval{sqrt($def[5]);} || 0;

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
  my ($title,$type,$def,$ref)=@_;

  print "TABLE $title OF $type\n";
  print "DEFAULT =\n";

  my $avg_ac=$accuracy+2;
  printf("%.${avg_ac}f,%d,%d,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f\n",
        $def->[1]/$def->[0],@{ $def });
  print "NUM_ROWS = ",scalar(@{ $amt_BDR }),"\n";
  print "DATA =\n";
  for (my $i=0; $i<scalar(@{ $amt_BDR });$i++){
    printf("%.2f,%d,%d,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f\n",
        $amt_BDR->[$i],@{ $ref->{$i} });
  }
}
