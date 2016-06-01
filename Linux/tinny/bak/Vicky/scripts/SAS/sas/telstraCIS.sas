options ps = 50 ls = 80 nocenter;
%macro mainmac;

%let NumFiles = 3;
%let f1 = TELSCIS20031008131254; 
%let f2 = another1_TELSCIS20031006223553; 
%let f3 = another2_TELSCIS20031006224256; 

%let ofname = flexcab;

/* 

%let NumFiles = 5;
%let f1 = mica1_TELSCIS20031009112611;
%let f2 = mica2_TELSCIS20031009112611;
%let f3 = mica3_TELSCIS20031009112611;
%let f4 = mica4_TELSCIS20031009112611;
%let f5 = mica5_TELSCIS20031009112611; 

%let ofname = mica;

*/


%let dirname = /telecom2/TELSTRA/production_data;


%macro readin(fname,fnum);



filename dat&fnum "&dirname./&fname..txt";



data f&fnum;
infile dat&fnum lrecl = 2048;
input @1 rectype $1. @;
if rectype = "D" then do;
	input %include "&dirname./layouts/cis.prod";;
	zipcust = cust_zip *1;
	zipbill = bill_zip * 1;
	output;
end;
else input;


%mend readin;

%do ind = 1 %to &NumFiles;
%readin(&&f&ind,&ind);
%end;
data cis;
set 
%do ind = 1 %to &NumFiles;
f&ind
%end;
;
run;

proc sql;
create table test123 as
select count(*) as TOTAL_RECORDS,
       count(carrier) as CARRIER_QTY, 
       count(acct_nbr) as ACCT_QTY, 
       count(svc_id) as SVC_ID_QTY,
       count(cis_date) as CIS_DATE_QTY, 
       count(cis_time) as CIS_TIME_QTY,
       count(accttype) as ACCT_TYPE_QTY,
       count(acctopdt) as ACCT_OPEN_QTY, 
       count(svctype) as SVC_TYPE_QTY,
       count(status) as STATUS_QTY,                            
       count(fname) as FIRST_NAME_QTY,
       count(lname) as LAST_NAME_QTY,
       count(billfname) as BILL_F_NAME_QTY,
       count(billlname) as BILL_L_NAME_QTY,
       count(lictype) as LIC_TYPE_QTY,
       count(lic_loc) as LIC_LOC_QTY,
       count(lic_num) as LIC_NUM_QTY,
       count(occ_cd) as OCC_CODE_QTY,
       count(sic_cd) as SIC_CODE_QTY,
       count(custstr) as CUST_STREET_QTY,
       count(custstr2) as CUST_STREET2_QTY,
       count(custcity) as CUST_CITY_QTY,
       count(cust_st) as CUST_STATE_QTY,
       count(cust_zip) as CUST_ZIP_QTY,
       count(cust_reg) as CUST_REG_QTY,
       count(custcntry) as CUST_CNTRY_QTY,
       count(billstr)  as BILL_STREET_QTY,
       count(billstr2)  as BILL_STREET2_QTY,    
       count(billcity) as BILL_CITY_QTY,
       count(bill_st)  as  BILL_STATE_QTY,
       count(bill_zip)   as BILL_ZIP_QTY,
       count(bill_reg)   as BILL_REG_QTY,
       count(billcntry)   as BILL_CNTRY_QTY,
       count(bill_phn)   as BILL_PHONE_QTY,
       count(bill_phn2)  as BILL_PHONE2_QTY,
       count(billagcy)  as BILL_AGCY_QTY,       
       count(curr_cd)  as CURR_CODE_QTY,       
       count(planname) as PLAN_NAME_QTY,
       count(prevname) as PREV_PLAN_NAME_QTY,   
       count(esn_imei) as ESN_QTY,
       count(min_imsi) as MIN_QTY,   
       count(clec_id) as CLEC_ID_QTY,
       count(dob) as DOB_QTY,       
       count(bus_num) as BUS_NUM_QTY,
       count(corp_num) as CORP_NUM_QTY,
       count(userdata1) as USERDATA1_QTY,
       count(userdata2) as USERDATA2_QTY,
       count(userdata3) as USERDATA3_QTY,
       count(userdata4) as USERDATA4_QTY
from cis;
quit;

ods rtf file="&dirname./rtf/&ofname._cis.rtf";

proc print data=test123;
title "CIS file: &ofname ";
run;
proc print data=cis(obs = 25);

proc means data = cis;

proc freq data=cis;
tables
     carrier        
     accttype       
     svctype
     cis_date  
     status         
     deactrsn       
     lictype  
     lic_loc
     cust_st
     custcntry
     bill_st
     billcntry
     billagcy      
     curr_cd            
     autopay        
     plantype       
     dscntpln       
     prevptyp       
     w_tech         
     pic_jur_ind    
     org_call_scr   
     deposit_ind    
     toll_blk       
     ISDN_IND       
     spec_svc_ind   
     bill_day       
     billperiod     
     billfreq       
     picc_ind       
     ;
  run;
  
proc means data = cis;
run;

proc contents data=cis;
run;
  
ods rtf close;

run;




%mend mainmac;


%mainmac;












