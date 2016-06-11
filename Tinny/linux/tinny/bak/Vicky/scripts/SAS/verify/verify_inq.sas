/**************************************************************
 FALCON                                                     
	Verify Consortium Inq files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readfraud.sas
	10/08/99 	js

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
/* Bin the Times                                              */
/**************************************************************/

proc format;
     value $tim_fmt 
     '000001'-'010000' = '00'
     '010001'-'020000' = '01'
     '020001'-'030000' = '02'
     '030001'-'040000' = '03'
     '040001'-'050000' = '04'
     '050001'-'060000' = '05'
     '060001'-'070000' = '06'
     '070001'-'080000' = '07'
     '080001'-'090000' = '08'
     '090001'-'100000' = '09'
     '100001'-'110000' = '10'
     '110001'-'120000' = '11'
     '120001'-'130000' = '12'
     '130001'-'140000' = '13'
     '140001'-'150000' = '14'
     '150001'-'160000' = '15'
     '160001'-'170000' = '16'
     '170001'-'180000' = '17'
     '180001'-'190000' = '18'
     '190001'-'200000' = '19'
     '200001'-'210000' = '20'
     '210001'-'220000' = '21'
     '220001'-'230000' = '22'
     '230001'-'235959' = '23'
      other            = '99'
;
run;
     
/**************************************************************/
/* Input                                                      */
/**************************************************************/

data inq;

  infile STDIN
	lrecl = 34
	missover;

  input
     @1   C001  $19.     /* ACCOUNT ID */
     @20  C020  $1.      /* TYPE */
     @21  C021  $8.      /* DATE */
     @29  C029  $6.	 /* TIME */
  ;

  label
     C001 = 'ACCT-ID'
     C020 = 'TYPE'
     C021 = 'DATE'
     C030 = 'TIME'
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

  /*** Calculate day-of-week ***/
  format inq_date date7.;
  inq_date = input(C021, yymmdd8.);
  inq_day   = weekday(inq_date);
  
  /*** Bin the time ***/
  inq_time = put(C029,$tim_fmt.);

  label
    acct_bin = 'Account 6 digits'
    inq_day = 'Inquiry day of week'
    inq_time = 'Inquiry time'
  ;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set inq
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
  var C001 C020 C021 C029;
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
  data=inq;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;

/* Types */
proc chart
  data=inq;
  hbar C020 /type=percent missing;
  title 'A013. Inquiry types';
run;

/* Dates */
proc chart
  data=inq;
  hbar C021 /type=percent missing;
  title 'A014B. Inquiry date';

proc chart
  data=inq;
  hbar inq_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A014C. Inquiry date by Day of Week';
run;

/* Time */
proc chart
  data=inq;
  hbar inq_time /type=percent missing;
  title 'A015. Time of inquiry';
run;

endsas;




