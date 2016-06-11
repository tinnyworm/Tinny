/**************************************************************/
/* FALCON                                                     */
/*         Generic Consortium Data Reader, Version ???        */
/*                                                            */
/*         Authorizations                                     */
/*

$Source: /work/price/falcon/dvl/sas/RCS/ufal_readauth.sas,v $

$Log: ufal_readauth.sas,v $
# Revision 1.5  1997/06/25  19:53:40  tjh
# removed accttype field (not in auth spec)
# changed format of FALSCORE to $4 from $10
# changed format of cskbkamt to $13 from $10
#
# Revision 1.4  1996/12/06  18:10:08  tjh
# Remove a proc sort that was accidently included
#
# Revision 1.3  1996/12/06  05:48:26  tjh
# Keep only 1 data set to minimize space needs TJH 12-5-96
#
# Revision 1.2  1996/10/02  05:56:56  tjh
# change to STDIN/STDOUT instead of INPUT/OUTPUT
# Typo fixes on hour and precent/percent
#
# Revision 1.1  1996/09/30  18:50:23  kmh
# Initial revision
#
# Revision 1.1  1996/09/30  18:48:15  kmh
# Initial revision
#

*/

/**************************************************************/

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
/* Bin the $ amounts                                          */
/**************************************************************/


proc format;
   value am2_bin
   00000.01-100.00=000100
   00100.01-200.00=000200
   00200.01-300.00=000300
   00300.01-400.00=000400
   00400.01-500.00=000500
   00500.01-600.00=000600
   00600.01-700.00=000700
   00700.01-800.00=000800
   00800.01-900.00=000900
   0900.01-1000.00=001000
   1000.01-1100.00=001100
   1100.01-1200.00=001200
   1200.01-1300.00=001300
   1300.01-1400.00=001400
   1400.01-1500.00=001500
   1500.01-1600.00=001600
   1600.01-1700.00=001700
   1700.01-1800.00=001800
   1800.01-1900.00=001900
   1900.01-2000.00=002000
   2000.01-2100.00=002100
   2100.01-2200.00=002200
   2200.01-2300.00=002300
   2300.01-2400.00=002400
   2400.01-2500.00=002500
   2500.01-2600.00=002600
   2600.01-2700.00=002700
   2700.01-2800.00=002800
   2800.01-2900.00=002900
   2900.01-3000.00=003000
   3000.01-3200.00=003200
   3200.01-3400.00=003400
   3400.01-3600.00=003600
   3600.01-3800.00=003800
   3800.01-4000.00=004000
   4000.01-4200.00=004200
   4200.01-4400.00=004400
   4400.01-4600.00=004600
   4600.01-4800.00=004800
   4800.01-5000.00=005000
   5000.01-6000.00=006000
   6000.01-7000.00=007000
   7000.01-8000.00=008000
   8000.01-9000.00=009000
 09000.01-10000.00=010000
 10000.01-11000.00=011000
 11000.01-12000.00=012000
 12000.01-13000.00=013000
 13000.01-14000.00=014000
 14000.01-15000.00=015000
 15000.01-16000.00=016000
 16000.01-17000.00=017000
 17000.01-18000.00=018000 
 18000.01-19000.00=019000
19000.01-100000.00=999999
     other         =000000
;
run;

proc format;
   value am1_bin
       0.01-10.00 = 0010
      10.01-20.00 = 0020
      20.01-30.00 = 0030
      30.01-40.00 = 0040
      40.01-50.00 = 0050
      50.01-60.00 = 0060
      60.01-70.00 = 0070
      70.01-80.00 = 0080
      80.01-90.00 = 0090
     090.01-100.00 = 0100
     100.01-110.00 = 0110
     110.01-120.00 = 0120
     120.01-130.00 = 0130
     130.01-140.00 = 0140
     140.01-150.00 = 0150
     150.01-160.00 = 0160
     160.01-170.00 = 0170
     170.01-180.00 = 0180
     180.01-190.00 = 0190
     190.01-200.00 = 0200
     200.01-210.00 = 0210
     210.01-220.00 = 0220
     220.01-230.00 = 0230
     230.01-240.00 = 0240
     240.01-250.00 = 0250
     250.01-260.00 = 0260
     260.01-270.00 = 0270
     270.01-280.00 = 0280
     280.01-290.00 = 0290
     290.01-300.00 = 0300
     300.01-350.00 = 0350
     350.01-400.00 = 0400
     400.01-450.00 = 0450
     450.01-500.00 = 0500
     500.01-600.00 = 0600
     600.01-700.00 = 0700
     700.01-800.00 = 0800
     800.01-900.00 = 0900
     900.01-1000.00 = 1000
     1000.01-1000000.00 = 9999
          other = 0000
;
run;     

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data xyz (compress=yes);

infile STDIN /*recfm=f lrecl=147*/ missover;


