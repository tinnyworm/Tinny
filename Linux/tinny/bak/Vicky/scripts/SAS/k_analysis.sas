/**************************************************************
        Discover Analysis from API data
	6/2003 	krh

**************************************************************/
libname fraud_in '/work/fred/AEG/krh/Discover/sensitivity_analysis_200306/API_DATA';
options
        nocenter
        linesize=80
        pagesize=60
        obs=max
        errors=1000
        compress=yes;

GOPTIONS  DEVICE=XCOLOR;

/**************************************************************/
/* Formats                                                    */
/**************************************************************/
data cntlacct (rename=(ACCOUNT_NUM=start TYPE=label));
  set fraud_in.frauds (keep=ACCOUNT_NUM TYPE);
  fmtname='frdtype';
run;
proc format cntlin=cntlacct;
run;
proc format;
  value am1_fmt
    low - < 0.    = 'Negative'
    0.00          = 0000
    0.01  - 10.   = 0010
    10.01 - 20.   = 0020
    20.01 - 30.   = 0030
    30.01 - 40.   = 0040
    40.01 - 50.   = 0050
    50.01 - 60.   = 0060
    60.01 - 70.   = 0070
    70.01 - 80.   = 0080
    80.01 - 90.   = 0090
    90.01 - 100.  = 0100
    100.01 - 150. = 0150
    150.01 - 200. = 0200
    200.01 - 250. = 0250
    250.01 - 300. = 0300
    300.01 - 350. = 0350
    350.01 - 400. = 0400
    400.01 - 450. = 0450
    450.01 - 500. = 0500
    500.01 - 600. = 0600
    600.01 - 700. = 0700
    700.01 - 800. = 0800
    800.01 - 900. = 0900
    900.01 - 1000. = 1000
    1000.01 - 2000. = 2000
    2000.01 - 3000. = 3000
    3000.01 - 4000. = 4000
    4000.01 - 5000. = 5000
    5000.01 - high = '5000++'
  ;

proc format;
  value $sic_fmt
    '6011' = '6011'
    '6010' = '6010'
    other  = 'Other'
  ;

proc format;
  value $miss_fmt
    ' ' = 'Blank'
    other = 'OK'
  ;
  
proc format;
  value t_f
     1='True'
     0='False'
     ;
run;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data api;

  infile STDIN
  lrecl=185
  missover;

  input
    ACCOUNT_NUMBER $	1 - 19
    AUTH_POST_FLAG $	20 - 20
    POSTCODE $		21 - 29
    CARD_CNTRY_CODE $ 	30 - 32
    ACCOUNT_OPEN_DATE $	33 - 40
    NEW_PLASTIC_DATE $	41 - 48
    PLASTIC_ISSUE_TYPE $	49 - 49
    ACCOUNT_EXPIRE_DATE $	50 - 57
    CARD_EXPIRE_DATE $	58 - 65
    AVAIL_CREDIT $	66 - 75
    CREDIT_LINE $	76 - 85
    GENDER $		86 - 86
    BIRTHDATE $		87 - 94
    NUM_CARDS $		95 - 97
    INCOME $		98 - 107
    CARD_TYPE $		108 - 108
    CARD_USE $		109 - 109
    TRAN_DATE $		110 - 117
    TRAN_TIME $		118 - 123
    TRAN_AMOUNT$	124 - 136
    TRAN_CURR_CODE $	137 - 139
    TRAN_CURR_CONV $	140 - 152
    AUTH_RESULT $	153 - 153
    TRAN_TYPE $		154 - 154
    MERCH_CAT $		155 - 158
    MERCH_POST_CODE $	159 - 167
    MERCH_CNTRY_CODE $	168 - 170
    AUTH_PIN_VERIFY $ 	171 - 171
    AUTH_CVV_VERIFY $ 	172 - 172
    AUTH_KEYED_SWIPED $	173 - 173
    POST_DATE $		174 - 181
    FRAUD_TAG $         182-182
  ;
  ACCOUNT_EXPIRE_MONTH = substr(ACCOUNT_EXPIRE_DATE,1,6);
  CARD_EXPIRE_MONTH = substr(CARD_EXPIRE_DATE,1,6);
  EXPIRE_MONTH_MATCH = ((ACCOUNT_EXPIRE_MONTH=CARD_EXPIRE_MONTH)
                        or (ACCOUNT_EXPIRE_MONTH='      ')
                        or (CARD_EXPIRE_MONTH='      '));
  EXPIRE_MONTH_DIFF = CARD_EXPIRE_MONTH - ACCOUNT_EXPIRE_MONTH;
  FRAUD_TYPE = put(input(ACCOUNT_NUMBER,19.0),frdtype.);
run;


  /************************************************************/
  /* Data Analysis                                            */
  /************************************************************/
proc freq data=api;
  format EXPIRE_MONTH_MATCH t_f.;
  title 'Expire Month Match by Transaction Type';
  tables EXPIRE_MONTH_MATCH*TRAN_TYPE;
run;
proc freq data=api;
  format EXPIRE_MONTH_MATCH t_f.;
  title 'Expire Month Match by Transaction Type For Convenience Check Fraud Type';
  where FRAUD_TYPE = 'K';
  tables EXPIRE_MONTH_MATCH*TRAN_TYPE;
run;
proc freq data=api;
  title 'Card Expire Month for all Convenience Check Transactions';
  where TRAN_TYPE = 'K';
  tables CARD_EXPIRE_MONTH;
run;
proc freq data=api;
  title 'Card Expire Month for all Balance Transfers';
  where TRAN_TYPE = 'T';
  tables CARD_EXPIRE_MONTH;
run;   
proc freq data=api;
  title 'Transaction Type by Fraud Tag';
  tables TRAN_TYPE*FRAUD_TAG;
run;

endsas;

