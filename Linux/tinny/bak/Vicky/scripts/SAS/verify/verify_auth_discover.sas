/**************************************************************
 FALCON                                                     
	Verify Consortium Auth files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readauth.sas
	10/01/99 	js

**************************************************************/

options 
        nocenter 
        linesize=80 
        pagesize=60 
        obs=max
        errors=1000 
        compress=yes;

/**************************************************************/
/* Assumes that SAS is invoked with -stdio option to allow    */
/* for STDIN and STDOUT filenames                             */
/* Note that this is no longer required (V7.x)                */
/**************************************************************/

/**************************************************************/
/* Formats                                                    */
/**************************************************************/

proc format;
  value am1_fmt
    low - < 0.    = 'Negative'
    0.00          = 0000
    0.01  - 10.   = 0010
    10.01 - 20.   = 0020
    20.01 - 30.   = 0030
    30.01 - 40.   = 0040
    40.01 - 50.   = 0050
    50.01 - 60.   = 0060
    60.01 - 70.   = 0070
    70.01 - 80.   = 0080
    80.01 - 90.   = 0090
    90.01 - 100.  = 0100
    100.01 - 150. = 0150
    150.01 - 200. = 0200
    200.01 - 250. = 0250
    250.01 - 300. = 0300
    300.01 - 350. = 0350
    350.01 - 400. = 0400
    400.01 - 450. = 0450
    450.01 - 500. = 0500
    500.01 - 600. = 0600
    600.01 - 700. = 0700
    700.01 - 800. = 0800
    800.01 - 900. = 0900
    900.01 - 1000. = 1000
    1000.01 - 2000. = 2000
    2000.01 - 3000. = 3000
    3000.01 - 4000. = 4000
    4000.01 - 5000. = 5000
    5000.01 - high = '5000++'
  ;

proc format;
  value $sic_fmt
    '6011' = '6011'
    '6010' = '6010'
    other  = 'Other'
  ;

