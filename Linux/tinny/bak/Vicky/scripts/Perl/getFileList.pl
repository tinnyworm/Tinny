#! /usr/local/bin/perl -w

### This program is to generate the monthly filelist for each type
die "Please specify the working directory!\n" if ($#ARGV == -1 );

@list = ("auth","card","post","pay","nonm","inq", "fraud");
$workdir=$ARGV[0]; 

if (!(-d "$workdir/filelist")){
		mkdir ("$workdir/filelist", 0755) or die "Could not mkdir $workdir/filelist\n"; 
}
$mon=""; 

while (defined($line=<STDIN>)){
	chop($line);	
	 	@allFile    = split('/', $line);
		$fileLoc    = @allFile - 1;
		$returnFile = $allFile[$fileLoc];

		@tmp = split('\.', $returnFile);
		$mon  = $tmp[0];
		$type = $tmp[3]; 
	$count_all ++; 	
#		print @mon, "\n"; 
		
	 foreach $key (@list){
		  $fh="f$key"; 
		
			 if ($type =~ /^$key/) {
				  $count_m ++; 
				  open ($fh, ">> $workdir/filelist/$mon.$key.filelist") || die "could
not write to
				 $workdir/filelist$mon.$key.filelist\n";
				print $fh $line,"\n";  
		  } 
		  close ($fh); 

		}
}

if ($count_all == $count_m){
	 print STDERR "Number of files matches: $count_all  : $count_m\n"; 
 } else {
	 $diff=$count_all-$count_m;
	print STDERR "Please check the file name for type: $diff file not	matches\n"; 
}
