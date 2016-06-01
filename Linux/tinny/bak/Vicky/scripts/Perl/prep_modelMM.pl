#! /usr/local/bin/perl -w

$usage="$0 <model file> <nets file>\n"; 

die "$usage" unless ($#ARGV > 0); 

$file = $ARGV[0]; 
$nnets= $ARGV[1]; 

open(NETS, "< $nnets") || die "Can not open $nnets\n"; 

while (defined($line=<NETS>))
{
	 chop($line);

	if ($line =~ /FUNCTION/)
	{
		 @tmp = split(/\s+/, $line);
		 $calc= $tmp[1]; 
		 print STDERR "NOTE: Found $calc\n"; 
	}
		
}

if (-e "nets/")
{
	$nets = rmDir($nnets) ; 
	system("cp $nnets nets/$nets"); 
}
else
{
	die "ERROR: No nets/ exists. It can only be run in the model directory\n"; 
}


open (IN, "< $file") || die "Can not open $file\n"; 
open (OUT, "> $file.tmp") || die "Can not update $file\n"; 

while (defined($_=<IN>))
{
	 chomp;

	if ($_ =~ /INCLUDE\s+\"nets\/atm/)
	{
		$_ = "INCLUDE \"nets/$nets\";\n";
  }
  	if ($_ =~ /\s+NSCORE\s+\:\=\s+\@CALC_/ )
	{
		$_ = " NSCORE \:\= \@$calc\(\)\;\n"; 
	}

	print OUT "$_\n"; 	
}


## subrutine

sub rmDir 
{
	my $dir = $_[0]; 
	my @allFile = split('/', $dir);
	my $fileLoc = @allFile - 1;
	my $returnFile = $allFile[$fileLoc];
	
	return($returnFile); 
}


