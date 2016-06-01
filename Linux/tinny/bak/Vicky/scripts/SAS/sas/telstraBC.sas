options ps = 50 ls = 75 nocenter;
%macro mainmac;


%let fname = TELSCIS20031008131254; 
%let dirname = /telecom2/TELSTRA/production_data;

%macro readin(fname,fnum);

filename dat&fnum "&dirname./&fname";

data f&fnum;

infile dat&fnum lrecl = 2048;
input @1 rectype $1. @;
if rectype = "D" then do;
	input %include "&dirname./layouts/bill.prod";;
	output;
end;
else input;

run;

%mend readin;

%do ind = 1 %to &NumFiles;
%readin(&&f&ind,&ind);
%end;
data bc;
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
       count(userdata1) as USERDATA1_QTY,
       count(userdata2) as USERDATA2_QTY,
       count(userdata3) as USERDATA3_QTY,
       count(userdata4) as USERDATA4_QTY
from bc;
quit;


ods rtf file = "/home/wsk/telstraBC_09252003_01.rtf";
proc print data=test123;
title "BILL file: &fname ";
run;


proc print data=bc(obs = 25);
title "BILL file: &fname ";
run;

proc means data = bc;

proc freq data=bc;
tables
carrier        
     acct_nbr       
     svc_id         
     billdate       
     lstbildt       
     numlines       
     crdtlmt        
     pmtrcvd        
     billadj        
     blncfwd        
     latechrg       
     del_refer      
     newchrg        
     totdue         
     duedate        
     bill30         
     bill60         
     bill90         
     bill120        
     bill150        
     bill150pl      
     monthspast     
     disp_unage     
     due_period     
     due_future     
     totlocal       
     totld          
     totint         
     totprem        
     totcard        
     totfax         
     totop          
     totinfo        
     totforward     
     tottomob       
     totfrmob       
     totwless       
     data_vol       
     data_amt       
     sms_vol        
     sms_amt        
     pos_vol        
     pos_amt        
     accchrg        
     airmou         
     airamt         
     roammou        
     romaamt        
     roamtollamt    
     rev_amt        
     servcall       
     featamt        
     svcamt         
     eqpamt         
     othrchrg       
     dscntamt       
     fedtax         
     statetax       
     cntytax        
     localtax       
     othertax       
     userdata1      
     userdata2      
     userdata3      
     userdata4;
  run;
ods rtf close;

ods rtf file = "/home/wsk/telstraBC_09252003_02.rtf";
proc print data=bc19;
title "fibc_systest2";
run;

proc freq data=bc19;
tables
carrier        
     acct_nbr       
     svc_id         
     billdate       
     lstbildt       
     numlines       
     crdtlmt        
     pmtrcvd        
     billadj        
     blncfwd        
     latechrg       
     del_refer      
     newchrg        
     totdue         
     duedate        
     bill30         
     bill60         
     bill90         
     bill120        
     bill150        
     bill150pl      
     monthspast     
     disp_unage     
     due_period     
     due_future     
     totlocal       
     totld          
     totint         
     totprem        
     totcard        
     totfax         
     totop          
     totinfo        
     totforward     
     tottomob       
     totfrmob       
     totwless       
     data_vol       
     data_amt       
     sms_vol        
     sms_amt        
     pos_vol        
     pos_amt        
     accchrg        
     airmou         
     airamt         
     roammou        
     romaamt        
     roamtollamt    
     rev_amt        
     servcall       
     featamt        
     svcamt         
     eqpamt         
     othrchrg       
     dscntamt       
     fedtax         
     statetax       
     cntytax        
     localtax       
     othertax       
     userdata1      
     userdata2      
     userdata3      
     userdata4;
  run;
ods rtf close;


ods rtf file = "/home/wsk/telstraBC_09252003_03.rtf";
proc print data=bc20;
title "mica_systest_bc_final";
run;

proc freq data=bc20;
tables
carrier        
     acct_nbr       
     svc_id         
     billdate       
     lstbildt       
     numlines       
     crdtlmt        
     pmtrcvd        
     billadj        
     blncfwd        
     latechrg       
     del_refer      
     newchrg        
     totdue         
     duedate        
     bill30         
     bill60         
     bill90         
     bill120        
     bill150        
     bill150pl      
     monthspast     
     disp_unage     
     due_period     
     due_future     
     totlocal       
     totld          
     totint         
     totprem        
     totcard        
     totfax         
     totop          
     totinfo        
     totforward     
     tottomob       
     totfrmob       
     totwless       
     data_vol       
     data_amt       
     sms_vol        
     sms_amt        
     pos_vol        
     pos_amt        
     accchrg        
     airmou         
     airamt         
     roammou        
     romaamt        
     roamtollamt    
     rev_amt        
     servcall       
     featamt        
     svcamt         
     eqpamt         
     othrchrg       
     dscntamt       
     fedtax         
     statetax       
     cntytax        
     localtax       
     othertax       
     userdata1      
     userdata2      
     userdata3      
     userdata4;
  run;
ods rtf close;


%mend mainmac;


%mainmac;

