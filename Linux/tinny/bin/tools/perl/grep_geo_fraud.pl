#! /usr/local/bin/perl

use warnings;

@dataset = qw(cibc_02 cibc_03 td_03 vdj_02);

$pre_acc = "0";
while(<>) {
	$acc = substr($_,0,19);
	$fraud_tag = substr($_,59,1);
	if (!$fraud_tag) {
		if ($acc ne $pre_acc) {
			print "Found a new incorrect-tagged account: $acc\n";
			($clean_acc, $_) = split(/\s/,$acc);
			foreach $dat (@dataset) {
				$rst_name = $clean_acc."_".$dat.".dat";
				print "\t Searching data set $dat, output to $rst_name\n";
				`zcat data/$dat/auths.dat.gz | grep \'^$acc\' > $rst_name`;
				$size=`ls -l $rst_name | awk '{print \$5}'`;
				if ($size != 0) {
					print "Found in $dat! size = $size\n";
					last;
				} else {
					print "Not found in $dat! remove the output! size =".
						"$size\n";
					`\\rm $rst_name`;
				}
			}
			$pre_acc = $acc;
		}
	}
}
