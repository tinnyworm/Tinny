%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/3.10/formats';
options mstored sasmstore=macrolib;
options error=2 ps=1000 ls=100 nodate;

%let sysparm1 = %scan(&sysparm,1,|);
%let strt = %scan(&sysparm,2,|);
%let stp = %scan(&sysparm,3,|);
%let exp = %scan(&sysparm,4,|);

%let prefix = frauds;

*** RUN MACROS TO CALCULATE STATS ***;

%read_fr;
%nobs(C001);
%bins(C001);
%catvar(C020,C020,type,$Ftype.,%str( ));
%datevar(C021,C021,ffd,%str( ),2,1,1);  *** past & future;
%catvar(C029,C029,meth,$Fmethod.,%str( ));
%timevar(C030,C030,fft);
%datevar(C036,C036,fdd,%str( ),%str( ),1,1);  *** past & future;
%timevar(C044,C044,fdt);


***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'frauds.rtf';

data _null_;
 file a notitles ls=1000;

%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Fraud Case File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Fraud Case File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Fraud Case File \b0" &e;
	   put &bl "\fs18\b Physical \b0" &e;

****** PHYSICAL ATTRIB ******;

 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
           rec_com=put(&num_rec,comma11.0);
           uni_com=put(&num_uniq,comma11.0);
	   put &bl "\fs18\b Attribute \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "Records analyzed" &cl rec_com &cc &cl &e;
	   put &bl "Unique accounts" &cl uni_com  &cc &cl &e;
	   put &bl "Record length" &cl "&rec_len"  &cc &cl &e;
	   &par; &par;

****** FIELD SPECIFIC **********;

	%rtf(1,b=0);
	   put &bl "\fs18\b Fraud Case File \b0" &e;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "FRAUD-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl "FRAUD-TYPE" &cl "&type"  &cc &cl &e;
	   put &bl "FRAUD-DATE-FIRST-FRAUD" &cl "&ffd"  &cc &cl &e;
	   put &bl "FRAUD-FIND-METHOD" &cl "&meth"  &cc &cl &e;
	   put &bl "FRAUD-TIME-FIRST-FRAUD" &cl "&fft"  &cc &cl &e;
	   put &bl "FRAUD-DETECTED-DATE" &cl "&fdd"  &cc &cl &e;
	   put &bl "FRAUD-DETECTED-TIME" &cl "&fdt"  &cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "FRAUD-OPEN-DATE" &cl "&fod"  &cc &cl &e;
	   put &bl "FRAUD-BALANCE" &cl "&balance"  &cc &cl &e;
	   put &bl "FRAUD-TOTAL-LOSS" &cl "&tot_loss"  &cc &cl &e;
	%end;


%rtf(100);

run;

%mend;

%run_all;
