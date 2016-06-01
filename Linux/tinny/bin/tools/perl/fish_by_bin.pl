#!/usr/local/bin/perl

#
# fish out auths, cards, frauds by bin_list
#

$usage = "Usage: \n\t".
	"zcat auths/cards/frauds | $0 bin_list | gzip -c > auths/cards/frauds\n";

$bin_list = shift(@ARGV)
	or die $usage;

open(BIN_LIST, "cat $bin_list |")
	or die "Can't open $bin_list!\n";

while(<BIN_LIST>) {
	 chomp;
	 $b_list{$_} = 1;
}
close(BIN_LIST);

while(<>) {
	 $acct = substr($_,0,6);
	 if (defined($b_list{$acct})) {
		  print $_;
	 }
}
