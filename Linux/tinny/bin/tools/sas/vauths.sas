*options pagesize=max;

*ods html body='Abody.html' contents='Acontents.html' frame='HTML/Aframe.html';

/* Define path to SAS macro and format catalogs */
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

data auth;

  infile STDIN lrecl=204 length=len missover;

  input
    @001 C001 $char19.   /* ACCT-ID         */
    @020 C020 $char8.    /* DATE            */
    @028 C028 $char6.    /* TIME            */
    @034 C034 $char13.   /* AMOUNT          */
    @047 C047 $char3.    /* CURR-CODE       */
    @050 C050 $char13.   /* CURR-RATE       */
    @063 C063 $char1.    /* DECISION        */
    @064 C064 $char1.    /* TRAN-TYPE       */
    @065 C065 $char10.   /* AVAIL-CRED      */
    @075 C075 $char10.   /* CRED-LINE       */
    @085 C085 $char4.    /* SIC             */
    @089 C089 $char9.    /* POSTAL-CODE     */
    @098 C098 $char3.    /* MRCH-CNTRY      */
    @101 C101 $char1.    /* PIN-VER         */
    @102 C102 $char1.    /* CVV             */
    @103 C103 $char1.    /* KEY-SWIPE       */
    @104 C104 $char8.    /* MSG-EXPIRE-DATE */
    @112 C112 $char6.    /* AUTH-ID         */
    @118 C118 $char5.    /* REASON-CODE     */
    @123 C123 $char1.    /* ADVICE          */
    @124 C124 $char16.   /* MRCH-ID         */
    @140 C140 $char6.    /* TERM-ID         */
    @146 C146 $char1.    /* WHICH-CARD      */
    @147 C147 $char13.   /* CASHBACK-AMT    */
    @160 C160 $char4.    /* FALCON-SCORE    */
    @164 C164 $char1.    /* FILLER          */
    @165 C165 $char22.   /* MERCHANT-NAME   */
    @188 C188 $char13.   /* MERCHANT-CITY   */
    @202 C202 $char3.    /* MERCHANT-STATE  */
  ;

  retain minlen 999 maxlen 0;
  minlen=min(minlen,len);
  maxlen=max(maxlen,len);

  label
    C001 = 'ACCT-ID'
    C020 = 'DATE'
    C028 = 'TIME'
    C034 = 'AMOUNT'
    C047 = 'CURR-CODE'
    C050 = 'CURR-RATE'
    C063 = 'DECISION'
    C064 = 'TRAN-TYPE'
    C065 = 'AVAIL-CRED'
    C075 = 'CRED-LINE'
    C085 = 'SIC'
    C089 = 'POSTAL-CODE'
    C098 = 'MRCH-CNTRY'
    C101 = 'PIN-VER'
    C102 = 'CVV'
    C103 = 'KEY-SWIPE'
    C104 = 'MSG-EXPIRE-DATE'
    C112 = 'AUTH-ID'
    C118 = 'REASON-CODE'
    C123 = 'ADVICE'
    C124 = 'MRCH-ID'
    C140 = 'TERM-ID'
    C146 = 'WHICH-CARD'
    C147 = 'CASHBACK-AMT'
    C160 = 'FALCON-SCORE'
    C164 = 'FILLER'
    C165 = 'MERCHANT-NAME (name)'
    C188 = 'MERCHANT-NAME (city)'
    C202 = 'MERCHANT-NAME (state)'
  ;

  if (_error_) then
    do;
      file STDERR;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

  /*** Bin the accounts ***/
  length bin $6;
  bin  = substr(C001,1,6);
  nbin = input(bin,??6.0);

  /*** Calculate TRX day-of-week ***/
  format aut_date date7.;
  aut_date = input(C020,??yymmdd8.);
  aut_day  = weekday(aut_date);

  /*** Cut out year+month part of date ***/
  aut_mnth = substr(C020, 1, 6);

  /*** Get TRX hour ***/
  aut_hour = input(substr(C028,1,2),??2.0);
  
  /*** Convert to numbers ***/
  C034N = input(C034,??13.2);  /* Transaction Amounts */
  C065N = input(C065,??10.0);  /* Available Credit    */
  C075N = input(C075,??10.0);  /* Credit Line         */
  C147N = input(C147,??13.2);  /* Cashback Amount     */
  C160N = input(C160,??4.0);   /* Falcon Socre        */

  /*** Bin the TX amounts ***/
  amt_bin = put(C034N,amounts.);

  /*** Bin the SIC codes for PIN checking ***/
  sic_bin = put(C085,$cashSIC.);

  /*** Find first 5 digits of postal code ***/
  pst_cde = substr(C089,1,5);

  /*** Count missing values ***/
  C112_mis = put(C112,$missing.);  /* Auth ID       */
  C118_mis = put(C118,$missing.);  /* Reason Code   */
  C123_mis = put(C123,$missing.);  /* Advice        */
  C124_mis = put(C124,$missing.);  /* Merchant ID   */
  C165_mis = put(C165,$missing.);  /* Merchant Name */
  C188_mis = put(C188,$missing.);  /* Merchant City */

  label
    bin      = 'Account 6-digits'
    aut_day  = 'Auth day of week'
    aut_hour = 'Auth hour'
    aut_mnth = 'Auth month'
    C034N    = 'Amount'
    amt_bin  = 'Binned amount'
    C065N    = 'Available credit'
    C075N    = 'Credit line'
    sic_bin  = 'Binned SIC codes'
    pst_cde  = 'Postal Code'
    C112_mis = 'Binned auth-IDs'
    C118_mis = 'Binned reason-codes'
    C123_mis = 'Binned advice code'
    C147N    = 'Cashback amount'
    C160N    = 'Falcon Score'
    C165_mis = 'Merchant name'
    C188_mis = 'Merchant city'
  ;
