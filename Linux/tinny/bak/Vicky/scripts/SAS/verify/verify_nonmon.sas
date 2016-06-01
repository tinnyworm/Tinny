/**************************************************************
 FALCON                                                     
	Verify Consortium Fraud files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readfraud.sas
	9/29/99 	js

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
/* Input                                                      */
/**************************************************************/

data nonmon;

  infile STDIN
	lrecl = 47
	missover;

  input
     @1   C001  $19.     /* ACCOUNT ID */
     @20  C020  $1.      /* TRAN TYPE */
     @21  C021  $8.      /* TRAN DATE */
     @29  C029  $9.      /* POSTAL CODE */
     @38  C038  $10.	 /* CRED LINE */
  ;

  label
     C001 = 'ACCT-ID'
     C020 = 'TRAN-TYPE'
     C021 = 'TRAN-DATE'
     C029 = 'POSTAL-CODE'
     C038 = 'CRED-LINE'
  ;

  if (_error_) then
   do;
      file print;
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

  /*** Get year+month ***/
  nm_mnth = substr(C021,1,6);

  /*** Calculate tran day-of-week ***/
  format nm_date date7.;
  nm_date = input(C021, yymmdd8.);
  nm_day   = weekday(nm_date);
  
  /*** Convert credit line to numbers ***/
  C038N = C038*1.;

  label
    acct_bin = 'Account 6 digits'
    nm_mnth = 'Date (year and month)'
    nm_day = 'Date (day of week)'
    C038N = 'Credit line'
  ;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set nonmon
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
  var C001 C020 C021 C029 C038;
  title 'A001. Visual inspection: random sample of 20 records';
run;

/* Remove temporary dataset */
proc datasets
  nolist
  library=work;
  delete random;
run;

proc chart
  data=nonmon;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';

proc chart
  data=nonmon;
  hbar C020 /type=percent missing;
  title 'A013. Fraud types';

proc chart
  data=nonmon;
  hbar nm_mnth /type=percent missing;
  title 'A014B. Month of Transaction';

proc chart
  data=nonmon;
  hbar nm_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A014C. Transaction by Day of Week';

/*** Find 20 most populated Postal codes ***/
proc freq
  data=nonmon
  order=freq;
  tables C029 / out=C029f noprint;
run;

proc print
  label
  data=C029f (obs=20);
  title 'A015. Most frequent Postal codes';
run;

proc freq
  data=nonmon
  order=freq;
  tables C029 / missing out=C029f noprint;
  where C020 = 'A';
run;

proc print
  label
  data=C029f(obs=20);
  title 'A016. Postal Codes (tran type = A)';
run;

proc means
  data=nonmon
  n nmiss min max mean;
  var C038N;
  title 'A016. Statistics of Available Credit';
run;

proc freq
  data=nonmon
  order=freq;
  tables C038 / missing out=C038f noprint;
  where C020 = 'L';
run;

proc print
  label
  data=C038f(obs=20);
  title 'A016. Available Credit (tran type = L)';
run;

endsas;
