%macro transfer(ifile, ofile);
%let ifile=/work/nobackup1_tmp1/jaccs/datas/&ifile;
%let ofile=/work/nobackup1_tmp1/jaccs/temp/&ofile;
filename ifile pipe "gunzip -c &ifile";
filename ofile pipe "gzip > &ofile"; 

data temp;
/*infile "&ifile"; */
infile ifile;
input     @1   ACCT_ID             $19.
          @20  pre_DATE            yymmdd8.
          @28  TIME                  6.
          @34  AMOUNT               13.
          @47  CURR_CODE             3.
          @50  CURR_RATE            13.
          @63  DECISION             $1.
          @64  TRAN_TYPE            $1.
          @65  AVAIL_CRED           10.
          @75  CRED_LINE            10.
          @85  SIC                   4.
          @89  POSTAL_CODE           9.
          @98  MRCH_CNTRY            3.
          @101 PIN_VER              $1.
          @102 CVV                  $1.
          @103 KEY_SWIPE            $1.
          @104 pre_MSG_EXPIRE_DATE yymmdd8.
          @112 AUTH_ID              $6.
          @118 REASON_CODE          $5.
          @123 ADVICE               $1.
          @124 MRCH_ID             $16.
          @140 TERM_ID              $6.
          @146 WHICH_CARD           $1.
          @147 CASHBACK_AMT         13.
          @160 FALCON_SCORE          4.
          @164 ITEM_CD               3.
          @167 ADDR_CD              15.
          @182 FILLER               1.;
count+1;
temp_date=input(put(pre_date, yymmdd10.), $10.);
temp_MSG_EXPIRE_DATE=input(put(pre_MSG_EXPIRE_DATE, yymmdd10.), $10.);
length Date MSG_EXPIRE_DATE $ 8;
substr(DATE, 1,4)=substr(temp_date, 1,4);
substr(Date, 5,2)=substr(temp_date, 6,2);
substr(date, 7,2)=substr(temp_date, 9,2);
substr(MSG_EXPIRE_DATE, 1,4)=substr(temp_MSG_EXPIRE_DATE, 1,4);
substr(MSG_EXPIRE_DATE, 5,2)=substr(temp_MSG_EXPIRE_DATE, 6,2);
substr(MSG_EXPIRE_DATE, 7,2)=substr(temp_MSG_EXPIRE_DATE, 9,2);
drop pre_date pre_MSG_EXPIRE_DATE temp_date temp_MSG_EXPIRE_DATE;
run;

data _null_ ;
set temp;
by count;
do x=1 while (x le last.count);
x+1;
ADDR_TERM_ID=repeat('0',14); end;
format ADDR_TERM_ID $14.;
file ofile;
put   (ACCT_ID) ($19. _char_) DATE (TIME) (6.) (AMOUNT) (13.) (CURR_CODE) (3.)
      (CURR_RATE) (13.) (DECISION) ($1.) (TRAN_TYPE) ($1.) (AVAIL_CRED)(10.)
      (CRED_LINE) (10.) (SIC) (4.) (POSTAL_CODE) (9.) (MRCH_CNTRY) (3.) (PIN_VER) ($1.)
      (CVV) ($1.) (KEY_SWIPE) ($1.) MSG_EXPIRE_DATE (AUTH_ID) ($6.)
      (REASON_CODE) ($5.) (ADVICE) ($1.) (MRCH_ID) ($16.) (TERM_ID) ($6.) (WHICH_CARD) ($1.)
      (CASHBACK_AMT) (13.) (FALCON_SCORE) (4.) (ITEM_CD) (3.) (ADDR_CD) (15.) (ADDR_TERM_ID) ($14.);
run;

%mend transfer;

%transfer(auth.200104.FAUT02.1.00195.dat.gz, auth.200104.FAUT02.1.00195.dat.gz)
%transfer(auth.200104.FAUT01.1.00195.dat.gz, auth.200104.FAUT01.1.00195.dat.gz);
%transfer(auth.200103.FAUT02.1.00195.dat.gz, auth.200103.FAUT02.1.00195.dat.gz);
%transfer(auth.200103.FAUT01.1.00195.dat.gz, auth.200103.FAUT01.1.00195.dat.gz);
%transfer(auth.200102.FAUT02.1.00195.dat.gz, auth.200102.FAUT02.1.00195.dat.gz);
%transfer(auth.200102.FAUT01.1.00195.dat.gz, auth.200102.FAUT01.1.00195.dat.gz);
%transfer(auth.200101.FAUT02.1.00195.dat.gz, auth.200101.FAUT02.1.00195.dat.gz);
%transfer(auth.200101.FAUT01.1.00195.dat.gz, auth.200101.FAUT01.1.00195.dat.gz);
%transfer(auth.200012.02.1.00195.dat.gz, auth.200012.02.1.00195.dat.gz);
%transfer(auth.200012.01.1.00195.dat.gz, auth.200012.01.1.00195.dat.gz);
%transfer(auth.200011.02.1.00195.dat.gz, auth.200011.02.1.00195.dat.gz);
%transfer(auth.200011.01.1.00195.dat.gz, auth.200011.01.1.00195.dat.gz);
