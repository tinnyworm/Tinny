#! /usr/local/bin/perl 

use warnings;

#$usage = "zcat bigscr.gz | $0 > bigscr.appfraudbyamt".
#	"Typical Use:\n".
#	" zcat bigscr.gz | $0 > bigscr.appfraudbyamt\n".
#	"Finds the approved fraud accounts (A: approved and R: referred)\n".

$old_acct = "0";
$total_amt = 0;
$current_amt = 0;

while(<>) {
	$fraud_flag = substr($_,59,1);
	$decision = substr($_,88,1);
	if ( ($fraud_flag eq "1") and 
		(($decision eq "A") or ($decision eq "R") or ($decision eq " ")) ){
		$current_acct = substr($_,0,19);
		$current_amt = substr($_,19,14);
		if ($current_acct eq $old_acct) { # same account
			$total_amt = $total_amt+$current_amt;
		} else {
			printf("%19s%12.2f\n",$old_acct,$total_amt);
			$total_amt = $current_amt+0;
			$old_acct = $current_acct;
		}
	}
}
