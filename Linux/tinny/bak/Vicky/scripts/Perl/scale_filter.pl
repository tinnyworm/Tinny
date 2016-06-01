#!/usr/local/bin/perl

$USAGE = "
##Used for sampling the scale file by different fraud type. 
## $0 -i <inbase model name> -o <outbase model name> -s <sampling rate> -f
<fraud type> -b <bigscore file>
"; 


#use Math::Random;
use Getopt::Std;

#####################################################
# To process the options	
#####################################################
 
die "$USAGE" if ($#ARGV == -1); 

&getopts('i:b:f:s:o:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

die "$USAGE" unless (defined $opts{'s'}); 
$samp_rate = $opts{'s'}*1; 

die "$USAGE" unless (defined $opts{'i'});
$in_base = $opts{'i'};

die "$USAGE" unless (defined $opts{'o'});
$out_base = $opts{'o'};

die "$USAGE" unless (defined $opts{'b'});
$big_scr = $opts{'b'};

die "$USAGE" unless (defined $opts{'f'});
$frd_type = $opts{'f'};

$in_scl_fn = $in_base . ".scl";
$in_tag_fn = $in_base . ".tag";

$out_scl_fn = $out_base . ".scl";
$out_tag_fn = $out_base . ".tag";

$tmp = `uname -s`;
chomp $tmp;
if ($tmp =~ /^Linux/)
{
	$filterScal = "/home/yxc/scripts/C/scl_filter.Linux"; 
}
else 
{
	$filterScal = "/home/yxc/models/scl_filter"; 
}
open (IN_BIG,  "gunzip -c $big_scr | ") || die "cannot read file $big_scr:$!\n";
open (OUT_FILTER, "> filter.$out_base")  || die "cannot create file filter.$big_scr :$!\n";

while ($line = <IN_BIG>) {
  
  $fdata_type = substr($line,79,1); 
  $i ++;  
  if ($fdata_type eq "$frd_type")
  {
		$rand = rand();	
		
		if ($rand > $samp_rate) 
		{
			$out_rec = "0"; 
		}
		else
		{
			$out_rec = "1"; 
		}
	
  }
  else 
  {
  	 $out_rec = "1"; 
  }
  
	print OUT_FILTER $out_rec . "\n"; 
}

close(IN_BIG);
close(OUT_FILTER);

print STDERR "$i\n"; 
$cmd = "$filterScal $in_base $out_base filter.$out_base\n"; 
print $cmd; 
`$cmd`; 
#system("/bin/rm filter.$out_base; "); 

