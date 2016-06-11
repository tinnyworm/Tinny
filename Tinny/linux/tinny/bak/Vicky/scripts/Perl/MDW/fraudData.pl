#! /usr/local/bin/perl -w

## This program is to update the TABLE: FRAUD_MONTH_DATA 
## This program will be extended based on additional requirement.


use strict;
use DBI;
use Getopt::Std;
use File::stat;

my $VERSION = '1.0';
my $DEBUG = 1;

my $USAGE = "
	Usage:	$0 [ -h ]  -p portfolio -f fraud_file -l fraud_filelist

	Version $VERSION

	Expects file names to have the format:
		yymm.client.content_[seq].invoice_number.record_length.format.gz
	where
		yymm 	   		= year and month data was recorded
		client 			= bank name
		content 			= {auth, card, fraud, inq, nonm, paym, post} which may contain \"_seq\" number if multiple files in one month. 
		invoice_number = identifies the source data file from AH code
		record_length  = number of characters per record
		format			= {ebcdic, ascii}

	Required:
		-f frauds file.
			Name of fraud file to process.
			Comma delimited list accepted.
			Wildcards are accepted, but place quotes around
			file names containing wildcards.
		-l fraud file list accepted.
		-p portfolio
			Must be of set {credit, debit, retail, commercial}
		
	Optional:
		-h shows this message

	Examples:

		1) To allow different clients and different months files:
			./fraudData.pl -p credit -f \"< 0* >\"
		2)	To run only one month frauds files per client:
         ./fraudData.pl -p credit -f /ana/mds_files1/iFALCON/WESTPAC/0506/0506.westpac.fraud.064028E.91.ebcdic.gz
  		3) To run a filelist contains multiple months and multiple clients. 
			./fraudData.pl -p credit -l filelist
";

############################################################
#       Write start time, machine name and version
############################################################
my $tmp = `date`;
chomp $tmp;
print STDERR "Command line: $0 @ARGV\n";
print STDERR ("Starting on $tmp\n");
$tmp = `uname -ns`;
chomp $tmp;
print STDERR ("Host is $tmp\n");
print STDERR ("Version $VERSION\n\n");


############################################################
#	Process options
############################################################
die "$USAGE" if ($#ARGV == -1 );
my %opts = ();
&getopts('f:p:l:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
die "Frauds file name or filelist required\n" unless (defined $opts{'f'} || defined $opts{'l'});
die "Portfolio type required\n" unless (defined $opts{'p'});

############################################################
#	Set 'portfolio' field
############################################################

my $prtfls = $opts{'p'}; 
$prtfls=~ tr/[A-Z]/[a-z]/;
############################################################
#	Collect fraud file names
############################################################
print STDERR "Frauds Files:\n";
# Allow for a comma delimited file list
my ($file, @file_list, $temp);
@file_list=();

if (defined $opts{'f'}){
		my @list = split(",", $opts{'f'});
		for $temp (@list) {
			 # Allow for wildcards in each file name
      		  for $file (glob $temp) {
            		    next unless (-f $file);
		                die "I need read permissions for '$file'\n" unless (-r _);
      		          push (@file_list, $file);
            		    print STDERR "\t$file\n";
		        }
		}
} elsif (defined $opts{'l'}){
	my $fl=$opts{'l'}; 		
	open (FL, "/usr/local/bin/cat $fl |") || die "Unable to open $fl for processing frauds \n"; 
	while (defined ($_ = <FL>)) {
		 chomp; 
		 $file = $_; 
			 # Allow for wildcards in each file name
            		    next unless (-f $file);
		                die "I need read permissions for '$file'\n" unless (-r _);
      		          push (@file_list, $file);
            		    print STDERR "\t$file\n";
		}


} else {
	die "$USAGE";
	exit (1);
}
die "No files to process\n" unless ($#file_list > -1);


############################################################
#	Parse frauds file names
#	Set 'client_id' and 'date_yyyymm'
############################################################
my ($name, $yymm, $client, $content, $length, $format, $ext, $invoice);
for $file (@file_list) {
	$name = substr($file, rindex($file,'/')+1);
	($yymm, $client, $content, $invoice, $length, $format, $ext) = split(/\./, $name);
	
	# File name format:  yymm.client.content.invoice_number.record_length.format.gz
	unless ((defined $yymm) && ($yymm =~ /^\d{4}/)) {
		die "Incorrect 'date' field in '$name'\n";
	}
	unless ((defined $content) && ($content =~ /^fraud/i)) {
		die "Content type must be 'fraud*':  $name\n";
	}
	unless ((defined $length) && ($length =~ /^\d+$/)) {
		die "Incorrect 'record_length' field in '$name'\n";
	}
	unless ((defined $format) && (($format=~/^ascii$/i) || ($format=~/^ebcdic$/i))) {
		die "Incorrect 'format' field in '$name'\n";
	}
}

################################################################
#	Allow the multiple clients and multiple month in the filelist
#	Process frauds files
#	push 'data_size', 'rec_count' into Hash array
############################################################
my ($key, $jk, %ref);
my %outH = ();
my %final =();
my %size =();
my %recCount =();

for $file (@file_list) {
		my	$name = substr($file, rindex($file,'/')+1);
		my ($yymm, $client, $content, $invoice, $length, $format, $ext) = split(/\./, $name); 
		
		$client =~ tr/[A-Z]/[a-z]/; 	
		$key='20' . $yymm . ":" . $client; 
		$outH{$key}=1;
		
		$ref{$key} = stat($file);
		$size{$key} += $ref{$key}->size;
		
		my $cmd = '';
		if ($ext =~ /^gz$/ || $ext =~ /^Z$/) {
				$cmd = "/usr/local/bin/gunzip -c $file | ";
		} else {
				$cmd = "/usr/local/bin/cat $file | ";
		}
	if ($format =~ /ebcdic/i) {
		$cmd .= "dd conv=ascii | addnl $length | ";
	}
	open (FIN, $cmd) || die "Unable to open $file for reading\n";
	
	print STDERR "###>>>  HERE is the file: $file  <<<###\n";
	
	while (defined ($_ = <FIN>)) {
#		last if (($outH{'rec_count'} == 10000) && ($DEBUG));

				$recCount{$key} ++;
	}
	close (FIN);
}

foreach $jk (keys %outH){
	 my ($tmp1, $tmp2)=split(':', $jk);
	 push ( @{$final{$jk}}, $tmp2, $tmp1, $prtfls,$recCount{$jk}, $size{$jk}); 
	 print STDERR "$final{$jk}[0] : $final{$jk}[1] :  $final{$jk}[2]  :  $final{$jk}[3] :  $final{$jk}[4]\n\n"; 
}



############################################################
#	Fill Database
############################################################

my $cmd = '';

### Connect
my $dbh = DBI->connect(
	'dbi:Oracle:mdwd0',
	'yxc',
	'yxc',
) || die "Database connection not made: $DBI::errstr";

### Insert statement
foreach $jk (keys %final) {
	 
	$cmd = "INSERT INTO FRAUD_MONTH_DATA (client_id,date_yyyymm,portfolio,rec_count,data_size) VALUES ('$final{$jk}[0]','$final{$jk}[1]','$final{$jk}[2]','$final{$jk}[3]','$final{$jk}[4]')";
	print STDERR "$cmd\n" if ($DEBUG);
	$dbh->do( $cmd );
}

#### Disconnect
$dbh->disconnect();



############################################################
#	End time
############################################################
$tmp = `date`;
chomp $tmp;print STDERR ("Finished on $tmp\n");
