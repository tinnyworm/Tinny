#! /usr/local/bin/perl -s

$usage = "$0 -sr=sample_rate -amt=t1,t2,t3,...\n\n";

die $usage unless $amt;
$sr=0.01 unless $sr;

@amt_thres = split /,/,$amt;

# Count
while(<>){
  if(!($. % 10000)){
    print STDERR "$.\r";
  }
  ($amount, $tag, $sic, $decision) =
    unpack "\@19a14 \@59a1 \@94a4 \@88a1", $_;
  $decision =~ tr / /A/;
  next unless($decision eq "A");
  $tag =~ tr /2/0/;
  
  $COUNT[$tag]{0} ++;
  $VALUE[$tag]{0} += $amount;
  foreach $val (@amt_thres){
    if($amount >= $val){
      $COUNT[$tag]{$val} ++;
      $VALUE[$tag]{$val} += $amount;
    }
  }
}
print STDERR "$.\n";

print "Threshold, legit vol, fraud vol, Tot vol, value legit, value fraud\n";
foreach $key (sort {$a <=> $b} keys %{$COUNT[0]}){
  printf("\$$key, %.2f%%, %.2f%%, %.2f%%, %.2f%%, %.2f%%\n",
	 $COUNT[0]{$key}/$COUNT[0]{0}*100,
	 $COUNT[1]{$key}/$COUNT[1]{0}*100,
	 ($COUNT[0]{$key}/$sr + $COUNT[1]{$key})
	 /($COUNT[0]{0}/$sr + $COUNT[1]{0})*100,
	 $VALUE[0]{$key}/$VALUE[0]{0}*100,
	 $VALUE[1]{$key}/$VALUE[1]{0}*100
	);
}
