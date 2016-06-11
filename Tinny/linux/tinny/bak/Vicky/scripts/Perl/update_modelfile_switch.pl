#! /usr/local/bin/perl -w

$file=$ARGV[0]; 
$model=$ARGV[1]; 
$temp=$ARGV[2]; 

@type=split ",", $temp; 

$out= $file . "_" . $model; 

open(IN, "< $file") || die "Could not open $file:\n"; 
open(OUT, "> $out") || die "Could not write $out:\n";

while (<IN>){
	 $line = $_; 
	 foreach $i (@type){
	 	$line=~ s/^INCLUDE \"$i/INCLUDE \"$i\_$model/g; 
	   
	}
	 print OUT $line;
}

