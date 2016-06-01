#!/usr/local/bin/perl -I/home/bxz/bin/perlmod
use S370;
$len=$ARGV[0]; 
while(read(STDIN,$buff,$len)){
  ($bf,$sc,$r1,$r2,$r3,$af,@f)=S370::unpackS370("A8 n n n n \@36 a603 x3" . "f " x 69,$buff);
  $af=~s/[\000-\040]/ /g;
  $af=~s/[\176-\377]/ /g;
  printf "%8s%4i %2i %2i %2i %13s %603s" ."%+12.5E" x 70 . "\n",$bf,$sc,$r1,$r2,$r3," ",$af,0.0,@f;
}

