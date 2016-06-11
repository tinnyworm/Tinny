#! /usr/local/bin/perl -w

while (<STDIN>){
  chomp;
  $_ =~ s/[@*]$//;
  print "$_\n";
}
