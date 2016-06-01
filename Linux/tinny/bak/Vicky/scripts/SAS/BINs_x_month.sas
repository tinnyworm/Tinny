options nocenter nonumber nodate ls=100 ps=1000;
filename auth  pipe "gunzip -c auths.dat.gz";
filename card  pipe "gunzip -c cards.dat.gz";
filename post  pipe "gunzip -c posts.dat.gz";
filename fraud pipe "gunzip -c frauds.dat.gz";
filename nonmn pipe "gunzip -c nonmons.dat.gz";
filename payment pipe "gunzip -c payments.dat.gz";
filename inq   pipe  "gunzip -c inqs.dat.gz";

%macro binmonth(dset,datevar) ;

proc printto new print="&datevar..bins_x_month.list";


proc sort nodupkey data=&dset out=bins(keep=nbin C001 &datevar); by C001;
proc freq data=bins noprint;
tables nbin / out=nbinout;
run;

proc sort data=nbinout;  by nbin;
proc sort data=&dset out=bins(keep=nbin &datevar); by nbin;

data newbins; merge bins(in=a) nbinout(in=b); by nbin;
  if a & b;
  month=substr(&datevar,1,6);
run;
proc chart data=newbins;
  title1 "Bin by month analysis";
  hbar month / group=nbin freq;
run;    
        
proc printto;
        
%mend;  

data 
auth
C001 (keep=C001 length nbin)
C020 (keep=C020)
     ;
infile auth lrecl=204 length=len missover;
input @001     all     $204.
      @001     C001    $19.
      @020     C020    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
card
C001 (keep=C001 length nbin)
C091 (keep=C091)
;
infile card lrecl=186 length=len missover;
input @001     all     $186.
      @001     C001    $19.
      @091     C091    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
post
C001 (keep=C001 length nbin)
C049 (keep=C049)
;
infile post lrecl=201 length=len missover;
input @001     all     $201.
      @001     C001    $19.
      @049     C049    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
fraud
C001 (keep=C001 length nbin)
C036 (keep=C036)
;
infile fraud  length=len missover;
input @001     all     $49.
      @001     C001    $19.
      @036     C036    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
nonm
C001 (keep=C001 length nbin)
C021 (keep=C021)
;
infile nonmn length=len missover;
input @001     all     $47.
      @001     C001    $19.
      @021     C021    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
payment
C001 (keep=C001 length nbin)
C082 (keep=C082)
;
infile payment  length=len missover;
input @001     all     $108.
      @001     C001    $19.
      @082     C082    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

data 
inq
C001 (keep=C001 length nbin)
C021 (keep=C021)
;
infile inq length=len missover;
input @001     all     $34.
      @001     C001    $19.
      @021     inqdate    $8.
      ;
length=len;
nbin=substr(C001,1,6)*1;
run;

%binmonth(auth, C020);
%binmonth(card, C091);
%binmonth(post, C049);
%binmonth(fraud, C036);
%binmonth(nonm, C021);
%binmonth(payment, C082);
%binmonth(inq, inqdate);


       