run;

/*** Generate indices for by-level processing ***/
proc datasets nolist library=work;
  modify auth;
  index create C047 C085 C098 C064;
run;

************************************************************;

/*** Print out the total number of observations & record lengths ***/
%let dsid = %sysfunc(open(auth));
%let ttlobs = %sysfunc(attrn(&dsid,nobs));
%let rc = %sysfunc(close(&dsid));

*%macro comment;
%doby(auth,bin,tmp,C001);

%let dsid     = %sysfunc(open(tmp));
%let uniqacts = %sysfunc(attrn(&dsid,nobs));
%let rc       = %sysfunc(close(&dsid));

proc datasets nolist library=work;
  delete tmp;
run;
*%mend comment;

data _null_;
  set auth;
  call symput('minlen',minlen);
  call symput('maxlen',maxlen);
run;

*ods proclabel "Account frequencies & record lengths";
data _null_;
  file print;
  put "Total number of records   = &ttlobs";
  put "Number of unique accounts = &uniqacts";
  put "Minimum record length = &minlen";
  put "Maximum record length = &maxlen";
  title 'Account Frequencies & Record Lengths';
run;

************************************************************;

/*** Display random sample of &RanObs records ***/
%let RanObs = 20;

data random (drop=k n);
  retain k &RanObs n;
  if (_n_ = 1) then n = total;
  set auth nobs=total;
  if (ranuni(7993652) <= k / n) then
    do;
      output;
      k = k - 1;
  end;
  n = n - 1;
  if (k = 0) then stop;
run;

*ods proclabel "Random sample of records";
proc print label data=random;
  var C001 C020 C028 C034 C047 C050 C063 C064 C065 C075 C085 C089
      C098 C101 C102 C103 C104 C112 C118 C123 C124 C140 C146 C147
      C160 C164 C165;
  title "A001. Visual inspection: random sample of &RanObs records";
run;

proc datasets nolist library=work;
  delete random;
run;

************************************************************;

/*** Find &BINNobs most populated Account bins ***/
%let BINNobs = 20;

proc freq data=auth order=freq;
  tables bin / out=acct_f noprint;
run;

/*** Save the total number of distinct BINs ***/
%let dsid  = %sysfunc(open(acct_f));
%let nBINs = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc means noprint data=auth;
  var nbin;
  output out=bin_m min=min max=max;
  format nbin bins.;
run;

