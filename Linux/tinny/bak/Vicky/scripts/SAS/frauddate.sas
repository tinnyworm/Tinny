data temp1;
infile 'firstfraud';
input first yymmdd8.;
format first yymmdd8.;
run;

data temp2;
infile 'blockfraud';
input block yymmdd8.;
format block yymmdd8.;
run;

data temp3;
merge temp1 temp2;
run;
proc sort data=temp3;
by first;
run;

data temp;
set temp3;
timeintv=intck( 'day' ,first, block);
retain count 0;
if timeintv le 0 then count+1;
/*length Month $ 4;
substr(Month, 1, 2)=substr(block,1,2);
substr(Month, 3, 2)=substr(block,4,2);*/
month=month(block);
year=year(block);
if year = '2003' then month=12+month;
num_obs=_n_;
drop year;
run;
proc sort data=temp;
by month;
run;
proc print data=temp;
run;

data last;
set temp;
by month;
if last.month;
netcount=dif(count);
run;

proc print data=last;
run;

filename giffile '/home/yxc/frauddate.gif';
goptions reset=all
          dev=gif
	  gsfname=giffile;

proc gplot data=last;
plot (count netcount)*month/overlay hminor=0 legend haxis=axis1 vaxis=axis2;
title 'Distribution of negative values between Block date and First date';
symbol1 v=diamond c=blue i=join l=1 w=2 h=1.5;
symbol2 v=star    c=red  i=join l=2 w=2 h=1.5;
axis1 label=('Month');
axis2 label=(angle=90 'Counts');
run;
