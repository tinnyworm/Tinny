#! /usr/local/bin/perl -w


my $VERSION = '2.0';
my $DEBUG = 0;
my $USAGE =<< "xxxEOFxxx";

################################################################################
################
# Product:
#         Compose a Makefile to run the sampling process in parallel.  
#
# Module: $0 [-d] <StartDate:EndDate,
# 20050401:20050731> [-w]
# <Working Dir> [-f] <data directory> 
# Note:  
# Version: $VERSION
#
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
&getopts('s:d:w:f:hractpni', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

#die "ERROR: Please specify the sampling rate <-s>.\n" unless (defined $opts{'s'} && $opts{'s'} =~ /\d+/); 
die "ERROR: Please specify the startDate and EndDate <-d>.\n" unless (defined $opts{'d'}); 

die "ERROR: Please specify the working directory.\n" unless (defined $opts{'w'}); 
die "ERROR: Please specify the data directory.\n" unless (defined $opts{'f'}); 
#####################################################
# To get file list and file type
#####################################################
$tmp = `date`;
chomp $tmp;
print STDERR "Command line: $0 @ARGV\n";
print STDERR ("Starting on $tmp\n");
$tmp = `uname -ns`;
chomp $tmp;
print STDERR ("Host is $tmp\n");
print STDERR ("Version $VERSION\n\n");

#####################################################
# To get file list and file type
#####################################################

@tmp= split(/:/, $opts{'d'}); 
$start=$tmp[0];
$end=$tmp[1];

#####################################################
# To create the directories
#####################################################

$workdir = $opts{'w'}; 

%t_list =(
		"auth" => "auths",
		"card" => "cards",
		"post" => "posts",
		"pay"	 => "paynmt",
		"nonm" => "nonmons",
		"inq"  => "inqs",
		"fraud"=> "frauds"
		);
	
if (!(-d "$workdir/log")){
			mkdir ("$workdir/log", 0755) or die "Could not mkdir	$workdir/log\n"; }

open (IN, "< $opts{'f'}") || die "ERROR: Cannot open directory file $opts{'f'}\n"; 
while (defined($line=<IN>)){
	 chop($line);
    push (@dataList, $line); 
 
	if (!(-d "$workdir/$line/dqr")){
			mkdir ("$workdir/$line/dqr", 0755) or die "Could not mkdir	$workdir/$line/dqr\n"; 
	}

#####################################################
# To dqr mb source files and command
#####################################################
$mbCode="/home/yxc/mb_dqr/dqr_new/mb_DQR.pl.v2";
$cmd="$mbCode -m mnappmb09 -c $line -v 410 -p Retail -w $workdir/$line -s $opts{'d'}"; 
		
if (!$DEBUG) {
	print STDERR $cmd . "\n"; 
	`$cmd`;
} else {
	print STDERR $cmd . "\n";
}

#####################################################
# To link the data in falconer4 standard name
#####################################################
 if (!$DEBUG) {
	$temp=`ls $workdir/$line/*gz > tempList`;
	open($temp, "< tempList") || die ("cannot open tempList!\n"); 
	while ($t_line =<$temp>){
		chop($t_line);
		$fname = substr($t_line, rindex($t_line,"/")+1); 
		($mon, $client, $type, $extra0, $ext) = split(/\./, $fname);
		if (!defined($t_list{$type})){
			die "Could not find such type :$type in $fname matches with keys in \%t_list\n"; 
		} else {
			$finalData= "$t_list{$type}.dat.gz";
		#	print STDERR "filename $t_line $finalData\n";  
			`cd $workdir/$line; ln -s $fname $finalData;`; 
		}
	}
#	close($temp);
	`rm tempList`;
 }
}

#########################################################
# tools will be used in the Makefile
#########################################################

$checkAll ="/home/yxc/check/check_all"; 
$addMore  ="/home/yxc/scripts/Script/add_more.sh"; 		

#########################################################
# Compose the Makefile for all the files chosn
#########################################################
$make = ""; 
$make = "#" x 20;
$make.= "\n# Makefile for sampling and merging and check";
$make.= "\n# make -j<njobs> -l<maxload> [-f <Makefile of interest>]\n";
$make.= "#" x 20;
$make.= "\nSHELL = /bin/sh\n\n";
$make.= "WORK_DIR =" . $workdir . "\n\n";


#########################################################
# Compose the Makefile for all the files chosn
#########################################################

### MAKE FILE write-up ####
$data  = ""; 
$rule0 = ""; 
$rule1 = ""; 
$rule2 = ""; 
#########################################################
#	Extract auths for BIN map re Debit/Credit
#########################################################

$rule0 .="\nANALYSIS = " . '$(foreach a, ${DATA' . '},${WORK_DIR}/log/${a}.' . "analysis.log\)\n";
$rule1 .= "\$\{ANALYSIS\} "; 
$rule2 .= '${WORK_DIR}/log/%.' . "analysis.log:\n";
$rule2 .= "\tcd " . '${WORK_DIR}/$*;\\' . "\n";
$rule2 .= "\t$checkAll " . '-c cards.dat.gz -a auths.dat.gz -f frauds.dat.gz -t posts.dat.gz >$@ ;\\'. "\n";

$rule0 .="\nADDMORE = " . '$(foreach a, ${DATA' . '},${WORK_DIR}/log/${a}.' . "addmore.log\)\n";
$rule1 .= "\$\{ADDMORE\} ";
$rule2 .= "\n". '${WORK_DIR}/log/%.addmore.log:'."\n";
$rule2 .= "\tcd " . '${WORK_DIR}/$*;\\' . "\n";
$rule2 .= "\t$addMore " . '-c cards.dat.gz -a auths.dat.gz -f frauds.dat.gz -t posts.dat.gz >$@ ;\\'. "\n";

#########################################################
#	Extract auths/cards/frauds by bin for credit and debit
#########################################################
$data .= "\nDATA = ";
$i=0; 
foreach (@dataList) {
		if ($i < $#dataList) {
			$data .= "\t" . $_ . "\\" . "\n";
		} else {
			$data .= "\t" . $_ . "\n";
		}
	$i++; 	
}

$make .= $data . $rule0; 

$make .= "\n\nall : $rule1 " . "\n\n"; 

$make .= $rule2; 

if ($DEBUG){
	 	 print  $make, "\n"; 
} else {
	open (OUT, "> $workdir/Makefile ") || die "Cannot write it to $workdir\n";	
	print OUT $make;

}

############################################################
####  Subrountines 
####  A) subrountines for dedup the duplicates elements
############################################################

#############################

sub dedup {
     my   %seen =();
     my   @uniqu = grep {! $seen{$_}++} @_;
     my  @sortuniqu = sort {$a cmp $b} @uniqu;
        return @sortuniqu;
}
