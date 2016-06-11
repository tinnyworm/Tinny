/**************************************************************
 FALCON                                                     
	Verify Consortium Card files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readcard.sas
	9/30/99 	js

	Note: Use as input cards file that has been stripped
	      of duplicates.

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
  value $itp_fmt
    ' ' = 'Blank'
    'I' = 'I'
    'R' = 'R'
    'F' = 'F'
    other = 'Invalid';

  value cred_fmt
    low - -.01 = 'Negative'
    .00 - .00  = 0
    .01 - high = 'Positive'
    other      = 'Invalid';

  value $ss5_fmt
    ' '               = 'Blank'
    0                 = '00000'
    '00001' - '99999' = 'OK'
    other             = 'Invalid';

run;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data cardu;

  infile STDIN
  lrecl=186
  missover;

  input
    @1   C001   $19.  /* ACCT-ID: ACCOUNT ID */
    @20  C020   $9.   /* POSTAL-CODE: */
    @29  C029   $3.   /* CNTRY: COUNTRY */
    @32  C032   $8.   /* OPEN-DATE: */
    @40  C040   $8.   /* ISSUE-DATE: */
    @48  C048   $1.   /* ISSUE-TYPE: */
    @49  C049   $8.   /* EXPIRE-DATE: EXPIRATION DATE */
    @57  C057   $10.  /* CRED-LINE: CREDIT LINE */
    @67  C067   $1.   /* GENDER: */
    @68  C068   $8.   /* BIRTHDATE: */
    @76  C076   $10.  /* INCOME: */
    @86  C086   $1.   /* TYPE: */
    @87  C087   $1.   /* USE: */
    @88  C088   $3.   /* NUM-CARDS: NUMBER OF CARDS */
    @91  C091   $8.   /* REC-DATE: RECORDING DATE */
    @99  C099   $26.  /* ADDR-LINE-1: ADDRESS 1 */ 
    @125 C125   $26.  /* ADDR-LINE-2: ADDRESS 2 */
    @151 C151   $5.   /* SSN-5: */
    @156 C156   $1.   /* ASSOC: ASSOCIATION */
    @157 C157   $1.   /* INCENTIVE: */
    @158 C158   $19.  /* XREF-PREV-ACCT: ENCRYPTED PREVIOUS ACCOUNT ID */
    @177 C177   $2.   /* STATUS: CARD STATUS */
    @179 C179   $8.   /* STATUS-DATE: */
  ;



  if (_error_) then
    do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

  /**************************************************************/
  /* Data Analysis                                              */
  /**************************************************************/

  /*** Bin the accounts ***/
  length acct_bin $6;
  acct_bin = substr(C001,1,6);

  /*** Bin Postal codes ***/
  post_bin = substr(C020,1,5);

  /*** Cut year and month from open date ***/
  opn_mnth = substr(C032,1,6);

  /*** Cut year and month from issue date ***/
  iss_mnth = substr(C040,1,6);

  /*** Bin the issue type ***/
  iss_type = put(C048, $itp_fmt.);

  /*** Make credit line numeric ***/
  C057N = C057*1.;

  /*** Bin the credit line ***/
  crd_bin = put(C057N, cred_fmt.);

  /*** Get birth year ***/
  brth_yr = substr(C068,1,4);

  /*** Make income numeric ***/
  C076N = C076*1.;

  /*** Bin the income ***/
  inc_bin = put(C076N, cred_fmt.);

  /*** Make number of cards numeric ***/
  C088N = C088*1.;

  /*** Bin ss5 ***/
  ss5_bin = put(C151, $ss5_fmt.);

  /*** Make status date numeric ***/
  C179N = C179*1.;

  label
    C001 = 'ACCT-ID'
    C020 = 'POSTAL-CODE'
    C029 = 'CNTRY'
    C032 = 'OPEN-DATE'
    C040 = 'ISSUE-DATE'
    C048 = 'ISSUE-TYPE'
    C049 = 'EXPIRE-DATE'
    C057 = 'CRED-LINE'
    C067 = 'GENDER'
    C068 = 'BIRTHDATE'
    C076 = 'INCOME'
    C086 = 'TYPE'
    C087 = 'USE'
    C088 = 'NUM-CARDS'
    C091 = 'REC-DATE'
    C099 = 'ADDR-LINE-1'
    C125 = 'ADDR-LINE-2'
    C151 = 'SSN-5'
    C156 = 'ASSOC'
    C157 = 'INCENTIVE'
    C158 = 'XREF-PREV-ACCT'
    C177 = 'STATUS'
    C179 = 'STATUS-DATE'
    acct_bin = 'Account 6 digits'
    post_bin = 'Postal code 5 digits'
    opn_mnth = 'Open date year and month'
    iss_mnth = 'Issue date year and month'
    iss_type = 'Issue type'
    C057N = 'CREDIT-LINE'
    crd_bin = 'Credit line catagory'
    brth_yr = 'Birth year'
    C076N = 'Income'
    inc_bin = 'Binned income'
    C088N = 'Number of cards'
    ss5_bin = 'SS number category'
    C179N = 'Status date'
  ;

