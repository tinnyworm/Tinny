#! /usr/local/bin/perl -w 

$usage =" $0 test.api.gz
 	which creates test.api.gz.atsOut.gz\n";

if ($ARGV[0] eq "-h" || $#ARGV < 0 ) 
{
	die "$usage"; 
}
else
{
	$file = $ARGV[0]; 
}

open (OUT, " | bmsort -T /tmp -S -c1-33 | ufalcut -c35- | gzip -c > $file.atsOut.gz ") || die "Could not write acctTimeSort.api.out.gz\n"; 

if ($file =~ /gz$/)
{
	open (IN, "gzip -dc $file | ") || die "Could not open $file\n"; 
}
else 
{
	open (IN, "cat $file | ") || die "Could not open $file\n";
}

	
while (defined($_ = <IN>))
{
	chomp; 
	($acct, $tag, $time) = unpack("a19 \@19 a1 \@117 a6", $_); 

	if ($tag eq "A")
	{
		$date = substr($_,109,8); 
  }
  elsif ($tag eq "P")
  {
  		$date = substr($_,173,8); 
  }
  else
  {
  		$date = substr($_, 109,8); 
  }

	$key = $acct . $date . $time; 

	print OUT "$key $_\n"; 

  
}

close(OUT);
close(IN); 
