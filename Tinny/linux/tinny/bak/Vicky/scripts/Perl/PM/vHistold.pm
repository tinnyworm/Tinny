package vHist;
require 5.000;

sub setData
{
  ($varlist,$numeric,$amount,$categorical)=@_;
  $cnt=0;
}

sub update
{
  my $data=shift;
  foreach $i (@$numeric){
    $cumavg{$i} = $cumavg{$i}*$cnt/($cnt+1)+$data->{$i}/($cnt+1);
    $cumsqravg{$i} = $cumsqravg{$i}*$cnt/($cnt+1)+$data->{$i}**2/($cnt+1);
    $nummin{$i} = $data->{$i} if ($cnt == 0 || $data->{$i} < $nummin{$i});
    $nummax{$i} = $data->{$i} if ($cnt == 0 || $data->{$i} > $nummax{$i});
  }
  foreach $i (@$amount){
    if($data->{$i}==0){}
    elsif($data->{$i}<500){$data->{$i}=10*int($data->{$i}/10);$data->{$i}=$data->{$i}?$data->{$i}:0.01;}
    elsif($data->{$i}<5000){$data->{$i}=50*int($data->{$i}/50);}
    else{$data->{$i}=100*int($data->{$i}/100);}
  }

  
  foreach $i (@$varlist){
    $hist{$i}{$data->{$i}}++;
    $max{$i}=$hist{$i}{$data->{$i}}>$max{$i}?$hist{$i}{$data->{$i}}:$max{$i};
    $maxlength{$i}=(length $data->{$i})>$maxlength{$i}?(length $data->{$i}):$maxlength{$i};
  }
  $cnt++;
}

sub printOut{
  foreach $i (@$varlist){
    print "Histogram of $i:\n";
    if (grep /$i/,@$numeric){
      @k_sort=sort{$a<=>$b} keys %{$hist{$i}};
    }elsif (grep /$i/,@$categorical){
      @k_sort=sort{$hist{$i}{$b}<=>$hist{$i}{$a}} keys %{$hist{$i}};
    }else{
      @k_sort=sort{$a cmp $b} keys %{$hist{$i}};
    }

    $cumfreq=0; 
    foreach $x (@k_sort){
      $pct=100*$hist{$i}{$x}/$cnt;
      $cumfreq+=$hist{$i}{$x};
      $cumpct=100*$cumfreq/$cnt;
      $charline="*" x int(20*$hist{$i}{$x}/$max{$i});
      printf("%-${maxlength{$i}}s |%-20s %9d %9d %6.2f%% %6.2f%%\n",$x,$charline,$hist{$i}{$x},$cumfreq,$pct,$cumpct);
    }
    print "\n\n";
    if (grep /$i/,@numeric){
      print "Number of entries: $cnt\n";
      print "Average: ", $cumavg{$i},"\n";
      print "Std Dev: ", $cnt>1?($cumsqravg{$i}-$cumavg{$i}**2)*$cnt/($cnt-1):0, "\n" ;
      print "Min: ", $nummin{$i}, "\n";
      print "Max: ", $nummax{$i}, "\n";
      print "\n\n";
    }
  }
}

1;
__END__
vHist::setData(\@vl,\@numeric,\@amt,\@categorical);
while (<STDIN>) {
  chomp;
  %data = &ReadFilePerFmt::parseLine ($_, %Fmt_a);
  foreach $i (@amt) {$data{$i}=$data{$i}/100;}
  $data{"Tran_Month"}=substr($data{"Tran_Date"},0,2);
  $data{"Tran_Hour"}=substr($data{"Tran_Time"},0,2);
  vHist::update(\%data);
}

vHist::printOut();

