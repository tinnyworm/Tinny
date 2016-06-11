package Interpolate;
sub int_3pt {
  my ($data1ref,$data2ref,$xx) = @_;
  my (%y,$i);
  my ($xl,$xc,$xg)=(-1e100,-1e100,-1e100);
  my ($yl,$yc,$yg)=(0,0,0);
  my $yy;

  for ($i=0;$i<=$#$data1ref;$i++){
    $y{$$data1ref[$i]}=$$data2ref[$i];
  }

  @k_sort=sort {$a<=>$b} keys %y;
  $k_last=$#_sort;
  if ($xx <= $k_sort[0]){
    $xl=$k_sort[0];
    $xg=$k_sort[1];
    $yl=$y{$k_sort[0]};
    $yg=$y{$k_sort[1]};
    $yy=($xg-$xl)!=0?$yl+($yg-$yl)/($xg-$xl)*($xx-$xl):$yl;
  } elsif ($xx >= $k_sort[$k_last]){
    $xl=$k_sort[$k_last-1];
    $xg=$k_sort[$k_last];
    $yl=$y{$k_sort[$k_last-1]};
    $yg=$y{$k_sort[$k_last]};
    $yy=($xg-$xl)!=0?$yl+($yg-$yl)/($xg-$xl)*($xx-$xl):$yl;
  } else {
    foreach $x (@k_sort){
      if ($xg < $xx || ($xx - $xc > $xg -$xx) ) {
        ($xl,$xc,$xg)=($xc,$xg,$x);
        ($yl,$yc,$yg)=($yc,$yg,$y{$x});
      } 
    }
    print "($xl,$xc,$xg) ($yl,$yc,$yg)\n";
    if ($xc == $xl || $xc == $xg){$yy=$yc;}
    else{
    $yy=($xx-$xc)*($xx-$xg)/($xl-$xc)/($xl-$xg)*$yl
      +($xx-$xl)*($xx-$xg)/($xc-$xl)/($xc-$xg)*$yc
      +($xx-$xl)*($xx-$xc)/($xg-$xl)/($xg-$xc)*$yg;
    }
  }
  return $yy;
}

sub int_2pt {
  my ($data1ref,$data2ref,$xx) = @_;
  my (%y,$i);
  my ($xl,$xg)=(0,0);
  my ($yl,$yg)=(0,0);
  my $yy;

  for ($i=0;$i<=$#$data1ref;$i++){
    $y{$$data1ref[$i]}=$$data2ref[$i];
  }

  @k_sort=sort {$a<=>$b} keys %y;
  foreach $x (@k_sort){
    if ($x == $xx){
      $yy=$y{$x};
      return $yy;
    }elsif ($xg < $xx) {
      ($xl,$xg)=($xg,$x);
      ($yl,$yg)=($yg,$y{$x});
    } 
  }
#  print "($xc,$yc)\n";
  $yy=($xg-$xl)!=0?$yl+($yg-$yl)/($xg-$xl)*($xx-$xl):$yl;
  return $yy;
}

1;

__END__
USAGE:

while(<STDIN>){
  split;
  push @x, $_[0];
  push @y, $_[1];
}

print int_3pt(\@x,\@y,20),"\n";
print int_2pt(\@x,\@y,20),"\n";
