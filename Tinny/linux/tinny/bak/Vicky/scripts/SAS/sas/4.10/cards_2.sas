%macro run_all;

libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 ls=100 ps=1000 nodate;

%let sysparm1 = %scan(&sysparm,1);
%let strt = %scan(&sysparm,2);
%let stp = %scan(&sysparm,3);
%let exp = %scan(&sysparm,4);

%let prefix=cards;

*** RUN MACROS TO CALCULATE STATS ***;

%read_crd; 

%binmonth(cards,C091);
proc datasets nolist library=work; delete cards; run;
%nobs(C001);
%bins(C001);
%postal(C020,C020,postal,%str( ));
%manyvar(C029,C029,country,$country.,3,%str( ));
%datevar(C032,C032,opendate,%str( ),%str( ),,1); *** future only;
%datevar(C040,C040,issudate,%str( ),%str( ),,1); *** future only ;
%catvar(C048,C048,issue,$issue.,%str( ));
%datevar(C049,C049,expdate,%str( ),1,1,); *** past only;
%if &sysparm1^=D %then %numvar(C057,C057,credline,%str( ));;
%catvar(C067,C067,gender,$gender.,%str( ));
%datevar(C068,C068,dob,%str( ),2,,1); *** future only;
%numvar(C076,C076,income,%str( ));
%catvar(C086,C086,type,$type.,%str( ));
%catvar(C087,C087,use,$use.,%str( ));
%catvar(C088,C088,numcards,$3.,%str( ));
%datevar(C091,C091,recdate,%str( ),1,1,1); *** past & future, plus extra;
%catvar(C099,addr,addr,$missing.,%str( ));
%catvar(C099,addr,addr2,$missing.,%str( ));
%catvar(C151,ssn,ssn,$SSN5dig.,%str( ));
%catvar(C156,C156,assoc,$assoc.,%str( ));
%catvar(C157,C157,incent,$incentv.,%str( ));
%catvar(C158,xref,xref,$missing.,%str( ));
%catvar(C177,C177,status,$status.,%str( ));
%datevar(C177,C179,statdate,%str(where C177 ^in ('NS','');),%str( ),,1); *** future only;
%blanks(C187,C187,portid);
%blanks(C201,C201,mastnum);
%catvar(C217,C217,stat,$blank.,%str( ));
%catvar(C218,C218,dyn,$blank.,%str( ));
%catvar(C220,C220,verify,$blank.,%str( ));
%catvar(C221,C221,rsk,$blank.,%str( ));
%catvar(C222,C222,ia,$blank.,%str( ));
%catvar(C223,C223,ac,$blank.,%str( ));
%catvar(C224,C224,dcycle,$1.,%str( ));
%numvar(C225,C225,delamt,%str( ));
%numvar(C238,C238,ccash,%str( ));
%numvar(C251,C251,cmerch,%str( ));
%manyvar(C264,C264,payhist,$blank.,5,%str( ));
%blanks(C276,C276,addrcode);
%catvar(C291,C291,media,$blank.,%str( ));



***** CREATE RTF-FORMATTED OUTPUT *****;

filename a 'cards.rtf';

data _null_;
 file a notitles ls=1000;

%rtf(0);

