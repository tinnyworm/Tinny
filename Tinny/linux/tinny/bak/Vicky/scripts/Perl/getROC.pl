#!/usr/local/bin/perl -I/home/bxz/bin/perlmod

##############################################
# usage added.

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	This program is to extract the information from Research file 
	and estimated the result for each AFPR point.

	Usage: $0 research.txt or $0 \"res.*\"

\n\n";

die "$USAGE" if ($#ARGV == -1);

##############################################

use Interpolate;

$input=$ARGV[0]; 
print "file,nf,f\n";

for $file (glob $input)
{
	next unless (-f $file); 

	@nfs=(); 
	@fs = (); 
	open (IN, "<$file");
		
    while($_=<IN>){
		  if($_ !~ /False Assigns   Valid Assigns/)
		  {
		  
		  }
		  else 
		  {
    	    $hof = <IN>; 
			 for ($i = 0 ; $i<101; $i++)
			 {
				$_ = <IN>; 
				$_ =~ s/\%//g; 
		      if ($_ =~ /^\s*(\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)/){
      		  push @scores,$1;
		        push @nfs,$2;
      		  push @fs,$3;
				  push @tfpr,$4;
			 }
		}
  }
  }
    
	 $out_ad=Interpolate::int_2pt(\@nfs,\@fs,0.5);
	
      print "$file,0.5,$out_ad\n";
		
}
