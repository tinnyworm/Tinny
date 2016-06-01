%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

%let sysparm1 = %scan(&sysparm,1,|);
%let strt = %scan(&sysparm,2,|);
%let stp = %scan(&sysparm,3,|);
%let exp = %scan(&sysparm,4,|);

%let prefix = auths;

*** RUN MACROS TO CALCULATE STATS ***;

%if &sysparm1=R %then %do;
  %ret_auth;
%end;

%else %do; %read_aut; %end;

%binmonth(auth,C020);
proc datasets nolist library=work; delete auth; run; 
%nobs(C001);
%bins(C001); 
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
%blanks(C146,C146,which);
%numvar(C147,C147,cashback,%str( ));
%numvar(C160,C160,falcon,%str( ));
%if &sysparm1=D %then %catvar(C164,C164,accttype,$accttyp.,%str( ));;
%blanks(C165,C165,merch);
%manyvar(C195,C195,city,$13.,5,%str( ));
%manyvar(C208,C208,state,$country.,3,%str( ));
%catvar(C211,C211,cvv2pres,$blank.,%str( ));
%catvar(C212,C212,cvv2res,$blank.,%str( ));
%catvar(C213,C213,avs,$blank.,%str( ));
%catvar(C214,C214,tran,$blank.,%str( ));
%catvar(C215,C215,fraud,$blank.,%str( ));
%catvar(C216,C216,acqid,$blank.,%str( ));
%manyvar(C228,C228,acqcount,$country.,3,%str( ));
%blanks(C231,C231,termid);
%catvar(C247,C247,term,$blank.,%str( ));
%catvar(C248,C248,terment,$blank.,%str( ));
%catvar(C249,C249,poscode,$blank.,%str( ));
%catvar(C251,C251,media,$blank.,%str( ));
%catvar(C252,C252,atmnet,$blank.,%str( ));
%catvar(C253,C253,kcheck,$missing.,%str( ));
%catvar(C259,C259,tvr,$missing.,%str( ));
%blanks(C269,C269,cvr);
%catvar(C279,C279,arqcv,$blank.,%str( ));
%catvar(C280,C280,atccrd,$missing.,%str( ));
%catvar(C285,C285,atchost,$missing.,%str( ));
%catvar(C290,C290,loatc,$missing.,%str( ));
%catvar(C292,C292,olinc,$missing.,%str( ));
%catvar(C294,C294,oltocard,$missing.,%str( ));
%numvar(C296,C296,recurf,%str( ));
%datevar(C298,C298,recurexp,%str( ),%str( ),,); *** past only;
%manyvar(C306,C306,item,$3.,3,%str( ));
%blanks(C309,C309,addrcode);
%catvar(C324,C324,payment,$1.,%str( ));
%if &sysparm1=R %then %do;
   %numvar(C325,C325,maxitem,%str( ));
   %numvar(C335,C335,numitem,%str( ));
   %manyvar(C337,C337,dept,$6.,3,%str( ));
   %blanks(C343,C343,pump);
   %catvar(C345,C345,prdcd,$2.,%str( ));
   %catvar(C347,C347,fuel,$2.,%str( ));
   %catvar(C349,C349,dcr,$1.,%str( ));
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
           rec_com=put(&num_rec,comma11.0)||" (sampled at x%)";
           uni_com=put(&num_uniq,comma11.0);
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

	   put &bl "AUTH-MCC" 		&cl "\fs16\b Overall:\b0 \par &sicover \par \b For cash:\b0 \par &siccash \fs18" &cc &cl &e;
	   put &bl "AUTH-POSTAL-CODE"	&cl "&postal" 			&cc &cl &e;
	   put &bl "AUTH-MERC-CNTRY"	&cl "&country" 			&cc &cl &e;
	   put &bl "AUTH-PIN-VER" 	&cl "\b Overall:\b0 \par &pin \par \b For 6011(ATM):\b0 \par &pinatm" &cc &cl &e;
	   put &bl "AUTH-CVV" 		&cl "\b For Swiped (U & S): \b0 \par &cvv \par \b For Keyed:\b0 \par &cvv2"  	&cc &cl &e;
 
	   put &bl "AUTH-POS-ENTRY-MODE" 	&cl "&swipe"		  	&cc &cl &e;
	   put &bl "AUTH-MSG-EXPIRE-DATE" 	&cl "&expdate"  	&cc &cl &e;
	   put &bl "AUTH-AUTH-ID" 	&cl "&auth"	  		&cc &cl &e;
	   put &bl "AUTH-REASON-CODE" 	&cl "&reason"	  		&cc &cl &e;
	   put &bl "AUTH-ADVICE" 	&cl "&advice"	  		&cc &cl &e;
	   put &bl "AUTH-MRCH-ID" 	&cl "&mrchid"	  		&cc &cl &e;
	   put &bl "AUTH-WHICH-CARD" 	&cl "&which"	  		&cc &cl &e;
	   put &bl "AUTH-CASHBACK-AMT" 	&cl "&cashback"  		&cc &cl &e;
	   put &bl "AUTH-FALCON-SCORE" 	&cl "&falcon" 	 		&cc &cl &e;

	%if &sysparm1=D %then %do;
	   put &bl "AUTH-ACCOUNT-TYPE"	&cl "&accttype"	  		&cc &cl &e;
	%end;

	   put &bl "AUTH-MERCHANT-NAME"	&cl "&merch"	  		&cc &cl &e;
	   put &bl "AUTH-MRCH-CITY"	&cl "&city"	  		&cc &cl &e;
	   put &bl "AUTH-MRCH-STATE"	&cl "&state"	  		&cc &cl &e;
	   put &bl "AUTH-CVV2-PRESENT"	&cl "&cvv2pres"	  		&cc &cl &e;
	   put &bl "AUTH-CVV2-RESPONSE"	&cl "&cvv2res"	  		&cc &cl &e;
	   put &bl "AUTH-AVS-RESPONSE"	&cl "&avs"	  		&cc &cl &e;
	   put &bl "AUTH-TRAN-CATEGORY"	&cl "&tran"	  		&cc &cl &e;
	   put &bl "AUTH-FRAUD-TRX"	&cl "&fraud"	  		&cc &cl &e;
	   put &bl "AUTH-ACQ-ID"	&cl "&acqid"	  		&cc &cl &e;
	   put &bl "AUTH-ACQ-CNTRY"	&cl "&acqcount"	  		&cc &cl &e;
	   put &bl "AUTH-TERMINAL-ID"	&cl "&termid"	  		&cc &cl &e;
	   put &bl "AUTH-TERM-TYPE"	&cl "&term"	  		&cc &cl &e;
	   put &bl "AUTH-TERM-ENTRY"	&cl "&terment"	  		&cc &cl &e;
	   put &bl "AUTH-POS-COND-CODE"	&cl "&poscode"	  		&cc &cl &e;
	   put &bl "AUTH-CARD-MEDIA"	&cl "&media"	  		&cc &cl &e;
	   put &bl "AUTH-ATM-NETWORK"	&cl "&atmnet"	  		&cc &cl &e;
	   put &bl "AUTH-KCHECK-NUMBER"	&cl "&kcheck"	  		&cc &cl &e;
	   put &bl "AUTH-TVR"		&cl "&tvr"	  		&cc &cl &e;
	   put &bl "AUTH-CVR"		&cl "&cvr"	  		&cc &cl &e;
	   put &bl "AUTH-ARQC-VALID"	&cl "&arqcv"	  		&cc &cl &e;
	   put &bl "AUTH-ATC-CRD"	&cl "&atccrd"	  		&cc &cl &e;
	   put &bl "AUTH-ATC-HOST"	&cl "&atchost"	  		&cc &cl &e;
	   put &bl "AUTH-LO-ATC"	&cl "&loatc"	  		&cc &cl &e;
	   put &bl "AUTH-OL-LIMIT-INCOMING"	&cl "&olinc"	  		&cc &cl &e;
	   put &bl "AUTH-OL-LIMIT-TOCARD"	&cl "&oltocard"	  		&cc &cl &e;
	   put &bl "AUTH-RECURRING-FREQUENCY"	&cl "&recurf"	  		&cc &cl &e;
	   put &bl "AUTH-RECURRING-EXPIRE-DATE"	&cl "&recurexp"	  		&cc &cl &e;
	   put &bl "AUTH-ITEM-CODE"	&cl "&item"	  		&cc &cl &e;
	   put &bl "AUTH-ADDR-CODE"	&cl "&addrcode"	  		&cc &cl &e;
	   put &bl "AUTH-PAYMENT-TYPE"	&cl "&payment"	  		&cc &cl &e;

	%if &sysparm1=R %then %do;
	   put &bl "AUTH-MAX-ITEM"	&cl "&maxitem"	  		&cc &cl &e;
	   put &bl "AUTH-ITEM-COUNT"	&cl "&numitem"	  		&cc &cl &e;
	   put &bl "AUTH-DEPT"		&cl "&dept"	  		&cc &cl &e;
	   put &bl "AUTH-PUMP"		&cl "&pump"	  		&cc &cl &e;
	   put &bl "AUTH-PRD-CD"	&cl "&prdcd"	  		&cc &cl &e;
	   put &bl "AUTH-FUEL-TYPE"	&cl "&fuel"	  		&cc &cl &e;
	   put &bl "AUTH-DCR-IND"	&cl "&dcr"	  		&cc &cl &e;
	%end;



%rtf(100);

run;


%mend;

%run_all;