input
 
     @1   ACCOUNT       $19.      /* ACCOUNT ID */
     @20  AUTHDATE      $8.       /* DATE */
     @28  AUTHTIME      $6.       /* TIME */
     @34  AMOUNT        $13.      /* AMOUNT */
     @47  CURRCODE      $3.       /* CURRENCY CODE */
     @50  CURRRATE      $13.      /* CURRENCY RATE */
     @63  AUTHDECI      $1.       /* DECISION */
     @64  TRANTYPE      $1.	  /* TRANSACTION TYPE */
     @65  AVAILBAL      $10.      /* AVAILABLE CREDIT */
     @75  TOTALBAL      $10.      /* CREDIT LINE */
     @85  SICCODE       $4.       /* SIC CODE */
     @89  POSTCODE      $9.       /* POSTAL CODE */
     @98  MRCHCNTY      $3.       /* MERCHANT COUNTRY */
     @101 PINVER        $1.       /* PIN VERIFICATION */
     @102 AUTHCVV       $1.       /* CVV */
     @103 KEYSWIPE      $1.	  /* KEY SWIPE */
     @104 EXPDATE       $8.	  /* EXPIRATION DATE */
     @112 AUTHID        $6.	  /* AUTHORIZATION ID */
     @118 REASON        $5.       /* REASON CODE */
     @123 ADVICE        $1.       /* ASSOC ADVICE */
     @124 MRCHID        $16.      /* MERCHANT ID */
     @140 TERMID        $6.       /* TERMINAL ID */
     @146 WHICHCRD      $1.       /* WHICH CARD */
     @147 FALSCORE      $4.       /* FALCON SCORE */
;

hour = substr(authtime,1,2);

if (_error_) then
   do;
      file STDERR;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
   end;

format aut_date date7.;
aut_mnth = substr(authdate,5,2);

aut_date = input(authdate, yymmdd8.);
aut_time = DHMS( aut_date, substr(authtime,1,2)*1.,
                           substr(authtime,3,2)*1.,
                           substr(authtime,5,2)*1. );
au_time = datepart(aut_time);
aut_day   = weekday(au_time);

n_amt = amount*1.0;
amtbin = put(n_amt, am1_bin.);
n_avlbal = availbal*1.0;
avlbabin = put(n_avlbal, am2_bin.);
n_ttlbal = totalbal*1.0;
ttlbabin = put(n_ttlbal, am2_bin.);
n_falscr = falscore*1.0;
sub_post = substr(postcode,1,3);

keep account aut_mnth authdate aut_date authtime aut_day hour amtbin currcode
     currrate authdeci trantype avlbabin n_ttlbal ttlbabin siccode sub_post mrchcnty
     pinver authcvv keyswipe expdate authid reason advice mrchid termid
     whichcrd n_falscr ;

run;

/*************************************************************/
/* Print sample output records                               */
/*************************************************************/ 

proc print data=xyz(obs=40);
     var account aut_mnth authtime aut_day hour amtbin currcode currrate
     authdeci trantype avlbabin ttlbabin siccode sub_post mrchcnty pinver
     authcvv keyswipe expdate authid reason advice mrchid termid whichcrd
     n_falscr ;
run;


/**************************************************************/
/* Data Analysis                                              */
/**************************************************************/


proc chart data=xyz;
     hbar aut_mnth /type=percent missing;
     title 'Month of Authorization';
run;
proc chart data=xyz;
     hbar aut_day /type=percent missing midpoints=1 2 3 4 5 6 7;
     title 'Authorization by Day of Week';
run;
proc chart data=xyz;
     hbar hour /type=percent missing;
     title 'Authorization by Time of Day';
run;

proc chart data=xyz;
     hbar amtbin /Type=percent missing;
     title 'Amount Authorized';
run;

proc chart data=xyz;
     hbar authdeci /type=percent missing;
     title 'Approved/Decline From Authorization System';
run;
 
proc chart data=xyz;
     hbar trantype /type=percent missing;
     title 'Transaction Type';
run;
proc chart data=xyz;
     hbar avlbabin /type=percent missing;
     title 'Available Credit/Available Balance';
run;
proc freq data=xyz;
     tables n_ttlbal / missing;
     title 'Credit Line/Total Balance';
run;
proc chart data=xyz;
     hbar ttlbabin /type=percent missing;
     title 'Credit Line/Total Balance';
run;
proc chart data=xyz;
     hbar siccode /type=percent missing;
     title 'SIC Code';
run;
proc chart data=xyz;
     hbar sub_post /type=percent missing;
     title 'First Three digits of Postal code';
run;
proc chart data=xyz;
     hbar mrchcnty /type=percent missing;
     title 'Merchant Country Code';
run;
proc chart data=xyz;
     hbar pinver /type=percent missing;
     title 'PIN Verification';
run;
proc chart data=xyz;
     hbar authcvv /type=percent missing;
     title 'CVV/CVC Outcome';
run;
proc chart data=xyz;
     hbar keyswipe /type=percent missing;
     title 'Keyed vs Swiped';
run;
proc chart data=xyz;
     hbar whichcrd /type=percent missing;
     title 'Which Card';
run;
proc freq data=xyz;
     tables n_falscr / missing;
     title 'Falcon Score';
run;

proc freq data=xyz;
     tables expdate / missing;
     title 'Auth Message Expiration Date';
run; 

proc chart data=xyz;
     hbar aut_date /type=percent discrete;
     title 'Authorization Date';
run; 

proc freq data=xyz;
     tables authdate / missing;
     title 'Authorization Date';
run; 

endsas;