******* HEADER INFO *********;
 
	%rtf(1,b=0);
	   put &bl "\fs24\b Cardholder File Data Analysis \b0" &e;
	   put &bl "\fs20 The summary of the data analysis for the Cardholder File is provided in the following tables: " &e;
	   &par;
	%rtf(1,b=0);
	   put &bl "\fs18\b Cardholder File \b0" &e;
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
	   put &bl "\fs18\b Cardholder File \b0" &e;
	   put &bl "\fs18\b Field Specific \b0" &e;
 	%rtf(4, 2 4 4 1, b=1, h=a, v=a);
	   put &bl "\fs18\b Field \b0" &cl "\b Results \b0" &cc "\b Client Comments \b0" &cc "\b Cond. \b0" &e;
	   put &bl "CARD-ACCT-ID" &cl "&acct"  &cc &cl &e;
	   put &bl "CARD-POSTAL-CODE" &cl "&postal"  &cc &cl &e;
	   put &bl "CARD-CNTRY" &cl "&country"  &cc &cl &e;
	   put &bl "CARD-OPEN-DATE" &cl "&opendate"  &cc &cl &e;
	   put &bl "CARD-ISSUE-DATE" &cl "&issudate"  &cc &cl &e;
	   put &bl "CARD-ISSUE-TYPE" &cl "&issue"  &cc &cl &e;
	   put &bl "CARD-EXPIRE-DATE" &cl "&expdate"  &cc &cl &e;
	%if &sysparm1^=D %then %do;
	   put &bl "CARD-CRED-LINE" &cl "&credline"  &cc &cl &e;
	%end;
	   put &bl "CARD-GENDER" &cl "&gender"  &cc &cl &e;
	   put &bl "CARD-BIRTHDATE" &cl "&dob"  &cc &cl &e;
	   put &bl "CARD-INCOME" &cl "&income"  &cc &cl &e;
	   put &bl "CARD-TYPE" &cl "&type"  &cc &cl &e;
	   put &bl "CARD-USE" &cl "&use"  &cc &cl &e;
	   put &bl "CARD-NUM-CARDS" &cl "&numcards"  &cc &cl &e;
	   put &bl "CARD-REC-DATE" &cl "&recdate"  &cc &cl &e;
	   put &bl "CARD-ADDR-LINE-1" &cl "&addr" &cc &cl &e;
	   put &bl "CARD-ADDR-LINE-2" &cl "&addr2" &cc &cl &e;
	   put &bl "CARD-SSN-5" &cl "&ssn"  &cc &cl &e;
	   put &bl "CARD-ASSOC" &cl "&assoc"  &cc &cl &e;
	   put &bl "CARD-INCENTIVE" &cl "&incent"  &cc &cl &e;
	   put &bl "CARD-XREF-PREV-ACCT" &cl "&xref"  &cc &cl &e;
	   put &bl "CARD-STATUS" &cl "&status"  &cc &cl &e;
	   put &bl "CARD-STATUS-DATE" &cl "\b For statused accounts: \b0 \par &statdate"  &cc &cl &e;
	   put &bl "CARD-PORTFOLIO-ID" &cl "&portid"  &cc &cl &e;
	   put &bl "CARD-MASTER-ACCTNUM" &cl "&mastnum"  &cc &cl &e;
	   put &bl "CARD-AIP-STAT" &cl "&stat"  &cc &cl &e;
	   put &bl "CARD-AIP-DYN" &cl "&dyn"  &cc &cl &e;
	   put &bl "CARD-AIP-VERIFY" &cl "&verify"  &cc &cl &e;
	   put &bl "CARD-AIP-RSK" &cl "&rsk"  &cc &cl &e;
	   put &bl "CARD-AIP-IA" &cl "&ia"  &cc &cl &e;
	   put &bl "CARD-AIP-AC" &cl "&ac"  &cc &cl &e;
	   put &bl "CARD-DEL-CYCLE" &cl "&dcycle"  &cc &cl &e;
	   put &bl "CARD-DEL-AMT" &cl "&delamt"  &cc &cl &e;
	   put &bl "CARD-CYCLE-CASH" &cl "&ccash"  &cc &cl &e;
	   put &bl "CARD-CYCLE-MERCH" &cl "&cmerch"  &cc &cl &e;
	   put &bl "CARD-PAY-HIST" &cl "&payhist"  &cc &cl &e;
	   put &bl "CARD-ADDR-CODE" &cl "&addrcode"  &cc &cl &e;
	   put &bl "CARD-MEDIA" &cl "&media"  &cc &cl &e;


%rtf(100);

run;


%mend;
%run_all;
