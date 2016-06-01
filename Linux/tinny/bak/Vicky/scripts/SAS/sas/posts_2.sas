%macro run_all;

libname macrolib '.';
*libname macrolib '/work/gold/projects/bin/CCDS/3.10/MACROS/2.1';
libname library '/work/gold/projects/bin/CCDS/3.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

%let sysparm1 = %scan(&sysparm,1);
%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);
%let exp = %scan(&sysparm,4);

%let prefix = posts;

*** RUN MACROS TO CALCULATE STATS ***;

%if &sysparm1=J %then %do; %read_jps; %end;
%else %do; %read_pst; %end;

%binmonth(post,C049);
proc datasets nolist library=work; delete post; run;
%nobs(C001);
%bins(C001);
%numvar(C020,C020,amount,%str( ));
%manyvar(C033,C033,currcode,$country.,3,%str( ));
%numvar(C036,C036,currrate,%str( ));
%datevar(C049,C049,postdate,%str( ),1,1,1); *** past & future;
%datevar(C057,C057,trndate,%str( ),%str( ),1,1); *** past & future;
%timevar(C065,C065,trntime);
%numvar(C071,C071,availcrd,%str( ));
%numvar(C081,C081,credline,%str( ));
%catvar(C091,C091,trantype,$Atype.,%str( ));
%catvar(C092,C092,authed,$yes_no.,%str( ));
%manyvar(C093,C093,sic,$sicfmt.,4,%str( ));
%postal(C097,C097,postal,%str( ));
%manyvar(C106,C106,mrchcnty,$country.,3,%str( ));
%blanks(C109,C109,authid);
%blanks(C115,C115,postid);
%blanks(C121,C121,mrchid);
%blanks(C137,C137,termid);
%blanks(C143,C143,which);
%*catvar(C143,C143,which,$1.,%str( ));
%numvar(C144,C144,cashback,%str( ));
%numvar(C157,C157,falcon,%str( ));
%if &sysparm1=D %then %catvar(C161,C161,accttype,$accttyp.,%str( ));;
%if &sysparm1=J %then %do;
  %manyvar(C161,C161,item,$3.,5,%str( ));
  %blanks(C164,C164,addrcd);
%end;
%else %do;
  %blanks(C162,C162,merch);
%end;



***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'posts.rtf';

data _null_;
 file a notitles ls=1000;


%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Posting Transaction File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Posting Transaction File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Posting Transaction File \b0" &e;
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
	   put &bl "\fs18\b Posting Transaction File \b0" &e;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "POST-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl "POST-AMOUNT" &cl "&amount"  &cc &cl &e;
	   put &bl "POST-CURR-CODE" &cl "&currcode"  &cc &cl &e;
	   put &bl "POST-CURR-RATE" &cl "&currrate"  &cc &cl &e;
	   put &bl "POST-POST-DATE" &cl "&postdate"  &cc &cl &e;
	   put &bl "POST-TRAN-DATE" &cl "&trndate"  &cc &cl &e;
	   put &bl "POST-TRAN-TIME" &cl "&trntime"  &cc &cl &e;
	%if &sysparm1=D %then %do;
	   put &bl "POST-AVAIL-BAL" &cl "&availcrd"  &cc &cl &e;
	   put &bl "POST-TOTAL-BAL" &cl "&credline"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl "POST-AVAIL-CRED" &cl "&availcrd"  &cc &cl &e;
	   put &bl "POST-CRED-LINE" &cl "&credline"  &cc &cl &e;
	%end;
	   put &bl "POST-TRAN-TYPE" &cl "&trantype"  &cc &cl &e;
	   put &bl "POST-AUTHED" &cl "&authed"  &cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "POST-DEPT" &cl "&sic"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl "POST-SIC" &cl "\fs16 &sic \fs18"  &cc &cl &e;
	%end;

	   put &bl "POST-POSTAL-CODE" &cl "&postal"  &cc &cl &e;
	   put &bl "POST-MRCH-CNTRY" &cl "&mrchcnty"  &cc &cl &e;
	   put &bl "POST-AUTH-ID" &cl "&authid"  &cc &cl &e;
	   put &bl "POST-POST-ID" &cl "&postid"  &cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "POST-STORE-ID" &cl "&mrchid"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl "POST-MRCH-ID" &cl "&mrchid"  &cc &cl &e;
	%end;

	   put &bl "POST-TERM-ID" &cl "&termid"  &cc &cl &e;
	   put &bl "POST-WHICH-CARD" &cl "&which"  &cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "POST-MAX-ITEM" &cl "&cashback"  &cc &cl &e;
	   put &bl "POST-ITEM-COUNT" &cl "&falcon"  &cc &cl &e;
	%end;
	%else %do;
	   put &bl "POST-CASHBACK-AMT" &cl "&cashback"  &cc &cl &e;
	   put &bl "POST-FALCON-SCORE" &cl "&falcon"  &cc &cl &e;
	%end;

	%if &sysparm1=D %then %do;
	   put &bl "POST-ACCOUNT-TYPE" &cl "&accttype"  &cc &cl &e;
	%end;

	%if &sysparm1=J %then %do;
	   put &bl "POST-ITEM-CD" &cl "&item"  &cc &cl &e;
	   put &bl "POST-ADDR-CD" &cl "&addrcd"  &cc &cl &e;
	%end;

	%else %do;
	   put &bl "POST-MERCHANT-NAME" &cl "&merch"  &cc &cl &e;
	%end;


%rtf(100);

run;


%mend;
%run_all;
