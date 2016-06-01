#!/usr/local/bin/perl

use warnings;

$usage = "\t cat tobemapped | $0 mapto\n";

$ref = shift
    or die $usage;

open(REF,"cat $ref |") 
    or die "Can NOT open $ref!\n";
$i=0;
while(<REF>) {
    @t=split /,/;
    $rtab[$i++]=$t[1];
}
close(REF);

$i=1;
while(<>) {
    @t=split /,/;
    #printf("%d, %f\n",$t[0],$t[1]);
    while ($t[1]<$rtab[$i]) {
        #print "\t in while: ",$i,", ",$rtab[$i],"\n";        
        $i++;
    }
    $scr=(($t[1]-$rtab[$i-1])/($rtab[$i]-$rtab[$i-1])+$i-1)*10;
    printf("%3d, %5.2f\n",$t[0],$scr);
}
