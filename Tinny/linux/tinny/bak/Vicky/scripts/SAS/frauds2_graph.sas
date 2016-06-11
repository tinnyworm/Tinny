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

  xfraud=1;
run;


options
  papersize    = letter
  topmargin    = "1.0 in"
  bottommargin = "1.0 in"
  leftmargin   = "1.0 in"
  rightmargin  = "1.0 in"
;

filename giffile 'frauds.gif';

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
  vbar month / 
  sumvar=xfraud outside=sum    
  width=7 discrete autoref clipref raxis=axis1 maxis=axis2;
  format month monyy5.;
run;

