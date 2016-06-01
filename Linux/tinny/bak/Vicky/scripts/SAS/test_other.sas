data _null_ (drop=count x);
infile './&file ';
input @1   ACCT_ID             $19.
      @20  DATE            YYMMDD8.
	  @28  TIME                  6.
	  @34  AMOUNT               13.
	  @47  CURR_CODE            13.
	  @50  CURR_RATE            13.
	  @63  DECISION             $1.
	  @64  TRAN_TYPE            $1.
	  @65  AVAIL_CRED           10.
	  @75  CRED_LINE            10.
	  @85  SIC                   4.
	  @89  POSTAL_CODE           9.
	  @98  MRCH_CNTRY            3.
	  @101 PIN_VER              $1.
	  @102 CVV                  $1.
	  @103 KEY_SWIPE            $1.
	  @104 MSG_EXPIRE_DATE YYMMDD8.
	  @112 AUTH_ID              $6.
	  @118 REASON_CODE          $5.
	  @123 ADVICE               $1.
	  @124 MRCH_ID             $16.
	  @140 TERM_ID              $6.
	  @146 WHICH_CARD           $1.
	  @147 CASHBACK_AMT         13.
	  @160 FALCON_SCORE          4.
	  @164 ITEM_CD               3.
	  @167 ADDR_CD              15.
	  @182 FILLER               1.;
count+1;
by count;
do x=1 while (x le last.count);
x+1;
ADDR_TERM_ID=repeat('0',13); end;
format ADDR_TERM_ID $13.;
file '../195.dat';
put   (ACCT_ID DATE TIME AMOUNT CURR_CODE           
	  CURR_RATE DECISION TRAN_TYPE AVAIL_CRED           
	  CRED_LINE SIC POSTAL_CODE MRCH_CNTRY PIN_VER              
	  CVV KEY_SWIPE MSG_EXPIRE_DATE AUTH_ID              
      REASON_CODE ADVICE MRCH_ID TERM_ID WHICH_CARD           
      CASHBACK_AMT FALCON_SCORE ITEM_CD ADDR_CD ADDR_TERM_ID;
run; 
