options linesize = 99 nocenter pagesize=90 obs=max errors=100;

filename auth pipe "gunzip -c auth.AM_CR_MC.gz";
filename post pipe "gunzip -c post.AM_CR_MC.gz";

%macro ifile(afile,ostat1, ostat2);
data &afile;
    infile &afile;
    input @1   amount    11.2
          @14  currcode  $3.
	  @17  mrchcnty  $3.
	  ;
run;    	 

proc sort data=&afile;
by currcode amount; 
run;

proc univariate data=&afile;
by currcode;
output out=&ostat1  n=n mean=mean median=median min=min max=max;
var amount;
run;

proc sort data=&afile ;
by mrchcnty amount;
run;

proc univariate data=&afile;
by mrchcnty;
output out=&ostat2  n=n mean=mean median=median min=min max=max;
var amount;
run;

data &ostat1;
set &ostat1(rename=(currcode=Country));
run;

proc sort data=&ostat1;
by descending n;

/* generate the table.htm*/

ods listing close;
ods html body="/home/yxc/&ostat1..htm";
proc print data=&ostat1;
var Country N Mean Median Min Max;
where n >= 20;
run;
ods html close;
ods listing;

data &ostat2;
set &ostat2(rename=(mrchcnty=Country));
run;

proc sort data=&ostat2;
by descending  n;

ods listing close;
ods html body="/home/yxc/&ostat2..htm";
proc print data=&ostat2;
var Country N Mean Median Min Max;
where n >=20;
run;
ods html close;
ods listing;

%mend;

%ifile(auth, authcc, authmc);
%ifile(post, postcc, postmc);

/*data cc;
set authcc  postcc ;
run;

data mc;
set authmc postmc;
run;

ods listing close;
ods html body='/home/yxc/amountcc.htm';
proc print data=cc;
var Name Country N Mean Median Min Max;
run;
ods html close;
ods listing;

ods listing close;
ods html body='/home/yxc/amountmc.htm';
proc print data=mc;
var Name Country N Mean Median Min Max;
run;
ods html close;
ods listing;

/*data _null_;
set authcc authmc postcc postmc;
file "results";
put _n_ namefile country n mean median min max;
put _all_;
run;

/*data _null_;
set authcc(rename=(currcode=Country)) postcc(rename=(currcode=Country)) 
    authmc(rename=(mrchcnty=Country)) postmc(rename=(mrchcnty=Country));
file "summary";
put country n mean median min max;
run;
proc print data=all;
var Country n mean median min max;
run;*/
