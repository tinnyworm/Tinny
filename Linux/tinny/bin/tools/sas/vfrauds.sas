*options pagesize=max;

*ods html body='Fbody.html' contents='Fcontents.html' frame='HTML/Fframe.html';

/* Define path to SAS macro and format catalogs */
libname library "$MSGBIN/CCDS/3.10/SAS";
options mstored sasmstore=library nofmterr;

data fraud;

  infile STDIN lrecl=49 length=len missover;

  input
     @01  C001  $char19.  /* ACCT-ID          */
     @20  C020  $char1.   /* TYPE             */
     @21  C021  $char8.   /* DATE-FIRST-FRAUD */
     @29  C029  $char1.   /* FIND-METHOD      */
     @30  C030  $char6.   /* TIME-FIRST-FRAUD */
     @36  C036  $char8.   /* DETECTED-DATE    */
     @44  C044  $char6.   /* DETECTED-TIME    */
  ;

  retain minlen 999 maxlen 0;
  minlen=min(minlen,len);
  maxlen=max(maxlen,len);

  label
     C001 = 'ACCT-ID'
     C020 = 'TYPE'
     C021 = 'DATE-FIRST-FRAUD'
     C029 = 'FIND-METHOD'
     C030 = 'TIME-FIRST-FRAUD'
     C036 = 'DETECTED-DATE'
     C044 = 'DETECTED-TIME'
  ;

  if (_error_) then
    do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

  /*** Bin the accounts ***/
  length bin $6;
  bin  = substr(C001,1,6);
  nbin = input(bin,??6.0);

  /*** Get fraud year & month ***/
  frd_mnth = substr(C021,1,6);        /* Fraud Date     */
  det_mnth = substr(C036,1,6);        /* Detection Date */
  month = mdy(substr(frd_mnth,5,2),1,substr(frd_mnth,1,4));

  /*** Calculate days-of-week ***/
  format frd_date det_date date7.;
  frd_date = input(C021,??yymmdd8.);   /* Fraud Date     */
  det_date = input(C036,??yymmdd8.);   /* Detection Date */
  frd_day  = weekday(frd_date);
  det_day  = weekday(det_date);

  /*** Calculate run days ***/
  run_days = det_date - frd_date;

  /*** Bin times ***/
  frd_time = input(substr(C030,1,2),??2.0);    /* Fraud Time     */
  det_time = input(substr(C044,1,2),??2.0);    /* Detection Time */

  label
    bin      = 'Account 6 digits'
    frd_mnth = 'Fraud month'
    frd_day  = 'Fraud day of week'
    frd_time = 'Fraud time (hour)'
    det_mnth = 'Detection month'
    det_day  = 'Detection day of week'
    frd_date = 'Fraud date'
    det_date = 'Detection date'
    run_days = 'Run days'
    det_time = 'Detection time'
  ;
run;

/*** Generate index for by-level processing ***/
proc datasets nolist library=work;
  modify fraud;
  index create C020;
run;

************************************************************;

/*** Print out the total number of observations & record lengths ***/
%let dsid   = %sysfunc(open(fraud));
%let ttlobs = %sysfunc(attrn(&dsid,nobs));
%let rc     = %sysfunc(close(&dsid));

%doby(fraud,bin,tmp,C001);

%let dsid     = %sysfunc(open(tmp));
%let uniqacts = %sysfunc(attrn(&dsid,nobs));
%let rc       = %sysfunc(close(&dsid));

proc datasets nolist library=work;
  delete tmp;
run;

data _null_;
  set fraud;
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
run;

************************************************************;

/*** Display random sample of &RanObs records ***/
%let RanObs = 20;

data random (drop=k n);
  retain k &RanObs n;
  if (_n_ = 1) then n = total;
  set fraud nobs=total;
  if (ranuni(7993652) <= k / n) then
    do;
      output;
      k = k - 1;
  end;
  n = n - 1;
  if (k = 0) then stop;
run;

*ods proclabel "Random Sample of records";
title "A001. Visual inspection: random sample of &RanObs records";
proc print label data=random;
  var C001 C020 C021 C029 C030 C036 C044;
run;

proc datasets nolist library=work;
  delete random;
run;

************************************************************;

/*** Find &BINNobs most populated Account bins ***/
%let BINNobs = 20;

proc freq data=fraud order=freq;
  tables bin / out=acct_f noprint;
run;

/*** Save the total number of distinct BINs ***/
%let dsid  = %sysfunc(open(acct_f));
%let nBINs = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc means noprint data=fraud;
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

*ods proclabel "Top &BINNobs BINs";
proc print label data=acct_f (obs=&BINNobs);
  title "A012A. Top &BINNobs most frequent account number 6 digits";
  format percent 6.2;
run;

