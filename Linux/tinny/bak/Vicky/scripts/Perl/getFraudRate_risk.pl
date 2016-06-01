#! /usr/local/bin/perl -w 

while (<STDIN>){
	chomp;
	if ($_ =~ m/ POST_RISK_DATA/io){
	#	 print STDERR "Found Table!\n"; 
		for ($h=0; $h< 3; $h++){
			 $head=<STDIN>;
			 if ($head =~ /^NUM_ROWS\s+=\s+(\d+)/){
			 	$numRow = $1*1 ; 
			 }  
		}
			$in=<STDIN>; 
		for ($k=0; $k< $numRow; $k++){
			 $in=<STDIN>;
			 chop $in; 
			@F = split ",", $in; 
			$entry=$F[0]; 
			$total=$F[1] * 1 ; 
			$fraud=$F[2] * 1 ;	
			$risk=sprintf("%.6f", $fraud/$total) if ($total > 0); 
			print "$entry  :  $risk\n"; 
		}
		
	}


}
