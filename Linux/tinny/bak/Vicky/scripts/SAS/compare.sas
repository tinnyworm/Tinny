filename ffile pipe "gunzip -c frauds.dat.gz";
filename frefer pipe "gunzip -c frauds.reference.gz";

data temp1;
infile ffile;
input @1  acct   $19. 
      @20 ffdtt  $16.
      @36 fddt   $14.
      ;
run;
proc sort data=temp1;
by acct;
run;

data temp2;
infile frefer;
input @1   acct   $19.
      @20 rffdtt  $16.
      @36 rfddt   $14.
     ;
run;
proc sort data=temp2;
by acct;
run;

data temp;
merge temp1 temp2;
by acct;
run;

proc print data=temp (obs=30);
run;

data _null_;
set temp;
file STDOUT;
*put _infile_;
put 
   @1  acct   $19. 
   @20 ffdtt  $16.
   @36 fddt   $14.
   @50 rffdtt $16.
   @66 rfddt  $14.
   ;
run;   

run;

