data temp (keep=bins BOFA_TSYS);
input extfile $;
infile dummy filevar=extfile end=lastrec;
do until(lastrec=1);
 input bins $6.;
  BOFA_TSYS=extfile;
  format bins $6.;
 output;
end;
cards;
consu09
cmerc09
BofAw09
;
run;

data temp4;
set temp;
length bins_4 $4.;
bins_4=substr(bins, 1, 4);
run;

/*proc print data=temp4 (obs=100);
run;*/

/*ods html body='/home/yxc/summary.html';*/
ods rtf file='/home/yxc/bin_4table09.rtf' style=statdoc;
proc freq data=temp4;
tables BINS_4 * BOFA_TSYS/ norow nocol nopercent;
title 'Frequence of Bins for TSYS and BofA';
run;
ods rtf close;
ods listing;


/*proc print data=tablefreq;
run;*/


/*filename giffile '/home/yxc/bins.gif';
goptions
  reset=all
  dev=gif  
  gsfname=giffile;
  
proc gplot data=tablefreq;
where bofa_tsys in ('BofAe09', 'BofAw09', 'consu09', 'cmerc09');
plot count*bins=bofa_tsys /hminor=0 legend;
title 'Distribution of BINS for Bank of America';
symbol1 v=diamond i=join c=blue l=1 w=2 h=1.5;
symbol2 v=star    i=join c=red  l=1 w=2 h=1.5;
symbol3 v=square   i=join c=green l=2 w=2 h=1.5;
symbol4 v=triangle i=join c=black l=2 w=2 h=1.5;
run;

filename giffile '/home/yxc/binsbar.gif';
proc gchart data=tablefreq;
vbar bins /group = bofa_tsys patternid=midpoints;
pattern1 c=blue;
pattern2 c=red;
pattern3 c=green;
pattern4 c=black;
title c=blue 'Histogram of Bins for Bank of America';
run;*/

