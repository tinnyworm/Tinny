#! /usr/local/bin/perl -w

use Data::Dumper;
$Data::Dumper::Purity=1;

$clientListText = $ARGV[0]; 
%clientName = ();
%clientPortfolio = ();

open (IN, "< $clientListText") || die "Couldn't open $clientListText";
while (<IN>){
	chomp;
	($raw, $outDB, $port) = split ("\t", $_);
	  
	### Check the client
	  if ($outDB =~ /^\w+(\d{4})/){
			$cltID= $1; 
	  } 

	  if ("$cltID" eq "$raw" || $raw =~ /^\w+/){
	  
			$clientName{$raw}= $outDB;
			$clientPortfolio{$raw} = $port; 
     } else {
 			print STDERR "Mismatch!! $outDB:   $raw\n"; 
			
 	  } 
	
#	$clientName{$raw}= $outDB; 	
}
close(IN); 

open (SAVE, "> tsys.client.hash" ) || die "Couldn't write tsys.client.hash :$!";
print SAVE Data::Dumper->Dump([\%clientName], ['*tsysClient']);
open (SAVE1, "> tsys.clientPortfolio.hash" ) || die "Couldn't write tsys.client.hash :$!";
print SAVE1 Data::Dumper->Dump([\%clientPortfolio], ['*tsysClientPortfolio']);
close SAVE1   || die "Couldn't close tsys.clientPortfolio.hash :$!";
close SAVE   || die "Couldn't close tsys.client.hash :$!";


