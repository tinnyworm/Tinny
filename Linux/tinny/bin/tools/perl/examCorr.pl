#!/usr/local/bin/perl -w

# examinate Pearson Correlation

$usage = "Usage:\n".
	"\t cat corr.txt | $0 threshold (0,1]\n".
	"\t note: eachline in corr.txt start with the VarName and then corr\n";
	
$th = shift(@ARGV)
	or die $usage;	

$rowi = 0;

while(<>) {
	 @corr = split /\s+/;
	 $varname = shift(@corr);
	 print "$varname\t";
	 $i = 0;
	 for (@corr) {
		  if ( (abs($_) >= $th) && ($rowi != $i) ) {
				print "VAR$i($_)\t";
		  }
		  $i++;
	 }
	 print "\n";

	 $rowi++;
}
		  
