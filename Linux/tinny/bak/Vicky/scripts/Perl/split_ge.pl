#!/usr/local/bin/perl -w


#### read in all the list
############################
# USAGE: zz raw_data/BRCConsortiumAuthorization_200502.dat.Z | ../scripts/split_ge.pl auth 200502 ./
############################
#`ls branch/* > temp.list`; 

open ($element, "< temp.list") || die "Couldnot find temp.list\n"; 

while ($line = <$element>){
	chop $line;
	$linetm = $line;
	$linetm =~ s/\.//g;
	$linetm =~ s/\//\_/g;
	 	
	$file="f$linetm";
	open ($file, " < $line ") || die "Couldn't open $line\n"; 

	while ($id = <$file>){
		chop $id;
		$clt{$id}=$linetm;
  }
  close($file);

}
close($element); 

#### list all keys

foreach $key (keys %clt){
#	print "$key  :  $clt{$key}\n";
	
	$len=length $key; 

  		if ($len eq 4){
			 $all4{$key}=1; 
    	} elsif ($len eq 6){
    		 $all6{$key}=1;
    	} else {
  				print STDERR "This length $len is not valid. Please check this $key and $clt{$key}\n"; 
		}
 
}

#
## read the data in and extract
$type=$ARGV[0]; 
$month=$ARGV[1]; 
$workdir = $ARGV[2]; 

if (!(-d "$workdir/_other")){
    mkdir ("$workdir/_other", 0755) or die "Could not mkdir $workdir/_other\n"; 
}


open (OTHER, " | gzip -c >> $workdir/_other/$month.other.$type.gz") || die "Couldnot write other.$type.gz\n"; 

while (defined($line = <STDIN>)){
	
 	 if(!($. % 1000)){
    		print STDERR "$.\r";
  	 }
	 chop $line; 	
	 $test = substr($line, 0, 1);  
	
	if ($test =~ /^[A-Z]*$/){
 	 	 $bin4=substr($line,0,4); 
		     if (!defined($all4{$bin4})){
													
							  print OTHER "$line\n"; 
							  next; 
				} else {
							$fhkey=$clt{$bin4}; 
							
							if (!(-d "$workdir/$fhkey")){
										 mkdir ("$workdir/$fhkey", 0755) or die "Could not mkdir $workdir/$fhkey\n"; 
							}
							
							if (!defined($fh{$fhkey})){
									 $out = "f$fhkey"; 
									 open $out, "| gzip -c >> $workdir/$clt{$bin4}/$month.$clt{$bin4}.$type.dat.gz"; 
									 $fh{$fhkey}=$out; 
						 		}
									$fh = "f$fhkey"; 
									print $fh "$line\n"; 
							}
#				close($fh);   
 	 
	  } elsif ($test =~ /^[0-9]*$/){
			  $bin6=substr($line,0,6);
				 if (! defined($all6{$bin6})){
							  print OTHER "$line\n"; 
							  next; 
				} else {
			  
							$fhkey6=$clt{$bin6};
							
							if (!(-d "$workdir/$fhkey6")){
										 mkdir ("$workdir/$fhkey6", 0755) or die "Could not mkdir $workdir/$fhkey6\n"; 
							}

								if (!defined($fh{$fhkey6})){
									 $out = "f$fhkey6"; 
									 open $out, "| gzip -c >> $workdir/$fhkey6/$month.$clt{$bin6}.$type.dat.gz"; 
									 $fh{$fhkey6}=$out; 
						 		}
									$fh6 = "f$fhkey6"; 
									print $fh6 "$line\n"; 
							}
 	#		close($fh6);
 	 		
		} else {
					print STDERR "What did this digit $test indicate? \n"; 
		}
 
  } 
  close(OTHER);
 # `/bin/rm temp.list`; 
