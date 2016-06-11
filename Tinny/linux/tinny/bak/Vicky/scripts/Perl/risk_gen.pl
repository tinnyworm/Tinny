#! /usr/local/bin/perl -w

################################################################
# add the risktable by different clients
#
### yxc created on 200604
################################################################

$VERSION = '1.0'; 
$USAGE = << "xxxEOFxxx";

################################################################################
################
# Product:
#	Do the risktable generation 
#
# VERSION: $VERSION
#
# Module:
#	$0 -c <client_input> -w <working directory> -s <data/select dir>
#		-o <risk Name>[-h]
#
# Required:
#	-c: client_input
#  -o: final risktable name
#		
# Optional:
#	-h: shows this message
#	-w: default is current directory
#	-s: default is current directory
#			
#	Example:
#		$0 -c client_input -w /work/minolta/GR_3.0/02_newRisk -s /work/minolta/GR_3.0 -o ge_2006rskt
#		
#		
################################################################################
#######
xxxEOFxxx
use Getopt::Std;

#####################################################
# To process the options	
#####################################################
 
die "$USAGE" if ($#ARGV == -1); 
&getopts('c:w:s:o:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

die "ERROR: No client input found. Quiting..." unless (defined $opts{'c'}); 
die "ERROR: No output risktable name. Quiting..." unless (defined $opts{'o'});

if (defined("$opts{'w'}")){
	die "ERROR: Please specify the working directory.\n" unless (defined $opts{'w'});
	$work = $opts{'w'}; 
} else {
	$work = "./"; 
	print STDERR "-w not specified. Use the default value of current directory.\n"; 
}

if (defined("$opts{'s'}")){
	die "ERROR: Please specify the directory for select and data.\n" unless (defined $opts{'s'});
	$set = $opts{'s'}; 
} else {
	$set = "./"; 
	print STDERR "-s not specified. Use the default value of current directory as the data/select input.\n"; 
}

$os=`uname -s`; 
chop($os); 

##<<<<<<<<<<<<<<<<< Change if needed >>>>>>>>>>>>>####
$script="/work/price/falcon/$os/bin/risktabgen40 -c840 -mC2U -w -t0 -D100000";

$amtScrpt="/work/price/falcon/$os/bin/riskamtgen40 -c036 -B75,150,400 -N -M100";

$script_2 = "cat ACR4tab ASR4tab APR4tab AUR4tab ASR4tab HWR4tab | sed 's/POST_RISK_DATA/POSTAL_RISK_DATA/' | /work/price/falcon/bin/fix_risk_penny-sdev.pl |/home/yxc/models/clean_postal_risk.pl > $opts{'o'}; "; 
######################################################

######################################################
# To process the script
######################################################

$client=$opts{'c'};
$set_i = ""; 
open (IN, "< $client"); 
open (OUT, "> $work/riskgen.sh") || die "Could not write for $work/riskgen.sh\n"; 

while (defined($line=<IN>)){
	chop($line); 
	($clt, $cov, $sr) = split(/\,/, $line);

  if (!(-d "$set/data/$sr/$cov/$clt")){
		die "ERROR: $clt-$cov-$sr data does not exist!\n"; 
  }

  if (!(-e "$set/select/$sr/rgen/select.rgen.$clt.$cov")){
  		die "ERROR: $clt-$cov-$sr select file does not exist!\n"; 
  }
	
  open (SLT, "<$set/select/$sr/rgen/select.rgen.$clt.$cov");
  while (defined($slt_line=<SLT>)){
		chop($slt_line); 
		if ($slt_line =~ /GMTOFFSET=/){
			 @temp=split(/=/, $slt_line);
			 if ($temp[1] =~ /\./){
				  @tmp=split(/\./, $temp[1]);  
				  $offset=$tmp[0]; 
			 }
			 print STDERR "$clt-$cov-$sr has offset as $offset\n"; 
		}	
	}
  
	$set_i .=" -i$set/data/$sr/$cov/$clt -l$set/select/$sr/rgen/select.rgen.$clt.$cov -z$offset "
}
close(IN);
$script .= $set_i; 
print OUT $script . "\n"; 

print OUT $amtScrpt . "\n"; 

print OUT $script_2 . "\n"; 

print STDERR "\nThe script is written out...\n";
print STDERR "$script\n\n$amtScrpt\n\n$script_2\n\n";

close(OUT); 
