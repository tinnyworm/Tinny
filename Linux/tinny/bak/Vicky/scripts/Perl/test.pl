#! /usr/local/bin/perl -w

$temp="model";

$temp=`ls $temp/*.cnf | /home/yxc/scripts/Perl/rmdir.pl > temp`; 
open COMMAND, "< temp"; 
$in=<COMMAND>;
print $in, "\n";
close COMMAND; 
