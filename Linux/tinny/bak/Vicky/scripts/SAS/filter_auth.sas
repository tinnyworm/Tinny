data _null_;
infile stdin lrecl=350;
file stdout;
input @1        ACCT_ID    $19.
      @20       DATE       $8.
      @28       TIME       $6.
      @34       AMOUNT     $13.
      @47       CURRCODE   $3.
      @50       CURRRATE   $13.
      @63       DECISION   $1.
      @64       TRANTYPE   $1.
      @65       AVALCRED   $10.
      @75       CREDLINE   $10.
      @85       MCC_SIC    $4.
      @89       POSTAL     $9.
      @98       MRCHCNTY   $3.
      @101      PIN_VER    $1.
      @102      CVV        $1.
      @103      PEMODE     $1.
      @104      MSGEXTDA   $8.
      @112      AUTH_ID    $6.
      @118      REASCODE   $5.
      @123      ADVICE     $1.
      @124      MRCH_ID    $16.
      @140      FILLER1    $6.
      @146      WHCHCARD   $1.
      @147      CASHBACK   $13.
      @160      FALCNSCO   $4.
      @164      ITEM_CD    $3.
      @167      ADDRCODE   $15.
      @182      TERM_ID    $14.
      ;

y_year=substr(date, 1, 4);
if y_year >='2002';
      
put (acct_id)($19.) (DATE)($8.) (TIME)($6.) (AMOUNT)($13.) (CURRCODE)($3.) (CURRRATE)($13.)
    (DECISION)($1.) (TRANTYPE)($1.) (AVALCRED)($10.) (CREDLINE)($10.) (MCC_SIC)($4.)
    (POSTAL)($9.) (MRCHCNTY)($3.) (PIN_VER)($1.) (CVV)($1.) (PEMODE)($1.) (MSGEXTDA)($8.)
    (AUTH_ID)($6.) (REASCODE)($5.) (ADVICE)($1.) (MRCH_ID)($16.) (FILLER1)($6.) (WHCHCARD)($1.)
    (CASHBACK)($13.) (FALCNSCO)($4.) (ITEM_CD)($3.) (ADDRCODE)($15.) (TERM_ID)($14.);
run;
/*proc print data=temp (obs=100);
var old1 old2 addr_code media;
run;
