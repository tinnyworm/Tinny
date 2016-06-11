%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

%let sysparm1 = %scan(&sysparm,1);
%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);
%let exp = %scan(&sysparm,4);

%let prefix = posts;

*** RUN MACROS TO CALCULATE STATS ***;

%read_pst; 

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
%blanks(C143,C143,which);
%catvar(C143,C143,which,$1.,%str( ));
%numvar(C144,C144,cashback,%str( ));
%numvar(C157,C157,falcon,%str( ));
%if &sysparm1=D %then %catvar(C161,C161,accttype,$accttyp.,%str( ));;
%blanks(C162,C162,merch);
%catvar(C192,C192,mcity,$missing.,%str( ));
%manyvar(C205,C205,mstate,$blank.,3,%str( ));
%catvar(C208,C208,trancat,$blank.,%str( ));
%catvar(C209,C209,fraud,$blank.,%str( ));
%catvar(C210,C210,acqid,$missing.,%str( ));
%manyvar(C222,C222,acqcount,$country.,3,%str( ));
%catvar(C225,C225,termid,$missing.,%str( ));
%catvar(C241,C241,termtype,$blank.,%str( ));
%catvar(C242,C242,terment,$blank.,%str( ));
%catvar(C243,C243,poscond,$blank.,%str( ));
%catvar(C245,C245,media,$blank.,%str( ));
%catvar(C246,C246,atm,$1.,%str( ));
%catvar(C247,C247,kcheck,$missing.,%str( ));
%catvar(C253,C253,tvr,$missing.,%str( ));
%blanks(C263,C263,cvr);
%catvar(C273,C273,arqc,$blank.,%str( ));
%catvar(C274,C274,atccrd,$missing.,%str( ));
%catvar(C279,C279,atchost,$missing.,%str( ));
%numvar(C284,C284,loatc,%str( ));
%numvar(C286,C286,olincom,%str( ));
%numvar(C288,C288,oltocard,%str( ));
%numvar(C290,C290,refreq,%str( ));
%datevar(C292,C292,recexp,%str( ),1,1,); *** past only;
%manyvar(C300,C300,item,$3.,3,%str( ));
%blanks(C303,C303,addrcode);
%catvar(C318,C318,paytype,$1.,%str( ));

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
	   put &bl "POST-MCC" &cl "\fs16 &sic \fs18"  &cc &cl &e;
	   put &bl "POST-POSTAL-CODE" &cl "&postal"  &cc &cl &e;
	   put &bl "POST-MRCH-CNTRY" &cl "&mrchcnty"  &cc &cl &e;
	   put &bl "POST-AUTH-ID" &cl "&authid"  &cc &cl &e;
	   put &bl "POST-POST-ID" &cl "&postid"  &cc &cl &e;
	   put &bl "POST-MRCH-ID" &cl "&mrchid"  &cc &cl &e;
	   put &bl "POST-WHICH-CARD" &cl "&which"  &cc &cl &e;
	   put &bl "POST-CASHBACK-AMT" &cl "&cashback"  &cc &cl &e;
	   put &bl "POST-FALCON-SCORE" &cl "&falcon"  &cc &cl &e;

	%if &sysparm1=D %then %do;
	   put &bl "POST-ACCOUNT-TYPE" &cl "&accttype"  &cc &cl &e;
	%end;

	   put &bl "POST-MERCHANT-NAME" &cl "&merch"  &cc &cl &e;
	   put &bl "POST-MRCH-CITY" &cl "&mcity"  &cc &cl &e;
	   put &bl "POST-MRCH-STATE" &cl "&mstate"  &cc &cl &e;
	   put &bl "POST-TRAN-CATEGORY" &cl "&trancat"  &cc &cl &e;
	   put &bl "POST-FRAUD-TRX" &cl "&fraud"  &cc &cl &e;
	   put &bl "POST-ACQ-ID" &cl "&acqid"  &cc &cl &e;
	   put &bl "POST-ACQ-CNTRY" &cl "&acqcount"  &cc &cl &e;
	   put &bl "POST-TERMINAL-ID" &cl "&termid"  &cc &cl &e;
	   put &bl "POST-TERM-TYPE" &cl "&termtype"  &cc &cl &e;
	   put &bl "POST-TERM-ENTRY" &cl "&terment"  &cc &cl &e;
	   put &bl "POST-POS-COND-CODE" &cl "&poscond"  &cc &cl &e;
	   put &bl "POST-CARD-MEDIA" &cl "&media"  &cc &cl &e;
	   put &bl "POST-ATM-NETWORK" &cl "&atm"  &cc &cl &e;
	   put &bl "POST-KCHECK-NUMBER" &cl "&kcheck"  &cc &cl &e;
	   put &bl "POST-TVR" &cl "&tvr"  &cc &cl &e;
	   put &bl "POST-CVR" &cl "&cvr"  &cc &cl &e;
	   put &bl "POST-ARQC-VALID" &cl "&arqc"  &cc &cl &e;
	   put &bl "POST-ATC-CRD" &cl "&atccrd"  &cc &cl &e;
	   put &bl "POST-ATC-HOST" &cl "&atchost"  &cc &cl &e;
	   put &bl "POST-LO-ATC" &cl "&loatc"  &cc &cl &e;
	   put &bl "POST-OL-LIMIT-INCOMING" &cl "&olincom"  &cc &cl &e;
	   put &bl "POST-OL-LIMIT-TOCARD" &cl "&oltocard"  &cc &cl &e;
	   put &bl "POST-RECURRING-FREQUENCY" &cl "&refreq"  &cc &cl &e;
	   put &bl "POST-RECURRING-EXPIRE-DATE" &cl "&recexp"  &cc &cl &e;
	   put &bl "POST-ITEM-CODE" &cl "&item"  &cc &cl &e;
	   put &bl "POST-ADDR-CODE" &cl "&addrcode"  &cc &cl &e;
	   put &bl "POST-PAYMENT-TYPE" &cl "&paytype"  &cc &cl &e;


%rtf(100);

run;


%mend;
%run_all;
