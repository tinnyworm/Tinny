options  compress=yes;
/************************************************/
/*                                              */
/* data analysis - extract samples and run proc */
/*                 means and freq               */
/*                                              */
/************************************************/
%let datafeed = billcharge;
%let version = 1.97;

%let low='20020213';
%let upp='20020219';

libname libd '.';
filename data "C";

data libd.ar;

  infile "./C" firstobs=1 missover lrecl=1000;
  input
        %include "ar_wrong.offset";

  ; 
  if _N_ = 1 then delete;
  if _N_ > 11830 then delete;

run;
proc freq data=libd.ar;
tables
              acct_nbr      
          svc_id        
          trandate  
         trantype     
          tran_amt    
          currbal    
          userdata1 
         userdata2   

;
title "One week's &datafeed categorical variables' statistics"; 
run;  


endsas;

proc sort data=libd.billchargedata out=libd.sorted_oneweek_&datafeed;
by intlflag;
run;


proc print data=libd.sorted_oneweek_&datafeed (obs=10);
title "Samples of &datafeed sorted by trantime";
run; 


data new_oneweek_&datafeed;
  set libd.sorted_oneweek_&datafeed;
  call_dur_num = input(call_dur,10.0);
  hold_dur_num = input(hold_dur,10.0);
  origgmt_num = input(origgmt,5.1);
  termgmt_num = input(termgmt,5.1);
  rateplwt_num = input(rateplwt, 13.1);
  rating_num = input(rating, 13.2);
  hh_call=substr(cdr_tm_stmp,1,2);
  handoff_num = input(handoff,2.0);
run;

proc means data=new_oneweek_&datafeed;
var 
call_dur_num 
hold_dur_num 
origgmt_num 
termgmt_num 
rateplwt_num 
rating_num 
handoff_num;
title "One week's &datafeed numerical variables' statistics"; 
run; 


proc freq data=new_oneweek_&datafeed;
tables
card_num
card_pin 
cdr_dt_stmp 
hh_call 
billtype 
cust_seg1 
otrnkcls 
org_loc 
org_calltype 
term_calltype 
inoutind 
callg_nat 
calld_nat 
charg_nat 
oli_digits 
calldir 
rlt_pres 
wtype
origst
origcnty
ttrnkcls
strtdate
odltsvgs
tdltsvgs
juris
intlflag
callfwd
callwait
confcall
vmail
pincode
vip_flag
hotnum
warmnum
hotcell
hotcntry
termstat
carrier_sel
tekno_disp
raterprd
discntcd
termcnty
origgmt
termgmt
;
title "One week's &datafeed categorical variables' statistics"; 
run;  


