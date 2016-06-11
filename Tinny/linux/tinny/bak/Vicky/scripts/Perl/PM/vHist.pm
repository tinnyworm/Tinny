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
    $dt=$data->{$i};
    $cumavg{$i} = $cumavg{$i}*$cnt/($cnt+1)+$dt/($cnt+1);
    $cumsqravg{$i} = $cumsqravg{$i}*$cnt/($cnt+1)+$dt**2/($cnt+1);
    $nummin{$i} = $dt if ($cnt == 0 || $dt < $nummin{$i});
    $nummax{$i} = $dt if ($cnt == 0 || $dt > $nummax{$i});
  }
  foreach $i (@$amount){
    $dt=$data->{$i};
    if($dt==0){}
    elsif($dt<500){$dt=10*int($dt/10);$dt=$dt?$dt:0.01;}
    elsif($dt<5000){$dt=50*int($dt/50);}
    else{$dt=100*int($dt/100);}
    $data->{$i}=$dt;
  }

  
  foreach $i (@$varlist){
    $hist{$i}{$data->{$i}}++;
  }
  $cnt++;
}

sub printOut{
  foreach $i (@$varlist){
    print "Histogram of $i:\n";
    if (grep /^$i$/,@$numeric){
      @k_sort=sort{$a<=>$b} keys %{$hist{$i}};
    }elsif (grep /^$i$/,@$categorical){
      @k_sort=sort{$hist{$i}{$b}<=>$hist{$i}{$a}} keys %{$hist{$i}};
    }else{
      @k_sort=sort{$a cmp $b} keys %{$hist{$i}};
    }
    $max=0;
    foreach $k (keys %{$hist{$i}}){
      $max=$hist{$i}{$k}>$max?$hist{$i}{$k}:$max;
      $maxlength=(length $k)>$maxlength?(length $k):$maxlength;
    }
    $cumfreq=0; 
    foreach $x (@k_sort){
      $pct=100*$hist{$i}{$x}/$cnt;
      $cumfreq+=$hist{$i}{$x};
      $cumpct=100*$cumfreq/$cnt;
      $charline="*" x int(20*$hist{$i}{$x}/$max);
      printf("%-${maxlength}s |%-20s %9d %9d %6.2f%% %6.2f%%\n",$x,$charline,$hist{$i}{$x},$cumfreq,$pct,$cumpct);
    }
    print "\n\n";
    if (grep /$i/,@$numeric){
      print "Number of entries: $cnt\n";
      print "Average: ", $cumavg{$i},"\n";
      print "Std Dev: ", $cnt>1?sqrt(($cumsqravg{$i}-$cumavg{$i}**2)*$cnt/($cnt-1)):0, "\n" ;
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

