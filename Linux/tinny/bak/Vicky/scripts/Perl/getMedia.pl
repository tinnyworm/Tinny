#! /usr/local/bin/perl
#

$i=0;
while (<>){
 chomp;
 $amt=substr($_,33,13);
 if (! ($amt =~ /^[ 0.]*$/) ) {
   $AMT[$i]=$amt; 
   $i ++; 
 } 
}
@AMT= sort {$a <=> $b} @AMT;
$k=int($i/2);
print "The media of the AUTH-AMOUNT is $AMT[$k]\n" ;
