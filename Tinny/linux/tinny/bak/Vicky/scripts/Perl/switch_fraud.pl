#! /usr/local/bin/perl

$usage= "zcat fraud | $0 auth_last \n\n"; 

die $usage if ($#ARGV = 0); 

$auth=$ARGV[0]; 

open (AUTH, "gzip -dc $auth | ") || die "Could not open $auth\n\n"; 

while (<AUTH>)
{
	chomp;
	($acct, $tranDate) = unpack "a19 a8", $_; 

	$acctHash{$acct}=$tranDate; 
#	print $acct . "  " . $tranDate . "\n"; 
}

while (defined($line=<STDIN>))
{
	($acctF, $ffd, $fdd)= unpack "a19 x a8 xxxxxxx a8", $line; 
#	print $acctF . "  " . $ffd . "  " . $fdd . "\n";
	
	if (defined($acctHash{$acctF}))
	{
		 if ($ffd > $acctHash{$acctF} and $fdd <= $acctHash{$acctF})
		{
			 substr($line, 20,8) = $fdd; 			 
			 substr($line, 35,8) = " " x 8;
			print $line;  
		}
		else
		{
			print $line ; 
		}
  }
  else
  {
  	 print $line ; 
  }

}
