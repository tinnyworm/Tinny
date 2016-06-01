options ps = 50 ls = 75 nocenter;


data wo14;
infile "/telecom2/TELSTRA/NEW_VALIDATION_DATA/fcab_systest_wo_finalrun" lrecl=2048;
input %include "/pascal/CONSORTIUM_LAYOUTS/Version_2.01.5/writeoff.offset";;

data wo15;
infile "/telecom2/TELSTRA/NEW_VALIDATION_DATA/fiwo_systest2" lrecl=2048;
input %include "/pascal/CONSORTIUM_LAYOUTS/Version_2.01.5/writeoff.offset";;

data wo16;
infile "/telecom2/TELSTRA/NEW_VALIDATION_DATA/mica_systest_wo_finalrun" lrecl=2048;
input %include "/pascal/CONSORTIUM_LAYOUTS/Version_2.01.5/writeoff.offset";;



ods rtf file="/home/wsk/telstraWO_09252003_01.rtf";

proc print data=wo14;
title "fcab_systest_wo_finalrun";
run;

proc freq data=wo14;
tables
carrier        
     acct_nbr       
     svc_id         
     wrtofflevel    
     wrtoffdisp     
     deliqsdt       
     deliqstm       
     deliqedt       
     deliqetm       
     lastpaydate    
     discdate       
     disctime       
     wrtoff_dt      
     discmthd       
     currency_cd    
     currency_rt    
     wrtoff_amt     
     comments;
  run;

ods rtf close;



ods rtf file="/home/wsk/telstraWO_09252003_02.rtf";

proc print data=wo15;
title "fiwo_systest2";
run;

proc freq data=wo15;
tables
carrier        
     acct_nbr       
     svc_id         
     wrtofflevel    
     wrtoffdisp     
     deliqsdt       
     deliqstm       
     deliqedt       
     deliqetm       
     lastpaydate    
     discdate       
     disctime       
     wrtoff_dt      
     discmthd       
     currency_cd    
     currency_rt    
     wrtoff_amt     
     comments;
  run;

ods rtf close;


ods rtf file="/home/wsk/telstraWO_09252003_03.rtf";

proc print data=wo16;
title "mica_systest_wo_finalrun";
run;

proc freq data=wo16;
tables
carrier        
     acct_nbr       
     svc_id         
     wrtofflevel    
     wrtoffdisp     
     deliqsdt       
     deliqstm       
     deliqedt       
     deliqetm       
     lastpaydate    
     discdate       
     disctime       
     wrtoff_dt      
     discmthd       
     currency_cd    
     currency_rt    
     wrtoff_amt     
     comments;
  run;

ods rtf close;