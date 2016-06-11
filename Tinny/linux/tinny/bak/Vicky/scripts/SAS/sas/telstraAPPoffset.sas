options ps = 50 ls = 75 nocenter;
%macro mainmac;
%let fname = adssll_prod2809; 


%let dirname = /telecom2/TELSTRA/production_data;

data app;

infile "&dirname./&fname" lrecl = 2048;

	input %include "&dirname./layouts/app.offset";;


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
    


ods rtf file="&dirname./&fname..rtf";

proc print data=test123;
title "APP file: &fname ";
run;

proc print data=app(obs = 25);
title "APP file: &fname ";
run;

          
proc freq data=app;
tables
carrier   
     acct_nbr  
     svc_id    
     appdt     	
     acctopdt  
     app_ref   
     svctype   
     startdt   
     accttype  
     numlines  
     fname     
     lname     
     ssn       
     dob       
     lictype   
     lic_loc   
     license   
     lic_val   
     cntctnbr  
     exstcust  
     custstdt  
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
     pos_id    
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
     propname  
     unit_num  
     str_num   
     str_name1 
     str_name2 
     str_type  
     city      
     state     
     zip       
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