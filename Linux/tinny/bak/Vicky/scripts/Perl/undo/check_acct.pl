#! /usr/local/bin/perl -w

while (<STDID>){
    chomp;
    $acct=substr($_,3,19);
    $cltid=substr($_,196,4);
    push (@clientID, $cltid);

}
    foreach $uid (dedup(@clientID)) {
      if ($cltid eq 
       ($acctHash{$acct} ++) or ($acctHash{$acct}=1);
    }


sub dedup {
	%seen =();
	@uniqu = grep {! $seen{$_}++} @_;
	@sortuniqu = sort {$a cmp $b} @uniqu;	
	return @sortuniqu;
	}
	#:","$fieldHash[$i]{$key}\n"); 

sub commify {
     my $text = reverse $_[0];
     $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
     return scalar reverse $text;
}
