options linesize = 99 nocenter pagesize=90 ;
%let mincount=60 ;

filename usaauth  pipe "cat bins.200210.auth     | awk '{print $1 $2}'" ;
filename cancard  pipe "cat bins.200210.can.card | awk '{print $1 $2}'" ;
filename canpay   pipe "cat bins.200210.can.pay  | awk '{print $1 $2}'" ;
filename canpost  pipe "cat bins.200210.can.post | awk '{print $1 $2}'" ;
filename usacard  pipe "cat bins.200210.card     | awk '{print $1 $2}'" ;
filename usapay   pipe "cat bins.200210.pay      | awk '{print $1 $2}'" ;
filename usapost  pipe "cat bins.200210.post     | awk '{print $1 $2}'" ;

%macro afile(afile) ;
data &afile ;
	infile &afile length=linesize ;
	input @ ;
	if linesize < 7 then do ;
		input  ;
		delete ;
		end    ;
	else do    ;
		input 
			@01 bin      $6.
			@07 &afile ;
		if &afile < &mincount then delete ;
		output ;
		end    ;
	run ;
proc sort data = &afile ;
	by bin ;
%mend ;

%afile(usaauth) ; 
%afile(cancard) ; 
%afile(canpost) ; 
%afile(canpay)  ; 
%afile(usacard) ; 
%afile(usapay)  ; 
%afile(usapost) ; 

data all ; 
	merge usaauth usacard usapay usapost cancard canpost canpay; 
	by bin ;
	proc print noobs;
	var bin usaauth usacard usapay usapost cancard canpost canpay ;
	format  usaauth usacard usapay usapost cancard canpost canpay comma10. ;
