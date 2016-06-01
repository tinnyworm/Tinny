#! /usr/local/bin/perl -w

use Getopt::Std;
print STDERR "Command line: $0 @ARGV\n"; 

$USAGE =<< "xxxEOFxxx";

#################################################
# To extract the Rule engine function from the directory
#
# Module:	
#	$0 -t <variables of interest> -v <variable list> -p <profile var>
#
#################################################
xxxEOFxxx

#### Chech argv list #######
############################

die "$USAGE" if ($#ARGV == -1 );
%opts = ();
&getopts('t:v:p:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
die "ERROR: Variable lists do not exist or empty!" unless (-f $opts{'t'} and -s _) ; 

#### Get files from wildcard ####
#################################

if (defined $opts{'v'}) {
	for $file (glob $opts{'v'}){
		next unless (-f $file);
		die "ERROR: I need read permission for '$file'\n" unless (-r _); 
		push (@temp_list, $file); 
	}
} 

die "ERROR: No files to process\n" unless ($#temp_list > -1);

foreach $inVar (@temp_list){
	 print $inVar, "\n"; 
	 $name= substr($inVar, rindex($inVar, "/")+1); 
	 $fh="f$name"; 
	 open ($fh, "< $inVar") || die "Could not open the file $inVar\n"; 
	 
	 while (defined($_ = <$fh>)){
		chomp;
		
			if ($_ =~ /^VAR\s+(\w+)/){
				 $variable = $1;
				 $variable =~ s/\s*$//g; 
				 $variable =~ s/^\s*//g;
				 $variable =~ tr/[a-z]/[A-Z]/; 
			}   
   
	   	if ($_ =~ /[\(]/){
				  $line = $_; 
				  $line =~ s/\;//; 
				  if ((defined($varList{$variable}[0]) && 
							 ($varList{$variable}[0] ne $line)) && 
							 (defined($varList{$variable}[1]) && 
							 ($varList{$variable}[1] ne $name))){				
					 print STDERR "ERROR: mapping $variable for two defination: " . "$line ,  $varList{$variable}[0]\n"; 
				}	
				 $varList{$variable}[0] = $line;
			    $varList{$variable}[1] = $name; 	 
			} 
  }
  close ($fh); 
}

#foreach $key (sort keys %varList){
#	 print "$key\t:\t $varList{$key}[0]\t: $varList{$key}[1] \n"; 
#}

######################################################
#### Read in profile variables
######################################################

if (defined $opts{'p'}){
	open(PROF, "< $opts{'p'}") || die "Could not open the file $opts{'p'}\n";
	while (<PROF>){
		 chomp;
		 if ($_ =~ /^INPUT\s+(\w+)/){
			  $line=$1;
		     $line =~ s/\s*$//g ;  
		     $line =~ s/^\s*//g ;
			  $profList{$line}=1; 
		  } else {
			 print STDERR "What is this $_?\n"; 
		  }
	}
}

#### Get files from the variable list of interest ####
######################################################

if (defined $opts{'t'}){
	 $seg=$opts{'t'};
	 $segNM=substr($seg, rindex($seg, "/")+1); 
	 
	open (OUT, "> Out.variable.list_$segNM"); 
	open (VAROT, "> $segNM.net"); 
	
	 open(IN, "< $seg") || die "Could not open the file $seg\n"; 
	 while (<IN>){
		  chomp;
		  if ($_ =~ /^INPUT\s+(\w+)/){
				$varInt = $1;
				$varInt =~ s/\s*$//g ;  
				$varInt =~ s/^\s*//g ;
				$varInt =~ tr/[a-z]/[A-Z]/; 
	#		 print $varInt, "\n"; 	
			 if (defined $varList{$varInt}){
					 print OUT "$varInt\t:\t$varList{$varInt}[0]\t:\t$varList{$varInt}[1]\n"; 
				} else {
		  			 print STDERR "Mismatch: $varInt\n"; 
				}
				
				if (!defined $profList{$varInt}){
		 			print VAROT "INPUT $varInt\n";
					print VAROT "SCALE_FUNC = NONE;\n\n"; 
		 		}
		  }
	 }
	 close(IN); 
}

close(OUT);
close(VAROT); 
