/**************************************************************
 FALCON                                                     
         Generic Consortium Data Reader, Version ???        
                                                            
         Frauds                                             

$Source: /work/door/falcon/dvl/sas/RCS/ufal_readfraud.sas,v $

$Log: ufal_readfraud.sas,v $
# Revision 1.2  1996/12/27  23:52:32  jcb
# added bins for difference in days detected - date first fraud
#
# Revision 1.1  1996/09/30  18:49:59  kmh
# Initial revision
#
# Revision 1.1  1996/09/30  18:49:59  kmh
# Initial revision
#
# Revision 1.1  1996/09/30  18:48:53  kmh
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

proc format;
     value $tim_bin 
     '000001'-'010000' = '01'
     '010001'-'020000' = '02'
     '020001'-'030000' = '03'
     '030001'-'040000' = '04'
     '040001'-'050000' = '05'
     '050001'-'060000' = '06'
     '060001'-'070000' = '07'
     '070001'-'080000' = '08'
     '080001'-'090000' = '09'
     '090001'-'100000' = '10'
     '100001'-'110000' = '11'
     '110001'-'120000' = '12'
     '120001'-'130000' = '13'
     '130001'-'140000' = '14'
     '140001'-'150000' = '15'
     '150001'-'160000' = '16'
     '160001'-'170000' = '17'
     '170001'-'180000' = '18'
     '180001'-'190000' = '19'
     '190001'-'200000' = '20'
     '200001'-'210000' = '21'
     '210001'-'220000' = '22'
     '220001'-'230000' = '23'
     '230001'-'235959' = '24'
      other            = '00'
;
run;
     
/***************************************************************/
/* Bin the Number of Days 				       */
/***************************************************************/
 
proc format;
   value day_bin
     0-0 = 000
     1-5 = 005
     6-10 = 010
     11-15 = 015
     16-20 = 020
     21-25 = 025
     26-30 = 030
     31-35 = 035
     36-40 = 040
     41-45 = 045
     46-50 = 050
     51-60 = 060
     61-70 = 070
     71-80 = 080
     81-90 = 090
     90-100 = 100
     101-120 = 120
     121-140 = 140
     141-160 = 160
     161-180 = 180
     181-200 = 200
     201-220 = 220
     221-240 = 240
     241-260 = 260
     261-280 = 280
     281-300 = 300
     301-320 = 320
     321-340 = 340
     341-360 = 360
     361-900 = 900
     other = 999
;
run;

%let dsname = fraud;                  /* DataSet Name */

/**************************************************************/
/* Input                                                      */
/**************************************************************/

data &dsname;

infile STDIN /*recfm=f lrecl=29*/;


input
     @1   ACCOUNT       $19.     /* ACCOUNT ID */
     @20  FRAUDTYP      $1.      /* TYPE */
     @21  FRDDATE       $8.      /* FRAUD DATE */
     @29  FFIND         $1.      /* FRAUD FIND METHOD */
     @30  FRDTIME       $6.	 /* FRAUD TIME */
     @36  DETDATE       $8. 	 /* FRAUD DETECTION DATE */
     @44  DETTIME       $6.      /* FRAUD DETECION TIME */
;

frdtmday = put(frdtime, $tim_bin.);

dettmday = put(dettime, $tim_bin.);

acct_bin = substr(account,1,6);

if (_error_) then
   do;
      file print;
      put 'Error in Record Number' _n_;
      put _infile_;
      delete;
   end;

run;

data xyz;
     set &dsname;

format frd_date date7.;
format frd_time datetime17.;
frd_date = input(frddate, yymmdd8.);
frd_mnth = substr(frddate,1,6);

frd_time = DHMS( frd_date, substr(frdtime,1,2)*1.,
                           substr(frdtime,3,2)*1.,
                           substr(frdtime,5,2)*1. );
fr_time = datepart(frd_time);
frd_day   = weekday(fr_time);

format det_date date7.;
format det_time datetime17.;
det_date = input(detdate, yymmdd8.);
det_time = DHMS( det_date, substr(dettime,1,2)*1.,
                           substr(dettime,3,2)*1.,
                           substr(dettime,5,2)*1. );
det_mnth = month(det_date);
de_time = datepart(det_time);
det_day   = weekday(de_time);

diff_day = det_date -frd_date;

daybin = put(diff_day,day_bin.);

keep account frddate fraudtyp ffind frdtime detdate dettime
     frd_date frd_mnth frd_day frdtmday det_mnth det_time det_day dettmday
     diff_day daybin;

run;

/**************************************************************/
/* Sort                                                       */
/**************************************************************/

proc sort data=xyz;
 by account;
run;

proc print data=xyz(obs=40);
     var account frddate fraudtyp ffind frdtime detdate dettime;
run;

/**************************************************************/
/* Data Analysis                                              */
/**************************************************************/

proc chart data=xyz;
     hbar fraudtyp /type=percent missing;
     title 'Type of Fraud';
run;

proc chart data=xyz;
     hbar ffind /type=percent missing;
     title 'Fraud Find Method';
run;
proc chart data=xyz;
     hbar frd_mnth /type=percent missing;
     title 'Month of First Fraud Transaction';
run;
proc chart data=xyz;
     hbar frd_day /type=percent missing midpoints=1 2 3 4 5 6 7;
     title 'First Fraud by Day of Week';
run;
proc chart data=xyz;
     hbar frdtmday /type=percent missing;
     title 'First Fraud by Time of Day';
run;
proc chart data=xyz;
     hbar det_mnth /type=percent missing midpoints=1 2 3 4 5 6 7 8 9 10 11 12;
     title 'Month of First Fraud Detection';
run;
proc chart data=xyz;
     hbar det_day /type=percent missing midpoints=1 2 3 4 5 6 7;
     title 'Fraud Detection by Day of Week';
run;
proc chart data=xyz;
     hbar dettmday /type=percent missing;
     title 'Fraud Detection by Time of Day';
run;

/*  Comment this one out in favor of the binned one next  jcb  12/27/96
proc chart data=xyz;
     hbar diff_day /type=percent missing;
     title 'Difference in Days Between Fraud Committed and Detected';
run;
*/

proc chart data=xyz;
     hbar daybin /type=percent missing;
     title 'Difference in Days Between Fraud Committed and Detected';
run;

proc sort data=xyz;
     by frd_date;
run;

proc chart data=xyz;
     hbar frd_date /type=percent missing discrete;
     title 'First Fraud Date';
run; 

endsas;
