options nocenter nonumber nodate ls=100 ps=1000;

%macro binmonth(dset,datevar) ;

proc printto new print="vtran.&datevar..bins_x_month.list";

**** BINS BY YEAR & MONTH FOR BINS WITH > 1000 CARDHOLDERS ****;
proc sort nodupkey data=&dset out=bins(keep=nbin C004 &datevar); by C004;
proc freq data=bins noprint;
tables nbin / out=nbinout;
run;

proc sort data=nbinout; *where count>500; by nbin;
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



%macro cal_freq(dset, catvar);
proc freq  data=&dset order=freq;
 table &catvar /out=&catvar missing noprint;
 run;
 proc print data=&catvar (obs=20);
 title "Frequence of top 20 percentage -- &catvar";
 run;
 
%mend;

%macro cal_median(dset, catvar);
data temp1;
set &dset;
amt=&catvar+0;
if amt^=. then amtcar=1;
else if &catvar=' ' then amtcar=2;
else amtcar=3;
run;

proc sort data=temp1;
by amt;
run;
proc freq data=temp1;
table amtcar /missing out=amtcar;
run;
proc print data=amtcar;
title2 "Dist'n of &catvar missing or invalid";
run;
run;
proc freq data=temp1 order=freq ;
where amtcar=3;
table &catvar / out=out3 noprint;
run;
proc print data=out3 (obs=20);
title 1 "Frequency of top 20 invalid numbers -- &catvar";
run;

proc univariate data=temp1 noprint;
 var amt;
 output out=&catvar n=n max=max min=min median=median;
run;

proc print data=&catvar;
title3 "Output for summary statisics -- &catvar";
run;
%mend;

data 
post 
C001 (keep=C001)
C004 (keep=C004 length nbin)
C023 (keep=C023)
C027 (keep=C027)
C031 (keep=C031)
C035 (keep=C035)
C039 (keep=C039)
C042 (keep=C042)
C043 (keep=C043)
C044 (keep=C044)
C052 (keep=C052)
C061 (keep=C061)
C071 (keep=C071)
C079 (keep=C079)
C087 (keep=C087)
C100 (keep=C100)
C101 (keep=C101)
C114 (keep=C114)
C127 (keep=C127)
C152 (keep=C152) 
C180 (keep=C180)
C185 (keep=C185)
C188 (keep=C188)
C192 (keep=C192)
     ;
infile stdin lrecl=196 length=len missover;
input @001     C001    $3.
      @004     C004    $19.
      @023     C023    $4.
      @027     C027    $4.
      @031     C031    $4.
      @035     C035    $4.
      @039     C039    $3.
      @042     C042    $1.
      @043     C043    $1. 
      @044     C044    $8.
      @052     C052    $9.
      @061     C061    $10.
      @071     C071    $8.
      @079     C079    $8.
      @087     C087    $13.
      @100     C100    $1.
      @101     C101    $13.
      @114     C114    $13.
      @127     C127    $25.
      @152     C152    $28.
      @180     C180    $5.
      @185     C185    $3.
      @188     C188    $4.
      @192     C192    $5.
      ;
length=len;
nbin=substr(C004,1,6)*1;
run;

%binmonth(post, C044);
/*%cal_freq(C001,C001);
%cal_freq(C004,C004);
%cal_freq(C023,C023);
%cal_freq(C027,C027);
%cal_freq(C031,C031);
%cal_freq(C035,C035);
%cal_freq(C039,C039);
%cal_freq(C042,C042);
%cal_freq(C043,C043);
%cal_median(C044,C044);
*%cal_freq(C052,C052);
%cal_median(C061,C061);
%cal_median(C071,C071);
%cal_median(C079,C079);
%cal_median(C087,C087);
%cal_freq(C100,C100);
%cal_median(C101,C101);
%cal_median(C114,C114);
%cal_freq(C127,C127);
%cal_freq(C152,C152);
%cal_freq(C180,C180);
%cal_freq(C185,C185);
%cal_freq(C188,C188);
%cal_freq(C192,C192);
  
   


       
