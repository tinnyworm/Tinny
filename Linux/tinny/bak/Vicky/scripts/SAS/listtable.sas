options linesize = 99 nocenter pagesize=90 ;
*%let mincount=60 ;

filename auth_ID  pipe "cat auth.ID" ;
filename card_ID  pipe "cat card.ID" ;

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
			@01 ID      $16.
			@17 &afile ;
		*if &afile < &mincount then delete ;
		output ;
		end    ;
	run ;
/*proc sort data = &afile ;
	by bin ;
	*/
proc sort data=&afile;
by ID;	
/*proc sort data=&afile;
by &afile;	
	
proc univariate data=&afile plot normal;
var &afile;
output out=&afile n=n mean=mean median=median min=min max=max q1=q1 q3=q3
std=std;
run;

proc print data=&afile;
var n mean std median min q1 q3 max;
run;*/

%mend ;

%afile(auth_ID) ; 
%afile(card_ID) ; 

data all ; 
	merge auth_ID card_ID ; 
	by ID;
	proc print noobs;
	var ID auth_ID card_ID ;


