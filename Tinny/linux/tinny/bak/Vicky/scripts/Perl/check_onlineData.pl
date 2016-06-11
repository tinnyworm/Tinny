#! /usr/local/bin/perl -w

while (<>){
	chomp;
	@temp=split(/\s+/, $_); 

	$size=$temp[4];
	$file=$temp[8];

	$name=substr($file, rindex($file,'/')+1);
	@temp2=split(/\./, $name); 
	$month=$temp2[0];
	if ($month =~ /^\d{4}/){
	$monthSize{$month} += $size; 
	}
}

foreach $mth (sort keys %monthSize){
	 print $monthSize{$mth}, "  " , $mth, "\n"; 
}
