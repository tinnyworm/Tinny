/**************************************************************
 FALCON                                                     
	Verify Consortium Post files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readauth.sas
	10/05/99 	js

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

data post;

  infile STDIN
  lrecl=204
  missover;

  input
    @1   C001       $19.      /* ACCOUNT ID */
    @20  C020       $13.      /* AMOUNT */
    @33  C033       $3.       /* CURRENCY CODE */
    @36  C036       $13.      /* CURRENCY RATE */
    @49  C049       $8.       /* POST DATE */
    @57  C057       $8.       /* TRAN DATE */
    @65  C065       $6.       /* TRAN TIME */
    @71  C071       $10.      /* AVAILABLE CREDIT */
    @81  C081       $10.      /* CREDIT LINE */
    @91  C091       $1.       /* TRANSACTION TYPE */
    @92  C092       $1.       /* AUTHED */
    @93  C093       $4.       /* SIC CODE */
    @97  C097       $9.       /* POSTAL CODE */
    @106 C106       $3.       /* MERCHANT COUNTRY */
    @109 C109       $6.       /* AUTH ID */
    @115 C115       $6.       /* POST ID */
    @121 C121       $16.      /* MERCHANT ID */
    @137 C137       $6.       /* TERMINAL ID */
    @143 C143       $1.       /* WHICH CARD */
    @144 C144       $13.      /* CASH BACK AMOUNT */
    @157 C157       $4.       /* FALCON SCORE */
    @161 C161       $1.       /* Filler */
    @162 C162       $22.      /* Merchant name (name) */
    @185 C185       $13.      /* Merchant name (city) */
    @199 C199       $3.       /* Merchant name (state/cntry) */
  ;

  label
    C001 = 'ACCT-ID'
    C020 = 'AMOUNT'
    C033 = 'CURR-CODE'
    C036 = 'CURR-RATE'
    C049 = 'POST-DATE'
    C057 = 'TRAN-DATE'
    C065 = 'TRAN-TIME'
    C071 = 'AVAIL-CRED'
    C081 = 'CRED-LINE'
    C091 = 'TRAN-TYPE'
    C092 = 'AUTHED'
    C093 = 'SIC'
    C097 = 'POSTAL-CODE'
    C106 = 'MRCH-CNTRY'
    C109 = 'AUTH-ID'
    C115 = 'POST-ID'
    C121 = 'MRCH-ID'
    C137 = 'TERM-ID'
    C143 = 'WHICH-CARD'
    C144 = 'CASHBACK-AMT'
    C157 = 'FALCON-SCORE'
    C161 = 'FILLER'
    C162 = 'MERCHANT-NAME (name)'
    C185 = 'MERCHANT-NAME (city)'
    C199 = 'MERCHANT-NAME (state)'
  ;

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

  /*** Bin the accounts ***/
  length acct_bin $6;
  acct_bin = substr(C001,1,6);

  /*** Convert amounts to numbers ***/
  C020N = C020*1.;

  /*** Bin the TX amounts ***/
  amt_bin = put(C020N,am1_fmt.);

  /*** Cut out year+month part of post date ***/
  pst_mnth = substr(C049, 1, 6);

  /*** Calculate TRX POST day-of-week ***/
  format post_date date7.;
  post_date = input(C049, yymmdd8.);
  post_day   = weekday(post_date);

  /*** Calculate TRX TRAN day-of-week ***/
  format tran_date date7.;
  tran_date = input(C057, yymmdd8.);
  tran_day   = weekday(tran_date);

  /*** Get TRX hour ***/
  tran_hour = substr(C065,1,2);
  
  /*** Convert avail credit to numbers ***/
  C071N = C071*1.;

  /*** Convert credit line to numbers ***/
  C081N = C081*1.;

  /*** Count number of missing values in Auth-ID ***/
  C109_mis = put(C109,$miss_fmt.);

  /*** Count number of missing values in Post-ID ***/
  C115_mis = put(C115,$miss_fmt.);

  /*** Count number of missing values in Term-ID ***/
  C137_mis = put(C137,$miss_fmt.);

  /*** Convert cashback to numbers ***/
  C144N = C144*1.;

  /*** Convert Falcon Score to numbers ***/
  C157N = C157*1.;

  /*** Count number of missing values in Merchant Name ***/
  C162_mis = put(C162,$miss_fmt.);

  /*** Count number of missing values in Merchant City ***/
  C185_mis = put(C185,$miss_fmt.);

  label
    acct_bin = 'Account 6 digits'
    C020N = 'Amount'
    amt_bin = 'Binned amount'
    post_day = 'Post date (day of week)'
    tran_day = 'Transaction date (day of week)'
    tran_hour = 'Transaction hour'
    C071N = 'Available credit'
    C081N = 'Credit line'
    C109_mis = 'Auth ID category'
    C115_mis = 'Post ID cetegory'
    C137_mis = 'Term ID category'
    C144N = 'Cashback amount'
    C157N = 'Falcon score'
    C162_mis = 'Merchant name category'
    C185_mis = 'Merchant city category'
  ;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set post
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
  var C001 C020 C033 C036 C049 C057 C065 C071 C081 C091 C092 C093
      C097 C106 C109 C115 C121 C137 C143 C144 C157 C161 C162;
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
  data=post
  order=freq;
  tables acct_bin / out=acct_f noprint;
