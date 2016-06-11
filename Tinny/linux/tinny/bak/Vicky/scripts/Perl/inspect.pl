#! /usr/bin/perl -w

# JSA 1/3/2005

$VERSION = "Falcon Credit Data Spec 3.10";
$DEBUG = 0;

# Make STDOUT hot
select((select(STDOUT), $|=1)[0]);

# bmsort path and executable
$BMSORT = "/work/price/falcon/bin/bmsort";

# sampstats path and executable
$SAMPSTATS = "/work/price/falcon/bin/sampstats";

# rmdupefrauds path and executable
$RMDUPEFRAUDS = "/work/price/falcon/bin/rmdupefrauds";

# testChecksum path and executable
$TESTCHECKSUM = "/work/price/falcon/bin/testChecksum";

# ufal_intersect.pl path and executable
$UFAL_INTERSECT = "/work/price/falcon/bin/ufal_intersect.pl";

# multi_freq.pl path and executable
$MULTI_FREQ = "/home/yxc/scripts/Perl/multi_freq.pl";

# tableHash.pl path and executable
$TABLEHASH = "/home/yxc/scripts/Perl/tableHash.pl";

# vfrauds.pl path and executable
$VFRAUDS = "/work/price/falcon/bin/vfrauds.pl";

# Abbreviated file names
@NAMES = ("auths", "cards", "fraud", "inq", "pay", "post", "nonmon");

# The columns of each file used to sort records by account id and time
@SORT_COLUMNS = ("1-33", "1-19,91-98", "1-19", "1-19,21-34", "1-19,82-89", "1-19,49-56", "1-19,21-28");

# The columns of each file used to sort records by date
@DATE_COLUMNS = ("20-27", "91-98", "36-43", "21-28", "82-89", "49-56", "21-28");

# The flags used for "sampstats"
@SAMP_FLAGS = ("-a", "-c", "", "", "-P", "-p", "-N");

# The flags used for "ufal_intersect"
@UFAL_FLAGS = ("-a", "-c", "-f", "", "-p", "-t", "-n");

# The columns used in "multi_freq"
@MULTI_COLUMNS = ("1-6,20-25", "1-6,91-96", "1-6,36-41", "1-6,21-26", "1-6,82-89", "1-6,49-54", "1-6,21-26");

# Indicates if FRAUD analysis files should be redone
$REDO_FLAG = 0;


print "To be used on data formatted according to $VERSION\n";

