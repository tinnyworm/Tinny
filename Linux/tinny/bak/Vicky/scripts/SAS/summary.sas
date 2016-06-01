
/*filename consumer pipe 'gunzip -c /work/nobackup1_tmp1/freq/2320.gz';
filename cmercial pipe 'gunzip -c /work/nobackup1_tmp1/freq/2330.gz';
filename BofAeast pipe 'gunzip -c /work/nobackup1_tmp1/freq/bofae.gz';
filename BofAwest pipe 'gunzip -c /work/nobackup1_tmp1/freq/bofaw.gz';
filename tableExl '/home/yxc/tablefreq.xls';*/

options nodate notitle;

data temp (keep=bins BOFA_TSYS);
input extfile $;
infile dummy filevar=extfile end=lastrec;
do until(lastrec=1);
 input bins;
  BofA_TSYS=extfile;
 output;
end;
cards;
consumer
cmercial
BofAwest
;
run;

/*ods html body='/home/yxc/summary.html';*/
ods rtf file='/home/yxc/summary.rtf' style=statdoc;
proc freq data=temp;
tables BINS * BOFA_TSYS/ norow nocol nopercent out=tablefreq noprint;
title 'Frequence of Bins for TSYS and BofA';
run;
ods rtf close;
ods listing;

/*proc print data=tablefreq;
run;

filename giffile '/home/yxc/wholebins.gif';
goptions
  reset=all
  dev=gif  
  gsfname=giffile;
  
proc gplot data=tablefreq;
/*where bofa_tsys in ('BofAe09', 'BofAw09', 'consu09', 'cmerc09');*/
/*plot count*bins=bofa_tsys /hminor=0 legend;
title 'Distribution of BINS for Bank of America';
symbol1 v=diamond i=join c=blue l=1 w=2 h=1.5;
symbol2 v=star    i=join c=red  l=1 w=2 h=1.5;
symbol3 v=square   i=join c=green l=2 w=2 h=1.5;
symbol4 v=triangle i=join c=black l=2 w=2 h=1.5;
run;

/*data _null_;
set tablefreq;
file tableExl;
if _n_=1 then put "Bins" "," "TSYS2320"  "," "TSYS2330" "," "BOFAeast" "," "BOFAwest";
put  bins "," tsys ","  bofa ","
run;*/
