#! /usr/local/bin/perl 

open (IN, "< remove.varList ") || die "Could not open remove.varList\n"; 
while (<IN>){
	chomp;
	$_ =~ tr/[a-z]/[A-Z]/; 
	$varHash{$_}=1;
	$count ++; 
}

while (defined($line = <STDIN>)){
	 chomp;
	 if ($line =~ /Name=/){
	 	$linem=$line;
		$linem=~ s/^Name= //g;
		$linem =~ s/\s+$//g; 
	   $linem =~ tr/[a-z]/[A-Z]/; 	
	 } 
	 if($varHash{$linem}){
		  
		if($line =~ /Slab/){
			 $line =~ s/INPUT/NONE/; 
			 $cc ++; 
			print $line ; 
	  } else {
 			print $line ;
 		}
	
  } else {
		print $line ;
	}


}
print STDERR "Total is $count and remove $cc\n"; 
