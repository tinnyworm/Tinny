options ps = 50 ls = 75 nocenter;
%macro mainmac;

 
%let NumFiles = 2;
%let f1 = adssmb_prod2809; 
%let f2 = adssll_prod2809; 

%let ofname = ar;


%let dirname = /telecom2/TELSTRA/production_data;

%macro readin(fname,fnum);	
	
data f&fnum;
infile "&dirname./&fname..txt" lrecl = 2048;
input @1 rectype $1. @;
if rectype = "D" then do;
	input %include "&dirname./layouts/ar.prod";;
	output;
end;
else input;
run;

%mend readin;

%do ind = 1 %to &NumFiles;
%readin(&&f&ind,&ind);
%end;
data ar;
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
       count(userdata2) as USERDATA2_QTY
from ar;
quit;

ods rtf file="&dirname./&ofname..rtf";

proc print data=test123;
title "AR file: &ofname ";
run;

proc print data=ar(obs = 25);
title "AR file: &ofname ";
run;

proc means data = ar;
var tran_amt currbal;

proc freq data = ar;
tables
     carrier    
     trandate  
     trantype    
     userdata1 
     userdata2  ;
  run; 
 
ods rtf close;

%mend mainmac;


%mainmac;



