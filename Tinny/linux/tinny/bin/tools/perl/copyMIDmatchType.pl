#!/usr/local/bin/perl -w

# for FP data, put the matching type information into Auth-avail-cred field,
# to the last position c74,
#
# 	FULL ----> 0
#	MCCD ----> 1
#	BMID ----> 2
#	GBLD ----> 3
#
# usage: cat auths.dat.gz | $0 | gzip -c > 

while(<>) {
	 $type = substr($_,320,4);
	 if ($type eq "FULL") {
		  substr($_,64,10) = "         "."0";
	 }
	 elsif ($type eq "MCCD") {
		  substr($_,64,10) = "         "."1";
	 }
	 elsif ($type eq "BMID") {
		  substr($_,64,10) = "         "."2";
	 }
	 elsif ($type eq "GBLD") {
		  substr($_,64,10) = "         "."3";
	 }
	else {
		 die "wrong type!";
	}

	 print $_;
}
