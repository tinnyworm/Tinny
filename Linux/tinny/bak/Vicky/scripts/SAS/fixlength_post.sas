data _null_;
infile stdin lrecl=350;
file stdout;
input @1    ACCT_ID      $19.
      @20   AMOUNT       $13.
      @33   CURRCODE     $3.
      @36   CURRRATE     $13.
      @49   POSTDATE     $8.
      @57   TRANDATE     $8.
      @65   TRANTIME     $6.
      @71   AVAICRED     $10.
      @81   CREDLINE     $10.
      @91   TRANTYPE     $1.
      @92   AUTHED       $1.
      @93   MCCSIC       $4.
      @97   POSTCODE     $9.
      @106  MRCHCNTY     $3.
      @109  AUTHID       $6.
      @115  POSTID       $6.
      @121  MRCHID       $16.
      @137  FILLER1      $6.
      @143  WHCHCARD     $1.
      @144  CASHBACK     $13.
      @157  FALCSCOR     $4.
      @161  FILLER2      $1.
      @162  EXTRA1       $78.
      @240  EXTRA2       $60.
      @300  ITEMCODE     $3.
      @303  ADDRCODE     $15.
      @318  PAYMENT      $1.
      ;
put  (acct_id)($19.) (AMOUNT)($13.) (CURRCODE)($3.) (CURRRATE)($13.) (POSTDATE)($8.)
     (TRANDATE)($8.) (TRANTIME)($6.) (AVAICRED)($10.) (CREDLINE)($10.) (TRANTYPE)($1.)
     (AUTHED)($1.) (MCCSIC)($4.) (POSTCODE)($9.) (MRCHCNTY)($3.) (AUTHID)($6.)
     (POSTID)($6.) (MRCHID)($16.) (FILLER1)($6.) (WHCHCARD)($1.) (CASHBACK)($13.)
     (FALCSCOR)($4.) (ITEMCODE)($3.) (ADDRCODE)($15.) (FILLER2)($1.);

run;
