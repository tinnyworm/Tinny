#! /usr/bin/perl -w -s

our($country);

if ( ! defined $country ) {
    print "Usage: $0 -country=cntry\n";
    exit 99;
}

my $prevTabName = "previous";
my $currTabName = "current";

while ( <> ) {

    chomp;
    #print "$_\n" if /^TABLE/;

    if ( /^TABLE/ ) {

	my @data = split /\s+/;
	$currTabName = $data[1];
	print STDERR "Table Name: $currTabName.$country\n";

	if ( $currTabName ne $prevTabName ) {
	    close(OUTFILE) if ( defined OUTFILE );
	    open( OUTFILE, "> ${currTabName}.$country" ) or die "$!: ${currTabName}.$country";
	}

	print OUTFILE "$_\n";

    }

	print OUTFILE "$_\n";


}

close(OUTFILE);

