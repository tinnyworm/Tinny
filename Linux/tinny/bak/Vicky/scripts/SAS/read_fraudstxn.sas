options errors=5;

filename testfile pipe 'cat test_frauds.txt ';
filename account  pipe 'cat account.txt ';

data temp1;
  infile testfile ;
  input
    @001 wacctid $16.   /* account               */
    @018 trandt $6.    /* tran date             */
    @025 trantp $1.    /* tran type             */
  ;

  /* Convert date fields into SAS dates */
  fraddt = input(put(trandt,$6.),??yymmdd6.);
  bin1=substr(wacctid, 1,6); 
  drop trandt;
run;

data temp2;
  infile account;
  input
    @001 acctid $19.;
 bin2=substr(acctid,1,6);
run;  

data temp;
 merge temp2 temp1;
 if bin1 ne bin2 then miss="1";
 else miss ="0";
 drop wacctid; 
run;

proc freq data=temp;
table miss/missing;
run;

proc sort data=temp;
by acctid fraddt;
run;

/*data _null_;
 set temp;
 file STDOUT;
 put @001 acctid $19.
     @020 trantp $1. 
     @021 fraddt yymmdd10.; 
run;*/

*******************************************************************************;

/* Create a data set with only the first transaction for each account */
data firstrx;
  set temp;
  by acctid;

  keep acctid trantp earliest;
  retain earliest;

  if first.acctid then do;
    earliest=fraddt;
  end;
  if last.acctid then output;
  attrib
    earliest format=yymmdd10.  label='Date of Earliest Fraud Transaction'
  ;
run;

proc sort data=firstrx;
  by acctid;
run;

proc print label data=firstrx(obs=10);
  title2 "Detail Records: First Transactions Only";
run;


data lastrx;
  set temp;
  by acctid;

  keep acctid trantp blockdt;
  retain blockdt;

  if last.acctid then do;
    blockdt=fraddt;
    output;
  end;

  attrib
    blockdt format=yymmdd10.  label='Date of last Fraud Transaction'
  ;
run;

proc sort data=lastrx;
  by acctid;
run;

proc print label data=lastrx(obs=10);
  title2 "Detail Records: last Transactions Only";
run;

*******************************************************************************;

/* Merge transaction and base records  */
data combo;
  merge firstrx(in=a) lastrx(in=b);
  by acctid;
run;

proc print label data=combo(obs=10);
  title2 "Merged first date and block date ";
run;

/* Keep only transactions with populated SMW2-FRAUD-DATE fields */
data tmp;
  set combo;
  by acctid;

  if earliest ne 0 
    then ffd = compress(put(earliest,yymmdd10.),"-");
    else = repeat(' ',6);
  
  if blockdt ne 0 
    then fdd = compress(put(blockdt,yymmdd10.),"-");
    else = repeat(' ',6);
run;

proc sort data=tmp;
  by acctid ffd fdd trantp;
run;

proc print label data=tmp(obs=10);
  title2 "Fraud file DataSet";
run;

options missing=' ';

data _null_ ;
  file STDOUT;
  set tmp;

  put
    @01  acctid   $19.
    @20  trantp   $1.
    @21  ffd      $8.
    @29  blank1   $7.
    @36  fdd      $8.
    @45  blank2   $6.;
 
run;
