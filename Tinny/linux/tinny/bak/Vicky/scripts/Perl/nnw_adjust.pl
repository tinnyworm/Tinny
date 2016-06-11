#!/usr/local/bin/perl 

$usage=
"Usage: $0 intercept slope
     Modify the wights and bias of the output node of the nnet file for 
     calibrating the score. 
     Read NNW file from <STDIN>, write new NNW file to <STDOUT>.

     Typical use:
     > cat tBD03Sh2.nnw | $0 1.028217 0.859640 >tBD03Sh2.nnw.new
";

die $usage if ($#ARGV != 1);
die $usage if ($ARGV[0] !~ /^-?[\d\.]+$/ || $ARGV[1] !~ /^-?[\d\.]+$/ );

while(<STDIN>){
  last if /set NNetSlab\(1\) \{/;
  print $_;
}
print $_;

while(<STDIN>){
  last if /weights\s+\{/;
  print $_;
}
print $_;
$ct=0;
while(<STDIN>){
  last if /^    \}\n/;
  if (/{/) {
    chomp;
    split;
    printf "        { ";
    for ($i=1;$i<=$#_;$i++){
      $_[$i]*=$ARGV[1];
      printf "%14.7e  ", $_[$i];
    }
    printf "\n";
  } elsif (/}/) {
    chomp;
    split;
    printf "          ";
    for ($i=0;$i<=$#_-2;$i++){
      $_[$i]*=$ARGV[1];
      printf "%14.7e  ", $_[$i];
    }
    $_[$#_-1]=$_[$#_-1]*$ARGV[1]+$ARGV[0];
    printf "%14.7e }\n", $_[$#_-1];
  } else {
    chomp;
    split;
    printf "          ";
    for ($i=0;$i<=$#_;$i++){
      $_[$i]*=$ARGV[1];
      printf "%14.7e  ", $_[$i];
    }
    printf "\n";
  }
}
print $_;
while(<STDIN>){
  print $_;
}
