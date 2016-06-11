libname mylib '/work/gold/projects/bin/CCDS/4.10/MACROS/2.2/yxc/';
options mstored sasmstore=mylib;

%macro read_fr / store des='Read frauds' ;

data fraud
C001(keep=C001 length nbin)
C020(keep=C020)
C021(keep=C021)
C029(keep=C029)
C030(keep=C030)
C036(keep=C036)
C044(keep=C044)
C050(keep=C050)
C058(keep=C058)
C064(keep=C064)
C072(keep=C072)
C078(keep=C078)
C086(keep=C086)
;

  infile STDIN lrecl=91 length=len missover ;

  input
     @01  C001  $19.     /* ACCOUNT ID           */
         @20  C020  $1.      /* TYPE                 */
         @21  C021  $8.      /* FRAUD DATE           */
         @29  C029  $1.      /* FRAUD FIND METHOD    */
         @30  C030  $6.      /* FRAUD TIME           */
         @36  C036  $8.      /* FRAUD BLOCK DATE */
         @44  C044  $6.      /* FRAUD BLOCK TIME  */
         @50  C050  $8.      /* PAY DATE FIRST FRAUD      */
         @58  C058  $6.      /* PAY TIME FIRST FRAUD      */
         @64  C064  $8.      /* NONMON DATE FIRST FRAUD   */
         @72  C072  $6.      /* NONMON TIME FIRST FRAUD   */
         @78  C078  $8.      /* INQ DATE FIRST FRAUD     */
         @86  C086  $6.      /* INQ TIME FIRST FRAUD     */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

  label C001='Account' C020='Type' C021='FF Date' C029='Find Method' C030='FF Time'
	C036='Block Date' C044='Block Time' C050='Pay Date' C058='Pay Time'
	C064='Nonmon Date' C072='Nonmon Time' C078='Inq Date' C086='Inq Time';
run;

proc printto new print='frauds.list';
proc print data=fraud(obs=10) split='*';
   title "FRAUDS: First 10 obs";
run;

proc printto ;

%mend read_fr;

**********************************************************;

%macro read_nm / store des='Read Nonmons';


data nonmon
C001(keep=C001 length nbin)
C020(keep=C020 C029 C038)
C021(keep=C021)
C048(keep=C048)
C063(keep=C063)
;

  infile STDIN lrecl=63 length=len missover;

  input
     @1   C001  $19.     /* ACCOUNT ID */
     @20  C020  $1.      /* TRAN TYPE */
     @21  C021  $8.      /* TRAN DATE */
     @29  C029  $9.      /* POSTAL CODE */
     @38  C038  $10.     /* CRED LINE */
     @48  C048  $15.     /* ADDR CODE */
     @63  C063  $1.     /* FRAUD TRX */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Type' C021='Date' C029='Postal' C038='Credit Line'
	C048='Address Code' C063='Fraud Transaction';
run;


proc printto new print='nonmons.list';
proc print data=nonmon(obs=10) split='*';
   title "NONMONS: First 10 obs";
   var C001 C020 C021 C029 C038 C048 C063;
run;

proc printto ;
%mend;

**********************************************************;

%macro read_pay /store des='Read Payments';

data payment
C001(keep=C001 length nbin)
C020(keep=C020)
C033(keep=C033)
C046(keep=C046)
C059(keep=C059)
C072(keep=C072)
C082(keep=C082)
C090(keep=C090)
C098(keep=C098)
C106(keep=C106)
C107(keep=C107)
C108(keep=C108)
C109(keep=C109)
C110(keep=C110)
;

  infile STDIN lrecl=115 length=len missover ;

  input
    @1   C001       $19.      /* ACCOUNT ID */
    @20  C020       $13.      /* AMOUNT */
    @33  C033       $13.      /* FILLER */
    @46  C046       $13.      /* FILLER */
    @59  C059       $13.      /* BALANCE */
    @72  C072       $10.      /* TOTAL BALANCE */
    @82  C082       $8.       /* DATE */
    @90  C090       $8.       /* FILLER */  
    @98  C098       $8.       /* STATE DATE */
    @106 C106       $1.       /* REVERSAL */
    @107 C107       $1.       /* DEPOSIT METHOD */
    @108 C108       $1.       /* DEPOSIT TYPE */
    @109 C109       $1.       /* FRAUD TRX */
    @110 C110       $6.       /* PAY TIME */
  ;
  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Amount' C059='Balance' C072='Total Balance'
	C082='Date' C098='Statement Date' C106='Reversal' C107='Deposit Meth'
	C108='Deposit Type' C109='Fraud Trx' C110='Pay Time';
run;

proc printto new print='payments.list';
proc print data=payment(obs=10) split='*';
   title "PAYMENTS: First 10 obs";
   var C001 C020 C033 C046 C059 C072 C082 C090 C098 C106 C107 C108 C109 C110;
run;

proc printto ;
%mend;

**********************************************************;

%macro read_crd /store des='Read Cardholder';


/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm1)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data cards
C001(keep= C001 length nbin)
C020(keep= C020)
C029(keep= C029)
C032(keep= C032)
C040(keep= C040)
C048(keep= C048)
C049(keep= C049)
C057(keep= C057)
C067(keep= C067)
C068(keep= C068)
C076(keep= C076)
C086(keep= C086)
C087(keep= C087)
C088(keep= C088)
C091(keep= C091)
C099(keep= addr)
C151(keep= ssn)
C156(keep= C156)
C157(keep= C157)
C158(keep= xref)
C177(keep= C177 C179)
C187(keep=C187)
C201(keep=C201)
C217(keep=C217)
C218(keep=C218)
C220(keep=C220)
C221(keep=C221)
C222(keep=C222)
C223(keep=C223)
C224(keep=C224)
C225(keep=C225)
C238(keep=C238)
C251(keep=C251)
C264(keep=C264)
C276(keep=C276)
C291(keep=C291)
;

  infile STDIN lrecl=291 length=len missover ;

  input
    @001 C001 $19. /* Unique account ID                                             */
    @020 C020 $9.  /* Cardholder ZIP or postal code                                 */
    @029 C029 $3.  /* Cardholder ISO numeric country code                           */
    @032 C032 $8.  /* Account open date                                             */
    @040 C040 $8.  /* Date of first plastic issue                                   */
    @048 C048 $1.  /* Plastic issue type                                            */
    @049 C049 $8.  /* Expiration date from cardholder master file                   */
    @057 C057 $10. /* Credit line                                                   */
    @067 C067 $1.  /* Gender of primary cardholder                                  */
    @068 C068 $8.  /* Birth date of primary card holder                             */
    @076 C076 $10. /* Income                                                        */
    @086 C086 $1.  /* Card type                                                     */
    @087 C087 $1.  /* Card use                                                      */
    @088 C088 $3.  /* Number of cards on account                                    */
    @091 C091 $8.  /* Date on which record is current                               */
    @099 C099 $26. /* First line of cardholder's address                            */
    @125 C125 $26. /* Second line of cardholder's address                           */
    @151 C151 $5.  /* First 5-digits of primary cardholder's social security number */
    @156 C156 $1.  /* Association                                                   */
    @157 C157 $1.  /* Incentive                                                     */
    @158 C158 $19. /* Encrypted previous account number                             */
    @177 C177 $2.  /* Current card status                                           */
    @179 C179 $8.  /* Status change date                                            */
    @187 C187 $14. /* Portfolio ID                                                  */
    @201 C201 $16. /* Master Account Num                                           */
    @217 C217 $1. /* AIP Stat                                                      */
    @218 C218 $2. /* AIP DYN                                                        */
    @220 C220 $1. /* AIP Verify                                                    */
    @221 C221 $1. /* AIP Rsk                                                       */
    @222 C222 $1. /* AIP IA                                                        */
    @223 C223 $1. /* AIP AC                                                        */
    @224 C224 $1. /* DEL CYCLE                                                     */
    @225 C225 $13. /* DEL AMOUNT                                                    */
    @238 C238 $13. /* CYCLE CASH                                                    */
    @251 C251 $13. /* CYCLE MERCH                                                   */
    @264 C264 $12. /* PAY HIST                                                      */
    @276 C276 $15. /* ADDR CODE                                                     */
    @291 C291 $1. /*  MEDIA                                                        */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

  **** formatting XREF ****;
  length addr $1;
  if C099^='' then addr='1';

  **** formatting SSN ****;
  length ssn $5;
  if C151 in ('00000','') then ssn=C151; 
  else if '00001'<=C151<='99999' then ssn='00001';
  else ssn='X';

  **** formatting XREF ****;
  length xref $1;
  if C158^='' then xref='1';

   label C001='Account' C020='Postal Code' C029='Country Code' C032='Open Date' 
	C040='Issue Date' C048='Issue Type' C049='Expire Date' C057='Credit Line'
	C067='Gender' C068='DOB' C076='Income' C086='Type' C087='Use'
	C088='Number of Cards' C091='Record Date' C099='Address1' C125='Address2'
	C151='SSN' C156='Assoc.' C157='Incentive' C158='Prev Acct' 
	C177='Status' C179='Change Date' C187='Portfolio ID' C201='Master Acct'
	C217='AIP Stat' C218='AIP AYN' C220='AIP Verify' C221='AIP Rsk' C222='AIP IA'
	C223='AIP AC' C224='DEL Cycle' C225='Del Amount' C238='Cycle Cash' C251='Cycle Merch'
	C264='Pay Hist' C276='Addr Code' C291='Media' ;