run;

/**************************************************************/
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set cardu
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
  var C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076 C086
      C087 C088 C091 C099 C125 C151 C156 C157 C158 C177 C179;
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
  data=cardu
  order=freq;
  tables acct_bin / out=acct_f noprint;
run;

proc print
  label
  data=acct_f (obs=50);
  title 'A012A. Most frequent account number 6 digits';
run;

/* Account numbers */
proc chart
  data=cardu;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';
run;

/* Postal codes */
proc freq
  data=cardu
  order=freq;
  tables post_bin / out=post_f noprint;
run;

proc print
  label
  data=post_f (obs=100);
  title 'A013. 100 most frequent postal codes';
run;

/* Cardholder country */
proc chart data=cardu;
  hbar C029 /type=percent missing nozeros;
  title 'A014. Country of Cardholder';
run;

/* Month opened */
proc chart data=cardu;
  hbar opn_mnth /type=percent missing nozeros;
  title 'A015. Month account opened';
run;

/* Month issued */
proc chart data=cardu;
  hbar iss_mnth /type=percent missing nozeros;
  title 'A016. Month card issued';
run;

/* Type */
proc chart data=cardu;
  hbar iss_type /type=percent missing nozeros;
  title 'A017. Card issue type';
run;

/* Expiration date */
proc chart data=cardu;
  hbar C049 /type=percent missing nozeros;
  title 'A018. Card expire date';
run;

/* Credit line */
proc means n nmiss min max mean median std
  data=cardu;
  var C057N;
  title 'A019A. Statistics of card credit lines';

proc chart data=cardu;
  hbar crd_bin /type=percent missing nozeros;
  title 'A019B. Card credit-lines by range';
run;

/* Gender */
proc chart data=cardu;
  hbar C067 /type=percent missing nozeros;
  title 'A020. Card Gender';
run;

/* Birth date */
proc chart data=cardu;
  hbar brth_yr /type=percent missing;
  title 'A021. Cardholder birth year';
run;

/* Income */
proc means n nmiss min max mean median std
  data=cardu;
  var C076N;
  title 'A022A. Statistics of cardholder incomes';
run;
   
proc chart data=cardu;
  hbar inc_bin /type=percent missing nozeros;
  title 'A022B. Cardholder incomes by range';
run;

/* Card Type */
proc chart data=cardu;
  hbar C086 /type=percent missing nozeros;
  title 'A023. Card Type';
run;

/* Use */
proc chart data=cardu;
  hbar C087 /type=percent missing nozeros;
  title 'A024. Card Use';
run;

/* Number of cards */
proc means n nmiss min max median std
  data=cardu;
  var C088N;
  title 'A025. Statistics of number of cards';
run;

/* SS numbers */
proc chart data=cardu;
  hbar ss5_bin /type=percent missing nozeros;
  title 'A029. Distribution of 5-digit SS numbers';
run;

/* Association */
proc chart data=cardu;
  hbar C156 /type=percent missing nozeros;
  title 'A030. Card Association';
run;

/* Incentive */
proc chart data=cardu;
  hbar C157 /type=percent missing nozeros;
  title 'A031. Card Incentive';
run;

/* Status */
proc chart data=cardu;
  hbar C177 /type=percent missing nozeros;
  title 'A033. Card Status';
run;

/* Status date */
proc means n nmiss min max
  data=cardu;
  var C179N;
  title 'A034. Statistics of card status dates';
   
run;

endsas;



