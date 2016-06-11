data temp;
infile "fdbaea_2002.txt";
input date $6. count @@;
month=mdy(substr(date, 5, 2), 1, substr(date, 1, 4));
keep month count;
run;

filename giffile '/home/yxc/fdbaea.gif';
goptions reset=all
         dev=gif
	 gsfname=giffile
	 ;
axis1 color=black label=(angle=90
proc gchart data=temp;

