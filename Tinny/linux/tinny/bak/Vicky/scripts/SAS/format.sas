options nocenter ls=80 ps=60 obs=max errors=1000 compress=yes;

data _null_;

infile stdin length=linesize;
file stdout;
input 
@1   ACCOUNT   $19.
@20  AMOUNT    $13.
@33  CURRCODE  $3.
@36  CURRRATE  $13.
@49  POSTDATE  $8.
@57  TRANDATE  $8.
@65  TRANTIME  $6.
@71  AVAILCRED $10.
@81  CREDLINE  $10.
@91  TRANTYPE  $1.
@92  AUTHED    $1.
@93  SIC       $4.
@97  POSTALCODE $9.
@106 MRCHCNTRY  $3.
@109 AUTHID     $6.
@115 POSTID     $6.
@121 MRCHID     $16.
@137 TERMID     $6.
@143 WHICHCARD  $1.
@144 CASHBACK   $13.
@157 FALSCORE   $4.
@161 FILLER     $1.
@162 MERCHNAME  $40.
;

put @1 _infile_; 

if linesize=175 then do;
 file record175;
 put 'length of 175 in Record Number' _n_;
 put _infile_;
 delete;
 end;

if (_error_) then do;
  file print;
  put 'Error in Record Number' _n_;
  put _infile_;
  delete;
end;

/*
proc print data=&dsname (obs=100);
var account amount currcode currrate postdate trandate trantime availcred credline trantype authed
    sic postalcode mrchcntry authid postid mrchid termid whichcard cashback falscore filler merchname;
run;*/
