#!/usr/local/bin/perl 

$start=$ARGV[0];
$end=$ARGV[1];
$temp=$ARGV[2];
$outdir=$ARGV[3]; 
$type=$ARGV[4]; 

$pos=$temp - 1; 

die "ERROR: Please use YYYYMM for the option -d.\n" unless (($start =~ /\d{6}/) && ($end =~ /\d{6}/)); 

if ($end < $start) {
    die "ERROR: End date is before start date!! Please check it...\n";
} else {
		$i=$start-1;
		while ($i < $end) {
			$i++; 
			if (($i % 100) > 12 ){
				$i += 88;
			}
			$month{$i}= 1 ; 
		}
}

while (defined($line = <STDIN>)) {
    $mon = substr($line,$pos,6);
    
	 if (!defined($month{$mon})) {
#        print STDERR "Error: no client defined for bin  $bin.  Skipping transaction...\n";
        next;
    }
     
    if (!defined($fh{$mon})) {
		  $file = "f$mon";
		  $yymm = substr($mon, 2,4); 
        open $file, "| gzip -c > $outdir/$yymm.$type.dat.gz";
        $fh{$mon} = $file;
    }

    $fh = "f$mon";
    print $fh $line;
}
