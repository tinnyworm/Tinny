********************************************************************
*-- PROGRAM: /work/gold/projects/tape-processing/CAP1-UK/FRAUDS/deselect_if_blank_in_acct.sas --*;
*-- USAGE:  /sw/sas-8.1ts1mo/sas deselect_if_blank_in_acct.sas & --*;
*-- PREREQUISITE JOBS: --*;
options  nocenter linesize=122 pagesize=122 errors=500 missing=' ';
libname  saslib     '/work/touchpoint/sasdata';
filename myin  '/work/gold/projects/tape-processing/CAP1-UK/FRAUDS/frauds.dat';
filename myout '/work/gold/projects/tape-processing/CAP1-UK/FRAUDS/new_frauds.dat';


data _null_;
  infile myin missover lrecl=49  end=eof;
  input @1 acct $char19.
        @1 allchars $char49.;
  acct = trim(acct);
  loc=index(acct,' ');
  len=length(acct);

  if loc lt len then embed+1;
  else if loc ge len then do;
    okay+1;
    file myout;
    put @1 allchars $char49.;
   end;
   
  if eof then do;
   file log;
    put embed= okay=;
   end;
run;

*proc print data=temp (obs=50);
*run;