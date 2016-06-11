#!/usr/local/bin/perl 

$usage=
"Usage: $0 intercept slope
     Modify the wights and bias of the output node of the nnet file for 
     calibrating the score. 
     Read NNET file from <STDIN>, write new NNET file to <STDOUT>.

     Typical use:
     > cat tBD175n4 | $0 1.028217 0.859640 >tBD175n4.new
";

die $usage if ($#ARGV != 1);
die $usage if ($ARGV[0] !~ /^-?[\d\.]+$/ || $ARGV[1] !~ /^-?[\d\.]+$/ );

while(<STDIN>){
  last if /Set_Array_Data.*OUTWT/;
  if (/Weights for hidden node (\d+)/){
    $nodes=$1;
  }
  print $_;
}
print $_;
$ct=0;
while(<STDIN>){
  last if /ENDRULE/;
  chomp;
  split(/, /);
  for ($i=0;$i<=$#_;$i++){
    $_[$i]*=$ARGV[1];
  }
  if ($ct == 0){
    $_[0]+=$ARGV[0];
  }
  $ct+=($#_ +1);
  printf "    ";
  for ($i=0;$i<$#_;$i++){
    printf "%13.6e, ", $_[$i];
  }
  if ($#_ == 4 && $ct < $nodes+1){
    printf "%13.6e,\n", $_[4];
  } else {
    printf "%13.6e);\n", $_[$#_];
  }
}
print $_;
while(<STDIN>){
  print $_;
}
