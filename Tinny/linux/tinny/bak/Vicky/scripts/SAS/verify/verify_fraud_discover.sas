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
     ' '               = 'Blank'
      other            = '99'
;
run;
     
/***************************************************************/
/* Bin the Number of Days 				       */
/***************************************************************/
 
proc format;
   value day_fmt
     low--1 = -1
     0-0 = 000
     1-5 = 005
     6-10 = 010
     11-15 = 015
     16-20 = 020
     21-25 = 025
     26-30 = 030
     31-35 = 035
     36-40 = 040
     41-45 = 045
     46-50 = 050
     51-60 = 060
     61-70 = 070
     71-80 = 080
     81-90 = 090
     90-100 = 100
     101-120 = 120
     121-140 = 140
     141-160 = 160
     161-180 = 180
     181-200 = 200
     201-220 = 220
     221-240 = 240
     241-260 = 260
     261-280 = 280
     281-300 = 300
     301-320 = 320
     321-340 = 340
     341-360 = 360
     361-900 = 900
     other = 999
;
run;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data fraud;

  infile STDIN
	lrecl = 52
	missover;

  input
     @1   C001  $19.     /* ACCOUNT ID */
     @20  C020  $1.      /* TYPE */
     @21  C021  $8.      /* FRAUD DATE */
     @29  C029  $1.      /* FRAUD FIND METHOD */
     @30  C030  $6.	 /* FRAUD TIME */
     @36  C036  $8. 	 /* FRAUD DETECTION DATE */
     @44  C044  $6.      /* FRAUD DETECION TIME */
     @50  C050  $2.      /* Fraud Subtype */
     @52  C052  $1.      /* Fraud Family Indicator */

  ;

     type_sub = C020 || " " || C050;
     type_fam = C020 || " " || C052;
     sub_fam  = C050 || " " || C052;

  label
     C001 = 'ACCT-ID'
     C020 = 'TYPE'
     C021 = 'DATE-FIRST-FRAUD'
     C029 = 'FIND-METHOD'
     C030 = 'TIME-FIRST-FRAUD'
     C036 = 'DETECTED-DATE'
     C044 = 'DETECTED-TIME'
     C050 = 'SUBTYPE'
     C052 = 'FAMILY'
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

  /*** Get fraud year+month ***/
  frd_mnth = substr(C021,1,6);

  /*** Calculate fraud day-of-week ***/
  format frd_date date7.;
  frd_date = input(C021, yymmdd8.);
  frd_day   = weekday(frd_date);
  
  /*** Bin the fraud time ***/
  frd_time = put(C030,$tim_fmt.);

  /*** Get detection year+month ***/
  det_mnth = substr(C036,1,6);

  /*** Calculate detection day-of-week ***/
  format det_date date7.;
  det_date = input(C036, yymmdd8.);
  det_day   = weekday(det_date);

  /*** Calculate run days ***/
  diff_day = det_date - frd_date;
/*  run_days = put(diff_day,day_fmt.);*/
  run_days = diff_day;

  /*** Bin the fraud detection time ***/
  det_time = put(C044,$tim_fmt.);

  label
    acct_bin = 'Account 6 digits'
    frd_mnth = 'Fraud month'
    frd_day = 'Fraud day of week'
    frd_time = 'Fraud time (hour)'
    det_mnth = 'Detection month'
    det_day = 'Detection day of week'
    run_days = 'Run days'
    det_time = 'Detection time'
  ;
run;

/**************************************************************/
/* Generate a new set that has only type and dates            */
/**************************************************************/
data fraud1;
  set fraud;
  keep C020 frd_mnth det_mnth;

  label
    C020 = 'Fraud Type'
    frd_mnth = 'Fraud month'
    det_mnth = 'Detection month'
  ;
run;

proc sort data=fraud1;
  by C020;
run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set fraud
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
  var C001 C020 C021 C029 C030 C036 C044 C050 C052;
  title 'A001. Visual inspection: random sample of 20 records';
run;

/* Remove temporary dataset */
proc datasets
  nolist
  library=work;
  delete random;
run;

/* Account numbers */
/*** Find 20 most populated Account bins ***/
/********************* Not relevant for Discover - skip *******************
proc freq
  data=fraud
  order=freq;
  tables acct_bin / out=acct_f noprint;
run;

proc print
  label
  data=acct_f (obs=20);
  title 'A012A. Most frequent account number 6 digits';
run;
****************************************************************************/

/* Show distribution */
/*********************
proc chart
  data=fraud;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;
************************/

/* Types */
proc chart
  data=fraud;
  hbar C020 /type=percent missing;
  title 'A013. Fraud types';
run;

/* Fraud date */
proc chart
  data=fraud;
  hbar frd_mnth /type=percent missing;
  title 'A014B. Month of First Fraud Transaction';

proc chart
  data=fraud1;
  hbar frd_mnth /type=percent missing;
  by C020;
  title 'A014B1. First fraud (month) by fraud type';
run;

proc chart
  data=fraud;
  hbar frd_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A014C. First Fraud by Day of Week';
run;

/* Find method */
proc chart
  data=fraud;
  hbar C029 /type=percent missing;
  title 'A015A. Fraud find method';
run;

/* Type-Subtype */
proc chart
  data=fraud;
  hbar type_sub /type=percent missing;
  title 'A015B. Fraud Type-Subtype';
run;

/* Subtype */
proc chart
  data=fraud;
  hbar C050 /type=percent missing;
  title 'A015C. Fraud Subtype';
run;

/* Type-Family */
proc chart
  data=fraud;
  hbar type_fam /type=percent missing;
  title 'A015D. Fraud Type-Family Indicator';
run;

/* Subtype-Family */
proc chart
  data=fraud;
  hbar sub_fam /type=percent missing;
  title 'A015E. Fraud SubType-Family Indicator';
run;

/* Family indicator */
proc chart
  data=fraud;
  hbar C052 /type=percent missing;
  title 'A015F. Fraud Family indicator';
run;

/* Time */
proc chart
  data=fraud;
  hbar frd_time /type=percent missing;
  title 'A016. Time of first fraud';
run;

/* Detection date */
proc chart
  data=fraud;
  hbar det_mnth /type=percent missing;
  title 'A017B. Month of fraud detection';

proc chart
  data=fraud1;
  hbar det_mnth /type=percent missing;
  by C020;
  title 'A017B1. Fraud detection date (month) by fraud type';
run;

proc chart
  data=fraud;
  hbar det_day /type=percent missing midpoints=1 2 3 4 5 6 7;
  title 'A017C. Fraud detection by Day of Week';
run;

proc chart
  data=fraud;
  hbar run_days /type=percent missing;
  title 'A017D. Fraud run days';
run;

proc freq
  data=fraud;
  tables run_days / missing;
  title 'A017E. Fraud run days (All)';
run;

/* Detection time */
proc chart
  data=fraud;
  hbar det_time /type=percent missing;
  title 'A018. Fraud detection time';
run;

proc chart
  data=fraud;
  hbar C021 / type=percent missing;
  title 'A019. Date of First Fraud (All)';
run;

/************************
proc freq
  data=fraud;
  tables C021 / missing;
  title 'A019A. Date of First Fraud (All)';
run;
****************************/

endsas;




