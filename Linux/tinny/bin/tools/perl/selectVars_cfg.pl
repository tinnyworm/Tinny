#! /usr/local/bin/perl -w
# -----------------------------------------------------------------------------
#
# Select variables in .cfg file (after scaling) according to varlists
# It actually comment out the variables which are not in the varlists, 
# make Slab= NONE
# 
# varlists format is the same as the one under scale/nets/
#
# NOTE: this should be done in scale directory
#
# Usage: 
# 		
#		cat old.cfg | selectVars_cfg.pl varlists > new.cfg
# -----------------------------------------------------------------------------

$usage = "Usage:\n\n".
			"\t cat old.cfg | $0 varlists > new.cfg\n";

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
	 if (/^Name/) {
		  print $_; # print it out
		  @t = split /\s+/;
		  if(!defined($vars{$t[1]})) {
				$_ = <>; # read in Type
				print $_; # print out this line
				$_ = <>; # read in Slab
				s/INPUT/NONE/; # make change
				print $_; # print out
		  }
	 } else {
	 	print $_;
  }
}
				
