#! /usr/local/bin/perl -w

while (<STDIN>){
	chomp;

	if ($_ =~ /^VAR\s+\w+/){
		@tt = split(':', $_); 
		if ($tt[1]=~ /FLOAT/ && $tt[0] !~ /DATE/&& $tt[0] !~ /TIME/ && $tt[0] !~ /COEF/){
			 
		   @temp = split(' ', $tt[0]); 
			print "INPUT $temp[1]\n";
		}
		
	}

}
