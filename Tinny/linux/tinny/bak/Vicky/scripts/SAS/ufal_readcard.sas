/**************************************************************
 FALCON                                                     
         Generic Consortium Data Reader, Version ???        
                                                            
         Cards                                              

$Source: /work/door/falcon/dvl/sas/RCS/ufal_readcard.sas,v $

$Log: ufal_readcard.sas,v $
# Revision 1.1  1996/09/30  18:50:09  kmh
# Initial revision
#
# Revision 1.1  1996/09/30  18:48:41  kmh
# Initial revision
#

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
/**************************************************************/

/**************************************************************/
/* Bin the Times                                              */
/**************************************************************/
     

%let dsname = card;                  /* DataSet Name */


/**************************************************************/
/* Input                                                      */
/**************************************************************/

data &dsname;

infile STDIN /*recfm=f lrecl=187*/;

  input
    @1   ACCOUNT      $19.     /* ACCOUNT ID */
    @20  POSTCODE     $9.      /* POSTAL CODE */
    @29  CARDCNTY     $3.      /* COUNTRY */
    @32  OPENDATE     $8.      /* OPEN DATE */
    @40  ISSUDATE     $8.      /* ISSUE DATE */
    @48  ISSUTYPE     $1.      /* ISSUE TYPE */
    @49  EXPRDATE     $8.      /* EXPIRATION DATE */
    @57  CREDLINE     $10.     /* CREDIT LINE */
    @67  CARDGNDR     $1.      /* GENDER */
    @68  BRTHDATE     $8.      /* BIRTH DATE */
    @76  INCOME	      $10.     /* INCOME */
    @86  CARDTYPE     $1.      /* TYPE */
    @87  CARDUSE      $1.      /* USE */
    @88  NUMCARD      $3.      /* NUMBER OF CARDS */
    @91  RECDATE      $8.      /* RECORDING DATE */
    @99  ADDR1	      $26.     /* ADDRESS 1 */ 
    @125 ADDR2	      $26.     /* ADDRESS 2 */
    @151 SSN5         $5.      /* SSN-5 */
    @156 ASSOC        $1.      /* ASSOCIATION */
    @157 INCENTIV     $1.      /* INCENTIVE */
    @158 PREV_ACC     $19.     /* ENCRYPTED PREVIOUS ACCOUNT ID */
    @177 STATUS       $2.      /* CARD STATUS */
;


if (_error_) then
   do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
   end;

op_year = substr(opendate,1,4);
is_year = substr(issudate,1,4);
br_year = substr(brthdate,1,4);

length bin $6;
bin = substr(account,1,6);

sub_post = substr(postcode,1,3);

n_income = income*1.0;
n_crline = credline*1.0;

run;

proc print data=&dsname(obs=40);
     var account postcode cardcnty opendate issudate issutype 
         exprdate credline cardgndr brthdate income cardtype 
         carduse numcard recdate addr1 addr2 ssn5 incentiv prev_acc status;
run;

/**************************************************************/
/* Data Analysis                                              */
/**************************************************************/

proc freq data=&dsname;
     tables recdate / missing;
     title 'Record Date';
run;

proc freq data=&dsname;
     tables exprdate / missing;
     title 'Account Expire Date';
run;

/**************************************************************/
/* Sort - want just one entry per account for remaining       */
/**************************************************************/

proc sort data=&dsname;
 by account descending recdate;
run;

proc sort nodupkey data=&dsname;
 by account;
run;

/**************************************************************/
/* More Data Analysis                                         */
/**************************************************************/

proc chart data=&dsname;
     hbar bin /type=percent missing;
     title 'BINs - 6 digits';
run;
proc chart data=&dsname;
     hbar sub_post /type=percent missing nozeros;
     title 'First Three Digits of Postal Code';
run;
proc chart data=&dsname;
     hbar cardcnty /type=percent missing nozeros;
     title 'Country of Cardholder';
run;
proc chart data=&dsname;
     hbar op_year /type=percent missing discrete;
     title 'Account Open Date';
run;
proc chart data=&dsname;
     hbar is_year /type=percent missing discrete;
     title 'Date of Last Plastic Issue';
run;
proc chart data=&dsname;
     hbar br_year /type=percent missing discrete;
     title 'Birth date of Primary Card Holder';
run;
proc chart data=&dsname;
     hbar n_income /type=percent missing;
     title 'Income';
run;
proc chart data=&dsname;
     hbar cardtype /type=percent missing;
     title 'Card type';
run;
proc chart data=&dsname;
     hbar carduse /type=percent missing;
     title 'Card use';
run;
proc chart data=&dsname;
     hbar numcard /type=percent missing;
     title 'Number of Cards on Account';
run;
proc chart data=&dsname;
     hbar assoc /type=percent missing;
     title 'Card Association';
run;
proc chart data=&dsname;
     hbar incentiv /type=percent missing;
     title 'Incentive';
run;
proc chart data=&dsname;
     hbar status /type=percent missing;
     title 'Current Card Status';
run;

endsas;



