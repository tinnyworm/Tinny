#! /usr/local/bin/perl -w


my $VERSION = '1.0';
my $DEBUG = 1;
my $USAGE =<< "xxxEOFxxx";

################################################################################
################
# Product:
#         Compose a Makefile to run the sampling process in parallel.  
#
# Module: $0 [-c/a/p/t/n/i]  [-w]
# <Working Dir> [-f] <filelist> 
# Note: filelist is the file for all the files need to be sampled. 
#
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


die "ERROR: Please specify the working directory.\n" unless (defined $opts{'w'}); 

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
# To create the directories
#####################################################

$workdir = $opts{'w'}; 

if (!(-d "$workdir/credit")){
		mkdir ("$workdir/credit", 0755) or die "Could not mkdir	$workdir/credit\n"; 
}

if (!(-d "$workdir/debit")){
		mkdir ("$workdir/debit", 0755) or die "Could not mkdir $workdir/debit\n"; 
}
 
if (!(-d "$workdir/log")){
		mkdir ("$workdir/log", 0755) or die "Could not mkdir $workdir/log\n"; 
}

if (!(-d "$workdir/map")){
		mkdir ("$workdir/map", 0755) or die "Could not mkdir $workdir/map\n"; 
}


#####################################################
# To prepare the frauds files and other files hashes
#####################################################

open (IN, "< $opts{'f'}") || die "ERROR: Cannot open filelist file $opts{'f'}\n"; 
while (defined($line=<IN>)){
	 chop($line);
	 push (@dataList, $line); 
 			 
}

#########################################################
# tools will be used in the Makefile
#########################################################

$bmsort  ="/work/price/falcon/bin/bmsort -T /tmp -S";
$multi   ="/home/yxc/scripts/Perl/multi_freq_stdout.pl ";
$tableAll="/home/yxc/scripts/Perl/tableHash_all.pl ";
$getBin  ="/home/yxc/scripts/Perl/get_crd_deb_bin_jfa.pl ";
$fish    ="/work/price/falcon/bin/fishUnsorted.pl ";
$tmp=`uname`;
chomp $tmp;
$fdrFix_a0="/home/yxc/fdr_scripts/fix_fdr_2000_auths.pl";
$fdrFix_a1="/home/yxc/fdr_scripts/fix_fdr_merch_cntry." . $tmp;
$fdrFix_a2="/home/yxc/fdr_scripts/fix_fdr_turkey.pl ";
$fdrFix_c ="/home/yxc/fdr_scripts/fix_fdr_2004_cards.pl ";
$fdrFix_f ="/home/yxc/fdr_scripts/blocktime.pl ";

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

$rule0 .="\nRESS_BIN = " . '$(foreach a, ${DATA' . '},${WORK_DIR}/map/${a}.' . "map.hash\)\n";
$rule1 .= "\$\{RESS_BIN\} "; 
$rule2 .= '${WORK_DIR}/map/%.' . "map.hash:\n";
$rule2 .= "\tcd " . '${WORK_DIR}/$*;\\' . "\n";
$rule2 .= "\t$multi " . '-f ${WORK_DIR}/$*/auths.orig.gz,1-6,181-181 > $@ ;\\'. "\n";
$rule2 .= "\t$tableAll " . '-f $@ > ${WORK_DIR}/map/$*.bin.map.all 2>&1;\\'. "\n";

$rule0 .="\nEXTRACT_BIN = " . '$(foreach a, ${DATA' . '},${WORK_DIR}/map/${a}.' . "map.log\)\n";
$rule1 .= "\$\{EXTRACT_BIN\} ";
$rule2 .= "\n". '${WORK_DIR}/map/%.map.log: ${RESS_BIN}'."\n";
$rule2 .= "\tcd " . '${WORK_DIR};\\' . "\n";
$rule2 .= "\t" . 'cat ${WORK_DIR}/map/$*.bin.map.all | ' . $getBin . '-c ${WORK_DIR}/map/$*.bin.credit -d ${WORK_DIR}/map/$*.bin.debit -e ${WORK_DIR}/map/$*.bin.exclude > $@ 2>&1;'. "\n";

#########################################################
#	Extract auths/cards/frauds by bin for credit and debit
#########################################################
$data .= "\nDATA = ";

for ($i=0; $i<=$#dataList; $i++) {
		if ($i < $#dataList) {
			$data .= "\t" . $dataList[$i] . "\\" . "\n";
		} else {
			$data .= "\t" . $dataList[$i] . "\n";
		}
  }	

  @typeList=("auth", "card", "fraud"); 
foreach (@typeList){
	$rule0 .= "\nRESS_$_ = " . '$(foreach a, ${DATA' . '},${WORK_DIR}/log/${a}.' . "$_.extract.log\)\n";
	$rule1 .= "\$\{RESS_$_\} "; 
	$rule2 .= "\n". '${WORK_DIR}/log/%.' . "$_.extract.log: " . '${EXTRACT_BIN}' . "\n";
	$rule2 .=  get_Target($_);

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
sub get_Target {
	my ($aa, $port);
	my $type = $_[0];

    foreach $port (("credit", "debit")){
		 $aa .= "\t/usr/local/bin/gunzip -c  ";
	 
		 if ($type =~ /^auth/ ){
			$aa .= '$*/auths.orig.gz |' . "$fish -b " .	'${WORK_DIR}/map/$*.bin.' . $port . ' -c1-6 -s1 - | ' . "$bmsort -c1-33 | $fdrFix_a0 | $fdrFix_a1 | $fdrFix_a2 | gzip -c " . '>${WORK_DIR}/' . $port . '/$*/auths.fix.gz 2>&1;\\' . "\n";
		} elsif ($type =~ /^card/ ){
			$aa .= '$*/cards.orig.gz |' . "$fish -b " . '${WORK_DIR}/map/$*.bin. '. $port . ' -c1-6 -s1 -	| ' . "$bmsort -c1-19,91-98 | $fdrFix_c | gzip -c " . '>${WORK_DIR}/' . $port . '/$*/cards.fix.gz 2>&1;\\' . "\n";
		} elsif ($type =~ /^fraud/ ){
			$aa .= '$*/frauds.orig.gz |' . "$fish -b " . '${WORK_DIR}/map/$*.bin.'. $port . ' -c1-6 -s1 - | ' . "$bmsort -c1-19,21-28 | $fdrFix_f " . '${WORK_DIR}/' . $port . '/auths.orig.gz' . " | gzip -c " . '>${WORK_DIR}/' . $port . '/$*/frauds.fix.gz 2>&1;\\' . "\n";
		} else {
			warn "Could not match this type $type!\n"; 
	   }
  } 
	return $aa;
}

#############################

sub dedup {
     my   %seen =();
     my   @uniqu = grep {! $seen{$_}++} @_;
     my  @sortuniqu = sort {$a cmp $b} @uniqu;
        return @sortuniqu;
}
