*options pagesize=max;

*ods html body='Cbody.html' contents='Ccontents.html' frame='HTML/Cframe.html';

/* Define path to SAS macro and format catalogs */
x setenv MSGBIN /work/gold/projects/bin;

libname library "$MSGBIN/CCDS/3.10/SAS";
options mstored sasmstore=library nofmterr;

/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data card;

  infile STDIN lrecl=186 length=len missover;

  input
    @001 C001 $char19. /* ACCT-ID        */
    @020 C020 $char9.  /* POSTAL-CODE    */
    @029 C029 $char3.  /* CNTRY          */
    @032 C032 $char8.  /* OPEN-DATE      */
    @040 C040 $char8.  /* ISSUE-DATE     */
    @048 C048 $char1.  /* ISSUE-TYPE     */
    @049 C049 $char8.  /* EXPIRE-DATE    */
    @057 C057 $char10. /* CRED-LINE      */
    @067 C067 $char1.  /* GENDER         */
    @068 C068 $char8.  /* BIRTHDATE      */
    @076 C076 $char10. /* INCOMDE        */
    @086 C086 $char1.  /* TYPE           */
    @087 C087 $char1.  /* USE            */
    @088 C088 $char3.  /* NUM-CARDS      */
    @091 C091 $char8.  /* REC-DATE       */
    @099 C099 $char26. /* ADDR-LINE-1    */
    @125 C125 $char26. /* ADDR-LINE-2    */
    @151 C151 $char5.  /* SSN-5          */
    @156 C156 $char1.  /* ASSOC          */
    @157 C157 $char1.  /* INCENTIVE      */
    @158 C158 $char19. /* XREF-PREV-ACCT */
    @177 C177 $char2.  /* STATUS         */
    @179 C179 $char8.  /* STATUS-DATE    */
  ;

  retain minlen 999 maxlen 0;
  minlen=min(minlen,len);
  maxlen=max(maxlen,len);

  if (_error_) then
    do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

  /*** Cut month and/or year ***/
  opn_mnth = substr(C032,1,6);    /* Open Date  */
  iss_mnth = substr(C040,1,6);    /* Issue Date */
  brth_yr  = substr(C068,1,4);    /* Birthdate  */

  /*** Convert to numbers ***/
  length bin $6;
  bin   = substr(C001,1,6);       /* Account 6-digits */
  nbin  = input(bin,??6.0);       /* Account BIN     */
  C057N = input(C057,??10.0);     /* Credit Line     */
  C076N = input(C076,??10.0);     /* Income          */
  C088N = input(C088,??2.0);      /* Number of Cards */

  /*** Bin various fields ***/
  crd_bin  = put(C057N,credit.);  /* Credit Line      */
  inc_bin  = put(C076N,credit.);  /* Income           */
  post_bin = substr(C020,1,5);    /* Postal Code      */
  xref_mis = put(C158,$missing.); /* Previous Account */

  label
    C001 = 'ACCT-ID'
    C020 = 'POSTAL-CODE'
    C029 = 'CNTRY'
    C032 = 'OPEN-DATE'
    C040 = 'ISSUE-DATE'
    C048 = 'ISSUE-TYPE'
    C049 = 'EXPIRE-DATE'
    C057 = 'CRED-LINE'
    C067 = 'GENDER'
    C068 = 'BIRTHDATE'
    C076 = 'INCOME'
    C086 = 'TYPE'
    C087 = 'USE'
    C088 = 'NUM-CARDS'
    C091 = 'REC-DATE'
    C099 = 'ADDR-LINE-1'
    C125 = 'ADDR-LINE-2'
    C151 = 'SSN-5'
    C156 = 'ASSOC'
    C157 = 'INCENTIVE'
    C158 = 'XREF-PREV-ACCT'
    C177 = 'STATUS'
    C179 = 'STATUS-DATE'
    bin = 'Account 6 digits'
    post_bin = 'Postal code 5 digits'
    opn_mnth = 'Open date year and month'
    iss_mnth = 'Issue date year and month'
    C057N = 'CREDIT-LINE'
    crd_bin = 'Credit line category'
    brth_yr = 'Birth year'
    C076N = 'Income'
    inc_bin = 'Binned income'
    C088N = 'Number of cards'
    xref_mis = 'Account Xref'
  ;
run;

************************************************************;

/*** Print out the total number of observations & record lengths ***/
%let dsid = %sysfunc(open(card));
%let ttlobs = %sysfunc(attrn(&dsid,nobs));
%let rc = %sysfunc(close(&dsid));

/*** cardstripl.pl functionality ***/
proc sort data=card;
  by C001 C091;
run;

/*
data card;
  set card;
  by C001;
  if first.C001;
run;
*/

proc datasets nolist library=work;
  change card=cardu(memtype=data);
run;

************************************************************;

*%macro comment;
%doby(cardu,bin,tmp,C001);

%let dsid     = %sysfunc(open(tmp));
%let uniqacts = %sysfunc(attrn(&dsid,nobs));
%let rc       = %sysfunc(close(&dsid));

proc datasets nolist library=work;
  delete tmp;
run;
*%mend comment;

data _null_;
  set cardu;
  call symput('minlen',minlen);
  call symput('maxlen',maxlen);
run;

