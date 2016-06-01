#! /usr/local/bin/perl -w 

$usage="Usage: \n\n $0 calibrationTable hiDollar_BigScore_file\n";

die $usage if ($#ARGV == -1);
die $usage if ($tt eq "-h"); 

$table=$ARGV[0];

open (TABLE, " < $table") || die "Couldn't open $table file";

while (<TABLE>){
	chomp;
	@list=split(",", $_);
	$inScr=$list[0];
	$calScr{$inScr}=$list[1];
}

while (<STDIN>){
	$score=substr($_,83,4);
	$score =~ s/^\s+//;
	$score =~ s/\s+$//;
	if (defined($calScr{$score})){
		$len=length $calScr{$score}; 
		$nd=4-$len;
		$blank=" "x$nd;
		$nscr=$calScr{$score};
		$fscr= join("",$blank, $nscr);
		substr($_,83,4)=$fscr;
#		print $score, " : ", $calScr{$score}, "\n"; 
		print $_; 
	}

}

