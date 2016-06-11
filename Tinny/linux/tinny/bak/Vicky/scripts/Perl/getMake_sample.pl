#! /usr/local/bin/perl -w


my $VERSION = '1.0';
my $DEBUG = 0;
my $USAGE =<< "xxxEOFxxx";

################################################################################
################
# Product:
#         Compose a Makefile to run the sampling process in parallel.  
#
# Module: $0 [-c/a/p/t/n/i] -s <sample rate> [-d] <StartDate:EndDate> [-w]
# <Working Dir> [-f] <filelist> [-r] <running frauds file>
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

#die "ERROR: Please specify the file types <-a/c/t/p/n/i>.\n" unless (defined $opts{'a'} || defined $opts{'c'} || defined $opts{'t'} || defined $opts{'p'} || defined $opts{'n'} || defined $opts{'t'}); 

die "ERROR: Please specify the sampling rate <-s>.\n" unless (defined $opts{'s'} && $opts{'s'} =~ /\d+/); 
die "ERROR: Please specify the startDate and EndDate <-d>.\n" unless (defined $opts{'d'}); 

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
# To get file list and file type
#####################################################

@tmp= split(/:/, $opts{'d'}); 
$start=$tmp[0];
$end=$tmp[1];

die "ERROR: Please use YYYYMM for the option -d.\n" unless (($start =~ /\d{6}/) && ($end =~ /\d{6}/)); 

if ($end < $start) {
    die "ERROR: End date is before start date!! Please check it...\n";
} else {
		$i=$start-1;
		while ($i < $end) {
			$i++; 
			if (($i % 100) > 12 ){
				$i += 88;
			}
				push (@monthList, substr($i, 2, 4));
		}
}

#####################################################
# To create the directories
#####################################################

$workdir = $opts{'w'}; 

if (!(-d "$workdir/datas")){
		mkdir ("$workdir/datas", 0755) or die "Could not mkdir $workdir/datas\n"; 
}

if (!(-d "$workdir/stats")){
		mkdir ("$workdir/stats", 0755) or die "Could not mkdir $workdir/stats\n"; 
}
 
if (!(-d "$workdir/log")){
		mkdir ("$workdir/log", 0755) or die "Could not mkdir $workdir/log\n"; 
}

if (!(-d "$workdir/merge")){
		mkdir ("$workdir/merge", 0755) or die "Could not mkdir $workdir/merge\n"; 
}

if (!(-d "$workdir/script")){
		mkdir ("$workdir/script", 0755) or die "Could not mkdir $workdir/script\n"; 
}

if (!(-d "$workdir/monIntersect")){
		mkdir ("$workdir/monIntersect", 0755) or die "Could not mkdir $workdir/monIntersect\n"; 
}


#####################################################
# To prepare the frauds files and other files hashes
#####################################################
@list = ("auth","card","post","pay","nonm","inq", "fraud");

open (IN, "< $opts{'f'}") || die "ERROR: Cannot open filelist file $opts{'f'}\n"; 
while (defined($line=<IN>)){
	 chop($line);
    $name = substr($line, rindex($line,'/')+1);
	 ($date, $client, $port, $content, $inventory, $length, $format, $est) = split(/\./, $name);
	 foreach $key (@list){
			 if ($content =~ /^$key/) {
					push (@ {$keyList{$key}}, $line);
					if ($key ne 'fraud') {
						 push (@keyIn, $key);  
 			 
					}
			 } 
	 }
}

$bmsort="/work/price/falcon/bin/bmsort -T /tmp -S";
$ss="/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats ";
#$rmfrd="/work/price/falcon/bin/rmdupefrauds";
$rmfrd="/home/yxc/scripts/C/rmdupefrauds";

#########################################################
# To process the frauds files if chosen.
#########################################################