proc format;
  value $miss_fmt
    ' ' = 'Blank'
    other = 'OK'
  ;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data auth;

  infile STDIN
  lrecl=229
  missover;

  input
    @1   C001       $19.      /* ACCOUNT ID */
    @20  C020       $8.       /* DATE */
    @28  C028       $6.       /* TIME */
    @34  C034       $13.      /* AMOUNT */
    @47  C047       $3.       /* CURRENCY CODE */
    @50  C050       $13.      /* CURRENCY RATE */
    @63  C063       $1.       /* DECISION */
    @64  C064       $1.       /* TRANSACTION TYPE */
    @65  C065       $10.      /* AVAILABLE CREDIT */
    @75  C075       $10.      /* CREDIT LINE */
    @85  C085       $4.       /* SIC CODE */
    @89  C089       $9.       /* POSTAL CODE */
    @98  C098       $3.       /* MERCHANT COUNTRY */
    @101 C101       $1.       /* PIN VERIFICATION */
    @102 C102       $1.       /* CVV */
    @103 C103       $1.       /* KEY SWIPE */
    @104 C104       $8.	      /* EXPIRATION DATE */
    @112 C112       $6.	      /* AUTHORIZATION ID */
    @118 C118       $5.       /* REASON CODE */
    @123 C123       $1.       /* ASSOC ADVICE */
    @124 C124       $16.      /* MERCHANT ID */
    @140 C140       $6.       /* TERMINAL ID */
    @146 C146       $1.       /* WHICH CARD */
    @147 C147       $13.      /* CASH BACK AMOUNT */
    @160 C160       $4.       /* FALCON SCORE */
    @164 C164       $1.       /* Filler */
    @165 C165       $30.      /* Merchant name */
    @195 C195       $13.      /* Merchant city */
    @208 C208       $3.       /* Merchant state/cntry */
    @211 C211       $1.       /* CVV2 Present */
    @212 C212       $1.       /* CVV2 Response */
    @213 C213       $1.       /* AVS Response */
    @214 C214       $1.       /* Tran Category */
    @215 C215       $1.       /* Fraud tran  */
    @216 C216       $8.	      /* ACCOUNT EXPIRATION DATE */
    @224 C224       $2.       /* FALCON Reason code_1 */
    @226 C226       $2.       /* FALCON Reason code_2 */
    @228 C228       $2.       /* FALCON Reason code_3 */
   ;

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
    C165 = 'MERCHANT-NAME'
    C195 = 'MERCHANT-CITY'
    C208 = 'MERCHANT-STATE'
    C211 = 'CVV2-PRESENT'
    C212 = 'CVV2-RESPONSE'
    C213 = 'AVS-RESPONSE'
    C214 = 'TRAN-CATEGORY'
    C215 = 'FRAUD-TRAN'
    C216 = 'ACCT-EXPIRE-DATE'
    C224 = 'FALCON-REASON-CODE_1'
    C226 = 'FALCON-REASON-CODE_2'
    C228 = 'FALCON-REASON-CODE_3'
  ;

  if (C064 eq " ") then C064 = 'X';

  if (_error_) then
    do;
      file STDERR;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

  /************************************************************/
  /* Data Analysis                                            */
  /************************************************************/

  /*** For merchant ID processing ***/
  splitvar = rank(substr(C124,1,1));

  /*** Bin the accounts ***/
  length acct_bin $6;
  acct_bin = substr(C001,1,6);

  /*** Calculate TRX day-of-week ***/
  format aut_date date7.;
  aut_date = input(C020, yymmdd8.);
  aut_day   = weekday(aut_date);

  /*** Cut out year+month part of date ***/
  aut_mnth = substr(C020, 1, 6);

  /*** Get TRX hour ***/
  aut_hour = substr(C028,1,2);
  
  /*** Convert amounts to numbers ***/
  C034N = C034*1.;

  /*** Bin the TX amounts ***/
  amt_bin = put(C034N,am1_fmt.);

  /*** Convert avail credit to numbers ***/
  C065N = C065*1.;

  /*** Convert credit line to numbers ***/
  C075N = C075*1.;

  /*** Bin the SIC codes for PIN checking ***/
  sic_bin = put(C085,$sic_fmt.);


  /*** Find first 5 digits of postal code ***/
  pst_cde = substr(C089,1,5);

  /*** Friendly name for C103 ***/
  key_swp = C103;

  /*** Convert the expiration date to numeric ***/
  /* C104N = C104*1.;  */

  /*** Count number of missing values in Auth-ID ***/
  C112_mis = put(C112,$miss_fmt.);

  /*** Count number of missing values in Reason-code ***/
  C118_mis = put(C118,$miss_fmt.);

  /*** Count number of missing values in Advice ***/
  C123_mis = put(C123,$miss_fmt.);

  /*** Convert cashback to numbers ***/
  C147N = C147*1.;

  /*** Convert Falcon Score to numbers ***/
  C160N = C160*1.;

  /*** Count number of missing values in Merchant Name ***/
  C165_mis = put(C165,$miss_fmt.);

  /*** Count number of missing values in Merchant City ***/
  C195_mis = put(C195,$miss_fmt.);

  label
    acct_bin = 'Account 6 digits'
    aut_day = 'Auth day of week'
    aut_hour = 'Auth hour'
    aut_mnth = 'Auth month'
    C034N = 'Amount'
    amt_bin = 'Binned amount'
    C065N = 'Available credit'
    C075N = 'Credit line'
    sic_bin = 'Binned SIC codes'
    pst_cde = 'Postal Code'
    C104N = 'Expiration date'
    C112_mis = 'Binned auth-IDs'
    C118_mis = 'Binned reaon-codes'
    C123_mis = 'Binned advice code'
    C147N = 'Cashback amount'
    C160N = 'Falcon Score'
    C165_mis = 'Merchant name'
    C195_mis = 'Merchant city'
    C216N = 'Account Expiration Date'
  ;
run;

/* Save the total number of observations */
%let dsid = %sysfunc(open(auth));
%let ttlobs = %sysfunc(attrn(&dsid,nobs));
%let rc = %sysfunc(close(&dsid));

/**************************************************************/
/* Define macros for processing merchant IDs                  */
/**************************************************************/

options mprint;

%macro doby(inset,what,outset);

/* Split &inset by &what into pieces.
The pieces will be stored in tmp datasets, numbered consecutively.
Process each piece, then reassemble the resulting pieces into &outset.
*/

/*
x echo Tallying &what;
*/

proc freq
  data=&inset
  noprint;
  tables &what /
    out = freqout;
run;

/* Determine the number of levels for &what */
%let dsid = %sysfunc(open(freqout));
%let npiece = %sysfunc(attrn(&dsid,nobs));
%let rc = %sysfunc(close(&dsid));

/* Generate SAS code for inclusion in later data step processing */
data _null_;
  file "doby.inc";
  set freqout;
  _piece = "x"||left(_n_);
  put "if &what = '" &what "' then do; output " _piece "; return; end;";
run;

/*
x echo Splitting by &what;
*/

/* Split into smaller tmp data sets */
data
  %do count = 1 %to &npiece;
    x&count
  %end;
  ;
  set &inset;
  %include "doby.inc";
run;

x /bin/rm doby.inc;

/* Processing each tmp data set */
%do count = 1 %to &npiece;
  data _null_;
    set freqout;
    if _n_ = &count then
      do;
        call symput('this', &what);
        call symput('nobs', count);
      end;
  run;

