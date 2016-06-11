#!/usr/local/bin/perl 
use Getopt::Long;

@optl=("c=i");
GetOptions(@optl) or die;
$start_col=$opt_c?($opt_c-1):34;

$bg=$ARGV[0];
$ed=$ARGV[1];
while(<STDIN>){
  $date=substr($_,$start_col,8);
  if ($date>=$bg && $date <$ed){print $_;}
}
