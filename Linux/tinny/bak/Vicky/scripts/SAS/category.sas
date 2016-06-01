%macro ifile(len);
data _null_;
infile stdin lrecl = 400 missover;
input @1   all  $&len.
      @1   BIN  $6.;

if BIN in ("541180" "543122" "518542" "496604" "492111" "492184" 
           "485600" "448404" "456079" "486401" "486402" "356210") then do;
	    file stdout; put @1 all $&len.; end;
run;

%mend;

%ifile(186);
	    
	    
