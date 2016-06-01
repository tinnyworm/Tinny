#! /usr/local/bin/perl -w

while (<STDIN>){
			$_ =~ s/\s+NSCORE\s+:=\s+/\/\/ NSCORE :=/g;
			$_ =~ s/\s+IF\s+\(NSCORE\s+\>/\/\/  IF (NSCORE >/g;
			$_ =~ s/\s+Get_Reasons\(/\/\/Get_Reasons(/g;
			if (/\s+Calc_NN_Slab\(/ and /_OUTPUT/){
				 	@temp=split(',', $_);
					$op=$temp[2];
					$op =~ s/ //g; 
			  }

			 $_ =~ s/NSCORE/$op\[1\]/ if (/\s+RETURN\(NSCORE\)/); 
				 
		print  $_; 
  }		