run;

proc printto new print='cards.list';

proc print data=cards(obs=10) split='*';
   title "CARDS: First 10 obs";
   var C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076 C086 
	C087 C088 C091 C099 C125 C151 C156 C157 C158 C177 C179 C187
	C201 C217 C218 C220 C221 C222 C223 C224 C225 C238 C251 C264
	C276 C291 ;
run;

proc printto ;
%mend;

**********************************************************;

%macro read_jcr /store des='Read Japan Cardholder';


/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm1)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data cards
C001(keep= C001 length nbin)
C020(keep= C020)
C029(keep= C029)
C032(keep= C032)
C040(keep= C040)
C048(keep= C048)
C049(keep= C049)
C057(keep= C057)
C067(keep= C067)
C068(keep= C068)
C076(keep= C076)
C086(keep= C086)
C087(keep= C087)
C088(keep= C088)
C091(keep= C091)
C099(keep= addr)
C151(keep= ssn)
C156(keep= C156)
C157(keep= C157)
C158(keep= xref)
C177(keep= C177 C179)
C187(keep= C187)
C202(keep= C202)
;

  infile STDIN lrecl=202 length=len missover ;

  input
    @001 C001 $19. /* Unique account ID                                             */
    @020 C020 $9.  /* Cardholder ZIP or postal code                                 */
    @029 C029 $3.  /* Cardholder ISO numeric country code                           */
    @032 C032 $8.  /* Account open date                                             */
    @040 C040 $8.  /* Date of first plastic issue                                   */
    @048 C048 $1.  /* Plastic issue type                                            */
    @049 C049 $8.  /* Expiration date from cardholder master file                   */
    @057 C057 $10. /* Credit line                                                   */
    @067 C067 $1.  /* Gender of primary cardholder                                  */
    @068 C068 $8.  /* Birth date of primary card holder                             */
    @076 C076 $10. /* Income                                                        */
    @086 C086 $1.  /* Card type                                                     */
    @087 C087 $1.  /* Card use                                                      */
    @088 C088 $3.  /* Number of cards on account                                    */
    @091 C091 $8.  /* Date on which record is current                               */
    @099 C099 $26. /* First line of cardholder's address                            */
    @125 C125 $26. /* Second line of cardholder's address                           */
    @151 C151 $5.  /* First 5-digits of primary cardholder's social security number */
    @156 C156 $1.  /* Association                                                   */
    @157 C157 $1.  /* Incentive                                                     */
    @158 C158 $19. /* Encrypted previous account number                             */
    @177 C177 $2.  /* Current card status                                           */
    @179 C179 $8.  /* Status change date                                            */
    @187 C187 $15.  /* Address Code                                            */
    @202 C202 $1.  /* Chip Card                                            */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

  **** formatting XREF ****;
  length addr $1;
  if C099^='' then addr='1';

  **** formatting SSN ****;
  length ssn $5;
  if C151 in ('00000','') then ssn=C151; 
  else if '00001'<=C151<='99999' then ssn='00001';
  else ssn='X';

  **** formatting XREF ****;
  length xref $1;
  if C158^='' then xref='1';

   label C001='Account' C020='Postal Code' C029='Country Code' C032='Open Date' 
	C040='Issue Date' C048='Issue Type' C049='Expire Date' C057='Credit Line'
	C067='Gender' C068='DOB' C076='Income' C086='Type' C087='Use'
	C088='Number of Cards' C091='Record Date' C099='Address1' C125='Address2'
	C151='SSN' C156='Assoc.' C157='Incentive' C158='Prev Acct' 
        C177='Status' C179='Change Date' C187='Address Code' C202='Chip Card';
run;

proc printto new print='cards.list';

proc print data=cards(obs=10) split='*';
   title "CARDS: First 10 obs";
   var C001 C020 C029 C032 C040 C048 C049 C057 C067 C068 C076 C086 
	C087 C088 C091 C099 C125 C151 C156 C157 C158 C177 C179
	C187 C202;
run;

proc printto ;

%mend;
**********************************************************;

%macro read_aut /store des='Read Auths';

/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm1)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data auth
C001(keep=C001 length nbin) 
C020(keep= C020)
C028(keep= C028)
C034(keep= C034)
C047(keep= C047)
C050(keep= C050)
C063(keep= C063)
C064(keep=C064 C085) 
C065(keep= C065)
C075(keep= C075)
C089(keep= C089)
C098(keep= C098)
C101(keep=C101 C085)
C103(keep=C102 C103)
C104(keep= C104)
C112(keep= C112)
C118(keep= C118)
C123(keep= C123)
C124(keep= C124)
C140(keep= C140)
C146(keep= C146 )
C147(keep= C147)
C160(keep= C160)
C164(keep= C164)
C165(keep= C165)
C195(keep= C195)
C208(keep= C208)
C211(keep= C211)
C212(keep= C212)
C213(keep= C213)
C214(keep= C214)
C215(keep= C215)
C216(keep= C216)
C228(keep= C228)
C231(keep= C231)
C247(keep= C247 C001)  
C248(keep= C248 C001)
C249(keep= C249)
C251(keep= C251)
C252(keep= C252)
C253(keep= C253)
C259(keep= C259)
C269(keep= C269)
C279(keep= C279)
C280(keep= C280)
C285(keep= C285)
C290(keep= C290)
C292(keep= C292)
C294(keep= C294)
C296(keep= C296)
C298(keep= C298)
C306(keep= C306)
C309(keep= C309)
C324(keep= C324)
;
/*** yxc added C001 08/18/02 ***/
  infile STDIN lrecl=324 length=len missover;

  input
    @001 C001 $19.   /* Unique account ID                                  */
    @020 C020 $8.    /* Date of transaction                                */
    @028 C028 $6.    /* Time of transaction                                */
    @034 C034 $13.   /* Transaction amount                                 */
    @047 C047 $3.    /* ISO numeric currency code                          */
    @050 C050 $13.   /* ISO currency conversion rate                       */
    @063 C063 $1.    /* Approve/decline (from authorization system)        */
    @064 C064 $1.    /* Transaction type                                   */
    @065 C065 $10.   /* Available credit                                   */
    @075 C075 $10.   /* Credit line                                        */
    @085 C085 $4.    /* SIC code                                           */
    @089 C089 $9.    /* Merchant ZIP or postal code                        */
    @098 C098 $3.    /* Merchant ISO numeric country code                  */
    @101 C101 $1.    /* PIN number verification                            */
    @102 C102 $1.    /* CVV/CVC outcome                                    */
    @103 C103 $1.    /* Keyed versus swiped                                */
    @104 C104 $8.    /* Card expiration date on auth message               */
    @112 C112 $6.    /* Authorization ID                                   */
    @118 C118 $5.    /* Processor's reason code                            */
    @123 C123 $1.    /* Association advice                                 */
    @124 C124 $16.   /* Acquirer's ID for merchant                         */
    @140 C140 $6.    /* ID number of the terminal where auth was initiated */
    @146 C146 $1.    /* Which card                                         */
    @147 C147 $13.   /* Cash back amount                                   */
    @160 C160 $4.    /* Falcon score                                       */
    @164 C164 $1.    /* Filler                                             */
    @165 C165 $30.   /* Merchant name (name)                               */
    @195 C195 $13.   /* Merchant name (city)                               */
    @208 C208 $3.    /* Merchant name (state/cntry)                        */
    @211 C211 $1.    /* CVV2 present                                       */
    @212 C212 $1.    /* CVV2 response                                      */
    @213 C213 $1.    /* AVS response                                       */
    @214 C214 $1.    /* Tran Category                                      */
    @215 C215 $1.    /* Fraud Trx                                          */
    @216 C216 $12.   /* ACQ ID                                             */
    @228 C228 $3.    /* ACQ Country                                        */
    @231 C231 $16.   /* Terminal ID                                        */
    @247 C247 $1.    /* Terminal Type                                      */
    @248 C248 $1.    /* Term Entry                                         */
    @249 C249 $2.    /* Pos Cond Code                                      */
    @251 C251 $1.    /* Card Media                                         */
    @252 C252 $1.    /* ATM Network                                        */
    @253 C253 $6.    /* Kcheck Number                                      */
    @259 C259 $10.   /* TVR                                                */
    @269 C269 $10.   /* CVR                                                */
    @279 C279 $1.    /* ARQC Valid                                         */
    @280 C280 $5.    /* ATC Crd                                            */
    @285 C285 $5.    /* ATC Host                                           */
    @290 C290 $2.    /* LO ATC                                             */
    @292 C292 $2.    /* OL Limit incoming                                  */
    @294 C294 $2.    /* OL Limit tocard                                    */
    @296 C296 $2.    /* Recurring Frequency                                */
    @298 C298 $8.    /* Recurring Expire Date                              */
    @306 C306 $3.    /* Item Code                                          */
    @309 C309 $15.   /* Addr Code                                          */
    @324 C324 $1.    /* Payment Type                                       */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Tran Date' C028='Tran Time' C034='Tran Amount' 
	C047='Curr Code' C050='Curr Rate' C063='Decision' C064='Tran Type'
	C065='Avail Cred' C075='Credit Line' C085='SIC' C089='Postal' C098='Country Code'
	C101='PIN' C102='CVV' C103='Keyed' C104='Expire Date' C112='Auth ID' 
	C118='Reason' C123='Advice' C124='Merch ID' C140='Term ID' C146='Which Card'
	C147='Cash Back' C160='Falcon' C165='Merch Name' C195='Merch City' 
	C208='Merch State' C211='CVV2 present' C212='CVV2 response' C213='AVS Response'
	C214='Tran Category' C215='Fraud Trx' C216='ACQ ID' C228='ACQ Country'
	C231='Terminal ID' C247='Term Type' C248='Term Entry' C249='Pos Cond Code'
	C251='Card Media' C252='ATM Network' C253='Kcheck Number' C259='TVR' C269='CVR'
	C279='ARCQ Valid' C280='ATC Crd' C285='ATC Host' C290='LO Atc' C292='OL Limit Incoming'
	C294='OL Limit Tocard' C296='Recurring frequency' C298='Recurring expire date' C306='Item Code'
	C309='Addr Code' C324='Payment Type' ;
