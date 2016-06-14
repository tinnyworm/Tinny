#!/usr/local/bin/perl 

use Getopt::Std;

getopts("plh");

$usage="
Usage: cat scale.scl [or prescl] \\
           |$0 [-p] [-l] <orig_var_list> <new_list> <new_new_list> \\
            > newscl.scl

           <new_new_list> is the new_list with the right order

           
           [-p]   is for shrink prescale file.
           [-l]   for linreg/logreg, var_list include tag.
                  without this option, it assumes var_list without tag\n";

die $usage if($opt_h);

$la=shift or die $usage;
$ln=shift or die $usage;
$nl=shift or die $usage;

open LA, "< $la" or die $usage;
open LN, "< $ln" or die $usage;
open NL, "> $nl" or die $usage;

if(defined($opt_l)) {
    $cnt=0;
} else {
    $cnt=1;
}

%selected=();
while(<LN>){
  chomp;
  $_=~s/(^\s*)|(\s*$)//g;
  $selected{$_}=1;
}
close(LN);

while(<LA>){
  chomp;
  $_=~s/(^\s*)|(\s*$)//g;
  if(exists($selected{$_})) {
      push @pi, $cnt;
      print NL $_,"\n";
  }
  $cnt++
}
close(LA);
close(NL);

if(defined($opt_l)) {
    $key="";
} else {
    $key="a4 ";
}
foreach $i (@pi){
  $key .= "\@" . $i * 4 . " a4 ";
}
print stderr $key;

if(!defined($opt_p)){
  read(STDIN,$buff,4);
  $rl = unpack("N",$buff);
  if(defined($opt_l)) {
      if($rl > 16378 || $rl <0){
        print pack("V",scalar(@pi));
      }else{
        print pack("N",scalar(@pi));
      }
  } else {
      if($rl > 16378 || $rl <0){
        print pack("V",scalar(@pi)+1);
      }else{
        print pack("N",scalar(@pi)+1);
      }
  }
}

while(read(STDIN,$buff,$cnt*4)){
  print unpack($key,$buff);
}