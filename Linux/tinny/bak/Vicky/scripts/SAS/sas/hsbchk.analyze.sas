%macro fraud;
options ps=65 ls=130 missing='0' nodate;

%do i=1 %to 8;
filename f020&i "020&i..clean";
data f020&i; infile f020&i lrecl=100 pad missover;
  input @21 yymm $6.;
  output; yymm='Total'; output;
run;

proc freq data=f020&i noprint;
  tables yymm / list out=freq020&i;
run;
data freq020&i(keep=yymm f020&i); set freq020&i;
  f020&i=count;
proc sort; by yymm;

%end;

%do i=11 %to 12;
filename f01&i "01&i..clean";
data f01&i; infile f01&i lrecl=100 pad missover;
  input @21 yymm $6.;
  output; yymm='Total'; output;
run;

proc freq data=f01&i noprint;
  tables yymm / list out=freq01&i;
run;
data freq01&i(keep=yymm f01&i); set freq01&i;
  f01&i=count;
proc sort; by yymm;

%end;

data final; merge
  %do j=1 %to 8; freq020&j %end; freq0111 freq0112 ; by yymm;
  total=sum(f0201,f0202,f0203,f0204,f0205,f0206,f0207,f0208,f0111,f0112);
run;

title1 j=c "HSBC-HK Fraud Accounts by Month";
proc print data=final noobs split="*" ; 
  var yymm f0111 f0112 f0201 f0202 f0203 f0204 f0205 f0206 f0207 f0208 total;
label 
  yymm='Month*Fraud*Occurred'
  f0111='Nov 2001*Tape'
  f0112='Dec 2001*Tape'
  f0201='Jan 2002*Tape'
  f0202='Feb 2002*Tape'
  f0203='Mar 2002*Tape'
  f0204='Apr 2002*Tape'
  f0205='May 2002*Tape'
  f0206='Jun 2002*Tape'
  f0207='Jul 2002*Tape'
  f0208='Aug 2002*Tape'
  total='Total'
;
run;

%mend;

%fraud;