run;

proc printto new print='auths.list';
proc print data=auth(obs=10) split='*';
   title1 "AUTHS: First 10 obs";
   var C001 C020 C028 C034 C047 C050 C063 C064 C065 C075 C085 C089 
	C098 C101 C102 C103 C104 C112 C118 C123 C124 C140 C146 C147 C160 C164 C165
	C195 C208 C211 C212 C213 C214 C215 C216 C228 C231 C247 C248 C249 C251 C252
	C253 C259 C269 C279 C280 C285 C290 C292 C294 C296 C298 C306 C309 C324;
run;



***** AMOUNT BY CURRENCY & COUNTRY CODES *****;
data amount; set auth(keep=C034 C047 C098);
  amount=C034+0;

proc sort data=amount; by C047;
proc univariate noprint; by C047;
  var amount;
  output out=curr n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=curr; by descending n;
proc printto new print='auths.amt_x_curr.list';
proc print ;
   title1 "Auth Amounts by Currency Code";
   var C047 n mean median min max std p5 p25 p75 p95 ;
run;

proc sort data=amount; by C098;
proc univariate noprint; by C098;
  var amount;
  output out=merch n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=merch; by descending n;
proc printto new print='auths.amt_x_country.list';
proc print ;
   title1 "Auth Amounts by Merchant Country Code";
   var C098 n mean median min max std p5 p25 p75 p95 ;
run;

proc printto;

%mend;

**********************************************************;

%macro ret_auth /store des='Read Retail Auths';

/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm1)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data auth
C001(keep=C001 length nbin) 
C020(keep= C020)
C028(keep= C028)
C034(keep= C034)
C047(keep= C047)
C050(keep= C050)
C063(keep= C063)
C064(keep=C064 C085) 
C065(keep= C065)
C075(keep= C075)
C089(keep= C089)
C098(keep= C098)
C101(keep=C101 C085)
C103(keep=C102 C103)
C104(keep= C104)
C112(keep= C112)
C118(keep= C118)
C123(keep= C123)
C124(keep= C124)
C140(keep= C140)
C146(keep= C146 )
C147(keep= C147)
C160(keep= C160)
C164(keep= C164)
C165(keep= C165)
C195(keep= C195)
C208(keep= C208)
C211(keep= C211)
C212(keep= C212)
C213(keep= C213)
C214(keep= C214)
C215(keep= C215)
C216(keep= C216)
C228(keep= C228)
C231(keep= C231)
C247(keep= C247)
C248(keep= C248)
C249(keep= C249)
C251(keep= C251)
C252(keep= C252)
C253(keep= C253)
C259(keep= C259)
C269(keep= C269)
C279(keep= C279)
C280(keep= C280)
C285(keep= C285)
C290(keep= C290)
C292(keep= C292)
C294(keep= C294)
C296(keep= C296)
C298(keep= C298)
C306(keep= C306)
C309(keep= C309)
C324(keep= C324)
C325(keep= C325)
C335(keep= C335)
C337(keep= C337)
C343(keep= C343)
C345(keep= C345)
C347(keep= C347)
C349(keep= C349)
;

  infile STDIN lrecl=349 length=len missover;

  input
    @001 C001 $19.   /* Unique account ID                                  */
    @020 C020 $8.    /* Date of transaction                                */
    @028 C028 $6.    /* Time of transaction                                */
    @034 C034 $13.   /* Transaction amount                                 */
    @047 C047 $3.    /* ISO numeric currency code                          */
    @050 C050 $13.   /* ISO currency conversion rate                       */
    @063 C063 $1.    /* Approve/decline (from authorization system)        */
    @064 C064 $1.    /* Transaction type                                   */
    @065 C065 $10.   /* Available credit                                   */
    @075 C075 $10.   /* Credit line                                        */
    @085 C085 $4.    /* SIC code                                           */
    @089 C089 $9.    /* Merchant ZIP or postal code                        */
    @098 C098 $3.    /* Merchant ISO numeric country code                  */
    @101 C101 $1.    /* PIN number verification                            */
    @102 C102 $1.    /* CVV/CVC outcome                                    */
    @103 C103 $1.    /* Keyed versus swiped                                */
    @104 C104 $8.    /* Card expiration date on auth message               */
    @112 C112 $6.    /* Authorization ID                                   */
    @118 C118 $5.    /* Processor's reason code                            */
    @123 C123 $1.    /* Association advice                                 */
    @124 C124 $16.   /* Acquirer's ID for merchant                         */
    @140 C140 $6.    /* ID number of the terminal where auth was initiated */
    @146 C146 $1.    /* Which card                                         */
    @147 C147 $13.   /* Cash back amount                                   */
    @160 C160 $4.    /* Falcon score                                       */
    @164 C164 $1.    /* Filler                                             */
    @165 C165 $30.   /* Merchant name (name)                               */
    @195 C195 $13.   /* Merchant name (city)                               */
    @208 C208 $3.    /* Merchant name (state/cntry)                        */
    @211 C211 $1.    /* CVV2 present                                       */
    @212 C212 $1.    /* CVV2 response                                      */
    @213 C213 $1.    /* AVS response                                       */
    @214 C214 $1.    /* Tran Category                                      */
    @215 C215 $1.    /* Fraud Trx                                          */
    @216 C216 $12.   /* ACQ ID                                             */
    @228 C228 $3.    /* ACQ Country                                        */
    @231 C231 $16.   /* Terminal ID                                        */
    @247 C247 $1.    /* Terminal Type                                      */
    @248 C248 $1.    /* Term Entry                                         */
    @249 C249 $2.    /* Pos Cond Code                                      */
    @251 C251 $1.    /* Card Media                                         */
    @252 C252 $1.    /* ATM Network                                        */
    @253 C253 $6.    /* Kcheck Number                                      */
    @259 C259 $10.   /* TVR                                                */
    @269 C269 $10.   /* CVR                                                */
    @279 C279 $1.    /* ARQC Valid                                         */
    @280 C280 $5.    /* ATC Crd                                            */
    @285 C285 $5.    /* ATC Host                                           */
    @290 C290 $2.    /* LO ATC                                             */
    @292 C292 $2.    /* OL Limit incoming                                  */
    @294 C294 $2.    /* OL Limit tocard                                    */
    @296 C296 $2.    /* Recurring Frequency                                */
    @298 C298 $8.    /* Recurring Expire Date                              */
    @306 C306 $3.    /* Item Code                                          */
    @309 C309 $15.   /* Addr Code                                          */
    @324 C324 $1.    /* Payment Type                                       */
    @325 C325 $10.   /* Max Item                                           */
    @335 C335 $2.    /* Item Count                                         */
    @337 C337 $6.    /* Dept                                               */
    @343 C343 $2.    /* Pump                                               */
    @345 C345 $2.    /* PRD CD                                             */
    @347 C347 $2.    /* Fuel Type                                          */
    @349 C349 $1.    /* DCR Ind                                            */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Tran Date' C028='Tran Time' C034='Tran Amount' 
	C047='Curr Code' C050='Curr Rate' C063='Decision' C064='Tran Type'
	C065='Avail Cred' C075='Credit Line' C085='SIC' C089='Postal' C098='Country Code'
	C101='PIN' C102='CVV' C103='Keyed' C104='Expire Date' C112='Auth ID' 
	C118='Reason' C123='Advice' C124='Merch ID' C140='Term ID' C146='Which Card'
	C147='Cash Back' C160='Falcon' C165='Merch Name' C195='Merch City' 
	C208='Merch State' C211='CVV2 present' C212='CVV2 response' C213='AVS Response'
	C214='Tran Category' C215='Fraud Trx' C216='ACQ ID' C228='ACQ Country'
	C231='Terminal ID' C247='Term Type' C248='Term Entry' C249='Pos Cond Code'
	C251='Card Media' C252='ATM Network' C253='Kcheck Number' C259='TVR' C269='CVR'
	C279='ARCQ Valid' C280='ATC Crd' C285='ATC Host' C290='LO Atc' C292='OL Limit Incoming'
	C294='OL Limit Tocard' C296='Recurring frequency' C298='Recurring expire date' C306='Item Code'
	C309='Addr Code' C324='Payment Type' C325='Max Item' C335='Item Count' C337='Dept'
	C343='Pump' C345='PRD CD' C347='Fuel Type' C349='DCR Ind' ;
