#!/usr/local/bin/perl

# fish
# Takes a 'bait' file and a 'pond' file and goes fishing; pulling records out
# of the pond file which match the bait on the key column (e.g. account number)
# -c specifies the key column in the bait file, -s is the starting column to
# match the key with in the pond file.
#
# $Source: /work/price/falcon/dvl/scripts/RCS/fish.pl,v $
#
#
#	kmh oct-16-96: Got this from mjk & made some changes.
#
#	kmh dec-06-96: Improved it such that the pond file can
#		also be compressed. 
#
#	kmh dec-10-96: Changed the use of "-u" so that -u means all files
#		are compressed and need to be uncompressed (inversed the usage). 
#
#	kmh dec-30-96: Added "-v" for writing complement.
#
#	kmh mar-05-97: Changed the use of "-u" so that -u means all files
#		are uncompressed. If -u not present, then the files are
#		already compressed and need to be uncompressed. 
#		 
#	kmh jun-05-97: Fixed serious bug in finding complement using -v option. 
#
#
#

#print "Start fishing...\n";

$usage = "\nUsage: fish -b <bait_file> -c <bait_col_spec> -s <pond_start_col>\n"
         . "               [ -u ] [ <pond_file>... ] [ > fishfile ]\n"
         . " where:\n"
         . "    -h    :   show this message\n"
         . "    -u    :   assume all files are uncompressed (bait & pond files both)\n"
         . "    -v    :   write complement (all records in pond file NOT in bait file)\n\n"
		 . "    LIMITATION: The files must already be sorted according to the columns specified.\n\n";

require "getopts.pl";

&Getopts('b:c:s:huv');
$bait_file = $opt_b;
$col_spec = $opt_c;
$pond_start_col = $opt_s - 1;
$dont_uncompress =  $opt_u;
$write_complement = $opt_v;
die "$usage" unless $opt_b && $opt_c && $opt_s;
die "Bad start column specification\n" if $pond_start_col < 0;

#if ( $opt_h ) { die "$usage"; }

# Parse column spec
if ( $col_spec =~ /(\d+)-(\d+)/ ) {
	$bait_start_col = $1 - 1;   # Covert to zero-based
	$length = $2 - $1 + 1;
	die "Bad column specification\n" if $length < 0 || $bait_start_col < 0;
} else {
	die "Bad column specification\n";
}

# $i is always the index of the LAST acct in the array.
$sentinel = "z" x $length;
@keys = ();
$last_key = '';

$bait_file_str = $dont_uncompress ? "<$bait_file"
                                : "/usr/local/bin/zcat $bait_file|";
print $bait_file_str "\n";
open(BAIT, "$bait_file_str") || die "Couldn't read $bait_file\n";
while(<BAIT>) {
	$key = substr($_, $bait_start_col, $length);
	if ($key ne $last_key) {
		push(@keys, $key);
		$last_key = $key;
	}
}
close(BAIT);

push(@keys, $sentinel);

$a=0;
$pond_file = shift(@ARGV);
while ( $pond_file ) {
	$pond_file_str = $dont_uncompress ? "<$pond_file"
                                : "/usr/local/bin/zcat $pond_file|";
	print $pond_file_str "\n";
	open(POND, "$pond_file_str") || die "Couldn't read $pond_file\n";
	while ( <POND> ) {
		$fish_key = substr($_, $pond_start_col, $length);
		while ($keys[$a] lt $fish_key) {
			$a++;
		}
		if ( $write_complement ) {
			print if ($keys[$a] ne $fish_key);
		} else {
			last if ($a == $#keys);		# Quick and dirty check for end of bait
			print if ($keys[$a] eq $fish_key);
		}
	}
	close(POND);
	$pond_file = "";
}

