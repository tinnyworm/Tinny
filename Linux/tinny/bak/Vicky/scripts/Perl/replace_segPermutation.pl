#! /usr/local/bin/perl -w

$USAGE="Usage: \n\n $0 -o orderList -m modelFile -w workDir\n\n
	For example: $0 -o orderList -m model/gencr140 -w `pwd` -k Makefile\n
"; 

use Getopt::Std;
die "$USAGE" if ($#ARGV == -1); 

&getopts('m:w:o:k:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

open (ORDER, "< $opts{'o'}") || die "Could not open $opts{'o'}!\n"; 

$mod = "$opts{'m'}";
$work = "$opts{'w'}"; 
$Makefile = $opts{'k'}; 

@all =split('/', $mod);
$modName=$all[$#all]; 


while (defined($line=<ORDER>))
{
	chop($line);

	print STDERR "$line\n"; 

	if (!(-d "$work/eval_$line"))
	{
		mkdir ("$work/eval_$line", 0755) or die "cannot mkdir $$work/eval_$line: $!";
	}

	if (!(-d "$work/eval_$line/eval"))
	{
		mkdir ("$work/eval_$line/eval", 0755) or die "cannot mkdir $$work/eval_$line/eval: $!";
   }

   if (!(-d "$work/eval_$line/log"))
	{
		mkdir ("$work/eval_$line/log", 0755) or die "cannot mkdir $$work/eval_$line/log: $!";
   }
	
   $cmd = "cd $work/eval_$line;\n";
	$cmd.= "model_copy40.sh $mod.cnf;\n";
   	
	print STDERR "$cmd";

	print &writeRul($line) . "\n"; 	
	`$cmd`; 
	
	open (MRUL, "< $mod.rul") || die "Could not open $mod.rul!\n";
	open (OUT, "> $work/eval_$line/new_$line.rul") || die "Can not write in $work/eval_$line/new_$line.rul\n"; 
	while (defined($rul=<MRUL>))
	{
		
		 $getRul = &writeRul($line); 
		 $output = $rul; 
		 $output =~ s/"__REPLACE__"/$getRul/; 
		 print OUT $output; 
		
	}
	
	close(MRUL); 
	close(OUT); 

	$cmd2 = "cd $work/eval_$line; /bin/rm $work/eval_$line/$modName.rul;ln -s new_$line.rul $modName.rul;\n"; 
	$cmd2.= "cp $Makefile Makefile;\n"; 
	print STDERR "$cmd2";
  `$cmd2`; 	

}

### sub-routine

sub writeRul {
	my $in = $_[0]; 
	my @tmp = split('', $in);
	
	if ($tmp[0] eq "y")
	{
		$outy = "IF (ActAge <= 45) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"yamcr140\", STATUS);\n";
	   	
  } 
  else
  {
  		$outy = "ELSE IF (ActAge <= 45) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"yamcr140\", STATUS);\n";
  }

  if ($tmp[0] eq "h")
  {
  		$outh = "IF ( DolAmt >= 250 )  THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"hidcr140\", STATUS);\n"; 
  }
  else
  {
  		$outh = "ELSE IF ( DolAmt >= 250 )  THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"hidcr140\", STATUS);\n";
  }

	if ($tmp[0] eq "r")
	{
		$outr = "IF ( IsRapid = 1 ) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"rapcr140\", STATUS);\n"; 
	}
	else
	{
		$outr = "ELSE IF ( IsRapid = 1 ) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"rapcr140\", STATUS);\n";
	}

	if ($tmp[0] eq "f")
	{
		$outf = "IF ( IS_FOREIGN = 1) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"forcr140\", STATUS);\n"; 
	}
	else
	{
		$outf = "ELSE IF ( IS_FOREIGN = 1) THEN
    SCOR.MODEL_ID := FIRE_MODEL(\"forcr140\", STATUS);\n";
  }

  $outg = "ELSE
    SCOR.MODEL_ID := FIRE_MODEL(\"gencr140\", STATUS);\n";

 	$outHash{'y'}=$outy;
  	$outHash{'r'}=$outr;
  	$outHash{'h'}=$outh;
  	$outHash{'f'}=$outf;
  	$outHash{'g'}=$outg;
 
   $outFinal = ""; 
 
	for ($i=0; $i<=$#tmp; $i++)
	{
		$outFinal .= $outHash{$tmp[$i]};
		 
  }
  return ($outFinal); 
}
