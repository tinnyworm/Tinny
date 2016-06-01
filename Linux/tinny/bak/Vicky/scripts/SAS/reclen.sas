options obs=max compress=yes;

data temp;
infile stdin length=linesize;
input @;
rec_len=linesize;
run;

proc freq data=temp;
tables rec_len/ missing out=table noprint;
run;

data _null_;
set table;
file stdout;
put _all_;
run;

