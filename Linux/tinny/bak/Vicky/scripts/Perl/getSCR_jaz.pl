#!/usr/local/bin/perl
#
# Increase the number of decimals to 4 for score distribution - jaz@04/16/07

while(<>){
  last if(/Threshold\s+False Assigns\s+Valid Assigns\s+TFPR/);
}
while(<>){
  last if(/^\s+$/);
  chomp;
  @l = split " ",$_;
  $l[1] =~ s/%//o;
  $l[2] =~ s/%//o;
  $t0=$l[3] if($l[0]==0);
  $scr_dstr[($l[0]/10)]=($l[2]+$t0*$l[1])/($t0+1);
}

while(<>){
  last if(/ACCOUNT BASED INFORMATION/);
}

while(<>){
  last if(/Threshold/);
}

$trash=<>;

while(<>){
  last if (/^\s+$/);
  split;
  $sc=$_[0];
  $afpr[$sc/10]=$_[1];
  $adr[$sc/10]=$_[2];
  $c1[$sc/10]=$_[3];
  $c2[$sc/10]=$_[4];
}

printf "%5s, %7s, %6s, %6s, %5s, %7s, %6s\n",
    "Score","AFPR","ADR","rtVDR","olVDR","K","Sdstr";
for($i=0;$i<=100;$i++){
  $rt=($c1[100]-$c1[$i])/$c1[100]*100.0;
  $ol=($c2[100]-$c2[$i])/$c2[100]*100.0;
  $k=($afpr[$i]+1.0)*$adr[$i]/100.0;
  printf "%5.0f, %7.2f, %6.2f, %6.2f, %5.2f, %7.2f, %6.4f\n",
      $i*10,$afpr[$i],$adr[$i],$rt,$ol,$k,$scr_dstr[$i];
}