run;

proc printto new print='auths.list';
proc print data=auth(obs=10) split='*';
   title1 "AUTHS: First 10 obs";
   var C001 C020 C028 C034 C047 C050 C063 C064 C065 C075 C085 C089 
	C098 C101 C102 C103 C104 C112 C118 C123 C124 C140 C146 C147 C160 C164 C165
	C195 C208 C211 C212 C213 C214 C215 C216 C228 C231 C247 C248 C249 C251 C252
	C253 C259 C269 C279 C280 C285 C290 C292 C294 C296 C298 C306 C309 C324
	C325 C335 C337 C343 C345 C347 C349 ;
run;



***** AMOUNT BY CURRENCY & COUNTRY CODES *****;
data amount; set auth(keep=C034 C047 C098);
  amount=C034+0;

proc sort data=amount; by C047;
proc univariate noprint; by C047;
  var amount;
  output out=curr n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=curr; by descending n;
proc printto new print='auths.amt_x_curr.list';
proc print ;
   title1 "Auth Amounts by Currency Code";
   var C047 n mean median min max std p5 p25 p75 p95 ;
run;

proc sort data=amount; by C098;
proc univariate noprint; by C098;
  var amount;
  output out=merch n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=merch; by descending n;
proc printto new print='auths.amt_x_country.list';
proc print ;
   title1 "Auth Amounts by Merchant Country Code";
   var C098 n mean median min max std p5 p25 p75 p95 ;
run;

proc printto;

%mend;

**********************************************************;

%macro read_jau /store des='Read Japan Auths';

/* Determine which additional formats to include */
%macro CrdOrDbt;
%if %upcase(&sysparm1)=D %then %do;
  options fmtsearch=(library.debit);
%end;
%else %do;
  options fmtsearch=(library.credit);
%end;
%mend CrdOrDbt;

%CrdOrDbt;

data auth
C001(keep=C001 length nbin) 
C020(keep= C020)
C028(keep= C028)
C034(keep= C034)
C047(keep= C047)
C050(keep= C050)
C063(keep= C063)
C064(keep=C064 C085) 
C065(keep= C065)
C075(keep= C075)
C089(keep= C089)
C098(keep= C098)
C101(keep=C101 C085)
C103(keep=C102 C103)
C104(keep= C104)
C112(keep= C112)
C118(keep= C118)
C123(keep= C123)
C124(keep= C124)
C140(keep= C140)
C146(keep= C146 )
C147(keep= C147)
C160(keep= C160)
C164(keep= C164)
C167(keep= C167)
C183(keep= C183)
;

  infile STDIN lrecl=195 length=len missover;

  input
    @001 C001 $19.   /* Unique account ID                                  */
    @020 C020 $8.    /* Date of transaction                                */
    @028 C028 $6.    /* Time of transaction                                */
    @034 C034 $13.   /* Transaction amount                                 */
    @047 C047 $3.    /* ISO numeric currency code                          */
    @050 C050 $13.   /* ISO currency conversion rate                       */
    @063 C063 $1.    /* Approve/decline (from authorization system)        */
    @064 C064 $1.    /* Transaction type                                   */
    @065 C065 $10.   /* Available credit                                   */
    @075 C075 $10.   /* Credit line                                        */
    @085 C085 $4.    /* SIC code                                           */
    @089 C089 $9.    /* Merchant ZIP or postal code                        */
    @098 C098 $3.    /* Merchant ISO numeric country code                  */
    @101 C101 $1.    /* PIN number verification                            */
    @102 C102 $1.    /* CVV/CVC outcome                                    */
    @103 C103 $1.    /* Keyed versus swiped                                */
    @104 C104 $8.    /* Card expiration date on auth message               */
    @112 C112 $6.    /* Authorization ID                                   */
    @118 C118 $5.    /* Processor's reason code                            */
    @123 C123 $1.    /* Association advice                                 */
    @124 C124 $16.   /* Acquirer's ID for merchant                         */
    @140 C140 $6.    /* ID number of the terminal where auth was initiated */
    @146 C146 $1.    /* Which card                                         */
    @147 C147 $13.   /* Cash back amount                                   */
    @160 C160 $4.    /* Falcon score                                       */
    @164 C164 $3.    /* Item Code                                          */
    @167 C167 $15.   /* Address Code                                       */
    @183 C183 $13.   /* AUTH-ID                                       */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Tran Date' C028='Tran Time' C034='Tran Amount' 
	C047='Curr Code' C050='Curr Rate' C063='Decision' C064='Tran Type'
	C065='Avail Cred' C075='Credit Line' C085='SIC' C089='Postal' C098='Country Code'
	C101='PIN' C102='CVV' C103='Keyed' C104='Expire Date' C112='Auth ID' 
	C118='Reason' C123='Advice' C124='Merch ID' C140='Term ID' C146='Which Card'
        C147='Cash Back' C160='Falcon' C164='Auth Item Cd' C167='Auth-Addr-CD'
        C183='AUTH ID';
run;

proc printto new print='auths.list';
proc print data=auth(obs=10) split='*';
   title1 "AUTHS: First 10 obs";
   var C001 C020 C028 C034 C047 C050 C063 C064 C065 C075 C085 C089 
	C098 C101 C102 C103 C104 C112 C118 C123 C124 C140 C146 C147 C160 C164 
	C167 C183 ;
run;



***** AMOUNT BY CURRENCY & COUNTRY CODES *****;
data amount; set auth(keep=C034 C047 C098);
  amount=C034+0;

proc sort data=amount; by C047;
proc univariate noprint; by C047;
  var amount;
  output out=curr n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=curr; by descending n;
proc printto new print='auths.amt_x_curr.list';
proc print ;
   title1 "Auth Amounts by Currency Code";
   var C047 n mean median min max std p5 p25 p75 p95 ;
