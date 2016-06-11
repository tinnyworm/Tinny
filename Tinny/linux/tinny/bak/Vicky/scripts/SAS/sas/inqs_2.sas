%macro run_all;

libname macrolib '.';
*libname macrolib '/work/gold/projects/bin/CCDS/3.10/MACROS/2.1';
libname library '/work/gold/projects/bin/CCDS/3.10/formats';
options mstored sasmstore=macrolib;
options errors=2;

%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);
%let exp = %scan(&sysparm,4);

%let prefix = inquiry;

*** RUN MACROS TO CALCULATE STATS ***;

%read_inq;
%nobs(C001);
%bins(C001);
%catvar(C020,C020,type,$Itype.,%str( ));
%datevar(C021,C021,inqdate,%str( ),%str( ),1,1); *** past & future;
%timevar(C029,C029,inqtime);



***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'inquiry.rtf';

data _null_;
 file a notitles ls=1000;


%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Automated Account Inquiry File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Automated Account Inquiry File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Automated Account Inquiry File \b0" &e;
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
	   put &bl "\fs18\b Automated Account Inquiry File \b0" &e;
	   put &bl "\fs18\b Physical \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "INQ-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl "INQ-TYPE" &cl "&type"  &cc &cl &e;
	   put &bl "INQ-DATE" &cl "&inqdate"  &cc &cl &e;
	   put &bl "INQ-TIME" &cl "&inqtime"  &cc &cl &e;


%rtf(100);

run;


%mend;
%run_all;
