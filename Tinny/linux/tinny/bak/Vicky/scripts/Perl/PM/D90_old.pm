#!/usr/local/bin/perl -w
package D90;

sub today90 ($){
  use strict 'vars';
  my $ymd=shift;
  my @mds=(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
  my ($yy,$mm,$dd);
  my $d90;
  my $i;

  ($yy,$mm,$dd)=unpack("a4 a2 a2",$ymd);

  if ($mm<1 || $mm>12) {return -1};
  if ($dd<1 || $dd > $mds[$mm-1]) {return -1};

  sub NUM_DAYS_IN_YEAR($) {
    my $i=shift;
    return (($i%4==0 && $i%100!=0) || ($i%400==0)) ? 366 : 365;
  }

  if($yy<1990)
  {
      for($d90=0, $i=1989; $i>$yy; $i--)
      {
        $d90 -= NUM_DAYS_IN_YEAR($i);
      }
      for($i=11; ($i+1)>$mm; $i--)
      {  if ($i==1 && NUM_DAYS_IN_YEAR($yy)==366)
         {
           $d90 -= $mds[$i]+1;
         }
         else
         {
           $d90 -= $mds[$i];
         }
      }

      if ($mm==2 && NUM_DAYS_IN_YEAR($yy)==366)
      {
         $d90 -= ($mds[$mm-1]+2) - $dd;
      }
      else
      {
         $d90 -= ($mds[$mm-1]+1) - $dd;
      }
   }
   else
   {  
      for($d90=0, $i=1990; $i<$yy; $i++)
      {
        $d90 += NUM_DAYS_IN_YEAR($i);
      }
      for($i=0; ($i+1)<$mm; $i++)
      {  if ($i==1 && NUM_DAYS_IN_YEAR($yy)==366)
         {
            $d90 += $mds[$i]+1;
         }
         else
         {
            $d90 += $mds[$i];
         }
      }

      $d90 += $dd-1;
   }
   return $d90;
}

1;
