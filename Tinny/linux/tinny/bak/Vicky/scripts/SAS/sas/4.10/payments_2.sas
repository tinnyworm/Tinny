%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 ps=1000 ls=100 nodate;

%let sysparm1 = %scan(&sysparm,1);
%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);

%let prefix = payments;

*** RUN MACROS TO CALCULATE STATS ***;

%read_pay;
%nobs(C001);
%bins(C001);
%numvar(C020,C020,amount,%str( ));
%if &sysparm1^=D %then %numvar(C033,C033,mindue,%str( ));;
%numvar(C059,C059,balance,%str( ));
%numvar(C072,C072,tot_bal,%str( ));
%datevar(C082,C082,depdate,%str( ),%str( ),1,1); *** past & future dates;
%if &sysparm1^=D %then %datevar(C090,C090,duedate,%str( ),%str( ),1,);; *** past only;
%datevar(C098,C098,stdate,%str( ),%str( ),1,1); *** past & future dates;
%catvar(C106,C106,rev,$reverse.,%str( ));
%catvar(C107,C107,depmeth,$method.,%str( ));
%catvar(C108,C108,deptype,$Ptype.,%str( ));
%catvar(C109,C109,fraud,$blank.,%str( ));
%timevar(C110,C110,paytime);



***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'payments.rtf';

data _null_;
 file a notitles ls=1000 ;

 length header $3; 
 %if &sysparm1=D %then %do; header='DEP'; %end;
 %else %do; header='PAY'; %end;



%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	%if &sysparm1=D %then %do;
	   put &bl "\fs24\b Deposit Transaction File Data Analysis \b0" &e;
        %end;
 	%else %do;
	   put &bl "\fs24\b Payment Transaction File Data Analysis \b0" &e;
	%end;
	   put &bl "\fs20 The summary of the data analysis for the Payment Transaction File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	%if &sysparm1=D %then %do;
	   put &bl "\fs18\b Deposit Transaction File Data Analysis \b0" &e;
        %end;
 	%else %do;
	   put &bl "\fs18\b Payment Transaction File Data Analysis \b0" &e;
	%end;
	   put &bl "\fs18\b Physical \b0" &e;

****** PHYSICAL ATTRIB ******;

 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
           rec_com=put(&num_rec,comma11.0)||" (sampled at x%)";
           uni_com=put(&num_uniq,comma11.0);
	   put &bl "\fs18\b Attribute \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "Records analyzed" &cl rec_com &cc &cl &e;
	   put &bl "Unique accounts" &cl uni_com  &cc &cl &e;
	   put &bl "Record length" &cl "&rec_len"  &cc &cl &e;
	   &par; &par;

****** FIELD SPECIFIC **********;

	%rtf(1,b=0);
	%if &sysparm1=D %then %do;
	   put &bl "\fs18\b Deposit Transaction File Data Analysis \b0" &e;
        %end;
 	%else %do;
	   put &bl "\fs18\b Payment Transaction File Data Analysis \b0" &e;
	%end;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl header +(-1) "-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl header +(-1) "-AMOUNT" &cl "&amount"  &cc &cl &e;

	%if &sysparm1^=D %then %do;
	   put &bl header +(-1) "-MIN-DUE" &cl "&mindue"  &cc &cl &e;
	%end;

	   put &bl header +(-1) "-BALANCE" &cl "&balance"  &cc &cl &e;

	%if &sysparm1=D %then %do;
	   put &bl header +(-1) "-TOTAL-BAL" &cl "&tot_bal"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl header +(-1) "-CRED-LINE" &cl "&tot_bal"  &cc &cl &e;
	%end;

	   put &bl header +(-1) "-DATE" &cl "&depdate"  &cc &cl &e;

	%if &sysparm1^=D %then %do;
	   put &bl header +(-1) "-DUE-DATE" &cl "&duedate"  &cc &cl &e;
	%end;

	   put &bl header +(-1) "-STATE-DATE" &cl "&stdate"  &cc &cl &e;
	   put &bl header +(-1) "-REVERSAL" &cl "&rev"  &cc &cl &e;

	%if &sysparm1=D %then %do;
	   put &bl "DEP-DEPOSIT-METH" &cl "&depmeth"  &cc &cl &e;
	   put &bl "DEP-DEPOSIT-TYPE" &cl "&deptype"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl "PAY-PAYMENT-METH" &cl "&depmeth"  &cc &cl &e;
	   put &bl "PAY-PAYMENT-TYPE" &cl "&deptype"  &cc &cl &e;
	%end;

	   put &bl header +(-1) "-FRAUD-TRX" &cl "&fraud"  &cc &cl &e;
	   put &bl header +(-1) "-TIME" &cl "&paytime"  &cc &cl &e;

%rtf(100);

run;


%mend;

%run_all;