run;

proc sort data=amount; by C098;
proc univariate noprint; by C098;
  var amount;
  output out=merch n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=merch; by descending n;
proc printto new print='auths.amt_x_country.list';
proc print ;
   title1 "Auth Amounts by Merchant Country Code";
   var C098 n mean median min max std p5 p25 p75 p95 ;
run;

proc printto;

%mend;

**********************************************************;

%macro read_pst / store des='Read Posts';


data post
C001(keep= C001 length nbin)
C020(keep= C020)
C033(keep= C033)
C036(keep= C036)
C049(keep= C049)
C057(keep= C057)
C065(keep= C065)
C071(keep= C071)
C081(keep= C081)
C091(keep= C091)
C092(keep= C092)
C093(keep= C093)
C097(keep= C097)
C106(keep= C106)
C109(keep= C109)
C115(keep= C115)
C121(keep= C121)
C137(keep= C137)
C143(keep= C143)
C144(keep= C144)
C157(keep= C157)
C161(keep= C161)
C162(keep= C162)
C192(keep= C192)
C205(keep= C205)
C208(keep= C208)
C209(keep= C209)
C210(keep= C210)
C222(keep= C222)
C225(keep= C225)
C241(keep= C241)
C242(keep= C242)
C243(keep= C243)
C245(keep= C245)
C246(keep= C246)
C247(keep= C247)
C253(keep= C253)
C263(keep= C263)
C273(keep= C273)
C274(keep= C274)
C279(keep= C279)
C284(keep= C284)
C286(keep= C286)
C288(keep= C288)
C290(keep= C290)
C292(keep= C292)
C300(keep= C300)
C303(keep= C303)
C318(keep= C318)
;

  infile stdin lrecl=318 length=len missover;

  input
    @001 C001  $19. /* ACCOUNT ID       */
    @020 C020  $13. /* AMOUNT           */
    @033 C033  $3.  /* CURRENCY CODE    */
    @036 C036  $13. /* CURRENCY RATE    */
    @049 C049  $8.  /* POST DATE        */
    @057 C057  $8.  /* TRAN DATE        */
    @065 C065  $6.  /* TRAN TIME        */
    @071 C071  $10. /* AVAILABLE CREDIT */
    @081 C081  $10. /* CREDIT LINE      */
    @091 C091  $1.  /* TRANSACTION TYPE */
    @092 C092  $1.  /* AUTHED           */
    @093 C093  $4.  /* SIC CODE         */
    @097 C097  $9.  /* POSTAL CODE      */
    @106 C106  $3.  /* MERCHANT COUNTRY */
    @109 C109  $6.  /* AUTH ID          */
    @115 C115  $6.  /* POST ID          */
    @121 C121  $16. /* MERCHANT ID      */
    @137 C137  $6.  /* TERMINAL ID      */
    @143 C143  $1.  /* WHICH CARD       */
    @144 C144  $13. /* CASH BACK AMOUNT */
    @157 C157  $4.  /* FALCON SCORE     */
    @161 C161  $1.  /* Filler           */
    @162 C162  $30. /* Merchant Name    */
    @192 C192  $13. /* Merchant City    */
    @205 C205  $3.  /* Merchant State   */
    @208 C208  $1.  /* Tran Category    */
    @209 C209  $1.  /* Fraud Trx        */
    @210 C210  $12.  /* Acq ID           */
    @222 C222  $3.  /* Acq Cntry        */
    @225 C225  $16.  /* Terminal ID      */
    @241 C241  $1.  /* Term Type        */
    @242 C242  $1.  /* Term Entry       */
    @243 C243  $2.  /* Pos Cond Code    */
    @245 C245  $1.  /* Card Media       */
    @246 C246  $1.  /* ATM Network      */
    @247 C247  $6.  /* Kcheck Number    */
    @253 C253  $10.  /* TVR              */
    @263 C263  $10.  /* CVR              */
    @273 C273  $1.  /* ARQC Valid       */
    @274 C274  $5.  /* ATC CRD              */
    @279 C279  $5.  /* ATC Host              */
    @284 C284  $2.  /* LO ATC           */
    @286 C286  $2.  /* OL LIMIT INCOME  */
    @288 C288  $2.  /* OL LIMIT TOCARD */
    @290 C290  $2.  /* REcurring Freq   */
    @292 C292  $8.  /* Recur Exp Date   */
    @300 C300  $3.  /* Item Code        */
    @303 C303  $15.  /* Addr Code        */
    @318 C318  $1.  /* Payment Type     */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

   label C001='Account' C020='Amount' C033='Curr Code' C036='Curr Rate' C049='Post Date'
	C057='Tran Date' C065='Tran Time' C071='Avail Credit' C081='Credit Line' 
	C091='Tran Type' C092='Authed' C093='SIC' C097='Postal' C106='Merch Country'
	C109='Auth ID' C115='Post ID' C121='Merch ID' C137='Term ID' C143='Which Card'
	C144='Cash Back' C157='Falcon' C162='Merch Name' C192='Merch City' 
	C205='Merch State' C208='Tran Category' C209='Fraud Trx' C210='ACQ ID'
	C222='ACQ Country' C225='Term ID' C241='Term Type' C242='Term Entry' C243='POS Cond'
	C245='Card Media' C246='ATM Net' C247='KCHECK Num' C253='TVR' C263='CVR'
	C273='ARQC Val' C274='ATC Card' C279='ATC Host' C284='LO ATC' C286='OL Lim incoming'
	C288='OL Lim Tocard' C290='Recurr Freq' C292='Recurr Exp date' C300='Item Code'
	C303='Addr Code' C318='Payment Type' ;

  infile stdin lrecl=318 length=len missover;
run;

proc printto new print='posts.list';

proc print data=post(obs=10) split='*';
   title "POST: First 10 obs";
   var C001 C020 C033 C036 C049 C057 C065 C071 C081 C091 C092 C093 C097 C106 C109
	C115 C121 C137 C143 C144 C157 C161 C162 C192 C205 C208 C209 C210 C222 C225
	C241 C242 C243 C245 C246 C247 C253 C263 C273 C274 C279 C284 C286 C288 C290
	C292 C300 C303 C318;
run;


***** AMOUNT BY CURRENCY & COUNTRY CODES *****;
data amount; set post(keep=C020 C033 C106);
  amount=C020+0;

proc sort data=amount; by C033;
proc univariate noprint; by C033;
  var amount;
  output out=curr n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=curr; by descending n;
proc printto new print='posts.amt_x_curr.list';
proc print ;
   title1 "Post Amounts by Currency Code";
   var C033 n mean median min max std p5 p25 p75 p95;
run;

proc sort data=amount; by C106;
proc univariate noprint; by C106;
  var amount;
  output out=merch n=n mean=mean median=median min=min max=max std=std p1=p1 p5=p5 p10=p10 q1=p25 q3=p75 p90=p90 p95=p95 p99=p99 ;
run;

proc sort data=merch; by descending n;
proc printto new print='posts.amt_x_country.list';
proc print ;
   title1 "Post Amounts by Merchant Country Code";
   var C106 n mean median min max std p5 p25 p75 p95;
run;

proc printto ;

%mend;

**********************************************************;

%macro read_inq /store des='Read Inquiries';


data inq
C001(keep=C001 length nbin)
C020(keep=C020)
C021(keep=C021)
C029(keep=C029)
C035(keep=C035)
C051(keep=C051)
C052(keep=C052)
C053(keep=C053)
;

  infile STDIN lrecl=53 length=len missover;

  input
     @1   C001  $19.     /* ACCOUNT ID */
     @20  C020  $1.      /* TYPE */
     @21  C021  $8.      /* DATE */
     @29  C029  $6.	 /* TIME */
     @35  C035  $16.	 /* ANI NUMBER */
     @51  C051  $1.	 /* CONNECT MODE */
     @52  C052  $1.	 /* RESPONSE MODE */
     @53  C053  $1.	 /* FRAUD TRX */
  ;

  length=len;
  nbin=substr(C001,1,6)*1;

run;

%mend;

**********************************************************;

%macro read_api /store des='Read API';


