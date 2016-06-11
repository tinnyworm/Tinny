%macro category(catvar,dset);
data temp;
 infile &dset;
 input &catvar  ;
 if &catvar = " " then numcat=1;
 else if &catvar = "," then numcat=2;
 else if &catvar = ";" then numcat=3;
 else do;
  clean=compress(&catvar);
  clean2=compress(clean,"-#&/$()*+.,@_:`';");
    clean3=upcase(clean2);
  check='ABCDEFGHIJKLMNOPQRSTUVWXYZ';

if verify(clean3,check)>0 then numcat=4;
else numcat=5;       
end;
run;

proc freq data=temp;
tables numcat / missing ;
title "Summary of &catvar";
run;
%mend category;

%macro numsum(numcat, dset);
data temp;
infile &dset;
input &numcat;
if &numcat = " " then num=1;
else num=2;
run;

proc freq data=temp;
where num=2;
table &numcat / missing out=freq1 noprint;
title "Summary of &numcat";
run;

proc print data=freq1(obs=20);
run;
%mend numsum;

filename acqID    pipe "gunzip -c ../auths.dat.gz | cut -c216-227";
filename mrchcity pipe "gunzip -c ../posts.dat.gz | cut -c192-204";

%category(acqID,acqID);
%numsum(mrchcity,mrchcity);
