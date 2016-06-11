/**************************************************************
 FALCON                                                     
	Verify Consortium Deposit files
	(Fraud Consortium Debit Data Spec. 3.10)
                                                            
 	Modified from verify_pay.sas
	02/29/00 	jag

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

data deposit;

  infile STDIN
  lrecl=108
  missover;

  input
    @1   C001       $19.      /* ACCOUNT ID */
    @20  C020       $13.      /* AMOUNT */
    @33  C033       $13.      /* FILLER */
    @46  C046       $13.      /* FILLER */
    @59  C059       $13.      /* BALANCE */
    @72  C072       $10.      /* TOTAL BALANCE */
    @82  C082       $8.       /* DATE */
    @90  C090       $8.       /* FILLER */  
    @98  C098       $8.       /* STATE DATE */
    @106 C106       $1.       /* REVERSAL */
    @107 C107       $1.       /* DEPOSIT METHOD */
    @108 C108       $1.       /* DEPOSIT TYPE */
  ;

  label
    C001 = 'ACCT-ID'
    C020 = 'AMOUNT'
    C033 = 'FILLER'
    C046 = 'FILLER'
    C059 = 'BALANCE'
    C072 = 'TOT-BALANCE'
    C082 = 'DATE'
    C090 = 'FILLER'
    C098 = 'STATE-DATE'
    C106 = 'REVERSAL'
    C107 = 'DEPOSIT-METH'
    C108 = 'DEPOSIT-TYPE'
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

  /*** Convert balance amounts to numbers ***/
  C059N = C059*1.;

  /*** Bin the balance amounts ***/
  bamt_bin = put(C059N,am1_fmt.);

  /*** Convert balance amounts to numbers ***/
  C072N = C072*1.;

  /*** Bin the balance amounts ***/
  camt_bin = put(C072N,am1_fmt.);

  /*** Calculate deposit date day-of-week ***/
  format dep_date date7.;
  dep_date = input(C082, yymmdd8.);
  dep_day   = weekday(dep_date);

  label
    acct_bin = 'Account numbers 6 Digits'
    C020N = 'Dep amount'
    amt_bin = 'Binned dep amount'
    C059N = 'Balance'
    bamt_bin = 'Binned balance amount'
    C072N = 'Total Balance'
    camt_bin = 'Binned total balance'
    dep_day = 'Deposit date day of week'
  ;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set deposit
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
proc chart
  data=deposit;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;

/* Deposit amount */
proc chart
  data=deposit;
  hbar amt_bin /type=percent missing;
  title 'A013A. TRX amounts (binned)';
run;

proc means
  data=deposit
  n nmiss min max mean median std;
  var C020N;
  title 'A013C. Statistics of TRX amounts';
run;
   
/* Balance */
proc chart
  data=deposit;
  hbar bamt_bin /type=percent missing;
  title 'A016A. BALANCE amounts (binned)';
run;

proc means
  data=deposit
  n nmiss min max mean median std;
  var C059N;
  title 'A016C. Statistics of BALANCE amounts';
run;
   
/* Cred Line */
proc chart
  data=deposit;
  hbar camt_bin /type=percent missing;
  title 'A017A. TOTAL-BALANCE amounts (binned)';
run;

proc means
  data=deposit
  n nmiss min max mean median std;
  var C072N;
  title 'A017C. Statistics of TOTAL-BALANCE amounts';
run;
   
/* Deposit date */
proc chart
  data=deposit;
  hbar C082 /type=percent missing;
  title 'A018B. Distribution of DEP DATE';
run;

proc chart
  data=deposit;
  hbar dep_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A018D. DEP DATE by Day of Week';
run;

/* State date */
proc chart
  data=deposit;
  hbar C098 /type=percent missing;
  title 'A020B. Distribution of STATE DATE';
run;

/* Reversal */
proc chart
  data=deposit;
  hbar C106 /type=percent missing;
  title 'A021. Distribution of REVERSAL';
run;

/* Method */
proc chart
  data=deposit;
  hbar C107 /type=percent missing;
  title 'A022. Distribution of Deposit METHOD';
run;

/* Type */
proc chart
  data=deposit;
  hbar C108 /type=percent missing;
  title 'A023. Distribution of Deposit TYPE';
run;

endsas;
