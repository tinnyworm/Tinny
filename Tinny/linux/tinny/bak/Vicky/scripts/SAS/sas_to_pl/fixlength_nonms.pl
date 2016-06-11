#!/usr/local/bin/perl 
while(<STDIN>){
  chomp;
  ( $ACCT_ID, $TRANTYPE, $TRANDATE, $POSTCODE, $CREDLINE, $ADDRCODE, $FRD_TRX )=
  unpack("\@0 a19 \@19 a1 \@20 a8 \@28 a9 \@37 a10 \@47 a15 \@62 a1 ",$_);

  foreach $i ($ACCT_ID, $TRANTYPE, $TRANDATE, $POSTCODE, $CREDLINE, $ADDRCODE, $FRD_TRX) {
    $i=~s/^ +//;
  }
  $spc=" ";

#--------------- Manually added stuff HERE!!!!! ----------------

  print (pack(
         "A19 A1 A8 A9 A10 ",
         $ACCT_ID, $TRANTYPE, $TRANDATE, $POSTCODE, $CREDLINE
         )."\n");
}