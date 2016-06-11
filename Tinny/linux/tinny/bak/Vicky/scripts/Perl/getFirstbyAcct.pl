#! /usr/local/bin/perl -w

$last=" ";

while (<>){
	chomp;

	$acct=substr($_,0,19);
#	$rest{$acct}= substr($_,19,17);

	if ($acct eq $last){
	   next;
	} else {
	#	print $acct, $rest{$acct}, "\n"; 
		print $_, "\n"; 
	}

	$last=$acct;
}
