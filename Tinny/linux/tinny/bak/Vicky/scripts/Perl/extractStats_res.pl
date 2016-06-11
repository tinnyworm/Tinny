#! /usr/local/bin/perl -w

$input = $ARGV[0]; 

print "file,basisPoint,medianRD,maxRD,nfTrxf,nfActf,fTrxfAct\n"; 

for $file (glob $input)
{
	next unless (-f $file); 
	
	open (IN, "< $file "); 	
	while ($line=<IN>)
	{
		 if ($line =~ /Maximum number of days of fraud\s+\=\s+(\d+)/)
		 {
		 	$maxRD = $1; 
	  	 }
	
		 if ($line =~ /Median number of days of fraud accounts\s+\=\s+(\d+)/)
		 {
		 	$medianRD = $1; 
		 }

		 if ($line =~ /All approved Trx \$\)\s+\=\s+(\d+)\.(\d+)/)
		 {
		 	$basis = $1 . "." . $2; 
		 }
		
		 if ($line =~ /Number of Non Fraud TRX per Fraud TRX\s+\.+\s+\=\s+(\d+)\.(\d+)/  )
		 {
		 	$nfTrxf= $1 . "." . $2; 
		 }
		 
		 if ($line =~ /Number of Non Fraud Accounts per Fraud Account\s+\=\s+(\d+)\.(\d+)/)
		 {
		 	$nfActf= $1 . "." . $2; 
		 }

		 if ($line =~ /Number of Fraud TRX per Fraud Account\s*\.*\s*\=\s*(\d+)\.(\d+)/)
		 {
		 	$fTrxfAct= $1 . "." . $2;
		 }
	 
	}

	close (IN); 
	print "$file,$basis,$medianRD,$maxRD,$nfTrxf,$nfActf,$fTrxfAct\n"; 
}
