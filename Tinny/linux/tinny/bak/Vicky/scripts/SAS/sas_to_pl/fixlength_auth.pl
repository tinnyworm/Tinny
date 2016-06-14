#!/usr/local/bin/perl 
while(<STDIN>){
  chomp;
  ( $ACCT_ID, $DATE, $TIME, $AMOUNT, $CURRCODE, $CURRRATE, $DECISION, $TRANTYPE, $AVALCRED, $CREDLINE, $MCC_SIC, $POSTAL, $MRCHCNTY, $PIN_VER, $CVV, $PEMODE, $MSGEXTDA, $AUTH_ID, $REASCODE, $ADVICE, $MRCH_ID, $FILLER1, $WHCHCARD, $CASHBACK, $FALCNSCO, $EXTRA1, $TERM_ID, $EXTRA2, $ITEM_CD, $ADDRCODE, $EXTRA3 )=
  unpack("\@0 a19 \@19 a8 \@27 a6 \@33 a13 \@46 a3 \@49 a13 \@62 a1 \@63 a1 \@64 a10 \@74 a10 \@84 a4 \@88 a9 \@97 a3 \@100 a1 \@101 a1 \@102 a1 \@103 a8 \@111 a6 \@117 a5 \@122 a1 \@123 a16 \@139 a6 \@145 a1 \@146 a13 \@159 a4 \@163 a67 \@230 a14 \@246 a59 \@305 a3 \@308 a15 \@323 a1 ",$_);

  foreach $i ($ACCT_ID, $DATE, $TIME, $AMOUNT, $CURRCODE, $CURRRATE, $DECISION, $TRANTYPE, $AVALCRED, $CREDLINE, $MCC_SIC, $POSTAL, $MRCHCNTY, $PIN_VER, $CVV, $PEMODE, $MSGEXTDA, $AUTH_ID, $REASCODE, $ADVICE, $MRCH_ID, $FILLER1, $WHCHCARD, $CASHBACK, $FALCNSCO, $EXTRA1, $TERM_ID, $EXTRA2, $ITEM_CD, $ADDRCODE, $EXTRA3) {
    $i=~s/^ +//;
  }
  $spc=" ";

#--------------- Manually added stuff HERE!!!!! ----------------

  print (pack(
         "A19 A8 A6 A13 A3 A13 A1 A1 A10 A10 A4 A9 A3 A1 A1 A1 A8 A6 A5 A1 A16 A6 A1 A13 A4 A3 A15 A14 ",
         $ACCT_ID, $DATE, $TIME, $AMOUNT, $CURRCODE, $CURRRATE, $DECISION, $TRANTYPE, $AVALCRED, $CREDLINE, $MCC_SIC, $POSTAL, $MRCHCNTY, $PIN_VER, $CVV, $PEMODE, $MSGEXTDA, $AUTH_ID, $REASCODE, $ADVICE, $MRCH_ID, $FILLER1, $WHCHCARD, $CASHBACK, $FALCNSCO, $ITEM_CD, $ADDRCODE, $TERM_ID
         )."\n");
}