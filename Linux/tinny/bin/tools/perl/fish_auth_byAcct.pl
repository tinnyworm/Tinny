#!/usr/local/bin/perl

#
# fish out all the transactions for the accounts in the acct_list,
# 

$usage = "Usage: \n\t".
	"zcat auth/api | $0 acct_list \n\n";

$acct_list = shift(@ARGV)
	or die $usage;

open(ACCT_LIST, "cat $acct_list |")
	or die "Can't open $acct_list!\n";

while(<ACCT_LIST>) {
	chomp;
	$a_list{$_} = 1;
}

close(ACCT_LIST);

while(<>) {
	$acct = substr($_,0,19);
	if (defined($a_list{$acct})) {
		print $_;
	}
}
