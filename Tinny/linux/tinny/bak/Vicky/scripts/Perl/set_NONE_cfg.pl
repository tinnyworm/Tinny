#! /usr/local/bin/perl 

$USAGE = << "xxxEOFxxx";
# cat file.cfg | $0 -v <selected variable list to keep> > newfile.cfg
#
# Option:
#    1) If '-v' is not specified, search for log/log_part1_2.scl* files. 
xxxEOFxxx

use Getopt::Std;

#####################################################
# To process the options	
#####################################################
 
#die "$USAGE" if ($#ARGV == -1); 
&getopts('v:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

if (defined($opts{'v'}))
{
	 open (IN, "< $opts{'v'}") || die "Could not open $opts{'v'}\n"; 
	 while (<IN>)
	 {
	 	chomp;
		$var = $_;
		$varm=$var;
		$varm=~ s/^\s+//g;
		$varm =~ s/\s+$//g; 
	   $varm =~ tr/[a-z]/[A-Z]/;
		$varHashI{$varm} += 1;  
	 }
	 
}
else
{
	 open (IN, "cat log/log_part1_2.scl_* | ") || die "Could not open remove.varList\n"; 
	while (<IN>){
		chomp;
		if (! ($_ =~ /^\-\sNote:/))
		{
			next; 
	   }	
	
		if ($_ =~ /^\-\sNote: Variable (\w+\_*) has 0 standard deviation/)
		{
			 $var = $1;
			 $varm=$var;
			 $varm=~ s/^\s+//g;
			 $varm =~ s/\s+$//g; 
	   	 $varm =~ tr/[a-z]/[A-Z]/;
			 $varHashE{$varm} += 1;  
		}
	}
}
print STDERR "NOTE: The variables below have 0 std and cfg is set as NONE.\n";
print STDERR "NOTE: Variable Name , Num of Seen in the log files\n"; 

while (defined($line = <STDIN>)){
	 chomp;
	 if ($line =~ /Name=/){
	 	$linem=$line;
		$linem=~ s/^Name= //g;
		$linem =~ s/\s+$//g; 
		$linem =~ s/^\s+//g;
		$linem =~ tr/[a-z]/[A-Z]/; 	
  }

  if (defined $opts{'v'})
  {
 	 if (defined($varHashI{$linem}))
	  {
			print $line; 	
		}
		else
  		{
			if($line =~ /Slab/){
				 $line =~ s/INPUT/NONE/; 
				 $cc ++; 
				 print $line ;
				 print STDERR "NOTE: $linem, $varHashI{$linem} is set to NONE.\n";  
			}
			else
			{
				 print $line ;
			}
		}

  }
	else   
	{
		 
		if(defined($varHashE{$linem})){
		  
		if($line =~ /Slab/){
			 $line =~ s/INPUT/NONE/; 
			 $cc ++; 
			 print $line ; 
			 print STDERR "NOTE: $key, $varHashE{$linem} is set to NONE\n";
	  } else {
 			print $line ;
 		}
	
  } else
  {
  	print $line;
  }
  }
  
}