*ods proclabel "BIN count & extremes";
data _null_;
  set bin_m;
  file print;
  title 'A012B. Extreme values of account number 6-digits';
  put "Number of BINs = &nBINs";
  put "Lowest BIN     = " min;
  put "Highest BIN    = " max;
  format min max z6.;
run;

*ods proclabel "Top &BINNobs BINS";
proc print label data=acct_f (obs=20);
  title "A012A.Top &BINNobs most frequent account number 6-digits";
  format percent 6.2;
run;

proc datasets nolist library=work;
  delete acct_f bin_m;
run;

*ods proclabel "BIN distribution";
proc chart data=auth;
  hbar bin / type=percent missing space=0;
  title 'A012E. Distribution of account number 6-digits';
run;

************************************************************;

proc means noprint data=auth;
  output out=tmp min=min max=max;
  var aut_date;
run;

data _null_;
  set tmp;
  call symput('start',min);
  call symput('finish',max);
run;

proc datasets nolist library=work;
  delete tmp;
run;
 
proc chart data=auth;
  hbar aut_date / type=percent missing midpoints=&start to &finish by 1 space=0;
  title 'A013B. Distribution of transaction dates by day';
run;

proc chart data=auth;
  hbar aut_mnth / type=percent missing space=0;
  title 'A013C. Distribution of transaction dates by month';
run;

proc chart data=auth;
  hbar aut_day / type=percent missing midpoints=1 2 3 4 5 6 7 space=0;
  format aut_day weekdays.;
  title 'A013D. TRX by Day of Week';
run;

proc chart data=auth;
  hbar aut_hour / type=percent missing midpoints=0 to 23 by 1 space=0;
  format aut_hour timernge.;
  title 'A014. TRX by hour of day';
run;

proc means data=auth n nmiss min max mean median std;
  var aut_hour;
  format aut_hour timernge.;
run;

************************************************************;

proc chart data=auth;
  hbar amt_bin / type=percent missing space=0;
  title 'A015A. TRX amounts (binned)';
run;

proc means data=auth n nmiss min max mean median std;
  var C034N;
  title 'A015C. Statistics of TRX amounts';
run;
   
************************************************************;

proc chart data=auth;
  hbar C047 / type=percent missing space=0 descending;
  format C047 $country.;
  title 'A016. Currency code';
run;

proc freq order=freq data=auth;
  tables C050 / list missing;
  title 'A017A. Currency Rate';
run;

proc freq data=auth;
  tables C050 / list missing;
  by C047;
  format C047 $country.;
  title 'A017B. Currency rate by country code';
run;

************************************************************;

%testfield(C063,auth,$decisn.,A018,Decision);

************************************************************;

%testfield(C064,auth,$Atype.,A019,Transaction Type);

proc freq order=freq data=auth;
  tables C085*C064 / missing;
  format C085 $sicfmt.;
  format C064 $Atype.;
  title 'A019A. Transaction type by SIC';
run;

proc freq order=freq data=auth;
  tables C085 / missing noprint out=tmp;
  where C064 = 'C';
  format C085 $sicfmt.;
  format C064 $Atype.;
  title 'A019B. Cash transactions by SIC';
run;

proc print label data=tmp;
  format percent 6.2;
run;

proc datasets nolist library=work;
  delete tmp;
run;

proc freq order=freq data=auth;
  tables C098*C064 / missing;
  format C098 $country.;
  format C064 $Atype.;
  title 'A019C. Transaction type by Merchant Country';
run;

************************************************************;

proc means data=auth n nmiss min max mean median std;
  var C065N;
  title 'A020. Statistics of Available Credit';
run;

proc means data=auth n nmiss min max mean median std;
  var C075N;
  title 'A021. Statistics of Credit Line';
run;

************************************************************;

/*** Find &SICObs most populated SIC codes ***/
%let SICObs = 10;

proc freq data=auth order=freq;
  tables C085 / out=C085f missing noprint;
  format C085 $sicfmt.;
run;

proc print label data=C085f (obs=&SICObs);
  format percent 6.2;
  title "A022. Top &SICObs most frequent SIC codes";
run;

proc datasets nolist library=work;
  delete C085f;
run;

************************************************************;
%let ZIPObs = 50;

/*** Find &ZIPObs most populated Postal codes ***/
proc freq data=auth order=freq;
  tables pst_cde / out=C089f missing noprint;
