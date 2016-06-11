#! /usr/local/bin/perl -w
# -----------------------------------------------------------------------------
#
# Select variables in .nncfg file according to varlists
# It actually comment out the variables which are not in the varlists, 
# make slab = none
# 
# varlists format is the same as the one under nets/
#
# Usage: 
# 		
#		cat old.nncfg | selectVars_nncfg.pl varlists > new.nncfg
# -----------------------------------------------------------------------------

$usage = "Usage:\n\n".
			"\t cat old.nncfg | $0 varlists > new.nncfg\n";

$varlist = shift(@ARGV);

die $usage unless $varlist;

# read in all the variables specified in varlists
open(LIST, "cat $varlist |") 
	or die "Can't open $varlist!\n";
while (<LIST>) {
	 if (/^INPUT/) {	
		@t = split /\s+/;
		$vars{$t[1]} = 1;
  }
}
close(LIST);

while (<>) {
	 if (/^\s+name/) {
		  print $_; # print it out
		  @t = split /\s+/;
		  if(!defined($vars{$t[2]})) {
				$_ = <>; # read in type
				print $_; # print out this line
				$_ = <>; # read in slab
				s/INPUT/NONE/; # make change
				print $_; # print out
		  }
	 } else {
	 	print $_;
  }
}
				
