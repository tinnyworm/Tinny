#!/usr/local/bin/perl -w 
$usage = "$0 profStats1 profStats2 #vars\n";

$prof1 = shift
	or die $usage;
$prof2 = shift
	or die $usage;
$nvars = shift
	or die $usage;

open(PROF1, "cat $prof1 |") or die "Can not open $prof1!\n";
open(PROF2, "cat $prof2 |") or die "Can not open $prof2!\n";

$i=1;
while(<PROF1>) {
	 if ($i <= $nvars) {
		 $tmp2 = <PROF2>;
		 if(/^\s+\d+/) {
			  @p1 = split /\s+/;
			  @p2 = split /\s+/, $tmp2;
			  $mean1[$i] = $p1[2];
			  $mean2[$i] = $p2[2];
			  # print $mean1[$i], "\t", $mean2[$i], "\n";
			  $diff[$i] = abs($mean2[$i]-$mean1[$i]);
			  if ($mean1[$i] > 0) {
					$diff[$i] /= $mean1[$i];
			  } else {
			  		$diff[$i] = 0;
			  }
			  $i++;
		 }
	} else {
		 last;
	}
}

close(PROF1);
close(PROF2);

print "VARn,    mean1,       mean2,       diff\n";
#print "----- ---------------- ----------------\n";
for ($i=1; $i<= $nvars; $i++) {
	 printf " %4i, %13g, %13g, %13.4f\n", 
	 	$i,
		$mean1[$i],
		$mean2[$i],
		$diff[$i];
}