data _null_;
  file print;
  put "Total number of records   = &ttlobs";
  put "Number of unique accounts = &uniqacts";
  put "Minimum record length = &minlen";
  put "Maximum record length = &maxlen";
run;

************************************************************;

/*** Display a Random Sample of &RanObs Records ***/
%let RanObs = 20;

data random (drop=k n);
  retain k &RanObs n;
  if (_n_ = 1) then n = total;
  set cardu nobs=total;
  if (ranuni(7993652) <= k / n) then
    do;
      output;
      k = k - 1;
  end;
  n = n - 1;
  if (k = 0) then stop;
run;

proc print label data=random;
  var C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076 C086
      C087 C088 C091 C099 C125 C151 C156 C157 C158 C177 C179;
  title "A001. Visual inspection: random sample of &RanObs records";
run;

proc datasets nolist library=work;
  delete random;
run;

************************************************************;

/*** Find &BINNobs most populated Account bins ***/
%let BINNobs = 20;

proc freq data=cardu order=freq;
  tables bin / out=acct_f noprint;
run;

/*** Save the total number of distinct BINs ***/
%let dsid  = %sysfunc(open(acct_f));
%let nBINs = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc means noprint data=cardu;
  var nbin;
  output out=bin_m min=min max=max;
  format nbin bins.;
run;

data _null_;
  set bin_m;
  file print;
  title 'A012B. Extreme values of account number 6-digits';
  put "Number of BINs = &nBINs";
  put "Lowest BIN     = " min;
  put "Highest BIN    = " max;
  format min max z6.;
run;

proc print label data=acct_f (obs=&BINNobs);
  format percent 6.2;
  title "A012A. Top &BINNobs most frequent account number 6 digits";
run;

proc datasets nolist library=work;
  delete acct_f bin_m;
run;

/* Account numbers */
proc chart data=cardu;
  hbar bin / type=percent missing space=0;
  title 'A012E. Distribution of account number 6 digits';
run;

************************************************************;

/* Postal codes */
%let ZIPobs = 100;

proc freq data=cardu order=freq;
  tables post_bin / out=post_f noprint missing;
run;

proc print label data=post_f (obs=&ZIPobs);
  format percent 6.2;
  title "A013. Top &ZIPobs most frequent postal codes";
run;

proc datasets nolist library=work;
  delete post_f;
run;

************************************************************;

/* Cardholder country */
proc chart data=cardu;
  hbar C029 / type=percent missing nozeros space=0 descending;
  format C029 $country.;
  title 'A014. Country of Cardholder';
run;

************************************************************;

/* Month opened */
proc chart data=cardu;
  hbar opn_mnth / type=percent missing nozeros space=0;
  title 'A015. Month account opened';
run;

/* Month issued */
proc chart data=cardu;
  hbar iss_mnth / type=percent missing nozeros space=0;
  title 'A016. Month card issued';
run;

************************************************************;

/* Issue Type */
%testfield(C048,cardu,$issue.,A017,Card Issue Type);

************************************************************;

/* Expiration date */
proc chart data=cardu;
  hbar C049 / type=percent missing nozeros space=0;
  title 'A018. Card expire date';
run;

************************************************************;

/* Credit line */
proc means data=cardu n nmiss min max mean median std;
  var C057N;
  title 'A019A. Statistics of card credit lines';

proc chart data=cardu;
  hbar crd_bin / type=percent missing nozeros space=0 descending;
  title 'A019B. Card credit-lines by range';
run;

************************************************************;

/* Gender */
%testfield(C067,cardu,$gender.,A020,Card Gender);

************************************************************;

/* Birth date */
proc chart data=cardu;
  hbar brth_yr / type=percent missing space=0;
  title 'A021. Cardholder birth year';
run;

************************************************************;

/* Income */
proc means data=cardu n nmiss min max mean median std;
  var C076N;
  title 'A022A. Statistics of cardholder incomes';
run;
   
proc chart data=cardu;
  hbar inc_bin / type=percent missing nozeros space=0 descending;
  title 'A022B. Cardholder incomes by range';
run;

************************************************************;

/* Card Type */
%testfield(C086,cardu,$type.,A023,Card Type);

/* Use */
%testfield(C087,cardu,$use.,A024,Card Use);

************************************************************;

/* Number of cards */
proc means data=cardu n nmiss min max median std;
  var C088N;
  title 'A025. Statistics of number of cards';
run;

************************************************************;

proc chart data=cardu;
  hbar C091 / type=percent missing nozeros space=0;
  title 'A026. Record dates';
run;

************************************************************;

/* SS numbers */
%testfield(C151,cardu,$SSN5dig.,A029,5-digit SSNs);

************************************************************;

/* Association */
%testfield(C156,cardu,$assoc.,A030,Card Association);

************************************************************;

/* Incentive */
%testfield(C157,cardu,$incentv.,A031,Card Incentive);

************************************************************;

proc chart data=cardu;
  hbar xref_mis / type=percent missing space=0 descending;
  title 'A032. Previous Account Cross-Reference';
run;

************************************************************;

/* Status */
%testfield(C177,cardu,$status.,A033,Card Status);

/* Status date */
proc chart data=cardu;
  hbar C179 / type=percent missing space=0;
  title 'A034. Statistics of card status dates';
run;

*ods html close;

endsas;
