#! /usr/local/bin/perl -w

print "Score,PercentNF,PecentF,DLScrDist,ScrDist\n"; 

while(<STDIN>)
{
	chomp;

	if ($_ =~ /^[0-9. ]*$/)
	{
 		@tmp = split(/\s+/, $_); 

	  if (defined(@tmp))
	  {
			print "$tmp[1],$tmp[4],$tmp[5],$tmp[10],$tmp[3]\n"; 
	  }
	}

}
