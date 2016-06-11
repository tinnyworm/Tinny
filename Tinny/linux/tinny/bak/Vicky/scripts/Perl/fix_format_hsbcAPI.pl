#! /usr/local/bin/perl -w

$template =  "\@264 a19 \@0 a1 a9 a3 a10 a10 a1 a10 a10 a10 \@66 a10 \@2 a1 \@253 a10 \@77 a3 \@203 a10 \@81 a1 a1 a10 \@94 a8 \@109 a13 a3 a13 a1 a1 a4 a9 a3 a1 a1  \@252 a1 \@159 a10";
while (<STDIN>){
	 chmod;

	@data = unpack($template, $_); 
	
	for ($i=0; $i<@data; $i++){
		$line=$data[$i];
		if ($line =~ /(\d\d\d\d\-\d\d\-\d\d)/ ){
			$line =~ s/-//g;	
			print $line;
		}
 elsif ($line =~ /(\d\d\.\d\d\.\d\d)/) {
			$line =~ s/\.//g; 
			print $line;
		} else {
			print $line; 
		}
	}
	print "\n"; 
	

#	 substr($ndate1,0,4)=substr($date1,0,4);
#	 substr($ndate1,4,2)=substr($date1,5,2);
#	 substr($ndate1,6,2)=substr($date1,8,2);
#
#	 substr($ndate2,0,4)=substr($date2,0,4);
#	 substr($ndate2,4,2)=substr($date2,5,2);
#	 substr($ndate2,6,2)=substr($date2,8,2);
#
# 	 substr($ndate3,0,4)=substr($date3,0,4);
#	 substr($ndate3,4,2)=substr($date3,5,2);
#	 substr($ndate3,6,2)=substr($date3,8,2);
#
#	 substr($ndate4,0,4)=substr($date4,0,4);
#	 substr($ndate4,4,2)=substr($date4,5,2);
#	 substr($ndate4,6,2)=substr($date4,8,2);
#
#	 substr($ndate5,0,4)=substr($date5,0,4);
#	 substr($ndate5,4,2)=substr($date5,5,2);
#	 substr($ndate5,6,2)=substr($date5,8,2);
#
#	 substr($ndate6,0,4)=substr($date6,0,4);
#	 substr($ndate6,4,2)=substr($date6,5,2);
#	 substr($ndate6,6,2)=substr($date6,8,2);
#
#	 substr($ndate7,0,4)=substr($date7,0,4);
#	 substr($ndate7,4,2)=substr($date7,5,2);
#	 substr($ndate7,6,2)=substr($date7,8,2);

	
#	 $ndate2=changDate($date2);
#	 $ndate3=changDate($date3);
#	 $ndate4=changDate($date4);
#	 $ndate5=changDate($date5);
#	 $ndate6=changDate($date6);
#	 $ndate7=changDate($date7);
#	$tt5n=substr($tt5,1,19); 	 
#	$add=" "*14;
#	 print $tt5n,$tt0,$ndate1, $ndate2, $tt1, $ndate3, $ndate4, $tt2, $add, $ndate5, $tt3, $ndate6, $tt4, $ndate7, "\n"; 

}

#sub changDate {
#	 $date = $_ ;
#	substr($out, 0, 4)=substr($date, 0, 4);
#	substr($out, 4, 2)=substr($date, 5, 2);
#	substr($out, 6, 2)=substr($date, 8, 2); 
#	return $out ; 
#	}