/*
  x echo Processing &what=&this \(&nobs obs \) `date`;
*/

  title &what=&this;

  /* Call to job specific code */
  %do_one(x&count);

  title;
%end;

/*
x echo Collecting pieces;
*/

data &outset;
  set
  %do count = 1 %to &npiece;
    f&count
  %end;
  ;
run;

proc sort
  data=&outset;
  by descending count;
run;

/* Clean-up SAS tmp */
proc datasets
  library=work
  nolist;
  delete
  %do count = 1 %to &npiece;
    f&count
  %end;
  ;
run;

%mend doby;

************************************************************;

%macro do_one(dset);

proc freq
  data=&dset
  order=freq
  noprint;
  tables C124 /
    list
    missing
    out=f&count
  ;
run;

/* Clean-up SAS tmp */
proc datasets
  library=work
  nolist;
  delete x&count;
run;

/* Only keep top 10 values */
data f&count;
  set f&count(obs=10);
  percent = count / &ttlobs * 100;
run;

%mend do_one;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set auth
    nobs=total;
  if (ranuni(7993652) <= k / n) then
    do;
      output;
      k = k - 1;
    end;
  n = n - 1;
  if (k = 0) then stop;
run;

/**************************************************************/
/* Show Results                                               */
/**************************************************************/

/* Show some samples */
proc print
  label
  data=random;
  var C001 C020 C028 C034 C047 C050 C063 C064 C065 C075 C085 C089
      C098 C101 C102 C103 C104 C112 C118 C123 C124 C140 C146 C147
      C160 C164 C165 C195 C208 C211 C212 C213 C214 C215 C216 C224 
      C226 C228;
  title 'A001. Visual inspection: random sample of 20 records';
run;

/* Remove temporary dataset */
proc datasets
  nolist
  library=work;
  delete random;
run;

/*** Find 20 most populated Account bins ***/
proc freq
  data=auth
  order=freq;
  tables acct_bin / out=acct_f noprint;
run;

proc print
  label
  data=acct_f (obs=20);
  title 'A012A. Most frequent account number 6 digits';
run;

proc chart
  data=auth;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;


proc chart
  data=auth;
  hbar C020 /type=percent missing;
  title 'A013B. Distribution of transaction dates by day';
run;

proc chart
  data=auth;
  hbar aut_mnth /type=percent missing;
  title 'A013C. Distribution of transaction dates by month';
run;

proc chart
  data=auth;
  hbar aut_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A013D. TRX by Day of Week';
run;

proc chart
  data=auth;
  hbar aut_hour /type=percent missing;
  title 'A014. TRX by hour of day';
run;

proc chart
  data=auth;
  hbar amt_bin /type=percent missing;
  title 'A015A. TRX amounts (binned)';
run;

proc means
  data=auth
  n nmiss min max mean median std;
  var C034N;
  title 'A015C. Statistics of TRX amounts';
run;
   
proc chart
  data=auth;
  hbar C047 /type=percent missing;
  title 'A016. Currency code';
run;

proc freq
  data=auth;
  tables C050 C047*C050;
  title 'A017. Currency rate by country code';
run;

proc chart
  data=auth;
  hbar C063 /type=percent missing;
  title 'A018. Decision';
run;

proc chart
  data=auth;
  hbar C064 /type=percent missing;
  title 'A019. Transaction type';
run;

/*** Find 20 most populated SIC codes ***/
proc freq
  data=auth
  order=freq;
  tables C085 / out=C085f noprint;
run;

proc print
  label
  data=C085f (obs=20);
  title 'A019X. Most frequent SIC codes';
run;

proc freq
  data=auth;
  tables C085 / list missing;
  title 'A019Y. All SIC codes';
run;

proc freq
  data=auth;
  tables C085*C064;
  title 'A019A. Transaction type by SIC';
run;

proc freq
  data=auth;
  tables C098*C064;
  title 'A019B. Transaction type by Merchant Country';
run;

proc freq
  order=freq
  data=auth;
  tables C085 / list missing;
  where C064 = 'C';
  title 'A019C. Cash transactions by SIC';
run;

proc means
  data=auth
  n nmiss min max mean median std;
  var C065N;
  title 'A020. Statistics of Available Credit';
run;

proc means
  data=auth
  n nmiss min max mean median std;
  var C075N;
  title 'A021. Statistics of Credit Line';
run;

/*** Find 50 most populated Postal codes ***/
proc freq
  data=auth
  order=freq;
  tables pst_cde / out=C089f noprint;
run;

proc print
  label
  data=C089f (obs=50);
  title 'A023. Most frequent Postal codes';
run;

/*** Find 20 most populated Merchant countries ***/
proc freq
  data=auth
  order=freq;
  tables C098 / out=C098f noprint;
