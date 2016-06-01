#! /usr/local/bin/perl -w

$usage = "$0 SCR_1 SCR_2\n".
  "Outputs SCR_1 - SCR_2\n";

$scr1 = shift @ARGV
  or die $usage;
$scr2 = shift @ARGV
  or die $usage;

open(SCR1, "cat $scr1 |")
  or die $usage;
open(SCR2, "cat $scr2 |")
  or die $usage;

while(<SCR1>){
  $line = <SCR2>;

  $s1 = substr($_, 22, 4);
  $s2 = substr($line, 22, 4);

  print(abs($s1-$s2),"\n");
}

close(SCR1);
close(SCR2);