data api;

  infile STDIN lrecl=438 length=len missover;

  input
     @001  C001     $19.       /*      ACCOUNT NUMBER       1   */
     @020  APFLAG   $1.        /*      AUTH_POST_CODE       4   */
     @021  CARDZIP  $9.        /*      POSTCODE            29   */
     @030  CRDCNTY  $3.        /*      CARD_CNTRY_CODE     30   */
     @033  OPENDAT  $8.        /*      ACCOUNT_OPEN_DATE   17   */
     @041  NEWPDAT  $8.        /*      NEW_PLASTIC_DATE    21   */
     @049  ISSUTYP  $1.        /*      PLASTIC_ISSUE_TYPE  22   */
     @050  CMFEXPD  $8.        /*      ACCOUNT_EXPIRE_DATE 19   */
     @058  CDEXPDT  $8.        /*      CARD_EXPIRE_DATE    18   */
     @066  AVLCRED  $10.       /*      AVAIL_CREDIT         6   */
     @076  CRDLINE  $10.       /*      CREDIT_LINE         20   */
     @086  GENDER   $1.        /*      GENDER              27   */
     @087  BIRTHDAT $8.        /*      BIRTHDATE           25   */
     @095  NUMCARDS $3.        /*      NUM_CARDS           26   */
     @098  INCOME   $10.       /*      INCOME              28   */
     @108  CRDTYPE  $1.        /*      CARD_TYPE  should be C or D         23   */
     @109  CARDUSE  $1.        /*      CARD_USE            24   */
     @110  TRANDAT  $8.0       /*      TRAN_DATE            2   */
     @118  TRANTIM  $6.        /*      TRAN_TIME            2   */
     @124  TRANAMT  $13.       /*      TRAN_AMOUNT          5   */
     @137  CURRCD   $3.        /*      TRAN_CURR_CODE       7   */
     @140  CURRCONV $13.       /*      TRAN_CURR_CONV       8   */
     @153  DECISON  $1.        /*      AUTH_RESULT          9   */
     @154  TRANTYP  $1.        /*      TRAN_TYPE            3   */
     @155  SICCODE  $4.        /*      MERCH_CAT           31   */
     @159  MRCHZIP  $9.        /*      MERCH_POST_CODE     32   */
     @168  MRCHCNT  $3.        /*      MERCH_CNTRY_CODE    33   */
     @171  PINVRFY  $1.        /*      AUTH_PIN_VERIFY     10   */
     @172  CVVVRFY  $1.        /*      AUTH_CVV_VERIFY     11   */
     @173  KEYED    $1.        /*      AUTH_KEYED_SWIPED   12   */
     @174  POSTDAT  $8.        /*      POST_DATE           13   */
     @182  POSTACD  $1.        /*      POST_AUTHED CODE    14   */
     @183  MISIND   $1.        /*      FILLER                   */
     @184  CASCRIND $1.        /*      FILLER                   */
     @185  USRIND1  $1.        /*      FILLER                   */
     @186  USRIND2  $1.        /*      FILLER                   */
     @187  USRDAT1  $10.       /*      FILLER                   */
     @197  USRDAT2  $10.       /*      FILLER                   */
     @207  MERCHID  $10.       /*      FILLER                   */
     @217  MRCDATAV $1.        /*      FILLER                   */
     @218  CRDDATAV $1.        /*      MERCH_ID                 */
     @219  EXTSCR1  4.0        /*      MERCH_ID                 */
     @223  EXTSCR2  4.0        /*      MERCH_ID                 */
     @227  EXTSCR3  4.0        /*      MERCH_ID                 */
     @231  CUSPRIND $1.        /*      MERCH_ID                 */
     @232  CATTYP   $1.        /*      MERCH_ID                 */
     @293  MODEL    $8.0       /*      MODEL_ID            15   */
     ;

  length=len;
  nbin=substr(C001,1,6)*1;

run;

proc print data=api(obs=10) split='*';
   title "API: First 10 obs";
run;

%mend;



**** CALCULATE # OF OBSERVATIONS & RECORD LENGTH ******;

%macro nobs(dset) /store des='Count Observations';

%global num_rec num_uniq rec_len;


************** all obs ********************;

%let dsid    = %sysfunc(open(&dset));
%let num_rec = %sysfunc(attrn(&dsid,NOBS));
%let rc      = %sysfunc(close(&dsid));


************* unique obs ********************;
proc freq data=&dset;
  tables C001 / noprint out=tmp;
run;


%let dsid     = %sysfunc(open(tmp));
%let num_uniq = %sysfunc(attrn(&dsid,NOBS));
%let rc       = %sysfunc(close(&dsid));

proc datasets nolist library=work;
  delete tmp;
run;

*********** record length ******************;

proc univariate data=&dset noprint;
  var length;
  output out=tmp max=max min=min;
run;

data _null_; set tmp;
  call symput('minlen',min);
  call symput('maxlen',max);
run;

%if &minlen = &maxlen %then %let rec_len = %left(&minlen);
%else %if  &minlen ^= &maxlen %then %let rec_len =%left(&minlen) to %left(&maxlen);

proc datasets nolist library=work;
  delete tmp;
run;

%mend nobs;


******* STATS for ACCOUNT BINS *************;

%macro bins(dset) /store des='Bin Frequencies';

%global acct;
proc printto new print="&prefix..bins.list";

proc freq data=&dset noprint order=freq;
  tables nbin / missing out=tmp;
run;

proc univariate data=tmp noprint;
  var nbin;
  output out=out n=n max=max min=min;
run;

data _null_; set out;
  call symput('n',n);
  call symput('min',put(min,z6.));
  call symput('max',put(max,z6.));
run;  

proc print data=tmp(obs=10);
  title1 "Top 10 Account Bins";
run;

data _null_; set tmp;
  %do i=1 %to 3;
	if _n_=&i & percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||put(nbin,z6.));
	else if _n_=&i & percent<0.1 then call symput("freq&i","<0.1% = "||put(nbin,z6.));
  %end;
run;

%let acct = Number of unique bins: %left(&n) \par Range: %left(&min) to %left(&max) \par %left(&freq1) \par %left(&freq2) \par %left(&freq3);

proc datasets nolist library=work;
  delete out tmp;
run;

proc printto;

%mend;

********* COUNT OF MONTH BY BIN **********;

%macro binmonth(dset,datevar) /store des='Bin by Month';

proc printto new print="&prefix..bins_x_month.list";

**** BINS BY YEAR & MONTH FOR BINS WITH > 1000 CARDHOLDERS ****;
proc sort nodupkey data=&dset out=bins(keep=nbin C001 &datevar); by C001;
proc freq data=bins noprint;
tables nbin / out=nbinout;
run;

proc sort data=nbinout; where count>1000; by nbin;
proc sort data=&dset out=bins(keep=nbin &datevar); by nbin;

data newbins; merge bins(in=a) nbinout(in=b); by nbin;
  if a & b;
  month=substr(&datevar,1,6);
run;

proc chart data=newbins; 
  title1 "Bin by month analysis";
  hbar month / group=nbin freq;
run;

proc printto;

%mend;


********** STATS for CATEGORICAL VARIABLES *******************;

%macro catvar(dset,catvar,outvar,fmt,cond) /store des='Categorical Var analysis';

%global &outvar;

proc printto new print="&prefix..&outvar..list";

proc freq data=&dset noprint order=freq; &cond;
  tables &catvar / missing out=tmp;
run;


%let dsid  = %sysfunc(open(tmp));
%let n     = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc sort data=tmp ; by descending percent; run;

data _null_; set tmp;
   %do i=1 %to &n;
     if _n_=&i then do;
	if percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||trim(put(&catvar,&fmt)));
        else if percent<0.1 then call symput("freq&i","<0.1% = "||trim(put(&catvar,&fmt)));
     end;
   %end;
run;


%macro do_loop;

%if &n>1 %then %do;
     %do i=1 %to %eval(&n-1); &&freq&i \par %end;  &&freq&n
%end;

%else %if &n=1 %then %do;
     %left(%bquote(&freq1))
%end;

%mend;


%let &outvar = %do_loop;

%put &outvar = &&&outvar;

proc datasets nolist library=work;
  delete tmp;
run;

%mend;



********** STATS for DATE VARIABLES *******************;

%macro datevar(dset,datevar,outvar,cond,extra,pdate,fdate) /store des='Date Var Analysis';

%put start = &strt;
%put stop = &stp;

%global &outvar;
%local zeroes blank invalid min max numgaps past future;

proc printto new print="&prefix..&outvar..list";

proc format;
  value days 1='Sunday' 2='Monday' 3='Tuesday' 4='Wednesday'
	5='Thursday' 6='Friday' 7='Saturday';
run;
 

