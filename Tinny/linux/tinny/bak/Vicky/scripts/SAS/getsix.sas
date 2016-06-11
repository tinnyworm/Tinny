filename afile pipe "gunzip -c samplewholeauth0204.gz";
filename ofile pipe "gzip >getsixwhole.gz";

data temp2;
infile afile;
input @1 fraudsID $19. 
      @20 authdate $8.
      ;
run;

data _null_;
set temp2;
if fraudsID='403769BJWCBBBMWM' or
   fraudsID='403769CBWCBBBMWM' or
   fraudsID='471924BJTFFXWPMC' or
   fraudsID='471924BJTFJCWTWF' or
   fraudsID='471924BJTFJFBMCK' or
   fraudsID='471924JTBXMXCKMF';
 file ofile;
 put fraudsID authdate ;
 run;
