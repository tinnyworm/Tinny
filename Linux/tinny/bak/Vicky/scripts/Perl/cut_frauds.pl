#!/usr/local/bin/perl

if ($#ARGV != 1) { die("usage: cut_frauds2.pl <start> <stop>"); }

while (<STDIN>) {
    $ffd=substr($_,20,8);
    unless (($ffd<"$ARGV[0]") || $ffd>"$ARGV[1]") {
	 print "$_";
    }
}
