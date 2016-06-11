%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/3.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

%let sysparm1 = %scan(&sysparm,1,|);
%let strt = %scan(&sysparm,2,|);
%let stp = %scan(&sysparm,3,|);
%let exp = %scan(&sysparm,4,|);

%let prefix = auths;

*** RUN MACROS TO CALCULATE STATS ***;

%if &sysparm1=J %then %do; %read_jau; %end;
%else %do; %read_aut; %end;

/*
%binmonth(auth,C020);
proc datasets nolist library=work; delete auth; run;
%nobs(C001);
%bins(C001); 
*/
%datevar(C020,C020,authdate,%str( ),2,1,1); *** past & future;
%timevar(C028,C028,authtime);
%numvar(C034,C034,amount,%str( ));
%manyvar(C047,C047,currcode,$country.,3,%str( ));
%numvar(C050,C050,currrate,%str( ));
%catvar(C063,C063,decision,$decisn.,%str( ));
%catvar(C064,C064,trantype,$Atype.,%str( ));
%numvar(C065,C065,credit,%str( ));
%numvar(C075,C075,totbal,%str( ));
%manyvar(C064,C085,sicover,$sicfmt.,4,%str( ));
%manyvar(C064,C085,siccash,$sicfmt.,3,%str(where C064='C'; ));
%postal(C089,C089,postal,%str( ));
%manyvar(C098,C098,country,$country.,3,%str( ));
%catvar(C101,C101,pin,$Apin.,%str( ));
%catvar(C101,C101,pinatm,$Apin.,%str(where C085='6011'; ));
%catvar(C103,C102,cvv,$Acvv.,%str(where C103 in ('S','U'); ));
%catvar(C103,C102,cvv2,$Acvv.,%str(where C103 in ('K'); ));
%catvar(C103,C103,swipe,$POSmode.,%str( ));
%datevar(C104,C104,expdate,%str( ),%str( ),1,); *** past only;
%blanks(C112,C112,auth);
%blanks(C118,C118,reason);
%catvar(C123,C123,advice,$advice.,%str( ));
%blanks(C124,C124,mrchid);
%blanks(C140,C140,termid);
%blanks(C146,C146,which);
%numvar(C147,C147,cashback,%str( ));
%numvar(C160,C160,falcon,%str( ));
%if &sysparm1=D %then %catvar(C164,C164,accttype,$accttyp.,%str( ));;
%if &sysparm1=J %then %do;
   %manyvar(C164,C164,item,$3.,5,%str( ));
   %blanks(C167,C167,addrcd);
   %blanks(C183,C183,authid);
%end;
%else %do;
 %blanks(C165,C165,merch);
%end;



***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'auths.rtf';

data _null_;
 file a notitles ls=1000;

%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Authorization Transaction File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Authorization Transaction File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Authorization Transaction File \b0" &e;
	   put &bl "\fs18\b Physical \b0" &e;

****** PHYSICAL ATTRIB ******;

 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
/*
           rec_com=put(&num_rec,comma11.0)||" (sampled at x%)";
           uni_com=put(&num_uniq,comma11.0);
*/
	   put &bl "\fs18\b Attribute \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "Records analyzed" &cl rec_com &cc &cl &e;
	   put &bl "Unique accounts" &cl uni_com  &cc &cl &e;
	   put &bl "Record length" &cl "&rec_len"  &cc &cl &e;
	   &par; &par;

 

