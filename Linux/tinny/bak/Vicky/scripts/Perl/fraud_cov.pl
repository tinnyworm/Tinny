#! /usr/local/bin/perl -w

$date=$ARGV[0]; 

while (<STDIN>){
	$ffd=substr($_,20,6); 
	if ($ffd >= $date) {
		print STDOUT $_; 
	}
}
