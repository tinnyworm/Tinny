options ps = 50 ls = 75 nocenter;
%macro mainmac;

 
%let NumFiles = 2;
%let f1 = adssmb_prod2809; 
%let f2 = adssll_prod2809; 

%let ofname = adss;


%let dirname = /telecom2/TELSTRA/production_data;


%macro readin(fname,fnum);

/* filename dat&fnum "&dirname./&fname..txt"; */

filename dat&fnum "&dirname./&fname";


data f&fnum;

infile dat&fnum lrecl = 2048;
 
input @1 rectype $1. @;
if rectype = "D" then do;
	input %include "&dirname./layouts/app.prod";;
	output;
end;
else input;


run;

%mend readin;

%do ind = 1 %to &NumFiles;
%readin(&&f&ind,&ind);
%end;
data app;
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
from app;
quit;

run;
    


ods rtf file="&dirname./rtf/&ofname..rtf";

proc print data=test123;
title "APP file: &ofname ";
run;

proc print data=app(obs = 25);
title "APP file: &ofname ";
run;

proc means data = app;
          
proc freq data=app;
tables
carrier   
     appdt     	
     acctopdt    
     svctype   
     startdt   
     accttype  
     numlines      
     lictype     
     lic_val   
     cntctnbr  
     exstcust   
     curr_cd   
     curr_rt   
     gmtoffst  
     htlstind  
     safescan  
     fourscr   
     credcard  
     bureauid  
     burmatch  
     paid_def  
     unpaid_def
     bankrpty  
     burcreat  
     burscore  
     cclass    
     pos_type     
     pos_zip   
     pos_rep   
     plantype  
     wroffmatch
     numwroff  
     wroffamt  
     mo_wroff  
     age       
     sex       
     occ_cd    
     res       
     str_type  
     city      
     state          
     region    
     country   
     addr_val  
     addr_yrs  
     addr_mos  
     prev_yrs  
     prev_mos  
     empl_yrs  
     empl_mos  
     score1    
     score2    
     score3    
     score4    
     score5    
     score6    
     duplicate 
     app_rec   
     OVERRIDE  
     OVER_RSN  
     userdata1 
     userdata2 
     userdata3 
     userdata4;
   run;
ods rtf close;

%mend mainmac;
%mainmac;