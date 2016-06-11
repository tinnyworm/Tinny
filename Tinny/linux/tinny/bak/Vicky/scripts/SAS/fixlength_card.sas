data _null_;
infile stdin lrecl=300;
file stdout;
input @1        ACCT_ID    $19.
      @20       POSTAL     $9.
      @29       CNTRY      $3.
      @32       OPENDATE   $8.
      @40       ISSUDATE   $8.
      @48       ISSUTYPE   $1.
      @49       EXPRDATE   $8.
      @57       CREDLINE   $10.
      @67       GENDER     $1.
      @68       BIRTDATE   $8.
      @76       INCOME     $10.
      @86       TYPE       $1.
      @87       USE        $1.
      @88       NUM_CARD   $3.
      @91       REC_DATE   $8.
      @99       ADRLINE1   $26.
      @125      ADRLINE2   $26.
      @151      SSN_5      $5.
      @156      ASSOC      $1.
      @157      INCENTIV   $1.
      @158      XRPRACCT   $19.
      @177      STATUS     $2.
      @179      STATDATE   $8.
      @187      EXTRA      $89.              
      @276      ADDR_CODE  $15.
      @291      MEDIA      $1.;
put (acct_id)($19.) (postal)($9.) (cntry)($3.) (opendate)($8.) (issudate)($8.) (issutype)($1.) (exprdate)($8.)
    (credline)(10.) (gender)($1.) (birtdate)($8.) (income)($10.) (type)($1.) (use)($1.) (num_card)($3.) 
    (rec_date)($8.) (adrline1)($26.) (adrline2)($26.) (ssn_5)($5.) (assoc)($1.) (incentiv)($1.) (xrpracct)($19.)
    (status)($2.) (statdate)($8.) (addr_code)($15.) (media)($1.);
run;
/*proc print data=temp (obs=100);
var old1 old2 addr_code media;
run;
