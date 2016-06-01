#!/usr/local/bin/perl

##############################################
# usage added.

require "getopts.pl";

$USAGE = "
        This program is to extract the information from Research file 
        and calculated K values, score distribution.

        Usage:  cat research file | $0 bin_range


                Optional:
                        -h shows this message
\n\n";

#die "$USAGE" if ($#ARGV == -1);
die "$USAGE" if ($opt_h);

##############################################
$bin=$ARGV[0]; 

while(<STDIN>){
  last if(/Threshold\s+False Assigns\s+Valid Assigns\s+TFPR/);
}
while(<STDIN>){
  last if(/^\s+$/);
  chomp;
  @l = split " ",$_;
  $l[1] =~ s/%//o;
  $l[2] =~ s/%//o;
  $t0=$l[3] if($l[0]==0);
  if ($l[0] % $bin == 0) {
		$scr_dstr[($l[0]/$bin)]=($l[2]+$t0*$l[1])/($t0+1);
  }
}

while(<STDIN>){
  last if(/ACCOUNT BASED INFORMATION/);
}

while(<STDIN>){
  last if(/Threshold/);
}

$trash=<STDIN>;

while(<STDIN>){
  last if (/^\s+$/);
  split;
  $sc=$_[0];
  if ($sc % $bin == 0) {
  	$num=$sc/$bin; 
  $afpr[$num]=$_[1];
  $adr[$num]=$_[2];
  $c1[$num]=$_[3];
  $c2[$num]=$_[4];
  }
}

$out=1000/$bin ; 

printf "%5s, %7s, %6s, %6s, %5s, %7s, %6s\n",
    "Score","AFPR","ADR","rtVDR","olVDR","K","Sdstr";
for($i=0;$i<=$out;$i++){
  $rt=($c1[$out]-$c1[$i])/$c1[$out]*100.0;
  $ol=($c2[$out]-$c2[$i])/$c2[$out]*100.0;
  $k=($afpr[$i]+1.0)*$adr[$i]/100.0;
  printf "%5.0f, %7.2f, %6.2f, %6.2f, %5.2f, %7.2f, %6.4f\n",
      $i*$bin,$afpr[$i],$adr[$i],$rt,$ol,$k,$scr_dstr[$i];
}

