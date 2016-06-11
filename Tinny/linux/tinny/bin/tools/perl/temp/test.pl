#!/usr/local/bin/perl

$input = shift;

$fmt="";
if($input =~ /((\d+)(-\d+)?,?)+/) {
   @cols = split /,/, $input;
   foreach $a (@cols) {
      if ($a =~ /^(\d+)$/) {
         $start=$1-1;
         if($start<0) {
            print STDERR "
         $fmt = $fmt."\\\@".($1-1)." a1 ";
      }
      elsif($a =~ /(\d+)-(\d+)/) {
         $fmt = $fmt."\\\@".($1-1)." a".($2-$1+1)." ";
      }
   }
   print $input,"\n";
   print $fmt,"\n";
}
else {
   print "err\n";
}