if (defined $opts{'r'}){
	foreach (@{$keyList{'fraud'}}){
		$name = substr($_, rindex($_,'/')+1);
		($date, $client, $port, $content, $inventory, $length, $format, $est) = split(/\./, $name); 
		print STDERR $name, "\n"; 
		push (@temp0, $length);
		push (@temp1, $format);
		push (@temp2, $est);
 	}
	
	@temp=dedup(@temp0); 
	die "Frauds files are not in equal length. Please ignore this option.\n" if ($#temp<0);
	@temp=dedup(@temp1); 
	die "Frauds files are not in equal format. Please ignore this option.\n" if ($#temp<0);
	@temp=dedup(@temp2); 
	die "Frauds files are not in equal extension. Please ignore this option.\n" if ($#temp<0);
		
		@fraud = @ {$keyList{'fraud'}};
 
		if (($est =~ /gz$/i) || ($est =~ /Z$/i)){
			  $cmd = "/usr/local/bin/gunzip -c @fraud | "; 
		} else {
		  	  $cmd = "/usr/local/bin/cat @fraud | "; 
		}

		$length=$temp0[0]; 

		if (($format =~ /ebcdic/i) || ($format =~ /mxfrmt/i)) {
				$cmd .= "/usr/local/bin/dd conv=ascii | /work/price/falcon/bin/addnl $length | ";
  		}
		$cmd .= "$bmsort -c1-19,21-28,20,29-49 | uniq | $rmfrd | gzip -c > $workdir/merge/frauds.clean.gz";

	if ($DEBUG) {
		foreach (keys %keyList){
		print "  @{ $keyList{$_}}, \n"; 
		}
	} else {
		`$cmd`;	
	}
}
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

$make .='CMRG  = /home/yxc/scripts/Script/col_merge.sh
MONINTER = /home/yxc/scripts/Perl/Intersectmatrix_gar.pl';
$make .= "\n"; 

#########################################################
# Compose the Makefile for all the files chosn
#########################################################
$srate=$opts{'s'};
$fraud="$workdir/merge/frauds.dat.gz";


### for all files input ###

foreach $type (dedup(@keyIn)) {
		print STDERR "$type is found!\n";	
		foreach (@ {$keyList{$type}}){
		 $cc=get_Target($_, $type, $srate, $fraud, $workdir);

 		 $name = substr($_, rindex($_,'/')+1); 
		 push (@ {$dataList{$type}}, $name); 
		 $fh = "f$name";
		 open ($fh, "> $workdir/script/$name.sh"); 
		 print $fh $cc;
	
		 open (AL, ">> $workdir/monIntersect/$type.list");
		 $name =~ s/\.Z/\.gz/g; 
		 print AL "$workdir/datas/$name\n";
		 	$count{$type} ++;  
		 	$sum ++; 
   	}
		$mergeSet .= "$typeH{$type}[2] ";
	   $listSet .= "$typeH{$type}[2] $type.list ";	

}

@v = values %count;
$ave = $sum / ( $#v + 1 ); 

#print STDERR "$sum  $ave\n";  

foreach (keys %count){
	 #	 print STDERR "$_ has $count{$_} files\n";
	 $value = $count{$_} * 1 ; 
	 warn "WARN: $_ has $value files\n" unless ($value == $ave); 
}



### MAKE FILE write-up ####
$data  = ""; 
$rule0 = ""; 
$rule1 = ""; 
$rule2 = ""; 

foreach (keys %dataList) {
	@value=@ {$dataList{$_}};
	$data .= "\nDATA_$_ = ";
	for ($i=0; $i<=$#value; $i++) {
		if ($i < $#value) {
			$data .= "\t" . $dataList{$_}[$i] . "\\" . "\n";
		} else {
			$data .= "\t" . $dataList{$_}[$i] . "\n";
		}
	}
	
	$rule0 .= "\nRESS_$_ = " . '$(foreach a, ${DATA_' . $_ . '},${WORK_DIR}/log/${a}.' . "$_.log\)\n";
	$rule1 .= "\$\{RESS_$_\} "; 
	$rule2 .= '${WORK_DIR}/log/%.' . "$_.log:\n";
	$rule2 .=  "\t\/bin\/sh $workdir/script/\$\*.sh \> $workdir/log/\$\*.$_.log 2\>\&1\;\n\n"; 
}
$rule0 .= 'MONTHINT = ${WORK_DIR}/log/monIntersect.log' . "\n";

$rule0 .= 'COLMERGE = ${WORK_DIR}/log/col_merge.log' , "\n";

$make .= $data . $rule0; 

$make .= "\n\nall : $rule1 " . '${COLMERGE} ' . '${MONTHINT} ' . "\n\n"; 

$rule2 .= "\n" . '${WORK_DIR}/log/monIntersect.log: ' . $rule1 . '
	cd ${WORK_DIR}/monIntersect; \
	${MONINTER} ' . $listSet . '-p 1 >$@ 2>&1;' . "\n";

$rule2 .= "\n" . '${WORK_DIR}/log/col_merge.log: ' . $rule1 . '
	cd ${WORK_DIR}; \
	${CMRG} ' . $mergeSet . '>$@ 2>&1;';
  
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
	my $file = $_[0]; 
	my	$name = substr($file, rindex($file,'/')+1);
	my ($date, $client, $port, $content, $inventory, $length, $format, $est) = split(/\./, $name); 
	my ($aa, $bmsort, $ss);
	my $type = $_[1];
	my $srate= $_[2];
	my $fraud= $_[3];
	my $workdir= $_[4];

	if (($est =~ /gz/i) || ($est =~ /Z/i)){
		 $aa = "/usr/local/bin/gunzip -c $file | "; 
	} else {
	  	 $aa = "/usr/local/bin/cat $file | "; 
	}
	
	if (($format =~ /ebcdic/i) || ($format =~ /mxfrmt/i) || ($format =~ /ebdic/i)) {
		  $aa .= "/usr/local/bin/dd conv=ascii |"; 
  			if ($length =~ /\d+/) {
		 		  $aa .= " /work/price/falcon/bin/addnl $length | ";
			} else {
		 		 die "Please check the length in the filename $name\n"; 
			}
	}
	
	%typeH=(
				'auth'  => ['-a', '-c1-33', '-a'], 
				'card'  => ['-c', '-c1-19,91-98', '-c'],
				'post'  => ['-p', '-c1-19,49-56','-t'],
				'pay'   => ['-P', '-c1-19,82-89','-p'],
				'nonm'  => ['-N', '-c1-19,21-28','-n'],
				'inq'   => [' ', '-c1-19,21-28','-i']
			);
	$bmsort="/work/price/falcon/bin/bmsort -T /tmp -S";
	$ss="/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats ";

	$aa .= "$ss -s $srate -f $fraud $typeH{$type}[0] -o $workdir/stats/$name| ";	  
	$name =~ s/\.Z$/\.gz/g; 
	
	$aa .= "$bmsort $typeH{$type}[1] | gzip -c > $workdir/datas/$name\n";
	
	return $aa;
}

#############################

sub dedup {
     my   %seen =();
     my   @uniqu = grep {! $seen{$_}++} @_;
     my  @sortuniqu = sort {$a cmp $b} @uniqu;
        return @sortuniqu;
}