run;

proc print
  label
  data=acct_f (obs=20);
  title 'A012A. Most frequent account number 6 digits';
run;

proc chart
  data=post;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;

proc chart
  data=post;
  hbar amt_bin /type=percent missing;
  title 'A013A. TRX amounts (binned)';
run;

proc means
  data=post
  n nmiss min max mean median std;
  var C020N;
  title 'A013C. Statistics of TRX amounts';
run;
   
proc chart
  data=post;
  hbar C033 /type=percent missing;
  title 'A014. Currency code';
run;

proc freq
  data=post;
  tables C036 C033*C036;
  title 'A015. Currency rate by country code';
run;

proc chart
  data=post;
  hbar C049 /type=percent missing;
  title 'A016B. Distribution of transaction POST dates';
run;

proc chart
  data=post;
  hbar pst_mnth /type=percent missing;
  title 'A016C. Distribution of transaction POST months';
run;

proc chart
  data=post;
  hbar post_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A016D. TRX POST by Day of Week';
run;

proc chart
  data=post;
  hbar C057 /type=percent missing;
  title 'A017B. Distribution of transaction TRAN dates';
run;

proc chart
  data=post;
  hbar tran_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A017D. TRX TRAN by Day of Week';
run;

proc chart
  data=post;
  hbar tran_hour /type=percent missing;
  title 'A018. TRX by hour of day';
run;

proc means
  data=post
  n nmiss min max mean;
  var C071N;
  title 'A019. Statistics of Available Credit';
run;

proc means
  data=post
  n nmiss min max mean;
  var C081N;
  title 'A020. Statistics of Credit Line';
run;

proc chart
  data=post;
  hbar C091 /type=percent missing;
  title 'A021. Transaction type';
run;

proc chart
  data=post;
  hbar C092 /type=percent missing;
  title 'A022. AUTHED decision';
run;

/*** Find 15 most populated SIC codes ***/
proc freq
  data=post
  order=freq;
  tables C093 / out=C093f noprint;
run;

proc print
  label
  data=C093f (obs=15);
  title 'A023. Most frequent SIC codes';
run;

/*** Find 15 most populated SIC codes (for cash transactions) ***/
proc freq
  data=post
  order=freq;
  tables C093 / out=C093f noprint;
  where C091 = 'C';
run;

proc print
  label
  data=C093f (obs=15);
  title 'A023. Most frequent SIC codes (type = C)';
run;

/*** Find 20 most populated Postal codes ***/
proc freq
  data=post
  order=freq;
  tables C097 / out=C097f noprint;
run;

proc print
  label
  data=C097f (obs=20);
  title 'A024. Most frequent Postal codes';
run;

/*** Find 10 most populated Merchant countries ***/
proc freq
  data=post
  order=freq;
  tables C106 / out=C106f noprint;
run;

proc print
  label
  data=C106f (obs=10);
  title 'A025. Most frequent Merchant Countries';
run;

proc chart
  data=post;
  hbar C109_mis /type=percent missing;
  title 'A026. Check Auth-Id';
run;

proc chart
  data=post;
  hbar C115_mis /type=percent missing;
  title 'A027. Check Post-Id';
run;

/*** Find 10 most populated Merchant IDs ***/
proc freq
  data=post
  order=freq;
  tables C121 / out=C121f noprint;
run;

proc print
  label
  data=C121f (obs=10);
  title 'A028. Most frequent Merchant IDs';
run;

proc chart
  data=post;
  hbar C137_mis /type=percent missing;
  title 'A029. Check Term-Id';
run;

proc chart
  data=post;
  hbar C143 /type=percent missing;
  title 'A030. Which card';
run;

proc means
  data=post
  n nmiss min max mean;
  var C144N;
  title 'A031. Statistics of Cashback Amount';
run;

proc means
  data=post
  n nmiss min max mean;
  var C157N;
  title 'A032. Statistics of Falcon Score';
run;

proc chart
  data=post;
  hbar C162_mis /type=percent missing;
  title 'A034. Merchant-name (name)';
run;

proc chart
  data=post;
  hbar C185_mis /type=percent missing;
  title 'A035. Merchant-name (City)';
run;

/*** Find 10 most populated Merchant States ***/
proc freq
  data=post
  order=freq;
  tables C199 / out=C199f noprint;
run;

proc print
  label
  data=C199f (obs=10);
  title 'A036. Most frequent Merchant States';
run;

endsas;