run;

proc print label data=C089f (obs=&ZIPObs);
  format percent 6.2;
  title "A023. Top &ZIPObs most frequent Postal codes";
run;

proc datasets nolist library=work;
  delete C089f;
run;

************************************************************;
%let CtyObs = 50;

/*** Find &CtyObs most populated Merchant countries ***/
proc freq data=auth order=freq;
  format C098 $country.;
  tables C098 / out=C098f missing noprint;
run;

proc print label data=C098f (obs=&CtyObs);
  format percent 6.2;
  title "A024. Top &CtyObs most frequent Merchant Countries";
run;

proc datasets nolist library=work;
  delete C098f;
run;

************************************************************;

%testfield(C101,auth,$Apin.,A025A,Pin Verification Distribution);

proc chart data=auth;
  hbar C101 / type=percent missing group=sic_bin space=0 descending;
  title 'A025B. Pin verification by SIC code';
  format C101 $Apin.;
run;

proc chart data=auth;
  hbar C101 / type=percent missing group=sic_bin g100 space=0 descending;
  title 'A025B. Pin verification by SIC code (groups total 100%)';
  format C101 $Apin.;
run;

%testfield(C102,auth,$Acvv.,A026A,CVV Overall Distribution);

proc chart data=auth;
  hbar C102 / type=percent missing group=C103 space=0 descending;
  title 'A026B. CVV by key swipe';
  format C102 $Acvv.;
  format C103 $POSmode.;
run;

proc chart data=auth;
  hbar C102 / type=percent missing group=C103 g100 space=0 descending;
  title 'A026C. CVV by key swipe (groups total 100%)';
  format C102 $Acvv.;
  format C103 $POSmode.;
run;

%testfield(C103,auth,$POSmode.,A027,Key/Swipe Overall Distribution);

************************************************************;

proc chart data=auth;
  hbar C104 / type=percent missing space=0;
  title 'A028A. Expiration Date';
run;

************************************************************;

proc chart data=auth;
  hbar C112_mis / type=percent missing space=0 descending;
  title 'A029. Check Auth-Id';
run;

************************************************************;

proc chart data=auth;
  hbar C118_mis / type=percent missing space=0 descending;
  title 'A030. Check Reason Code';
run;

************************************************************;

proc chart data=auth;
  hbar C123_mis / type=percent missing space=0 descending;
  title 'A031. Check Advice';
run;

************************************************************;

/*** Find &MchObs most populated Merchant IDs ***/
proc chart data=auth;
  hbar C124_mis / type=percent missing space=0 descending;
  title 'A032. Merchant IDs';
run;

************************************************************;

/*** Find &TrmObs most populated Term IDs ***/
%let TrmObs = 10;

proc freq data=auth order=freq;
  tables C140 / out=C140f missing noprint;
run;

proc print label data=C140f (obs=&TrmObs);
  format percent 6.2;
  title "A033. Top &TrmObs most frequent Term IDs";
run;

proc datasets nolist library=work;
  delete C140f;
run;

************************************************************;

proc chart data=auth;
  hbar C146 / type=percent missing space=0 descending;
  title 'A034. Which card';
run;

************************************************************;

proc means data=auth n nmiss min max mean median std;
  var C147N;
  title 'A035. Statistics of Cashback Amount';
run;

************************************************************;

proc means data=auth n nmiss min max mean median std;
  var C160N;
  title 'A036. Statistics of Falcon Score';
run;

************************************************************;

proc chart data=auth;
  hbar C165_mis / type=percent missing space=0 descending;
  title 'A038. Merchant-name (name)';
run;

proc chart data=auth;
  hbar C188_mis / type=percent missing space=0 descending;
  title 'A039. Merchant-name (city)';
run;

/*** Find &StaObs most populated Merchant States ***/
%let StaObs = 10;

proc freq data=auth order=freq;
  tables C202 / out=C202f missing noprint;
run;

proc print label data=C202f (obs=&StaObs);
  format percent 6.2;
  title "A040. Top &StaObs most frequent Merchant States";
run;

proc datasets nolist library=work;
  delete C020f;
run;

*ods html close;

endsas;