****** FIELD SPECIFIC **********;

	%rtf(1,b=0);
	   put &bl "\fs18\b Authorization Transaction File \b0" &e;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "AUTH-ACCT-ID" 	&cl "&acct"  			&cc &cl &e;
	   put &bl "AUTH-DATE" 		&cl "&authdate"			&cc &cl &e;
	   put &bl "AUTH-TIME" 		&cl "&authtime"  		&cc &cl &e;
	   put &bl "AUTH-AMOUNT" 	&cl "&amount"  			&cc &cl &e;
	   put &bl "AUTH-CURR-CODE" 	&cl "&currcode"  		&cc &cl &e;
	   put &bl "AUTH-CURR-RATE" 	&cl "&currrate"	 		&cc &cl &e;
	   put &bl "AUTH-DECISION" 	&cl "&decision"  		&cc &cl &e;
	   put &bl "AUTH-TRAN-TYPE" 	&cl "&trantype"  		&cc &cl &e;

	%if &sysparm1=D %then %do;
	   put &bl "AUTH-AVAIL-BAL" 	&cl "&credit"  			&cc &cl &e;
	   put &bl "AUTH-TOTAL-BALANCE" &cl "&totbal"  			&cc &cl &e;
	%end;
	%else %do;
	   put &bl "AUTH-AVAIL-CRED" 	&cl "&credit"  			&cc &cl &e;
	   put &bl "AUTH-CRED-LINE" &cl "&totbal"  			&cc &cl &e;
	%end;

	%if &sysparm1=R %then %do;
	   put &bl "AUTH-DEPT" 		&cl "\b Overall:\b0 \par &sicover \par \b For cash:\b0 \par &siccash" &cc &cl &e;
	%end;
	%else %do;
	   put &bl "AUTH-SIC" 		&cl "\fs16\b Overall:\b0 \par &sicover \par \b For cash:\b0 \par &siccash \fs18" &cc &cl &e;
	%end;

	   put &bl "AUTH-POSTAL-CODE"	&cl "&postal" 			&cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "AUTH-STORE-CNTRY"	&cl "&country" 			&cc &cl &e;
	%end;
	%else %do;
	   put &bl "AUTH-MERC-CNTRY"	&cl "&country" 			&cc &cl &e;
	%end;

	   put &bl "AUTH-PIN-VER" 	&cl "\b Overall:\b0 \par &pin \par \b For 6011(ATM):\b0 \par &pinatm" &cc &cl &e;
	   put &bl "AUTH-CVV" 		&cl "\b For Swiped (U & S): \b0 \par &cvv \par \b For Keyed:\b0 \par &cvv2"  	&cc &cl &e;
 
	   put &bl "AUTH-KEY-SWIPE" 	&cl "&swipe"		  	&cc &cl &e;
	   put &bl "AUTH-MSG-EXPIRE-DATE" 	&cl "&expdate"  	&cc &cl &e;
	   put &bl "AUTH-AUTH-ID" 	&cl "&auth"	  		&cc &cl &e;
	   put &bl "AUTH-REASON-CODE" 	&cl "&reason"	  		&cc &cl &e;
	   put &bl "AUTH-ADVICE" 	&cl "&advice"	  		&cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "AUTH-STORE-ID" 	&cl "&mrchid"	  		&cc &cl &e;
	%end;
	%else %do;
	   put &bl "AUTH-MRCH-ID" 	&cl "&mrchid"	  		&cc &cl &e;
	%end;

	   put &bl "AUTH-TERM-ID" 	&cl "&termid"	  		&cc &cl &e;
	   put &bl "AUTH-WHICH-CARD" 	&cl "&which"	  		&cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "AUTH-MAX-ITEM" 	&cl "&maxitem"	  		&cc &cl &e;
	   put &bl "AUTH-ITEM-COUNT" 	&cl "&numitem"	  		&cc &cl &e;
	%end;
	%else %do;
	   put &bl "AUTH-CASHBACK-AMT" 	&cl "&cashback"  		&cc &cl &e;
	   put &bl "AUTH-FALCON-SCORE" 	&cl "&falcon" 	 		&cc &cl &e;
	%end;

	%if &sysparm1=D %then %do;
	   put &bl "AUTH-ACCOUNT-TYPE"	&cl "&accttype"	  		&cc &cl &e;
	%end;

	%if &sysparm1=J %then %do;
	   put &bl "AUTH-ITEM-CD"	&cl "&item"	  		&cc &cl &e;
	   put &bl "AUTH-ADDR-CD"	&cl "&addrcd"	  		&cc &cl &e;
	   put &bl "AUTH-TERM-ID"	&cl "&authid"	  		&cc &cl &e;
	%end;

	%else %do;
	   put &bl "AUTH-MERCHANT-NAME"	&cl "&merch"	  		&cc &cl &e;
	%end;


%rtf(100);

run;


%mend;

%run_all;
