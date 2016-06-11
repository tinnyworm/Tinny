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

  /************************************************************/
  /* Data Analysis                                            */
  /************************************************************/

run;

/**************************************************************/
/* Generate a new set that has only unique acc numbers        */
/**************************************************************/
data cardu;
  set card;
  keep C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076
       C086 C087 C088 C091 C151 C156 C157 C158 C177 C179;
run;

proc sort nodupkey data=cardu;
 by C001 descending C091;
run;

/**************************************************************/
/* Formats                                                    */
/**************************************************************/

proc format;
  value $ityp_fmt
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
/* Data Analysis                                              */
/**************************************************************/
data cardu;
  set cardu;

  /*** Bin the accounts ***/
  length acct_bin $6;
  acct_bin = substr(C001,1,6);

  /*** Bin Postal codes ***/
  post_bin = substr(C020,1,2);

  /*** Cut year and month from open date ***/
  opn_mnth = substr(C032,1,6);

  /*** Cut year and month from issue date ***/
  iss_mnth = substr(C040,1,6);

  /*** Bin the issue type ***/
  iss_type = put(C048, $ityp_fmt.);

  /*** Make credit line numeric ***/
  C057N = C057*1.;

  /*** Bin the credit line ***/
  crd_bin = put(C057N, cred_fmt.);

  /*** Make birth date numeric ***/
  C068N = C068*1.;

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
    acct_bin = 'Account 6 digits'
    post_bin = 'Postal code 2 digits'
    opn_mnth = 'Open date year and month'
    iss_mnth = 'Issue date year and month'
    iss_type = 'Issue type'
    C057N = 'CREDIT-LINE'
    crd_bin = 'Credit line catagory'
    C068N = 'Birth date'
    C076N = 'Income'
    inc_bin = 'Binned income'
    C088N = 'Number of cards'
    ss5_bin = 'SS number category'
    C179N = 'Status date'
  ;

run;

/**************************************************************/
/* Show Results                                               */
/**************************************************************/

/* Show some samples */
proc print
  label
  data=card (obs=10);
  var C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076 C086
      C087 C088 C091 C099 C125 C151 C156 C157 C158 C177 C179;
  title 'A001. Visual inspection: First 10 entries in file';

proc chart
  data=cardu;
  hbar acct_bin /type=percent missing;
  title 'A012E. Distribution of account number 6 digits';

proc chart data=cardu;
  hbar post_bin /type=percent missing nozeros;
  title 'A013. First Three Digits of Postal Code';

proc chart data=cardu;
  hbar C029 /type=percent missing nozeros;
  title 'A014. Country of Cardholder';

proc chart data=cardu;
  hbar opn_mnth /type=percent missing nozeros;
  title 'A015. Month account opened';

proc chart data=cardu;
  hbar iss_mnth /type=percent missing nozeros;
  title 'A016. Month card issued';

proc chart data=cardu;
  hbar iss_type /type=percent missing nozeros;
  title 'A017. Card issue type';

proc chart data=cardu;
  hbar C049 /type=percent missing nozeros;
  title 'A018. Card expire date';

proc means n nmiss min max mean;
  var C057N;
  title 'A019A. Statistics of card credit lines';
   
proc chart data=cardu;
  hbar crd_bin /type=percent missing nozeros;
  title 'A019B. Card credit-lines by range';

proc chart data=cardu;
  hbar C067 /type=percent missing nozeros;
  title 'A020. Card Gender';

proc means n nmiss min max;
  var C068N;
  title 'A021. Statistics of birth dates';
   
proc means n nmiss min max mean;
  var C076N;
  title 'A022A. Statistics of cardholder incomes';
   
proc chart data=cardu;
  hbar inc_bin /type=percent missing nozeros;
  title 'A022B. Cardholder incomes by range';

proc chart data=cardu;
  hbar C086 /type=percent missing nozeros;
  title 'A023. Card Type';

proc chart data=cardu;
  hbar C087 /type=percent missing nozeros;
  title 'A024. Card Use';

proc means n nmiss min max;
  var C088N;
  title 'A025. Statistics of number of cards';
   
proc chart data=card;
  hbar C091 /type=percent missing nozeros;
  title 'A026. Record dates';

proc chart data=cardu;
  hbar ss5_bin /type=percent missing nozeros;
  title 'A029. Distribution of 5-digit SS numbers';

proc chart data=cardu;
  hbar C156 /type=percent missing nozeros;
  title 'A030. Card Association';

proc chart data=cardu;
  hbar C157 /type=percent missing nozeros;
  title 'A031. Card Incentive';

proc chart data=cardu;
  hbar C177 /type=percent missing nozeros;
  title 'A033. Card Status';

proc means n nmiss min max;
  var C179N;
  title 'A034. Statistics of card status dates';
   
run;

endsas;



