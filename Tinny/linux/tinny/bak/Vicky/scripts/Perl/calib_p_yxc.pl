#!/usr/local/bin/perl -I/home/bxz/bin/perlmod
use Getopt::Long;  
use Statistics::OLS;

$usage = 
"Usage: $0 
           [-f <PCTFRAUD>] [-n <PCTNOFRAUD>] [-r <account_sampling_rate>] 
           [-a <adjust_factor>] [-s|-p] <researchfile> 
 OPTIONS:
      -f:  PCTFRAUD. (Default: 1)
      -n:  PCTNOFRAUD. (Default: 1)
      -r:  Sampling rate. (Default: 0.01)
      -a:  Adjust factor. (Default: 0)
      -s:  Generate the look-up table. (Default)
      -p:  Print out the Intersection and Slope.

 PURPOSE:
      Do probability based calibration. 
      Note: no reference research files are needed! Scores are calibrated to
      the probabilities they supposed to equal to. For operational reason, the 
      entire model suite should be calibrate again by traditional AFPR matching
      method.
";
@optl=("help","s","p","f=f","n=f","r=f","a=f");
die $usage unless GetOptions(@optl);
if ($opt_help) {print $usage; exit 0;}

$fpc=$opt_f?$opt_f:1;
$nfpc=$opt_n?$opt_n:1;
$rate=$opt_r?$opt_r:0.01;
$adjust=$opt_a?$opt_a:0;

while(<>){
  last if ($_=~/Division Break Down:/);
}
$line=<>;
$line=<>;
while(<>){
  ($i,$fct,$nfct)
    =/Threshold = *(\d+) *Frauds *(\d+) *Non Frauds *(\d+)/;
	
  if (($fct != 0 ) and ($nfct != 0))
 { 
  $wstmp=log($fct*$nfpc*$rate/$nfct/$fpc/0.01);
  $wrtmp=log(($i+5)/(995-$i));
#  print "$wrtmp $wstmp\n";

	@ws=(@ws,$wstmp);
	@wr=(@wr,$wrtmp);
  }
  last if ($i == 990);
}
my $ls = Statistics::OLS->new; 
my @xdata = @wr;
my @ydata = @ws;
$ls->setData (\@xdata, \@ydata);
$ls->regress() or die ( $ls->error() );
my ($intercept, $slope) = $ls->coefficients();
$intercept += $adjust;

if ($opt_p)
{
  print "Intercept = $intercept\n" . 
        "Slope =     $slope\n";
}
else {
  print "0,0\n";
    for ($i=1;$i<=999;$i++) {
      $wr=log($i/(1000-$i));
      $ewr=exp($slope*$wr+$intercept);
      $s=int(1000*$ewr/(1+$ewr));
      print "$i,$s\n";
    }
  print "1000,999\n";
}

