#!/usr/local/bin/perl -I/home/bxz/bin/perlmod
use S370;
while(read(STDIN,$buff,1005)){
  $pattern="A725 " . ("f " x 70);
  $printpt="%725s ". ("%e " x 70). "\n";
  ($ascii,@flt)=S370::unpackS370($pattern,$buff);
  printf $printpt, $ascii, @flt;
}
