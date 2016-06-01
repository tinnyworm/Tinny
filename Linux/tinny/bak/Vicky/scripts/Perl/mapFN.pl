#! /usr/local/bin/perl -w


open (NEW, "cat nonm.list |") || die "couldn't open nonm.list";
open (RAW, "cat filelist | ") || die "couldn't open filelist" ;

while (<NEW>) {
  chomp;
  $tmp= $_;
  $tmp=~ tr/A-Z/L/;
  $tmp=~ tr/0-9/#/;
  $loc= index($tmp, '######L');
  $inv=substr($_,$loc,7);
  push(@new, $inv);
  push(@filenm, $_);
}
close (NEW);

while (<RAW>) {
  chomp;
  $tmp= $_;
  $tmp=~ tr/A-Z/L/;
  $tmp=~ tr/0-9/#/;
  $loc= index($tmp, '######L');
  $inv=substr($_,$loc,7);
  for ($i=0; $i < @new; $i ++){
    if ($inv eq $new[$i]) {
    print "$_  $filenm[$i]\n";
    rename( $_, $filenm[$i]) or warn "Couldn't rename $_ to $filenm[$i]:$!\n" ;
    }
  }
}
close (RAW);


