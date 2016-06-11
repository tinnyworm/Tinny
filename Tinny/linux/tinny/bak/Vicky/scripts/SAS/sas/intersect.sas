%macro run_all;

libname macrolib '.';
*libname macrolib '/work/gold/projects/bin/CCDS/3.10/MACROS/2.1';
libname library '/work/gold/projects/bin/CCDS/3.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;


******** READ-IN INTERSECT FILE ********;

data auths cards; infile STDIN lrecl=100 pad;
 input @1 temp $80. @11 file $ @56 percent 8. ;
 format percent 8.2;
 x=index(temp,'in cards ..');
 y=index(temp,'in auths ..');
 if x then output cards;
 if y then output auths;
run;

proc sort data=cards(keep=file percent); by file;
data cards; set cards; by file; if last.file; 
proc sort data=auths(keep=file percent); by file;
data auths; set auths; by file; if last.file; 



***** CREATE RTF-FORMATTED OUTPUT (AUTHS) *****;
filename a 'auths_int.rtf';

data _null_; set auths end=last;
    file a notitles ls=1000;

if _n_=1 then do;
     %rtf(0);
	%rtf(1,b=0);
	   put &bl "\fs18\b Intersect Analysis \b0" &e;
	   put &bl "\fs18\b Percentage of Accounts that Appear in the Authorization Transaction File \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b File \b0" &cl "\b Intersection Rate \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
end;

	   put &bl file &cl percent &cc &cl &e;

if last then do;
     %rtf(100);
end;
run;



***** CREATE RTF-FORMATTED OUTPUT (CARDS) *****;
filename a 'cards_int.rtf';

data _null_; set cards end=last;
    file a notitles ls=1000;

if _n_=1 then do;
     %rtf(0);
	%rtf(1,b=0);
	   put &bl "\fs18\b Intersect Analysis \b0" &e;
	   put &bl "\fs18\b Percentage of Accounts that Appear in the Cardholder File \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b File \b0" &cl "\b Intersection Rate \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
end;

	   put &bl file &cl percent &cc &cl &e;

if last then do;
     %rtf(100);
end;
run;

%mend;

%run_all;

