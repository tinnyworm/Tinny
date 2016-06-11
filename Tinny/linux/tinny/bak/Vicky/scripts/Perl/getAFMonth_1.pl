#! /usr/local/bin/perl -w


@month1=(307 .. 312);
@month2=(401 .. 409); 	
push (@month1, @month2); 

foreach $key (@month1){
	 $mm="0$key"; 
	 $monthHash{$mm}=1; 
	 print $mm, "\n"; 
}

while (defined($line=<STDIN>)){
	 chop($line);
	$mth=substr($line,21,4); 
	$amt=substr($line,33,13); 

	if (!defined($monthHash{$mth})){
		next; 
	}	

			if (!defined($fh0{$mth})){
				 $outIS="f$mth";
				 open $outIS, " | /home/yxc/scripts/Perl/histogram.pl -n > $mth.amount.hist"; 
				 $fh0{$mth} = $outIS; 
			}
			$fh0 = "f$mth";
			print $fh0 "$amt\n" ; 
}
