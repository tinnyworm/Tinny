#!/usr/local/bin/perl
#
# fishUnsorted 
#
# Takes a 'bait' file and a 'pond' file and goes fishing; pulling records out
# of the pond file which match the bait on the key column (e.g. account number)
# -c specifies the key column in the bait file, -s is the starting column to
# match the key with in the pond file.
#
# $Source: /work/price/falcon/dvl/perl/RCS/fishUnsorted.pl,v $
#
#
#    kmh oct-16-96: Got this from mjk & made some changes.
#
#    kmh dec-06-96: Improved it such that the pond file can
#        also be compressed. 
#
#    kmh dec-10-96: Changed the use of "-u" so that -u means all files
#        are compressed and need to be uncompressed (inversed the usage). 
#
#    kmh dec-30-96: Added "-v" for writing complement.
#
#    kmh mar-05-97: Changed the use of "-u" so that -u means all files
#        are uncompressed. If -u not present, then the files are
#        already compressed and need to be uncompressed. 
#         
#    kmh jun-05-97: Fixed serious bug in finding complement using -v option. 
#
#    pcd jun-11-01: Fixed bug not allowing more than one inputfile
#                   Added ability to use an unsorted input file
#                   Made gzip'd/plain determination file-by-file
#
#
#

#print "Start fishing...\n";

$usage = "\nUsage: $0 -b <bait_file> -c <bait_col_spec> -s <pond_start_col>\n"
         . "               <pond_file> [...] [ > fishfile ]\n"
         . " where:\n"
         . "    -h    :   show this message\n"
         . "    -v    :   write complement (all records in pond file NOT in bait file)\n"
         . "\n"
         . " Uses the length of <bait_col_spec> determine how many columns to use from the pond.\n"
         . " You may use gzipped or ungzipped files at will.\n"
         . "\n"
         . " NOTE: to use STDIN for the pond, you must give the <pond_file> name \"-\"\n"
         . "\n";

require "getopts.pl";

&Getopts('b:c:s:hv');
$bait_file = $opt_b;
$col_spec = $opt_c;
$pond_start_col = $opt_s - 1;
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

%keys = ();

if (substr($bait_file, -3) eq ".gz") {
    $bait_file_str = "/usr/local/bin/zcat $bait_file|";
} else {
    $bait_file_str = "<$bait_file";
}
print STDERR $bait_file_str, "\n";
open(BAIT, "$bait_file_str") || die "Couldn't read $bait_file\n";
while(<BAIT>) {
    $key = substr($_, $bait_start_col, $length);
    $varname{$key}=substr($_,11,40);
    $keys{$key} = 1;
}
close(BAIT);

while ( $pond_file = shift(@ARGV) ) {

    if (substr($pond_file, -3) eq ".gz") {
        $pond_file_str = "/usr/local/bin/zcat $pond_file|";
    } else {
        $pond_file_str = "<$pond_file";
    }
    print STDERR $pond_file_str, "\n";
    open(POND, "$pond_file_str") || die "Couldn't read $pond_file\n";

    while ( <POND> ) {
        $fish_key = substr($_, $pond_start_col, $length);
          if (!defined ($keys{$fish_key}))
             {print};
          if (defined ($keys{$fish_key}))
             {printf("%s%s\n",$varname{$fish_key},substr($_,18,60))};
    }

    close(POND);
    $pond_file = "";
}