data tmp; set &dset;
  numdate=&datevar +0;
  date=input(&datevar,yymmdd8.);
  wdate=weekday(date);
  strdate=input("&strt",yymmdd8.);
  stpdate=input("&stp",yymmdd8.);
  length qdate $6; qdate=substr(&datevar,1,6);
  if date^=. then datecat=1; *** valid dates ***;
  else if &datevar='00000000' then datecat=2; *** zero dates ***;
  else if &datevar='' then datecat=3; *** missing dates ***;
  else datecat=4; *** other invalid dates ***;
  if datecat=1 & &datevar>"&stp" then future=1; *** future dates ***;
  if datecat=1 & &datevar<"&strt" then future=0; *** past dates ***;
run;



**** only some dates get charted ****;
%if &extra>0 %then %do;
**** print out stats to lst file ****;
proc chart data=tmp; where datecat=1;
  title1 "Distribution by Month -- &outvar";
  hbar qdate / type=percent missing space=0;
run;
%end;

***** if First Fraud Date, show graph of type by date *****;
%if &dset=fraud & &datevar=C021 %then %do;
proc sort data=tmp; by C020;
proc chart data=tmp; where datecat=1; by C020;
  title1 "Distribution by Month & Fraud Type -- &outvar";
  hbar qdate / type=percent missing space=0;
  format C020 $FType.;
run;
%end;


**** only some dates get weekday charts ****;
%if &extra>1 %then %do;
**** print out stats to lst file ****;
proc chart data=tmp; where datecat=1;
  title1 "Distribution by Weekday -- &outvar";
  format wdate days.;
  hbar wdate / type=percent midpoints=1 2 3 4 5 6 7 space=0;
run;
%end;


**** invalid values *****;
proc freq data=tmp order=freq noprint; where datecat=4;
tables &datevar /out=lstinv;
run;
proc print data=lstinv(obs=20) ; 
  title1 "Frequency of top 20 invalid dates -- &outvar";
run;


**** gaps in auth dates ****;
%if &outvar=authdate %then %do;
   data authtmp; infile 'authdates.tmp' lrecl=20;
   	input @1 count 10. @13 authdate yymmdd8.;
   proc sort; by authdate;


   proc univariate data=authtmp noprint;
	var count; 
	output out=authout std=std mean=mean;
   run;
   
   %local authmean authstd;
   data _null_; set authout;
  	call symput('authmean',trim(left(put(mean,12.))));
  	call symput('authstd',trim(left(put(std,8.2))));
   run;

   %put authmean = &authmean;
   %put authstd = &authstd;

   data authtmp; set authtmp; by authdate;
   	prevdate=lag1(authdate);
	diff=abs(&authmean-count);
	*** keep if days not consecutive (missing day);
   	if authdate-prevdate >1 then check=1;
	*** also keep if more than 3std from the mean;
   	else if diff>3*&authstd & prevdate^='.' then check=2;
	if check>0 then output;
   run;


   proc univariate data=authtmp noprint;
  	var authdate;
  	output out=numgaps n=n;
   run;


   data _null_; set numgaps;
  	call symput('numgaps',trim(left(put(n,3.))));
   run;

   %put numgaps=&numgaps;

   %if %eval(&numgaps+0) %then %do;
  	data _null_; set authtmp;
   	   %do i=1 %to &numgaps;
		if _n_=&i & check=1 then call symput("gap&i",'\par \b Gap from '||put(prevdate,yymmn6.)||put(day(prevdate),z2.)||' to '||put(authdate,yymmn6.)||put(day(authdate),z2.)||" \b0");
		else if _n_=&i & check=2 then call symput("gap&i",'\par \b Extreme obs. count on '||put(authdate,yymmn6.)||put(day(authdate),z2.)||" \b0");
     	   %end;
  	run;
   %end;

   %*put gap1=&gap1;
   %*put gap2=&gap2;

%end;


proc univariate data=tmp noprint; where numdate^=0;
  var numdate;
  output out=minimax min=min max=max;
run;

data _null_; set minimax;
  call symput('min',put(min,z8.));
  call symput('max',put(max,z8.));
run;

%put range= &min to &max;

proc freq data=tmp noprint; &cond;
  tables datecat / missing out=invalid;
run;


data _null_; set invalid;
  if datecat=2 & percent>=0.1 then call symput('zeroes',trim("\par "||trim(left(put(percent,4.1)))||"% = 00000000 "));
  else if datecat=2 & percent<0.1 then call symput('zeroes',trim("\par "||"<0.1% = 00000000 "));

  if datecat=3 & percent>=0.1 then call symput('blank',trim("\par "||trim(left(put(percent,4.1)))||"% = blank "));
  else if datecat=3 & percent<0.1 then call symput('blank',trim("\par "||"<0.1% = blank "));

  if datecat=4 & percent>=0.1 then call symput('invalid',trim("\par "||trim(left(put(percent,4.1)))||"% = invalid format "));
  else if datecat=4 & percent<0.1 then call symput('invalid',trim("\par "||"<0.1% = invalid format "));
run;


proc freq data=tmp noprint; &cond;
  tables future / missing out=future;
run;


data _null_; set future;
  if future=1 & percent>=0.1 then call symput('future',trim("\par "||trim(left(put(percent,4.1)))||"% = dates after &stp "));
  else if future=1 & percent<0.1 then call symput('future',trim("\par "||"<0.1% = dates after &stp "));

  if future=0 & percent>=0.1 then call symput('past',trim("\par "||trim(left(put(percent,4.1)))||"% = dates before &strt "));
  else if future=0 & percent<0.1 then call symput('past',trim("\par "||"<0.1% = dates before &strt "));
run;

%macro datelet;

%if &min^='' %then %do;
  %if &min^=&max %then %do;
  \par Range: %left(&min) to %left(&max) 
  %end;
  %else %if &min=&max & &min^='' %then %do;
  \par 100% = %left(&min) 
  %end;
%end;

%if &zeroes^='' %then %do; %left(&zeroes) %end;
%if &blank^='' %then %do; %left(&blank) %end;
%if &invalid^='' %then %do; %left(&invalid) %end;
%if &pdate=1 %then %do;
   %if &past^='' %then %do; %left(&past) %end;
%end;
%if &fdate=1 %then %do;
   %if &future^='' %then %do; %left(&future) %end;
%end;
%if %eval(&numgaps+0) %then %do;
	%do i=1 %to &numgaps;
		%left(&&gap&i)
	%end;
%end;

%mend;


%let &outvar = %substr(%datelet,5);


proc datasets nolist library=work;
  delete tmp invalid minimax authtmp numgaps;
run;

proc printto ;

%mend;



********** STATS for TIME VARIABLES *******************;

%macro timevar(dset,timevar,outvar) /store des='Time Var Analysis';

%global &outvar;
%local blank invalid;
proc printto new print="&prefix..&outvar..list";

data tmp; set &dset;
  hour=substr(&timevar,1,2)*1; min=substr(&timevar,3,2)*1; sec=substr(&timevar,5,2)*1;
  newtime=put(hour,z2.)||":"||put(min,z2.)||":"||put(sec,z2.);
  time=input(newtime,time8.);
  if time^=. then timecat=1; *** valid times ***;
  else if &timevar='' then timecat=2; *** missing times ***;
  else timecat=3; *** other invalid times ***;
run;


**** print out stats to lst file ****;
proc chart data=tmp; where timecat=1;
  title1 "Distribution by Hour -- &outvar";
  hbar hour / type=percent missing midpoints=0 to 23 by 1 space=0;
run;


**** invalid values *****;
proc freq data=tmp order=freq noprint; where timecat=3;
tables &timevar /out=lstinv;
run;
proc print data=lstinv(obs=20); 
  title1 "Frequency of top 20 invalid times -- &outvar";
run;


proc freq data=tmp noprint;
  tables timecat / missing out=invalid;
run;


data _null_; set invalid;
  if timecat=2 & percent>=0.1 then call symput('blank',"\par "||trim(left(put(percent,4.1)))||"% = blank ");
  else if timecat=2 & percent<0.1 then call symput('blank',"\par "||"<0.1% = blank ");

  if timecat=3 & percent>=0.1 then call symput('invalid',"\par "||trim(left(put(percent,4.1)))||"% = invalid format ");
  else if timecat=3 & percent<0.1 then call symput('invalid',"\par "||"<0.1% = invalid format ");
run;


%macro timelet;

%if &blank^='' %then %do; %left(&blank) %end;
%if &invalid^='' %then %do; %left(&invalid) %end;

