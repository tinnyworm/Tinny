#!/usr/local/bin/perl

################################################################################
# $Source:$
# $Id:$
################################################################################
# $Log:$
################################################################################

use Env;
use lib "$HOME/lib";

chomp($date = `date`);

@files = @ARGV;

foreach $file (@files) {
   $qalevel = 2;
   if (-B $file) {
      $CAT = 'gzip -dc';
   }
   else {
      $CAT = 'cat';
   }

   ($fail,$pass) = `$CAT $file | cut -c1-19 | testChecksum | sort | uniq -c`;

   $fail =~ s/^\s+//;
   $pass =~ s/^\s+//;
   ($fail) = (split(/\s+/,$fail))[0];
   ($pass) = (split(/\s+/,$pass))[0];
   $total = $fail + $pass;

   $pctpass = $pass / $total * 100;
   $pctfail = $fail / $total * 100;

   write;
   $- = 0;
}

format STDOUT_TOP =
@|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
'Falcon Data Analysis Report'

@<<<<<<<<<<<<<<<<<<<<<<<<<<<     QA Level: @<<      @>>>>>>>>>>>>>>>>>>>>>>>>>>>
$file,$qalevel,$date
--------------------------------------------------------------------------------
.

format STDOUT =
@||||||||||||||||||||||||||||||||||
'testChecksum'

Passed:     @##.##%     @##########
$pctpass,$pass
Failed:     @##.##%     @##########
$pctfail,$fail
===================================
Totals:     @##.##%     @##########
$pctpass + $pctfail,$total
.
