data _null_;
infile stdin lrecl=100;
file stdout;
input @1    ACCT_ID      $19.
      @20   TRANTYPE     $1.
      @21   TRANDATE     $8.
      @29   POSTCODE     $9.
      @38   CREDLINE     $10.
      @48   ADDRCODE     $15.
      @63   FRD_TRX      $1.
      ;
put  (ACCT_ID)($19.) (TRANTYPE)($1.) (TRANDATE)($8.)
     (POSTCODE)($9.) (CREDLINE)($10.) ;
     
run;           
      
