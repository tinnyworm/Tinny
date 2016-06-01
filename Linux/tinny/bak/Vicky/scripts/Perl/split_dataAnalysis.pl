#!/usr/local/bin/perl  -w

$indir="/work/ffp6/HomeImprove3.0/yxc/homeImpro3.0/";
$son=$ARGV[0];
$pos=$ARGV[1];
$type=$ARGV[2];

if (!(-d "$indir/$son/split")){
	mkdir ("$indir/$son/split", 0755) or die "cannot mkdir $indir/$son/split: $!";
}

if ($type eq "auths"){
  print $type,"\n"; 
	while (<STDIN>) {
		$month=substr($_,$pos,6);
	
	
		if (!defined($month)) {
			next;
		}
		
		if (!defined($fh{$month})){
			$out = "f$month";
			open $out, "| /work/price/falcon/bin/vauths.pl >$indir/$son/split/read.$month.auths.analyze";
			$fh{$month} = $out;	

		} 
	
		$fh = "f$month";
		print $fh $_; 
 	
	}
} elsif ($type eq "cards"){

	while (<STDIN>) {
		$month=substr($_,$pos,6);
		
		if (!defined($month)) {
			next;
		}
	
		if (!defined($fh{$month})){
			$out = "f$month";
			open $out, "| /work/price/falcon/bin/vcards.pl	>$indir/$son/split/read.$month.cards.analyze";
			$fh{$month} = $out;	

		} 
	
		$fh = "f$month";
		print $fh $_; 
	
	}

} else {
		print "Couldn't recgnize the type for $type.\n Please input as cards
		or auths\n\n";
}
