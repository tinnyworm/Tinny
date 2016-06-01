#! /usr/local/bin/perl 
use File::stat;

while (<STDIN>) {
	chomp; 
	$ref = stat($_); 
	$size = $ref->size; 
	if ($size<20) {
		print STDERR "$_ : $size\n";
		`/bin/rm $_`; 
	}
}

