#!/usr/local/bin/perl

#
# splite file into two by key value
#

$usage = "Usage: \n\t".
	"STDIN | $0 key start_col(from 0) key_length file_same file_diff \n".
	"\t note: key can be combination of several actual keys,\n".
	"\t\t \"826,000,   ,\"";

$key = shift(@ARGV);
$start_col = shift(@ARGV);
$key_length = shift(@ARGV);
$file1 = shift(@ARGV);
$file2 = shift(@ARGV);

#print "$key \t $stat_col \t $key_length \t $file1 \t $file2\n";

die $usage unless (defined $key and defined $start_col and defined
	 $key_length and defined $file1 and defined $file2);

open(F1, "| gzip > $file1") or die "Can not open $file1 to write!\n";
open(F2, "| gzip > $file2") or die "Can not open $file2 to write!\n";

while(<>) {
	 $k = substr($_,$start_col,$key_length);
	 if ($key =~ /$k/) {
		  print F1 "$_";
	 }
	 else {
		  print F2 "$_";
	 }
}
close(F1);
close(F2);
