#! /usr/local/bin/perl -w

## This program is to process the frauds file.

use Getopt::Std;

$VERSION = '1.0';
$DEBUG = 0;

$USAGE = "
	Usage:	$0 [ -h ] 

	Version $VERSION

";

&getopts('l:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
die "Fraud filelist required\n" unless (defined $opts{'l'});

$fl = $opts{'l'}; 

open (FL, "/usr/local/bin/cat $fl |") || die "Unable to open $fl for processing frauds \n"; 
	while (defined ($_ = <FL>)) {
		 chomp; 
		 $file = $_; 
            		    next unless (-f $file);
		                die "I need read permissions for '$file'\n" unless (-r _);
      		          push (@file_list, $file);
            		    print STDERR "$file\n";
		}

die "No files to process\n" unless ($#file_list > -1);
close (FL); 
############################################################
#	Parse frauds file names
#	Set 'client_id' and 'date_yyyymm'
############################################################
for $file (@file_list) {
	 
	 $name = substr($file, rindex($file,'/')+1);
 	 
	 unless ((defined $name) && ($name =~ /fraud/i)) {
		die "The file must be 'fraud*':  $name\n";
	}
	$cmd="";

	if ($name =~ /gz$/ || $name =~ /Z$/) {
				$cmd = "/usr/local/bin/gunzip -c $file | ";
	} else {
				$cmd = "/usr/local/bin/cat $file | ";
	}
	
	if ($name =~ /[ebcdic|ebdic]/i){
			if ($name =~ m/.*.(\d{2}).*/) {	
				 $length=$1;
				 $cmd .="dd conv=ascii | addnl $length | "
			  } else {
		  		die "Please specify the length of records.\n";
		  }
	 } 

	 $cmd .=" gzip -c > temp.$name";

	 if ($DEBUG){
	 	print "$cmd\n";
  	 } else {
		`$cmd`;
		print STDERR "$cmd\n"; 
	 }
}


#### process the frauds files

$process="";

$process .="/usr/local/bin/zcat temp.\*fraud\*.gz | cut -c1-49 | bmsort -T /tmp -S -c1-19,21-28,20,29-49 | uniq | rmdupefrauds | gzip -c > frauds.clean.49.gz ";

if ($DEBUG){
	 	print "$process\n";
} else {
		`$process`;
		print STDERR "$process\n"; 
}

#### Clean the temp files

$rm="";
$rm .="/bin/rm temp.*";

if ($DEBUG){
	 	print "$rm\n";
} else {
		`$rm`;
		print STDERR "$rm \n"; 
}




