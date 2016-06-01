filename ifile pipe "gunzip -c /aisle18/yxc/merge/usaposts.gz";

data temp;
infile ifile length=linesize;
input @;
rec_len=linesize;
run;

proc freq data=temp;
tables rec_len;
run;
