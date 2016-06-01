
/**************************************************************/
/* FALCON                                                     */
/*         Consortium Data Reader, Version 2.11               */
/*                                                            */
/**************************************************************/

options 
        nocenter 
        linesize=115 
        pagesize=46 
        obs=10000000 
        error=5 
        compress=yes;

filename datauth PIPE '/usr/local/bin/zcat ./auths.dat.gz';
filename datcard PIPE '/usr/local/bin/zcat ./cards.dat.gz';
filename datfraud PIPE '/usr/local/bin/zcat ./frauds.dat.gz';
filename datnmon PIPE '/usr/local/bin/zcat ./nonmons.dat.gz';
filename datpmt PIPE '/usr/local/bin/zcat ./paymnts.dat.gz';

data card;
infile datcard ;
  input
    @1   ACCOUNT      $19.     /* CARD ACCT ID */
    @20  CPSTCOD      $9.      /* CARD POSTAL CODE */
    @86  CTYPE        $1.      /* CARD TYPE */
  ;

/*
 if ctype in ('C','D') then cardtype = 'OFFLINE'; 
 else if ctype in ('N','V') then delete; 
 */

 if _error_ = 1 then delete;
run;

data card;
  set card;
  by account;
  if first.account;
run;

/* proc print data=card(obs=10);
run; */

data fraud;
  infile datfraud;
  input
    @1    ACCOUNT   $19.        /* account number */
    @20   FRTYPE    $1.         /* fraud type */
    @21   FRDDATE   yymmdd8.         /* date of first fraud */
    @30	  FRDTIME   $6.		/* first fraud time */
 ;
 if _error_ = 1 then delete;
 format firstdat date7.;
 firstdat = frddate;
 drop frddate frdtime;
run;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data auth;
 tag = _n_;
 infile datauth /*recfm=f lrecl=117*/;
 input
     @1   ACCOUNT       $19.      /* AUTH ACCT ID */
     @20  ADATE         $8.       /* AUTHORIZATION DATE */
     @28  ATIME         $6.       /* AUTHORIZATION TIME */
     @34  AMOUNT        13.2      /* AUTHORIZATION AMOUNT */
     @64  TRTYPE        $1.	  /* TRANS TYPE */
     @85  SIC           $4.       /* SIC CODE */
     @101 PINVER        $1.       /* PIN VERIFICATION */
     @102 CVVCVC        $1.       /* AUTH CVV */
     @103 KEYSWP        $1.	  /* AUTH KEY SWIPE */
 ;

* if _error_ = 1 then delete;

 if pinver in (' ') then pinver = 'M';
 if cvvcvc in (' ') then cvvcvc = 'M';
 if keyswp in (' ') then keyswp = 'M';

run;

data nonmon;
  infile datnmon;
  input
    @1    ACCOUNT   $19.        /* account number */
    @20   NONTYPE    $1.         /* fraud type */
    @21   NONDATE   yymmdd8.         /* date of first fraud */
 ;
 if _error_ = 1 then delete;

 format nmondate date7.;
 nmondate = nondate;
 drop nondate;

run;
 

/*    
     M = Missing (i.e., blank)    
*/

/*
proc print data=auth(obs=20);
   var amount amtbin;
run;
*/

/*
proc print data=auth(obs=20);
   var atime timebin;
run;
*/

/*
endsas;
*/

/* proc print data=auth(obs=10);
run; */

/******************************************************/
/* Merge Auth and Card information                    */
/******************************************************/

/************************
data authcard;
 merge auth(in=b_auth) card(in=b_card);
 by account;

 format trandate date7.;
 format trantime datetime17.;
 trandate = input(adate,yymmdd8.);
 trantime = DHMS( trandate ,
                substr(atime,1,2)*1. ,
                substr(atime,3,2)*1. ,
                substr(atime,5,2)*1. );
 
 if b_auth and b_card;
 
 keep
      tag
      account
      trandate
      trtype
      sic
      pinver
      cvvcvc
      keyswp
 ;
 
run;

**********************************/

/*************************************************************/
/* Tag transactions with fraud flags                         */
/*************************************************************/
 
data nmons00;
 merge nonmon(in=a) fraud(in=b);
 by account;
 if (b=1) then fraud = 1; else fraud = 0;
 if (b=1) and (firstdat <= nmondate+00)
    then fraudq = 'Fraud    ' ; else fraudq = 'NonFraud' ;
 if (a=1);
run;
 
data nmons10;
 merge nonmon(in=a) fraud(in=b);
 by account;
 if (b=1) then fraud = 1; else fraud = 0;
 if (b=1) and (firstdat <= nmondate+10)
    then fraudq = 'Fraud    ' ; else fraudq = 'NonFraud' ;
 if (a=1);
run;
 
data nmons20;
 merge nonmon(in=a) fraud(in=b);
 by account;
 if (b=1) then fraud = 1; else fraud = 0;
 if (b=1) and (firstdat <= nmondate+20)
    then fraudq = 'Fraud    ' ; else fraudq = 'NonFraud' ;
 if (a=1);
run;
 
data nmons30;
 merge nonmon(in=a) fraud(in=b);
 by account;
 if (b=1) then fraud = 1; else fraud = 0;
 if (b=1) and (firstdat <= nmondate+30)
    then fraudq = 'Fraud    ' ; else fraudq = 'NonFraud' ;
 if (a=1);
run;
 
/* proc print data=authcard(obs=10);
run; */

/* proc means data=authcard;
run; */

proc freq data=nmons00;
   title 'Nonmons from Date of First Fraud';
   tables fraudq*nontype;
run;

proc freq data=nmons00;
   title 'Nonmons from Date of First Fraud';
   tables frtype*fraudq*nontype;
run;

proc freq data=nmons10;
   title 'Nonmons from Date of First Fraud - 10days';
   tables fraudq*nontype;
run;

proc freq data=nmons10;
   title 'Nonmons from Date of First Fraud - 10days';
   tables frtype*fraudq*nontype;
run;

proc freq data=nmons20;
   title 'Nonmons from Date of First Fraud - 20days';
   tables fraudq*nontype;
run;

proc freq data=nmons20;
   title 'Nonmons from Date of First Fraud - 20days';
   tables frtype*fraudq*nontype;
run;

proc freq data=nmons30;
   title 'Nonmons from Date of First Fraud - 30days';
   tables fraudq*nontype;
run;

proc freq data=nmons30;
   title 'Nonmons from Date of First Fraud - 30days';
   tables frtype*fraudq*nontype;
run;

endsas;

proc freq data=auths;
   tables keyswp*fraudq*cvvcvc;
run;

/*
endsas;
 */

proc freq data=auths;
   tables fraudq*keyswp;
run;

proc freq data=auths;
   tables fraudq*trtype;
run;

proc freq data=auths;
   tables fraudq*sic;
run;
