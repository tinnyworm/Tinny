/**************************************************************
 FALCON                                                     
	Verify Consortium Post files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readauth.sas
	10/08/99 	jas

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
    100.01 - 200. = 0200
    200.01 - 300. = 0300
    300.01 - 400. = 0400
    400.01 - 500. = 0500
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

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data pay;

  infile STDIN
  lrecl=108
  missover;

  input
    @1   C001       $19.      /* ACCOUNT ID */
    @20  C020       $13.      /* AMOUNT */
    @33  C033       $13.      /* MIN DUE */
    @46  C046       $13.      /* FILLER */
    @59  C059       $13.      /* BALANCE */
    @72  C072       $10.      /* CREDIT LINE */
    @82  C082       $8.       /* DATE */
    @90  C090       $8.       /* DUE DATE */
    @98  C098       $8.       /* STATE DATE */
    @106 C106       $1.       /* REVERSAL */
    @107 C107       $1.       /* PAYMENT METHOD */
    @108 C108       $1.       /* PAYMENT TYPE */
  ;

  label
    C001 = 'ACCT-ID'
    C020 = 'AMOUNT'
    C033 = 'MIN-DUE'
    C046 = 'FILLER'
    C059 = 'BALANCE'
    C072 = 'CRED-LINE'
    C082 = 'DATE'
    C090 = 'DUE-DATE'
    C098 = 'STATE-DATE'
    C106 = 'REVERSAL'
    C107 = 'PAYMENT-METH'
    C108 = 'PAYMENT-TYPE'
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

  /*** Convert min-due amounts to numbers ***/
  C033N = C033*1.;

  /*** Bin the min-due amounts ***/
  mamt_bin = put(C033N,am1_fmt.);

  /*** Convert balance amounts to numbers ***/
  C059N = C059*1.;

  /*** Bin the balance amounts ***/
  bamt_bin = put(C059N,am1_fmt.);

  /*** Convert cred-line amounts to numbers ***/
  C072N = C072*1.;

  /*** Bin the credit line amounts ***/
  camt_bin = put(C072N,am1_fmt.);

  /*** Calculate pay date day-of-week ***/
  format pay_date date7.;
  pay_date = input(C082, yymmdd8.);
  pay_day   = weekday(pay_date);

  label
    acct_bin = 'Account numbers 6 Digits'
    C020N = 'Pay amount'
    amt_bin = 'Binned pay amount'
    C033N = 'Minimum due'
    mamt_bin = 'Binned minimum due amount'
    C059N = 'Balance'
    bamt_bin = 'Binned balance amount'
    C072N = 'Credit line'
    camt_bin = 'Binned credit line'
    pay_day = 'Payment date day of week'
  ;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set pay
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
  var C001 C020 C033 C046 C059 C072 C082 C090 C098 C106 C107 C108;
  title 'A001. Visual inspection: random sample of 20 records';
run;

/* Remove temporary dataset */
proc datasets
  nolist
  library=work;
  delete random;
run;

/* Account numbers */
/**********************
proc chart
  data=pay;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;
***************************/

/* Pay amount */
proc chart
  data=pay;
  hbar amt_bin /type=percent missing;
  title 'A013A. TRX amounts (binned)';
run;

proc means
  data=pay
  n nmiss min max mean median std;
  var C020N;
  title 'A013C. Statistics of TRX amounts';
run;
   
/* Pay min due */
proc chart
  data=pay;
  hbar mamt_bin /type=percent missing;
  title 'A014A. MIN-DUE amounts (binned)';
run;

proc means
  data=pay
  n nmiss min max mean median std;
  var C033N;
  title 'A014C. Statistics of MIN-DUE amounts';
run;
   
/* Balance */
proc chart
  data=pay;
  hbar bamt_bin /type=percent missing;
  title 'A016A. BALANCE amounts (binned)';
run;

proc means
  data=pay
  n nmiss min max mean median std;
  var C059N;
  title 'A016C. Statistics of BALANCE amounts';
run;
   
/* Cred Line */
proc chart
  data=pay;
  hbar camt_bin /type=percent missing;
  title 'A017A. CRED-LINE amounts (binned)';
run;

proc means
  data=pay
  n nmiss min max mean median std;
  var C072N;
  title 'A017C. Statistics of CRED-LINE amounts';
run;
   
/* Pay date */
proc chart
  data=pay;
  hbar C082 /type=percent missing;
  title 'A018B. Distribution of PAY DATE';
run;

proc chart
  data=pay;
  hbar pay_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A018D. PAY DATE by Day of Week';
run;

/* Due date */
proc chart
  data=pay;
  hbar C090 /type=percent missing;
  title 'A019B. Distribution of DUE DATE';
run;

/* State date */
proc chart
  data=pay;
  hbar C098 /type=percent missing;
  title 'A020B. Distribution of STATE DATE';
run;

/* Reversal */
proc chart
  data=pay;
  hbar C106 /type=percent missing;
  title 'A021. Distribution of REVERSAL';
run;

/* Method */
proc chart
  data=pay;
  hbar C107 /type=percent missing;
  title 'A022. Distribution of Payment METHOD';
run;

/* Type */
proc chart
  data=pay;
  hbar C108 /type=percent missing;
  title 'A023. Distribution of Payment TYPE';
run;

endsas;
