#! /usr/local/bin/perl -w

$USAGE = "
## This is a sub-programe for modeling to modify the content of model file
or net file in order to run nnet or evaluation smoothly. 
$0 -m <model name> -s [for scaling mod] 
   -p [for evaluation on model file] -a [for evaluation
	 on net file] -w <work dir>
##
"; 

use Getopt::Std;

die "$USAGE" if ($#ARGV == -1); 

&getopts('w:m:hspa', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

if (defined $opts{'w'})
{
	$work=$opts{'w'}; 
}
else
{
	$work="."; 
}

die "ERROR: No model file specified!\n$USAGE" unless (defined $opts{'m'} && (-s "$work/$opts{'m'}")); 

$modName = $opts{'m'}; 

####### For Scalling mod
if (defined $opts{'s'})
{
	if (!(-s "$work/$modName.hold_0"))
	{
		 warn "NOTE: $modName.hold_0 does not exist. Will copy one. \n"; 
		 `cp $work/$modName $work/$modName.hold_0`; 
	}

	open (IN, "< $work/$modName.hold_0 ");
	open (OUT, " > $work/$modName") || die "Could not write to $modName"; 

	while (<IN>){
		$_ =~ s/^\s+IF\s+\(SCOR.SCORE/\/\/  IF (SCOR.SCORE/; 
		$_ =~ s/^\s+Get_Reasons/\/\/  Get_Reasons/;
		$_ =~ s/^\s+SCOR.REASON1/\/\/   SCOR.REASON1/;
		print OUT $_;
	}

	close(IN);
	close(OUT);
}

####### For evaluation on model file
if (defined $opts{'p'})
{
	if (! (-s "$work/$modName.hold_1"))
	{
		 warn "NOTE: $modName.hold_1 does not exist. Will copy one. \n"; 
		 `cp $work/$modName $work/$modName.hold_1`; 
	}

	open (IN, "< $work/$modName.hold_1 ");
	open (OUT, " > $work/$modName") || die "Could not write to $modName"; 
	  while (<IN>){
		$_ =~ s/\/\/\s+IF\s+\(SCOR.SCORE/  IF (SCOR.SCORE/; 
		$_ =~ s/\/\/\s+Get_Reasons/  Get_Reasons/;
		$_ =~ s/\/\/\s+SCOR.REASON1/    SCOR.REASON1/;
		print OUT $_; 
	  }
	close(IN);
	close(OUT);
}

####### For evaluation on net file
if (defined $opts{'a'})
{
	if (! (-s "$work/$modName.hold_2"))
	{
		 warn "NOTE: $modName.hold_2 does not exist. Will copy one. \n"; 
		 `cp $work/$modName $work/$modName.hold_2`; 
	}
	
	open (IN, "< $work/$modName.hold_2 ");
	open (OUT, " > $work/$modName") || die "Could not write to $modName"; 
	while (<IN>){
		$_ =~ s/\s+NSCORE\s+:=\s+\@CALC\_/   RAW_SCORE := \@CALC\_/; 
		print OUT $_; 
		if ( /\s+RAW_SCORE\s+:=\s+\@CALC\_/) { 			
	     @temp =split('_', $_); 
		  $netsNm=$temp[2]; 
	     print STDERR "nets file name is $netsNm. Is it true??";
		}
	}
	close(IN);
	close(OUT);

	if (!(-s "$work/nets/$netsNm.hold")){
		 warn "NOTE: $work/nets/$netsNm.hold does not exist. Will copy one. \n";
		 `cp $work/nets/$netsNm $work/nets/$netsNm.hold`; 
	}
	
	open (IN, "< $work/nets/$netsNm.hold ") || die "Could not open	$work/nets/$netsNm.hold";
	open (OUT, " > $work/nets/$netsNm") || die "Could not write to $work/nets/$netsNm"; 
	while (<IN>){
		$_ =~ s/^\s+NSCORE\s+:=\s+/\/\/ NSCORE :=/g;
		$_ =~ s/^\s+IF\s+\(NSCORE\s+\>/\/\/  IF (NSCORE >/g;
		$_ =~ s/^\s+Get_Reasons\(/\/\/Get_Reasons(/g;
		if (/\s+Calc_NN_Slab\(/ and /_OUTPUT/){
		 	@temp=split(',', $_);
			$op=$temp[2];
			$op =~ s/ //g; 
	  }
   	$_ =~ s/NSCORE/$op\[1\]/ if (/\s+RETURN\(NSCORE\)/); 
		print OUT $_; 
  }
	close(IN);
	close(OUT);
	
}
