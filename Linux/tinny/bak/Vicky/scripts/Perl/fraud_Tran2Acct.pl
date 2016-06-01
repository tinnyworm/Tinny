#! /usr/bin/env perl

$DEBUG=0;
$VERSION=1; 

$usage=<< "xxxEOFxxx";
###########################################
#	This program is to convert the fraud 
#	transaction level to account level. 
#
#	Example:
#  zcat file | $0 -c <-t tag.frauds.file>
#
#	Option: 
#		-c: To convert to accout level upon ffd.
#		-t: To adjust the bigScore NF trans. 
#
#	Author: yxc 
#	Date  : Dec.2, 2005
#
###########################################
xxxEOFxxx
############################################################
#       Process options
############################################################
$opt_t = 0;
$opt_c = 0;

use Getopt::Std;
getopts('ch:t:');

#die "$usage" if ($#ARGV == -2 || $opt_h);

############################################################
#       Write start time, machine name and version
############################################################
my $tmp = `date`;
chomp $tmp;
my $sttime=$tmp;
print STDERR "Command line: $0 @ARGV\n";
print STDERR ("Starting on $tmp\n");
$tmp = `uname -ns`;
chomp $tmp;
print STDERR ("Host is $tmp\n");
print STDERR ("Version $VERSION\n\n");

$client=$ARGV[0]; 

############################################################
# Start to process
############################################################

if ($opt_c) {
	 while (<STDIN>){
	 	chomp; 
		($acct, $ffd, $fft)= unpack ("\@0 a19 \@20 a8 \@29 a6", $_);  
		
		if ($ffd !~ /^\s+$/ and $fft !~ /^\s+$/){
			 $ffdt=$ffd . $fft; 
		} elsif ($ffd !~ /^\s+$/ ) {
			 $ffdt=$ffd . "      "; 
		}else {
  			$count ++; 
	  }
	  	  		push ( @{$out{$acct}}, $ffdt); 
 	}
		
	 if ($count > 0 ) {
	 	warn "$count records are found without any ffd and fft info!\n"; 
  }
  
	open (OUT, "|/usr/local/bin/gzip > fraud.byRec.gz") || die "Could not write in fraud.byRec.gz\n";  
	 
	 	foreach (sort keys %out){
			@value= sort {$a <=> $b} @{$out{$_}}; 
			$ffdt_0 = $value[0]; 
			$fddt   = $value[$#value-1]; 
			$line = $_ . " " . substr($ffdt_0,0,8) . " " . substr($ffdt_0,8,6) . $fddt;
		
			print OUT $line, "\n"; 
		  }
	 close(OUT); 	  
} 
elsif (defined $opt_t) {
	open (IN, "/usr/local/bin/zcat $opt_t | ") || die "Could not open $opt_t!\n"; 	
	while (<IN>){
		chomp; 
		$class{$_}=1; 
  }

  open (OUT1, "|/usr/local/bin/gzip -c > ./$client.bigScr.adjust.gz") || die "Could not write in bigScr.adjust.gz\n";
  while (defined($line= <STDIN>)) {
  		chop($line);
		($acct, $amt, $date, $time, $type, $pos, $sic, $mzip, $mcntry)=unpack("\@5 a14 \@20 a13 \@34 a8 \@52 a6 \@90 a1 \@92 a1 \@94 a4 \@127 a9 \@138 a3", $line); 

		$comKey="     " . $acct . $date . $time . $amt . $sic . $mzip . $mcntry . $pos . $type;
#	print $comKey, "\n"; 

		$fraudTag=substr($line, 59,1); 

		if ($fraudTag eq "1"){
		
			if (!defined($class{$comKey})){
					 substr($line, 20, 13) = "         0.01";
					 print OUT1 $line, "\n"; 
				#	print STDERR "$line\n";  
		   } else {
 				print OUT1 $line, "\n"; 
 			}
	  } else {
 				print OUT1 $line, "\n";
 	  }
  }

	 

}
else {
	die "Please choose one of the two options. See usage:\n$usage"; 
}

