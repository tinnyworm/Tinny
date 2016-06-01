%macro graph;


******* GRAPH OPTIONS **********;
goptions
  reset=all xpixels=750 ypixels=600 ;

%global title;
%inc "bank.names";
filename fileref "&sysparm..gif";
%put &sysparm;

goptions
  reset=global gunit=pct cback=white ctext=blue
  ftitle=zapfb ftext=swissb htitle=3.5 htext=2.5
  device=gif
  gsfname=fileref
  colors=(red green blue yellow orange purple);


******* READ IN DATA *******;

data temp; infile STDIN lrecl=120 pad missover;
  length type $8 dsn $17;
  input file $ date $ reclen $ records  dsn $;
  nonmon=index(dsn,"NON");
  auth=  index(dsn,"AUTH");
  post=  index(dsn,"POST");
  pay=   index(dsn,"PY");
  fraud= index(dsn,"FRAUD");
  lrec=reclen+0;
  if "&sysparm"="FINALBA" & lrec=221 then type='auths';
  else if "&sysparm"="BARCLAY-CREDIT" & lrec=200 then type='cards';
  else if "&sysparm"="BARCLAY-CREDIT" & lrec=120 then type='payments';
  else if %bquote(&sysparm)="BARCLAY-CREDIT" & lrec=160 & auth>0 then type='auths';
  else if %bquote(&sysparm)="BARCLAY-CREDIT" & lrec=160 & post>0 then type='posts';
  else if %bquote(&sysparm)="BARCLAY-CREDIT" & lrec=60 & nonmon>0 then type='nonmons';
  else if %bquote(&sysparm)="BARCLAY-CREDIT" & lrec=60 & fraud>0 then type='_frauds';
  else if %bquote(&sysparm)="BARCLAY-CORPORATE" & lrec=201 & pay>0 then type='payments';
  else if "&sysparm"="FINALBA" & lrec=211 then type='cards';
  else if "&sysparm"="FINALBA" & lrec=276 then type='posts';
  else if "&sysparm"="LEUMI" & lrec=50 then type='_frauds';
  else if "&sysparm"="FDRL-GB_826" & lrec=160 then type='posts';
  else if "&sysparm"="EXPERIAN" & lrec=122 then type='payments';
  else if "&sysparm"="EXPERIAN" & lrec=63 then type='_frauds';
  else if "&sysparm"="EXPERIAN" & lrec=200 then type='cards';
  else if lrec in (186,202) then type='cards';
  else if lrec in (164,204,195,182) then type='auths';
  else if lrec in (179,201,161) then type='posts';
  else if lrec in (107,108) then type='payments';
  else if lrec in (62,47) then type='nonmons';
  else if lrec=49 then type='_frauds';
  else delete;
  create=input(date,yymmdd8.);
proc sort; by type create;


proc univariate noprint; by type create;
  var records;
  output out=out sum=sum;
run;

data all; set out; if type^='_frauds'; 
data fraud(keep=fsum create type); set out; if type='_frauds'; fsum=sum;
  do while (mod(fsum,10)>0);
	fsum+1;
  end;
run;


**fix type axis***;
proc univariate data=all noprint; var sum; output out=max max=max; run;
data _null_; set max; 
call symput('imax',put(max,8.));
run;
%if &imax<1000 %then %do; %let divise=1; %let tit=''; %end;
%else %if &imax>=1000 & &imax<1000000 %then %do; %let divise=1000; %let tit=(thousands); %end;
%else %if &imax>=1000000 %then %do; %let divise=1000000; %let tit=(millions); %end;
%else %do; %let divise=1; %let tit=''; %end;


%put max= &imax  divise=&divise tit=&tit;

data all; set all fraud; sum=sum/&divise; format create date7.; run;
footnote1; title1 "Record check for &sysparm data";
proc print data=all; var type create sum fsum;


******* GRAPH STATEMENTS **********;

symbol1 interpol=join value=dot line=33 height=1 color=red;
symbol2 interpol=join value=dot line=1 height=1 ;

axis1  label=(j=r 'Actual' j=r 'Fraud' j=r 'Records' color=red)
	major=(number=3)
        color=red ;
axis2  label=("Date Tapes Created")
        color=blue
        order=('01JAN02'd '01APR02'd '01JUL02'd '01OCT02'd '31DEC02'd )
        ;
axis3  label=(j=l 'Estimated' j=l 'Records' j=l "&tit")
        color=blue ;

legend1 label=none;

title1 j=c color=blue "&title: Falcon Data received for 2002";
footnote1 h=2.5 color=blue j=l "Note: Record counts are upper-bound estimates based on the block size.";


******* PLOT STATEMENTS **********;

proc gplot data=all;
   plot fsum*create/
	vaxis=axis1
	haxis=axis2
        ;
   plot2 sum*create=type  /
	vaxis=axis3
	legend=legend1
	;
  format create date7.;
run;
quit;

%mend graph;
%graph
