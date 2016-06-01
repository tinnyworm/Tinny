#! /usr/local/bin/perl 

# clean the risk tab for postal code
# find and discard any postal code which contains characters other than
# \w == [A-Za-z0-9_] or \s == space
#
# examples of invalid postal code
#	"** ", "*DA", ",  ", ".  "

use warnings;
use strict;

while(<>){
  if(/^\"(.{3})\"/){
		if($1 =~ /[\w|\s]{3}/) {
				print;
		  }
	} else {
		print;
  }		
}