%if &blank=%str() & &invalid=%str( ) %then %do; %left(\par Format OK) %end;

%mend;


%let &outvar = %substr(%timelet,5);
%put outvar= &&outvar;

proc datasets nolist library=work;
  delete tmp invalid ;
run;

proc printto;

%mend;


********** STATS for NUMERIC VARIABLES *******************;

%macro numvar(dset,numvar,outvar,cond) /store des='Numerical Var Analysis';

%global &outvar;
%local median blank invalid min max;
proc printto new print="&prefix..&outvar..list";

data tmp; set &dset; &cond;
  numvar=&numvar +0;
  if numvar^=. then numcat=1; *** valid numbers ***;
  else if &numvar='' then numcat=2; *** missing numbers ***;
  else numcat=3; *** other invalid numbers ***;
run;


**** invalid values *****;
proc freq data=tmp order=freq noprint; where numcat=3;
tables &numvar /out=lstinv;
run;
proc print data=lstinv(obs=20); 
  title1 "Frequency of top 20 invalid numbers -- &outvar";
run;



proc univariate data=tmp noprint; 
  var numvar;
  output out=minimax min=min max=max median=median;
run;

data _null_; set minimax;
  call symput('min',min);
  call symput('max',max);
  if median^=. then call symput('median',left(median));
run;

%put range= &min to &max median=&median;

proc freq data=tmp noprint;
  tables numcat / missing out=invalid;
run;


data _null_; set invalid;
  if numcat=2 & percent>=0.1 then call symput('blank',"\par "||trim(left(put(percent,4.1)))||"% = blank ");
  else if numcat=2 & percent<0.1 then call symput('blank',"\par "||"<0.1% = blank ");

  if numcat=3 & percent>=0.1 then call symput('invalid',"\par "||trim(left(put(percent,4.1)))||"% = invalid format ");
  else if numcat=3 & percent<0.1 then call symput('invalid',"\par "||"<0.1% = invalid format ");
run;

%put median = &median;
%put min = &min;
%put max = &max;
%put blank = &blank;
%put invalid = &invalid;


%macro numlet;

%if "&min" ^= "&max" %then %do;
\par Range: %left(&min) to %left(&max) 
%end;

%else %if "&min"="&max" & &min^=%str(.) %then %do;
\par 100% = %left(&min) 
%end;

%if &median^=  & "&min"^="&max" %then %do; 
\par Median = %left(&median)
%end;

%if &blank^='' %then %do; %left(&blank) %end;
%if &invalid^='' %then %do; %left(&invalid) %end;

%mend;


%let &outvar = %substr(%numlet,5);

%put &outvar = &&&outvar;

proc datasets nolist library=work;
  delete tmp invalid minimax;
run;

proc printto;

%mend;


********** STATS for POSTAL CODES *******************;

%macro postal(dset,postvar,outvar,cond) /store des='Postal Var Analysis';

%global &outvar;
%local blank zeroes;
proc printto new print="&prefix..&outvar..list";

data tmp; set &dset;
  post=substr(&postvar,1,5);
  if post='' then postcat=1; *** missing codes ***;
  else if post='00000' then postcat=2; *** zeroes ***;
  else postcat=3; *** valid values ***;
run;

proc freq data=tmp noprint; &cond;
  tables postcat / missing out=invalid;
run;


data _null_; set invalid;
  if postcat=1 & percent>=0.1 then call symput('blank',"\par "||trim(left(put(percent,4.1)))||"% = blank ");
  else if postcat=1 & percent<0.1 then call symput('blank',"\par "||"<0.1% = blank ");

  if postcat=2 & percent>=0.1 then call symput('zeroes',"\par "||trim(left(put(percent,4.1)))||"% = 00000 ");
  else if postcat=2 & percent<0.1 then call symput('zeroes',"\par "||"<0.1% = 00000 ");
run;

%macro postlet;

%if &blank^='' %then %do; %left(&blank) %end;
%if &zeroes^='' %then %do; %left(&zeroes) %end;

%if &blank=%str() & &zeroes=%str() %then %do; %left(\parFormat OK) %end;

%mend;


%let &outvar = %substr(%postlet,5);
%put postal = &postal;


proc datasets nolist library=work;
  delete tmp invalid ;
run;

proc printto;

%mend;


********** STATS for CATEGORICAL VARIABLES with MANY CATEGORIES *******************;

%macro manyvar(dset,catvar,outvar,fmt,numcat,cond) /store des='Multiple Category Analysis';

%global &outvar;
%local x;
proc printto new print="&prefix..&outvar..list";

proc freq data=&dset noprint order=freq; &cond;
  tables &catvar / missing out=tmp;
run;


%let dsid  = %sysfunc(open(tmp));
%let n     = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc sort data=tmp ; by descending percent; run;

*** use &numcat unless &n<&numcat;
%if &numcat>&n %then %let x=&n;
%else %let x=&numcat;

%put n=&n, numcat=&numcat, x=&x;


data _null_; set tmp;
   %do i=1 %to &x;
     if _n_=&i & percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||trim(put(&catvar,&fmt)));
     else if _n_=&i & percent<0.1 then call symput("freq&i","<0.1% = "||trim(put(&catvar,&fmt)));
   %end;
run;


%macro do_loop;

%if &x>1 %then %do;
     %do i=1 %to %eval(&x-1); %bquote(&&freq&i) \par %end; %bquote(&&freq&x)
%end;

%else %if &x=1 %then %do;
     %left(%bquote(&freq1))
%end;

%mend;


%let &outvar = %bquote(%do_loop);

proc datasets nolist library=work;
  delete tmp;
run;

proc printto;

%mend;




********** BLANK COUNTS *******************;

%macro blanks(dset,catvar,outvar) /store des='Blank Frequency';

%global &outvar;
proc printto new print="&prefix..&outvar..list";

proc freq data=&dset noprint ; 
  tables &catvar / missing out=tmp;
run;

proc sort data=tmp; by &catvar;

data _null_; set tmp;
     if _n_=1 & &catvar='' & percent>=0.1 then call symput("&outvar",trim(left(put(percent,4.1)))||"% = blank ");
     else if _n_=1 & &catvar='' & percent<0.1 then call symput("&outvar","<0.1% = blank");
     else %let &outvar = Format OK;;
run;


proc datasets nolist library=work;
  delete tmp;
run;
proc printto;

%mend;

********************** STATS for TERM-TYPE and TERM-ENTRY **********************;

%macro termII(dset,catvar1,catvar2,outvar,fmt1,fmt2,cond) /store des='Term-type/term-entry analysis';

%global &outvar;

proc printto new print="&prefix..&outvar..list";

proc freq data=&dset noprint order=freq; &cond;
  tables &catvar2 / missing out=tmp;
run;


%let dsid  = %sysfunc(open(tmp));
%let n     = %sysfunc(attrn(&dsid,nobs));
%let rc    = %sysfunc(close(&dsid));

proc sort data=tmp ; by descending percent; run;

data _null_; set tmp;
%if %substr(&catvar1,1,1)='4' %then %do;
   %do i=1 %to &n;
     if _n_=&i then do;
	if percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||trim(put(&catvar2,&fmt1)));
        else if percent<0.1 then call symput("freq&i","<0.1% = "||trim(put(&catvar2,&fmt1)));
     end;
   %end;
%end;
%else %if %substr(&catvar1,1,1)='5' %then %do;
  %do i=1 %to &n;
     if _n_=&i then do;
	if percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||trim(put(&catvar2,&fmt2)));
        else if percent<0.1 then call symput("freq&i","<0.1% = "||trim(put(&catvar2,&fmt2)));
     end;
   %end;
%end;
%else %do;
  %do i=1 %to &n;
     if _n_=&i then do;
	if percent>=0.1 then call symput("freq&i",trim(left(put(percent,4.1)))||"% = "||trim(put(&catvar2,$1.)));
        else if percent<0.1 then call symput("freq&i","<0.1% = "||trim(put(&catvar2,$1.)));
     end;
   %end;
%end;   
run;


%macro do_loop;

%if &n>1 %then %do;
     %do i=1 %to %eval(&n-1); &&freq&i \par %end;  &&freq&n
%end;

%else %if &n=1 %then %do;
     %left(%bquote(&freq1))
%end;

%mend;


%let &outvar = %do_loop;

%put &outvar = &&&outvar;

proc datasets nolist library=work;
  delete tmp;
run;

%mend;



