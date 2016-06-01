filename giffile "/home/yxc/east.gif";

goptions reset=all dev=gif gsfname=giffile;

proc gplot data=work.Bins;
plot  BofAeast*bins / hminor=0 legend vaxis=axis1;
title 'Distribution of Bank of America East';
axis1 label=(a= -90 h=15pt 'Count');
symbol1 v=diamond c=blue h=1.5;
/*symbol2 v=star    c=red  h=1.5;
symbol3 v=square  c=green h=1.5;
symbol4 v=triangle c=black h=1.5;*/
run;