print "Enter directories in which data is processed (end with an empty line):\n";
while (<>) {
	chomp;
	next if (/^\s*#/);	# Skip lines starting with "#"
	last if (/^$/);		# Exit loop on empty line

	die "$_ does not exist or is not a directory" if !(-d $_);
	die "Do not have read permission for $_" if !(-r $_);
	# Add "/" to end of directory if not there
	if (rindex($_, "/") != (length($_)-1)) {
		$_ .= "/";
	}
	push(@dirs, $_);
	print ("(" . ($#dirs+1) . ") data directory = $_\n");
}

print "Enter temporary workspace directory:\n";
$work_dir = &skipComment;
chomp $work_dir;
die "$work_dir does not exist or is not a directory" if !(-d $work_dir);
die "Do not have execute permission to $work_dir" if !(-x $work_dir);
die "Do not have write permission to $work_dir" if !(-w $work_dir);
# Add "/" to end of directory if not there
if (rindex($work_dir, "/") != (length($work_dir)-1)) {
	$work_dir .= "/";
}
print "working directory = $work_dir\n";
# Change to working directory
chdir($work_dir);

# Find AUTH index
for ($auth_index=0; $auth_index <= $#NAMES; $auth_index++) {
	$auth_name = $NAMES[$auth_index];
	last if ($auth_name =~ /auth/i);
}
print "\nauth name = $auth_name\nauth index = $auth_index\n" if ($DEBUG);

# Find CARD index
for ($card_index=0; $card_index <= $#NAMES; $card_index++) {
	$card_name = $NAMES[$card_index];
	last if ($card_name =~ /card/i);
}
print "\ncard name = $card_name\ncard index = $card_index\n" if ($DEBUG);

# Find FRAUD index
for ($fraud_index=0; $fraud_index <= $#NAMES; $fraud_index++) {
	$fraud_name = $NAMES[$fraud_index];
	last if ($fraud_name =~ /fraud/i);
}
print "\nfraud name = $fraud_name\nfraud index = $fraud_index\n" if ($DEBUG);

print "Enter a DATE string to search (optional):\n";
$date = &skipComment;
chomp $date;
print "date search string = $date\n";
if ($date =~ /^$/) {
	$date = "([0-9_-]+)";
} else {
	$date = "([^.]*${date}[^.]*)";
}
print "$date\n" if ($DEBUG);

print "Enter a CLIENT string to search (optional):\n";
$client = &skipComment;
chomp $client;
print "client search string = $client\n";
if ($client =~ /^$/) {
	$client = "([a-zA-Z_]+)";
} else {
	$client = "([^.]*${client}[^.]*)";
}
print "$client\n" if ($DEBUG);

print "Enter an INVENTORY string to search (optional):\n";
$inventory = &skipComment;
chomp $inventory;
print "inventory search string = $inventory\n";
if ($inventory =~ /^$/) {
	$inventory = "([^.]+)";
} else {
	$inventory = "([^.]*${inventory}[^.]*)";
}
print "$inventory\n" if ($DEBUG);

# Set sampling percentage
print "Enter the sampling percentage (no fractions of a percent; default = 1):\n";
$pct = &skipComment;
chomp $pct;
$pct = 1 if ($pct =~ /^$/);
die "No fractions of a percent for sampling rate" if ($pct =~ /\./);
die "Bad sampling percentage" if (($pct<1) || ($pct>99));

# Seach and extract file names from directories
foreach $dir (@dirs) {
	foreach $longfile (<${dir}*>) {
		$file = substr($longfile, rindex($longfile, '/') + 1);
		print "$dir$file\n" if ($DEBUG);
		foreach $name (@NAMES) {
			#if ( $file =~ /^([0-9_-]+)\.([a-zA-Z_]+)\.(${name}[a-zA-Z]*)\.([^.]+)/i ) {
			if ( $file =~ /^${date}\.${client}\.(${name}[a-zA-Z]*)\.${inventory}/i ) {
				print "\tMatch:\t$1\t$2\t$3\t$4\n" if ($DEBUG);

				# ID used to tag ModelBuilder files
				$id = $2;

				if ( defined($file_hash{$name}) ) {
					$file_hash{$name} .= "$file\n";
					$longfile_hash{$name} .= "$longfile\n";
				} else {
					$file_hash{$name} = "$file\n";
					$longfile_hash{$name} = "$longfile\n";
				}
			}
		}
	}
}

# Display files in each file type (where file type is AUTH, CARD, etc.)
foreach $name (@NAMES) {
	print "$name\n";
	if ($file = $file_hash{$name}) {
		@array = split(/\n/,$file);
		foreach $var (@array) {
			print "\t$var\n";
		}
	} else {
		# AUTH, CARD, and FRAUD files are essential for processing
		die "No $name files exist" if ($name =~ $auth_name);
		die "No $name files exist" if ($name =~ $card_name);
		die "No $name files exist" if ($name =~ $fraud_name);
		print "\t<EMPTY>\n"
	}
}

# Process fraud file types
print "Processing big FRAUD files ...\n";

# Skip if file already exists and has non-zero size
if (-s "${fraud_name}.sorted.cleaned.gz") {
	print "${fraud_name}.sorted.cleaned.gz already exists.  Skipping processing steps.\n";
} else {
	$REDO_FLAG = 1;

	# Create system command to process fraud files
	$file = $longfile_hash{$fraud_name};
	@array = split (/\n/, $file);
	$cmd = "gunzip -c";

	foreach $var (@array) {
		$cmd .= " $var";
	}
	print ("$cmd > ${fraud_name}.gunzip") if ($DEBUG);
	print "gunzip ...\n";
	$system_out = `$cmd > ${fraud_name}.gunzip`;
	print $system_out;
	print "gunzip done\n";

	print ("$BMSORT -c$SORT_COLUMNS[$fraud_index] < ${fraud_name}.gunzip > ${fraud_name}.sorted") if ($DEBUG);
	print "sorting ...\n";
	$system_out = `$BMSORT -c$SORT_COLUMNS[$fraud_index] < ${fraud_name}.gunzip > ${fraud_name}.sorted`;
	print $system_out;
	print "sorting done\n";

	print ("unlink (\"${fraud_name}.gunzip\")\n") if ($DEBUG);
	unlink ("${fraud_name}.gunzip");

	print ("$RMDUPEFRAUDS < ${fraud_name}.sorted > ${fraud_name}.sorted.cleaned") if ($DEBUG);
	print "cleaning ...\n";
	$system_out = `$RMDUPEFRAUDS < ${fraud_name}.sorted > ${fraud_name}.sorted.cleaned`;
	print $system_out;
	print "cleaning done\n";

	print ("unlink (\"${fraud_name}.sorted\")\n") if ($DEBUG);
	unlink ("${fraud_name}.sorted");
}
print "done processing big FRAUD files\n";

$ufal_cmd = "$UFAL_INTERSECT -s " . $pct/100 . " ";
$multi_cmd = "$MULTI_FREQ -f ";
$table_cmd = "$TABLEHASH -f ";

# Process non-fraud file types
print "\nProcessing big OTHER files ...\n";
for ($i=0; $i <= $#NAMES; $i++) {
	$name = $NAMES[$i];
	next if ($name =~ /fraud/i);	# Do not process FRAUD files in this loop

	next if !($file = $file_hash{$name});	# Skip if no files available
	$longfile = $longfile_hash{$name};	# Retrieve path and file name

	$ufal_cmd .= " $UFAL_FLAGS[$i] ${name}.sample.sorted.gz";
	$multi_cmd .= "${name}.sample.sorted.gz,$MULTI_COLUMNS[$i]:"; 
	$table_cmd .= "hash.${name}.sample.sorted.gz.save:";

	# Skip if file already exists and has non-zero size
	if (-s "${name}.sample.sorted.gz") {
		print "${name}.sample.sorted.gz already exists.  Skipping processing steps.\n";
		next;
	}

	$REDO_FLAG = 1;

	$cmd = "cat";

	@array = split(/\n/,$file);
	@longarray = split(/\n/,$longfile);
	print "\nSingle $name files:\n";
	for ($j=0; $j <= $#array; $j++) {
		print "$array[$j]\n";
		print ("gunzip -c $longarray[$j] > $array[$j].gunzip") if ($DEBUG);
		print "gunzip ...\n";
		$system_out = `gunzip -c $longarray[$j] > $array[$j].gunzip`;
		print $system_out;
		print "gunzip done\n";

		print ("$SAMPSTATS -l19 -f${fraud_name}.sorted.cleaned $SAMP_FLAGS[$i] -s$pct -o$array[$j].stats < $array[$j].gunzip > $array[$j].sample") if ($DEBUG);
		print "sampling ...\n";
		$system_out = `$SAMPSTATS -l19 -f${fraud_name}.sorted.cleaned $SAMP_FLAGS[$i] -s$pct -o$array[$j].stats < $array[$j].gunzip > $array[$j].sample`;
		print $system_out;
		print "sampling done\n";

		print ("unlink (\"$array[$j].gunzip\")\n") if ($DEBUG);
		unlink ("$array[$j].gunzip");

		print ("$BMSORT -c$SORT_COLUMNS[$i] <  $array[$j].sample > $array[$j].sample.sorted") if ($DEBUG);
		print "sorting ...\n";
		$system_out = `$BMSORT -c$SORT_COLUMNS[$i] <  $array[$j].sample > $array[$j].sample.sorted`;
		print $system_out;
		print "sorting done\n";

		print ("unlink(\"$array[$j].sample\")\n") if ($DEBUG);
		unlink("$array[$j].sample");

		$cmd .= " $array[$j].sample.sorted";
	}
	# Concatenate files
	print "All $name files:\n";
	print ("$cmd | $BMSORT -c$SORT_COLUMNS[$i] > ${name}.sample.sorted") if ($DEBUG);
	print "concatenating and re-sorting ...\n";
	$system_out = `$cmd | $BMSORT -c$SORT_COLUMNS[$i] > ${name}.sample.sorted`;
	print $system_out;
	print "concatenating and re-sorting done\n";

	foreach $var (@array) {
		print ("unlink(\"${var}.sample.sorted\")\n") if ($DEBUG);
		unlink("${var}.sample.sorted");
	}

	open (HEAD, "head -n1000 ${name}.sample.sorted | cut -c1-19 | $TESTCHECKSUM | ") || die "Unable to run $TESTCHECKSUM";
	$passed = 0;
	$failed = 0;
	$total = 0;
	while (<HEAD>) {
		$passed++ if (/passed/);
		$failed++ if (/failure/);
		$total++;
	}
	close (HEAD);
	printf ("Checksum results:\n\ttotal lines read = %d\n\tpercent passed = %.2f%%\n\tpercent failed = %.2f%%\n", $total, 100*${passed}/${total}, 100*${failed}/${total});

	print ("gzip ${name}.sample.sorted ...\n");
	$system_out = `gzip ${name}.sample.sorted`;
	print $system_out;
	print "gzip done\n";
}
print "done processing big OTHER files\n";

if (!(-s "${fraud_name}.sorted.cleaned.gz")) {
	print ("gzip ${fraud_name}.sorted.cleaned ...\n");
	$system_out = `gzip ${fraud_name}.sorted.cleaned`;
	print $system_out;
	print "done\n";
}

$ufal_cmd .= " $UFAL_FLAGS[$fraud_index] ${fraud_name}.sorted.cleaned.gz";
$multi_cmd .= "${fraud_name}.sorted.cleaned.gz,$MULTI_COLUMNS[$fraud_index]"; 
$table_cmd .= "hash.${fraud_name}.sorted.cleaned.gz.save";

print "\n";

# Execute if output file does not exist or is zero size
if ((-s "ufal_intersect.results") && ($REDO_FLAG==0)) {
	print "Skipping $UFAL_INTERSECT process\n";
} else {
	print ("$ufal_cmd > ufal_intersect.results") if ($DEBUG);
	print "Running $UFAL_INTERSECT ...\n";
	$system_out = `$ufal_cmd > ufal_intersect.results`;
	print $system_out;
	print "done\n";
}

# Execute if output file does not exist or is zero size
$TABLEHASH_REDO_FLAG = 0;
foreach $name (@NAMES) {
	$var = "hash\.${name}\.*\.save";
	@array = < ${var} >;
	if ((($#array == -1) && (defined($file_hash{$name}))) || ($REDO_FLAG==1)) {
		print ("$multi_cmd") if ($DEBUG);
		print "Running $MULTI_FREQ ...\n";
		$system_out = `$multi_cmd`;
		print $system_out;
		print "done\n";
		$tmp = 0;

		$TABLEHASH_REDO_FLAG = 1;

		last;
	}
}
print "Skipping $MULTI_FREQ process\n" if ($TABLEHASH_REDO_FLAG==0);

# Execute if output file does not exist or is zero size
if ((-s "tableHash.results") && ($TABLEHASH_REDO_FLAG==0)) {
	print "Skipping $TABLEHASH process\n";
} else {
	print ("$table_cmd > tableHash.results") if ($DEBUG);
	print "Running $TABLEHASH ...\n";
	$system_out = `$table_cmd > tableHash.results`;
	print $system_out;
	print "done\n";
}

# Execute if output file does not exist or is zero size
if ((-s "${fraud_name}.trans.per.month") && ($REDO_FLAG==0)) {
	print "Skipping fraudulent transactions per month histogram\n";
} else {
	# Produce histogram for number of fraudulent transactions per month
	print ("gunzip -c ${fraud_name}.sorted.cleaned.gz | cut -c21-26 | sort | uniq -c > ${fraud_name}.trans.per.month") if ($DEBUG);
	print "Producing histogram for fraudulent transactions per month ...\n";
	$system_out = `gunzip -c ${fraud_name}.sorted.cleaned.gz | cut -c21-26 | sort | uniq -c > ${fraud_name}.trans.per.month`;
	print $system_out;
	print "done\n";
}

# Execute if output file does not exist or is zero size
if ((-s "${auth_name}.trans.per.day") && ($REDO_FLAG==0)) {
	print "Skipping number of authorizations per day histogram\n";
} else {
	# Produce histogram for number of authorizations per day
	print ("gunzip -c ${auth_name}.sample.sorted.gz | cut -c20-27 | sort | uniq -c > ${auth_name}.trans.per.day") if ($DEBUG);
	print "Producing histogram for authorized transactions per day ...\n";
	$system_out = `gunzip -c ${auth_name}.sample.sorted.gz | cut -c20-27 | sort | uniq -c > ${auth_name}.trans.per.day`;
	print $system_out;
	print "done\n";
}

# Execute if output file does not exist or is zero size
if ((-s "${fraud_name}.run.days") && ($REDO_FLAG==0)) {
	print "Skipping fraud run days histogram\n";
} else {
	# Produce histogram for time between fraud committed and fraud detected (fraud run days)
	print ("gunzip -c ${fraud_name}.sorted.cleaned.gz | $VFRAUDS > ${fraud_name}.run.days") if ($DEBUG);
	print "Producing histogram for fraud run days ...\n";
	$system_out = `gunzip -c ${fraud_name}.sorted.cleaned.gz | $VFRAUDS > ${fraud_name}.run.days`;
	print $system_out;
	print "done\n";
}

print "\n";

# Create DQR script for Model Builder

# Find start and end dates for AUTH file
print ("gunzip -c ${auth_name}.sample.sorted.gz | cut -c$DATE_COLUMNS[$auth_index] | $BMSORT > ${auth_name}.sample.sorted.tmp\n") if ($DEBUG);
print "Finding start and end dates of $auth_name file\n";
$system_out = `gunzip -c ${auth_name}.sample.sorted.gz | cut -c$DATE_COLUMNS[$auth_index] | $BMSORT > ${auth_name}.sample.sorted.tmp`;

$start_date = `head -n1 ${auth_name}.sample.sorted.tmp`;
chomp $start_date;
$end_date = `tail -1 ${auth_name}.sample.sorted.tmp`;
chomp $end_date;

print ("unlink (\"${auth_name}.sample.sorted.tmp\")\n") if ($DEBUG);
unlink ("${auth_name}.sample.sorted.tmp");

# Find end date for FRAUD file
print ("gunzip -c ${fraud_name}.sorted.cleaned.gz | cut -c$DATE_COLUMNS[$fraud_index] | $BMSORT > ${fraud_name}.sorted.cleaned.tmp\n") if ($DEBUG);
print "Finding end date of $fraud_name file\n";
$system_out =  `gunzip -c ${fraud_name}.sorted.cleaned.gz | cut -c$DATE_COLUMNS[$fraud_index] | $BMSORT > ${fraud_name}.sorted.cleaned.tmp`;

$fraud_end_date = `tail -1 ${fraud_name}.sorted.cleaned.tmp`;
chomp $fraud_end_date;

print ("unlink (\"${fraud_name}.sorted.cleaned.tmp\")\n") if ($DEBUG);
unlink ("${fraud_name}.sorted.cleaned.tmp");

print "${auth_name}.sample.sorted.gz start date = $start_date\n";
print "${auth_name}.sample.sorted.gz end date = $end_date\n";
print "${fraud_name}.sorted.cleaned.gz end date = $fraud_end_date\n";

open (DQR, ">${id}_DQR_script.txt") || die "Unable to open ${id}_DQR_script.txt";
$tmp = &setDQRstring;

$tmp =~ s/REPLACE_WITH_ID/${id}/g;
$tmp =~ s/REPLACE_WITH_OUTPUT_FILE/${work_dir}${id}.dqr.rtf/g;
$tmp =~ s/REPLACE_WITH_START_DATE/$start_date/g;
$tmp =~ s/REPLACE_WITH_END_DATE/$end_date/g;
$tmp =~ s/REPLACE_WITH_FRAUD_END_DATE/$fraud_end_date/g;

foreach $name (@NAMES) {
	# If input files are available then convey fact to ModelBuilder
	if ($name eq $fraud_name) {
		$tmp =~ s/REPLACE_WITH_${name}_FILE/${work_dir}${name}.sorted.cleaned.gz/g;
	} else {
		if (-s "${name}.sample.sorted.gz") {
			$tmp =~ s/REPLACE_WITH_${name}_FILE/${work_dir}${name}.sample.sorted.gz/g;
		} else {
			$tmp =~ s/REPLACE_WITH_${name}_FILE/${work_dir}${name}/g;
			$tmp =~ s.//Begin ${name}./*//Begin ${name}.g;
			$tmp =~ s.//End ${name}.//End ${name}*/.g;
		}
	}
}
print DQR $tmp;
close (DQR);




############################################################################
#	Sub Functions
############################################################################

sub skipComment {
	local($tmp) = "";
	while ($tmp = <>) {
		last if (!($tmp =~ /^\s*#/));
	}
	$tmp;
}

############################################################################

sub setDQRstring {
$_ = '//*********************************************************************************//
//************DO NOT COMMENT ANY LINE BELOW, ONLY CHANGE THE PARAMETERS************//
//***********THIS PART OF THE SCRIPT FILE IS FOR SETTING ALL PARAMETERS************//
//**********IF A FILE TYPE IS NOT PRESENT, LEAVE THE PARAMETERS AS IT IS***********//

//Parameters for CARDS file
System.setProperty("cardsStatName", "cardsStats_REPLACE_WITH_ID"); //Give the Statistics Name for CARDS
System.setProperty("cardsQuantileName", "cardsQuantiles_REPLACE_WITH_ID"); //Give the Quantile Name for CARDS
System.setProperty("inFileCards","REPLACE_WITH_CARD_FILE"); //input file location for CARDS
System.setProperty("cardsSettingsName", "cardsSettings_REPLACE_WITH_ID"); //Give the Settings Name for CARDS
      
//Parameters for the POSTING File
System.setProperty("postStatName", "postStats_REPLACE_WITH_ID"); //Give the Statistics Name for POSTING
System.setProperty("postQuantileName", "postQuantiles_REPLACE_WITH_ID"); //Give the Quantile Name for POSTING
System.setProperty("inFilePost", "REPLACE_WITH_POST_FILE"); //input file location for POSTING
System.setProperty("postSettingsName", "postSettings_REPLACE_WITH_ID"); //Give the Setting Name for POSTING   

//Parameters for the AUTH File
System.setProperty("authStatName", "authStats_REPLACE_WITH_ID"); //Give the Statistics Name for AUTH
System.setProperty("authQuantileName1", "authQuantiles1_REPLACE_WITH_ID"); //Give the Quantile Name for AUTH
System.setProperty("authQuantileName2", "authQuantiles2_REPLACE_WITH_ID"); //Give another Quantile Name for AUTH
System.setProperty("inFileAuth", "REPLACE_WITH_AUTH_FILE"); //input file location for AUTH
System.setProperty("authSettingsName", "authSettings_REPLACE_WITH_ID"); //Give the Setting Name Name for AUTH

//Parameters for the NONMON File
System.setProperty("nonmonStatName", "nonmonStats_REPLACE_WITH_ID"); //Give the Statistics Name for NONMON
System.setProperty("nonmonQuantileName", "nonmonQuantiles_REPLACE_WITH_ID"); //Give the Quantile Name for NONMON
System.setProperty("inFileNonmon", "REPLACE_WITH_NONMON_FILE"); //input file location for NONMON
System.setProperty("nonmonSettingsName", "nonmonSettings_REPLACE_WITH_ID"); //Give the Setting Name for NONMON

//Parameters for the PAYMENT File
System.setProperty("payStatName", "payStats_REPLACE_WITH_ID"); //Give the Statistics Name for PAYMENT
System.setProperty("payQuantileName", "payQuantiles_REPLACE_WITH_ID"); //Give the Quantile Name for PAYMENT
System.setProperty("inFilePay", "REPLACE_WITH_PAY_FILE"); //input file location for PAYMENT
System.setProperty("paySettingsName", "paySettings_REPLACE_WITH_ID"); //Give the Setting Name for PAYMENT

//Parameters for the FRAUD File
System.setProperty("fraudStatName", "fraudStats_REPLACE_WITH_ID"); //Give the Statistics Name for FRAUD
System.setProperty("inFileFraud","REPLACE_WITH_FRAUD_FILE"); //input file location for FRAUD
System.setProperty("fraudSettingsName", "fraudSettings_REPLACE_WITH_ID"); //Give the Setting Name for FRAUD

//Parameters for the INQUIRY File
System.setProperty("inquiryStatName", "inquiryStats_REPLACE_WITH_ID"); //Give the Statistics Name for INQUIRY
System.setProperty("inquiryQuantileName", "inquiryQuantiles_REPLACE_WITH_ID"); //Give the Quantile Name for INQUIRY
System.setProperty("inFileInquiry", "REPLACE_WITH_INQ_FILE"); //input file location for INQUIRY
System.setProperty("inquirySettingsName", "inquirySettings_REPLACE_WITH_ID"); //Give the Setting Name for INQUIRY

//Output file location
System.setProperty("outFile","REPLACE_WITH_OUTPUT_FILE");

//StartDate and endDate refers to the time period of data for which DQR is run 
System.setProperty("startDate", "REPLACE_WITH_START_DATE"); //First Day of the first month of the window of data
System.setProperty("endDate", "REPLACE_WITH_END_DATE"); //Last day of the last month of the window of data (Except FRAUD)
System.setProperty("endDateFraud", "REPLACE_WITH_FRAUD_END_DATE"); //Last day of the last month of the window of FRAUD data

//*********************************************************************************//
//*********************************************************************************//


//*********************************************************************************//
//*******************************DO NOT TOUCH THIS PART*****************************//
String cardsStatName = System.getProperty("cardsStatName");
String cardsQuantileName = System.getProperty("cardsQuantileName");

String authStatName = System.getProperty("authStatName");
String authQuantileName1 = System.getProperty("authQuantileName1");
String authQuantileName2 = System.getProperty("authQuantileName2");

String postStatName = System.getProperty("postStatName");
String postQuantileName = System.getProperty("postQuantileName");

String nonmonStatName = System.getProperty("nonmonStatName");
String nonmonQuantileName = System.getProperty("nonmonQuantileName");

String payStatName = System.getProperty("payStatName");
String payQuantileName = System.getProperty("payQuantileName");

String fraudStatName = System.getProperty("fraudStatName");

String inquiryStatName = System.getProperty("inquiryStatName");
String inquiryQuantileName = System.getProperty("inquiryQuantileName");
//*********************************************************************************//
//*********************************************************************************//

//*********************************************************************************//
//*********THIS PART OF THE SCRIPT FILE IS FOR RUNNING VARIOUS PROCESSES***********//

//Creating Various Bins (Never touch this line)
Process.run("CreateBinnings"); 

//Creating the output file (Never touch this line)
Process.run("StartFile");

//Begin CARD
//Processes for CARD file (comment the four lines below if no CARDS file)
Process.setParams("VCards", new String("cardS="+cardsStatName+", cardQ="+cardsQuantileName)); 
Process.run("VCards"); // this is the time consuming part and is not necessary to repeat unless error occurred
Process.run("DumpCardsStats"); // Need to run this every time
Process.run("CardsTableCreation"); // Need to run this every time
//End CARD

//Begin AUTH
//Processes for AUTH file (comment the four lines below if no AUTH file)
Process.setParams("VAuth", new String("authS="+authStatName+", authQ="+authQuantileName1)); 
Process.run("VAuth"); 
Process.run("DumpAuthStats"); 
Process.run("AuthTableCreation");
//End AUTH

//Begin POST
//Processes for POSTING file (comment the four lines below if no POSTING file)
Process.setParams("VPost", new String("postS="+postStatName+", postQ="+postQuantileName)); 
Process.run("VPost"); 
Process.run("DumpPostStats"); 
Process.run("PostTableCreation");
//End POST

//Begin NONMON
//Processes for NONMON file (comment the four lines below if no NONMON file)
Process.setParams("VNonmon", new String("nonmonS="+nonmonStatName+", nonmonQ="+nonmonQuantileName)); 
Process.run("VNonmon"); 
Process.run("DumpNonmonStats"); 
Process.run("NonmonTableCreation");
//End NONMON

//Begin PAY
//Processes for PAYMENT file (comment the four lines below if no PAYMENT file)
Process.setParams("VPay", new String("payS="+payStatName+", payQ="+payQuantileName)); 
Process.run("VPay"); 
Process.run("DumpPayStats"); 
Process.run("PayTableCreation");
//End PAY

//Begin FRAUD
//Processes for FRAUD file (comment the four lines below if no FRAUD file)
Process.setParams("VFraud", new String("fraudS="+fraudStatName));
Process.run("VFraud");
Process.run("DumpFraudStats");
Process.run("FraudTableCreation");
//End FRAUD

//Begin INQ
//Processes for INQUIRY file (comment the four lines below if no INQUIRY file)
Process.setParams("VInquiry", new String("inquiryS="+inquiryStatName));
Process.run("VInquiry");
Process.run("DumpInquiryStats");
Process.run("InquiryTableCreation");
//End INQ

//CLosing the Output file (Never touch this line)
Process.run("EndFile"); 
//*********************************************************************************//
//**********************************END OF FILE************************************//
';
}
