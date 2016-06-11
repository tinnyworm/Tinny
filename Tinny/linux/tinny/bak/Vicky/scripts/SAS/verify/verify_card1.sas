/**************************************************************
 FALCON                                                     
	Verify Consortium Card files
	(Fraud Consortium Credit Data Spec. 3.10)
                                                            
 	Modified from ufal_readcard.sas
	9/30/99 	js

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

data card;

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
  ;


  if (_error_) then
    do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
  end;

run;

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
/* Create random sample of records                            */
/**************************************************************/

data random
  (drop=k n);
  retain k 20 n;
  if (_n_ = 1) then n = total;
  set card
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

/* Show dates */
proc chart data=card;
  hbar C091 /type=percent missing nozeros;
  title 'A026. Record dates';
run;

endsas;



