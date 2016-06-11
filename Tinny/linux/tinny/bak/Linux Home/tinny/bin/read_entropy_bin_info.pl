#! /usr/bin/perl -w

my $flag = 0;

my $number_regex = '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?';

while ( <> ) {

    chomp;

    if ( /^(Field\s\d+:)/ ) {
	print "$1\n";
    }

    # starting point
    if (/Output Summary/) {
	$flag = 1;
    }



    if ( $flag > 0 ) {
	#print STDERR "$_\n";
	if ( /^\(\s*(${number_regex})\D+(${number_regex})\s*\)/ ) {
	    #print "$1 === $2 ==== $3 ==== $4\n";
	    print $3+0, ", ";
	}
    }

    # ending point
    if ( /Running time/ ) {
	$flag = 0;
	print "\n\n";
    }

}
