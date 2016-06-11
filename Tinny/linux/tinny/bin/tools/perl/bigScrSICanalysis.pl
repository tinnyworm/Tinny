#! /usr/local/bin/perl

# Count
while(<>){
  if(!($. % 10000)){
    print STDERR "$.\r";
  }
  ($amount, $tag, $sic) = unpack "\@19a14 \@59a1 \@94a4", $_;
  $tag =~ tr /2/0/;
  
  $COUNT[$tag]{$sic} ++;
  $VALUE[$tag]{$sic} += $amount;
}
print STDERR "$.\n";

print "SIC, nr legit, nr fraud, value legit, value fraud\n";
foreach $key (sort keys %{$COUNT[0]}){
  printf("$key, %d, %d, \$%.2f, \$%.2f\n",
	 $COUNT[0]{$key}, $COUNT[1]{$key},
	 $VALUE[0]{$key}, $VALUE[1]{$key});
}
