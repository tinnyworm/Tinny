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
      @137  TERMID       $6.
      @143  WHCHCARD     $1.
      @144  CASHBACK     $13.
      @157  FALCSCOR     $4.
      @161  ITEMCODE     $3.
      @164  ADDRCODE     $15.
      @179  FILLER       $1.
      ;
y_year=substr(POSTDATE,1, 4);
if y_year >= '2002';      
put  (acct_id)($19.) (AMOUNT)($13.) (CURRCODE)($3.) (CURRRATE)($13.) (POSTDATE)($8.)
     (TRANDATE)($8.) (TRANTIME)($6.) (AVAICRED)($10.) (CREDLINE)($10.) (TRANTYPE)($1.)
     (AUTHED)($1.) (MCCSIC)($4.) (POSTCODE)($9.) (MRCHCNTY)($3.) (AUTHID)($6.)
     (POSTID)($6.) (MRCHID)($16.) (TERMID)($6.) (WHCHCARD)($1.) (CASHBACK)($13.)
     (FALCSCOR)($4.) (ITEMCODE)($3.) (ADDRCODE)($15.) (FILLER)($1.);

run;