proc datasets nolist library=work;
  delete acct_f bin_m;
run;

*ods proclabel "BIN distribution";
proc chart data=fraud;
  hbar bin / type=percent missing space=0;
  title 'A012E. Distribution of account number 6 digits';
run;

************************************************************;

%testfield(C020,fraud,$Ftype.,A013,Fraud Types);

************************************************************;

/*** Fraud date ***/
*ods proclabel "Fraud month distribution";
proc chart data=fraud;
  hbar frd_mnth / type=percent missing space=0;
  title 'A014B. Month of First Fraud Transaction';
run;

*ods proclabel "Fraud month distribution by type";
proc chart data=fraud;
  hbar frd_mnth / type=percent missing space=0;
  by C020;
  format C020 $Ftype.;
  title 'A014B1. First fraud (month) by fraud type';
run;

*ods proclabel "Fraud days of week";
proc chart data=fraud;
  hbar frd_day / type=percent missing midpoints=1 2 3 4 5 6 7 space=0;
  format frd_day weekdays.;
  title 'A014C. First Fraud by Day of Week';
run;

************************************************************;

%testfield(C029,fraud,$Fmethod.,A015,Fraud Find Method);

************************************************************;

/*** Time ***/
*ods proclabel "Fraud times";
proc chart data=fraud;
  hbar frd_time / type=percent missing midpoints=0 to 23 by 1 space=0;
  format frd_time timernge.;
  title 'A016. Time of first fraud';
run;

proc means data=fraud n nmiss min max mean median std;
  var frd_time;
  format frd_time timernge.;
run;

************************************************************;

/*** Detection date ***/
*ods proclabel "Detection month distribution";
proc chart data=fraud;
  hbar det_mnth / type=percent missing space=0;
  title 'A017B. Month of fraud detection';
run;

proc means noprint data=fraud;
  output out=tmp min=min max=max;
  var det_date;
  title 'A017B1. Month of fraud detection';
run;

*ods proclabel "Detection date stats";
proc print label data=tmp;
  var min max;
  format min max date7.;
  label
    min = 'Min date'
    max = 'Max date'
  ;
run;

proc datasets nolist library=work;
  delete tmp;
run;

*ods proclabel "Detection month distribution by type";
proc chart data=fraud;
  hbar det_mnth / type=percent missing space=0;
  by C020;
  format C020 $Ftype.;
  title 'A017B2. Fraud detection date (month) by fraud type';
run;

*ods proclabel "Detection days of week";
proc chart data=fraud;
  hbar det_day / type=percent missing midpoints=1 2 3 4 5 6 7 space=0;
  format det_day weekdays.;
  title 'A017C. Fraud detection by Day of Week';
run;

************************************************************;

*ods proclabel "Run days";
proc chart data=fraud;
  hbar run_days / type=percent missing space=0;
  title 'A017D. Fraud run days';
run;

*ods proclabel "Run days (all)";
proc freq data=fraud;
  tables run_days / missing;
  title 'A017E. Fraud run days (All)';
run;

************************************************************;

/*** Detection time ***/
*ods proclabel "Detection times";
proc chart data=fraud;
  hbar det_time / type=percent missing midpoints = 0 to 23 by 1 space=0;
  format det_time timernge.;
  title 'A018. Fraud detection time';
run;

*ods proclabel "Detection time stats";
proc means data=fraud n nmiss min max mean median std;
  var det_time;
  format frd_time timernge.;
run;

************************************************************;

proc means noprint data=fraud;
  output out=tmp min=min max=max;
  var frd_date;
run;

data _null_;
  set tmp;
  call symput('start',min);
  call symput('finish',max);
run;

*ods proclabel "Fraud distribution (all)";
proc chart data=fraud;
  hbar frd_date / type=percent missing midpoints=&start to &finish by 1 space=0;
  title 'A018. Date of First Fraud (All)';
run;

*ods html close;

options
  papersize    = letter
  topmargin    = "1.0 in"
  bottommargin = "1.0 in"
  leftmargin   = "1.0 in"
  rightmargin  = "1.0 in"
;

filename giffile 'vfrauds.gif';

goptions reset=all;

goptions
  reset=global
  gunit=pct
  xpixels=576 /* = 6" @ 96 pixels/inch */
  ypixels=384 /* = 4" @ 96 pixels/inch */
  ctext=black
  htitle=5
  htext=3
  ftitle=zapfb
  ftext=swissb
  dev=gif
  gsfname=giffile
;

axis1 color=black minor=(number=1) label=(angle=90 'Count');
axis2 color=black label=('Month');
pattern color=CX0000FF value=solid;

proc gchart data=fraud;
  vbar month / width=7 discrete autoref clipref raxis=axis1 maxis=axis2;
  format month monyy5.;
run;

endsas;
