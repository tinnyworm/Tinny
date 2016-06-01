#! /usr/local/bin/perl -w

$USAGE =<< "xxxEOFxxx";

##############################################################
# Usage: $0 -f <file/files> -p <afpr's>
# 
# Example: $0 -f "node*/eval/res.test*" -p "5 10 20 40 60"
#                
##############################################################
xxxEOFxxx
use Getopt::Std;
use File::stat;

die "$USAGE" if ($#ARGV == -1 );
%opts = ();
&getopts('f:p:h', \%opts);
die "$USAGE" if (defined $opts{'h'});
@file_list=();

if (defined $opts{'f'}){
      # Allow for wildcards in each file name
  for $file (glob $opts{'f'}) {
     next unless (-f $file);
     die "ERROR: I need read permissions for '$file'\n" unless (-r _);
     $ref = stat($file); 
     $size = $ref->size;
    if ($size >20) {
        push (@temp_list, $file);
    } else { 
		  print STDERR "WARN: $file seems empty. \n";
    }
  }
}

@file_list = dedup(@temp_list); 
die "ERROR: No files to process\n" unless ($#file_list > -1);

#if (defined $opts{'a'}) {
#	@afprList=split(',', $opts{'a'}); 
#}

$afprs=$opts{'p'}; 
############################################################
#	get afprs for each file_list
############################################################
$GETAFPR="/work/price/falcon/bin/getafpr"; 
#print STDERR "Node#,Epch#,$afprs\n"; 
open (ADR, "> summary_onADRs.csv") || die "Could not write in summary_onADRs.out.\n"; 
open (RTVDR, "> summary_onRTVDRs.csv") || die "Could not write in summary_onRTVDRs.out.\n"; 
open (OLVDR, "> summary_onOLVDRs.csv") || die "Could not write in summary_onOLVDRs.out.\n"; 

foreach $ff (sort @file_list){
#	 @tmp0=split('/', $ff); 
#	 $tmp1=$tmp0[0]; 
#	 @tmp2=split('\.',$tmp1); 
	 	 
	 #print STDERR "\n$GETAFPR $ff $afprs\n\n"; 
	 #	print "$tmp2[0],$tmp2[1]";  
	 print ADR "$ff"; 
	 print  RTVDR "$ff";
	 print OLVDR "$ff"; 
	
	open (IN, "$GETAFPR $ff $afprs |") ; 
	while (<IN>){
   	 if (/^\s+\d+\.\d+\s+\d+\s+(\d+\.\d+)/){
			  $adr = $1;
			  $_ =~ s/\$/ /g;
			  $_ =~ s/\%/ /g;
			  @tmp = split /\s+/, $_; 
			  $rtvdr = $tmp[7];
			  $olvdr = $tmp[8];  
			  print ADR ",$adr"; 
			  print RTVDR ",$rtvdr"; 
			  print OLVDR ",$olvdr"; 
	  		}

	  
	}
	close(IN); 
	print ADR "\n"; 
	print RTVDR "\n";
	print OLVDR "\n"; 
}

close(ADR);
close(RTVDR);
close(OLVDR);
############################################################
####  Subrountines 
####  A) subrountines for dedup the duplicates elements
############################################################

sub dedup {
     my   %seen =();
     my   @uniqu = grep {! $seen{$_}++} @_;
     my  @sortuniqu = sort {$a cmp $b} @uniqu;
        return @sortuniqu;
}


