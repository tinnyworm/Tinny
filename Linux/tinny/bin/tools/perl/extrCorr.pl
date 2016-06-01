#!/usr/local/bin/perl -w

# extract Pearson Correlation from SAS result of scl2lib.sas, output to
# stdout, one line per variable
#
# Two things need to be done before running this script
#	1. cut out the CORR Procedure part from scl2lib.lst save it in a seperate
#		file, e.g. corr.lst
#	2. change VAR0000 to VAR0 in corr.lst
#
# usage:
#	cat corr.lst | extrCorr.pl <number of vars (include var0)>
#

$usage = "Usage:\n".
	"	cat corr.lst | $0 <number of vars (include var0)>\n";

$nvars = shift(@ARGV)
	or die $usage;	

for ($i=0; $i<=$nvars; $i++) {
	 $key = "VAR".$i;
	 @{$key} = ();
	 $vars{$key} = \@{$key};
}

while(<>) {
	 if(/^VAR/) {
		 	chomp;
		 	@t = split / /;
		 	$key = shift(@t);
	 		push @{$key}, @t;
	}
} 

for ($i=0; $i<=$nvars; $i++) {
	 $key = "VAR".$i;
	 print "$key\t @{$vars{$key}}\n";
}
