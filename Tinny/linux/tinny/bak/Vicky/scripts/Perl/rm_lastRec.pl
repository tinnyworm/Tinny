#! /usr/local/bin/perl -w

$i=0; 

while(<>){
	$i++;
	$out[$i%2]=$_;

		 if ($i > 2) {
		 		print $out[1-$i%2]; 
		 }
}
