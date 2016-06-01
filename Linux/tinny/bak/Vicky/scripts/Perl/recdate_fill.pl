#!/usr/local/bin/perl

($day) = @ARGV;

while (<STDIN>) {

   substr($_,90,8) = $day;

   print STDOUT $_;

}