run;

proc print
  label
  data=C098f (obs=20);
  title 'A024. Most frequent Merchant Countries';
run;

proc chart
  data=auth;
  hbar C101 /type=percent missing;
  title 'A025A. Pin verification distribution';
run;

proc chart
  data=auth;
  hbar C101 /type=percent missing group=sic_bin;
  title 'A025B. Pin verification by SIC code';
run;

proc chart
  data=auth;
  hbar C102 /type=percent missing;
  title 'A026A. CVV overall distribution';
run;

proc chart
  data=auth;
  hbar C102 /type=percent missing group=key_swp;
  title 'A026B. CVV by key swipe.';
run;

proc chart
  data=auth;
  hbar C103 /type=percent missing;
  title 'A027. Key/Swipe overall distribution';
run;

/* proc means
  data=auth
  n nmiss min max;
  var C104N;
  title 'A028. Statistics of Expiration Date';
run; */

proc chart
  data=auth;
  hbar C104 /type=percent missing;
  title 'A028A. Expiration Date';
run;

proc chart
  data=auth;
  hbar C112_mis /type=percent missing;
  title 'A029. Check Auth-Id';
run;

/*** Find 20 most populated Reason Codes ***/
proc freq
  data=auth
  order=freq;
  tables C118 / out=C118f noprint;
run;

proc print
  label
  data=C118f (obs=20);
  title 'A030A. Most frequent Reason Codes';
run;

proc chart
  data=auth;
  hbar C118_mis /type=percent missing;
  title 'A030B. Check Reason Code';
run;

proc chart
  data=auth;
  hbar C123_mis /type=percent missing;
  title 'A031. Check Advice';
run;

/*** Find 20 most populated Merchant IDs ***/
%doby(auth,splitvar,C124f);

proc print
  label
  data=C124f (obs=20);
  title 'A032. Most frequent Merchant IDs';
run;

/*** Find 10 most populated Term IDs ***/
proc freq
  data=auth
  order=freq;
  tables C140 / out=C140f noprint;
run;

proc print
  label
  data=C140f (obs=10);
  title 'A033. Most frequent Term IDs';
run;

proc chart
  data=auth;
  hbar C146 /type=percent missing;
  title 'A034. Which card';
run;

proc means
  data=auth
  n nmiss min max mean median std;
  var C147N;
  title 'A035. Statistics of Cashback Amount';
run;

proc means
  data=auth
  n nmiss min max mean median std;
  var C160N;
  title 'A036. Statistics of Falcon Score';
run;

/*** Find 50 most populated Merchant Names ***/
proc freq
  data=auth
  order=freq;
  tables C165 / out=C165f noprint;
run;

proc print
  label
  data=C165f (obs=50);
  title 'A038A. Most frequent Merchant Names';
run;

/*******
proc chart
  data=auth;
  hbar C165 /type=percent missing;
  title 'A038B. Merchant-name';
run;
*******/

/*** Find 50 most populated Merchant Cities ***/
proc freq
  data=auth
  order=freq;
  tables C195 / out=C195f noprint;
run;

proc print
  label
  data=C195f (obs=50);
  title 'A039A. Most frequent Merchant Cities';
run;

/*****************
proc chart
  data=auth;
  hbar C195 /type=percent missing;
  title 'A039B. Merchant city';
run;
*******************/

/*** Find 20 most populated Merchant States ***/
proc freq
  data=auth
  order=freq;
  tables C208 / out=C208f noprint;
run;

proc print
  label
  data=C208f (obs=20);
  title 'A040. Most frequent Merchant States';
run;

proc chart
  data=auth;
  hbar C211 /type=percent missing;
  title 'A041. CVV2 Present';
run;

proc chart
  data=auth;
  hbar C212 /type=percent missing;
  title 'A042. CVV2 Response';
run;

proc chart
  data=auth;
  hbar C213 /type=percent missing;
  title 'A043. AVS Response';
run;

proc chart
  data=auth;
  hbar C214 /type=percent missing;
  title 'A044. Tran Category';
run;

proc chart
  data=auth;
  hbar C215 /type=percent missing;
  title 'A045. Fraud Tran';
run;

proc chart
  data=auth;
  hbar C216 /type=percent missing;
  title 'A046. Account Expiration Date';
run;

proc chart
  data=auth;
  hbar C224 /type=percent missing;
  title 'A047a. FALCON REASON CODE_1';
run;

proc chart
  data=auth;
  hbar C226 /type=percent missing;
  title 'A047b. FALCON REASON CODE_2';
run;

proc chart
  data=auth;
  hbar C228 /type=percent missing;
  title 'A047c. FALCON REASON CODE_3';
run;

endsas;
