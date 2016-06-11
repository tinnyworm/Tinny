filename ffile pipe "gunzip -c fraudsboth.gz";
filename afile pipe "gunzip -c authswhole0204.gz";
filename ofile pipe "gzip >redo_frads_authwhole.gz";
filename outfile pipe "gzip >sixaccoutwhole.gz";

data temp1;
infile ffile;
input @1 fraudsID $19.
      @20 fraudstype $1.
      @21 fraudsdate $8.;
run;
proc sort data=temp1;
by fraudsID;
run;

data temp2;
infile afile;
input @1 fraudsID $19. 
      @20 authdate $8.;
run;
proc sort data=temp2;
by fraudsID;
run;

data temp;
merge temp1(in=one) temp2(in=two);
by fraudsID;
if one=1 and two=1;
run;

data result;
set temp;
if authdate='20020426' and fraudsdate='20020426';
*file ofile;
*put fraudsID authdate fraudstype fraudsdate;
run;

data _null_;
set result;
if fraudsID='403769BJWCBBBMWM' or
   fraudsID='403769CBWCBBBMWM' or
   fraudsID='471924BJTFFXWPMC' or
   fraudsID='471924BJTFJCWTWF' or
   fraudsID='471924BJTFJFBMCK' or
   fraudsID='471924JTBXMXCKMF';
 file outfile;
 put fraudsID authdate;
 run;


