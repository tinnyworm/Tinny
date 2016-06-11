#!/usr/local/bin/perl -w
#
# fish out fraud type A, C, I, L, M, S from the bigScr file
#

#$usage = "Usage: \n\t".
#	"zcat bigScr.gz | $0 baseName\n\n";

$baseName = shift(@ARGV);
	
open(OUT_A, "| gzip -c > $baseName.A.gz") 
	or die "Can not open $baseName.A.gz \n";
open(OUT_C, "| gzip -c > $baseName.C.gz") 
	or die "Can not open $baseName.C.gz \n";
open(OUT_I, "| gzip -c > $baseName.I.gz") 
	or die "Can not open $baseName.I.gz \n";
open(OUT_L, "| gzip -c > $baseName.L.gz") 
	or die "Can not open $baseName.L.gz \n";
open(OUT_M, "| gzip -c > $baseName.M.gz") 
	or die "Can not open $baseName.M.gz \n";
open(OUT_S, "| gzip -c > $baseName.S.gz") 
	or die "Can not open $baseName.S.gz \n";
	
while(<>) {
	$ftype = substr($_,79,1);
	SWITH: {
		if($ftype eq "A") {printf OUT_A "$_"; last SWITH;}
		if($ftype eq "C") {printf OUT_C "$_"; last SWITH;}
		if($ftype eq "I") {printf OUT_I "$_"; last SWITH;}
		if($ftype eq "L") {printf OUT_L "$_"; last SWITH;}
		if($ftype eq "M") {printf OUT_M "$_"; last SWITH;}
		if($ftype eq "S") {printf OUT_S "$_"; last SWITH;}
	}
}

close(OUT_A);
close(OUT_C);
close(OUT_I);
close(OUT_L);
close(OUT_M);
close(OUT_S);
