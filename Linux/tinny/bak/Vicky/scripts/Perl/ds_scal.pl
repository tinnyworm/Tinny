#! /usr/local/bin/perl -w

# This is a conditional step for down sample the scale file if the size
# exceeds 2G. 
#

$scl=$ARGV[0]; 

$tmp = `uname -s`;
chomp $tmp;

if ($tmp =~ /^Linux/)
{
## downsample the combined scale file
	#$SCAL_SAMP="unix /work/price/falcon/bin/sample_scl";
	$SCAL_SAMP="unix /home/yxc/models//scale_sample.pl"; 
}
else
{
## downsample the combined scale file
	#$SCAL_SAMP="/work/price/falcon/bin/sample_scl";
	$SCAL_SAMP="/home/yxc/models//scale_sample.pl";
}


if (!(-e "./$scl.scl"))
{
	die "ERROR: The scale file $scl.scl does not exist!\n"; 
}
else 
{
	@stats= stat ("./$scl.scl"); 
	if ($stats[7] > 2002047044)
	{
		 $ratio = 1*substr(2000000000/$stats[7],0,8); 
		 #$cmd = "$SCAL_SAMP $scl sampled_$scl $ratio;\n";
		 $cmd = "$SCAL_SAMP $ratio $scl sampled_$scl;\n";
	#	 $cmd.= "cat sampled_$scl.tag | " . 'perl -ne \'if($_ =~ /^train: / && $_=~/[test|train]$/){ @tmp=split(" " , $_); print "$tmp[0] $tmp[1]\n $tmp[2] $tmp[3]\n";} else {print $_;}\'' . "| > sampled_$scl.tag.fixed;\n";  
	#	 $cmd.= "mv sampled_$scl.tag sampled_$scl.tag.unfixed;\n"; 
	#	 $cmd.= "ln -s sampled_$scl.tag.fixed sampled_$scl.tag\n"; 
	}
	else
	{
		$cmd = "ln -s $scl.scl sampled_$scl.scl;\nln -s $scl.tag	sampled_$scl.tag\n"; 
	}
}

print STDERR "$cmd"; 
`$cmd`; 
