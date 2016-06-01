%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 ps=1000 ls=100 nodate;

%let sysparm1 = %scan(&sysparm,1);
%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);
%let exp = %scan(&sysparm,4);

%let prefix = nonmons;

*** RUN MACROS TO CALCULATE STATS ***;

%read_nm;

%nobs(C001);
%bins(C001);
%catvar(C020,C020,type,$Ntype.,%str( ));
%datevar(C021,C021,trandate,%str( ),%str( ),1,1); *** past and future dates;
%postal(C020,C029,postal,%str(where C020='A';));
%if &sysparm1^=D %then %numvar(C020,C038,cline,%str(where C020='L';));;
%blanks(C048,C048,addrcd);
%catvar(C063,C063,fraud,$blank.,%str( ));


***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'nonmons.rtf';

data _null_;
 file a notitles ls=1000;

%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Non-monetary Transaction File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Non-monetary Transaction File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Non-monetary Transaction File \b0" &e;
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
	   put &bl "\fs18\b Non-monetary Transaction File \b0" &e;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "NONMON-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl "NONMON-TRAN-TYPE" &cl "&type"  &cc &cl &e;
	   put &bl "NONMON-TRAN-DATE" &cl "&trandate"  &cc &cl &e;
	   put &bl "NONMON-POSTAL-CODE" &cl "\b For address change: \b0 \par &postal"  &cc &cl &e;
	%if &sysparm1^=D %then %do;
	   put &bl "NONMON-CREDIT-LINE" &cl "\b For credit line change: \b0 \par &cline"  &cc &cl &e;
	%end;
	   put &bl "NONMON-ADDR-CD" &cl "&addrcd"  &cc &cl &e;
	   put &bl "NONMON-FRAUD-TRX" &cl "&fraud"  &cc &cl &e;



%rtf(100);

run;


%mend;

%run_all;
