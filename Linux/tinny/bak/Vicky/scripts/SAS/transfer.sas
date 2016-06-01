*filename ifile "Hnc1102_Credit.txt";
filename ifile "Hnc1102_Debit.txt";
*filename ofile pipe "gzip >frauds.200211_Credit.dat.gz";
filename ofile pipe "gzip >frauds.200211_Debit.dat.gz";

*data _null_;
data temp;
infile ifile dlm=',';
input   ACCT_ID :  $19. 
        TYPE      $ 
        FIR_pre   mmddyy8.
	ddd       $
        METHOD    $ 
	fir_time  $
        DET_pre   mmddyy8.
	mmm       $
        det_time  $;
format fir_pre det_pre mmddyy8.;	
run;
*proc print data=temp(obs=100);
*run;

*data temp2;
data _null_;
set temp;
acct_id=substr(acct_id, 2,16);
type=substr(type,2,1);
method=substr(method, 2,6);

fir_time=substr(fir_time, 2, 6);
det_time=substr(det_time, 2, 6);

*fir_pre=substr(fir_pre, 1, 8);
*det_pre=substr(det_pre, 1, 8);

/*fir_date='20'|substr(fir_pre,7,2)|substr(fir_pre,1,2)|substr(fir_pre,4,2);
det_date='20'|substr(det_pre,7,2)|substr(det_pre,1,2)|substr(fir_pre,4,2);*/


temp_first=input(put(fir_pre, mmddyy10.), $10.);
temp_detected=input(put(det_pre, mmddyy10.), $10.);
substr(fir_date, 1, 4)=substr(temp_first, 7, 4);
substr(fir_date, 5, 2)=substr(temp_first, 1, 2);
substr(fir_date, 7, 2)=substr(temp_first, 4, 2);
substr(det_date, 1, 4)=substr(temp_detected, 7, 4);
substr(det_date, 5, 2)=substr(temp_detected, 1, 2);
substr(det_date, 7, 2)=substr(temp_detected, 4, 2);

drop temp_first temp_detected fir_pre det_pre ddd mmm;
file ofile;
put (acct_id)($19.) (type)($1.) (fir_date)($8.) (method)($1.) (fir_time)($6.) (det_date)($8.) (det_time)($6.);
run;

*proc print data=temp2(obs=100);
*run;
