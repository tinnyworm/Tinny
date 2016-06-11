#! /usr/local/bin/perl

$last=" ";
$wholeRec=" "; 
$lastRec=" "; 

while (<STDIN>)
{
	chomp;
	$acct = substr($_,0,19); 
	$wholeRec = $_;
	
	if ($acct eq $last)
	{
		$lastRec = $wholeRec;
   } 
   else 
   {
		if ($lastRec =~ /^\s*$/)
		{
			print $wholeRec . "\n"; 
		}
		else
		{
			print $lastRec ."\n"; 
		}
		
   }
	
  $last = $acct;
  $lastRec = $wholeRec;  
  
}
