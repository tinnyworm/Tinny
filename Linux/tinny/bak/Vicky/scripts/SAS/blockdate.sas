options 
        nocenter 
        linesize=80 
        pagesize=60
        compress=yes;

filename authsdat pipe '/usr/local/bin/zcat auths.dat.gz';

/*
   Read authorizations.  The is_appr variable is a coarse version of
   auth-decision.  It will be used to determine the fraud detection
   date (block date).  Thus decisions such as referral are still considered
   to be approved, since a referral reflects suspicion of fraud rather than
   a definite detection of fraud.
*/

data auths (keep= account a_date is_appr);
  infile authsdat;
  input
    @01  account  $char19.
    @20  a_date   yymmdd8.
    @63  a_decis  $char1.
  ;
  if a_decis='A' then is_appr=1;
  else if a_decis='R' then is_appr=1;
  else if a_decis='I' then is_appr=1;
  else is_appr=0;
run;

/*
   Read frauds.
*/

data frauds;
  infile stdin;
  input
    @01  account  $char19.
    @20  f_type   $char1.
    @21  f_date   $char8.
    @29  f_meth   $char1.
    @30  f_time   $char6.
  ;
run;

/*
   Identify fraudulent authorizations.
*/

data fauths;
  merge auths (in=in_auth) frauds (in=in_fraud);
  by account;
  if in_auth and in_fraud then output;
run;

/*
   Go through the fraudulent authorizations and keep only the approved
   authorizations.  However, if the first authorization for an account is
   not approved, change the authorization date to one day earlier and
   keep this transaction.  Then if all of the authorizations for such
   an account are not approved, the block date will be one day prior to
   the first authorization.
*/

data fauths (keep= account a_date f_type f_date f_meth f_time);
  set fauths;
  by account;
  if is_appr then output;
  else if first.account and ^is_appr then do;
    a_date=a_date-1;
    output;
  end;
  filler = "      ";
run;

/*
   Output the new frauds file.  The block date is the date of the last
   approved authorization for each fraud account.
*/


data _null_;
  set fauths;
  by account;
  file stdout;
  if last.account then do;
    b_date=put(a_date, yymmdd10.);
    b_date=compress(b_date, '-');
    put
      @01  account  $char19.
      @20  f_type   $char1.
      @21  f_date   $char8.
      @29  f_meth   $char1.
      @30  f_time   $char6.
      @36  b_date   $char8.
      @44  filler   $char6.
    ;
  end;
run;
