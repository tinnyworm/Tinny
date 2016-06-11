#! /usr/local/bin/perl -w

while(<>){
  if(/^\"(.{3})\"/){
    next if($1 ne "   " and $1 =~ /\W/);
  }
  print;
}
