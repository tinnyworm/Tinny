proc format;
  invalue  $ch    '<' = 'L' 
               other = [$CHAR1.];
run;

data _null_;
infile stdin ;
file stdout;
input @1     acct_bin $6.
      @7     acct7    $ch1.
      @8     acct8    $ch1.
      @9     acct9    $ch1.
      @10    acct10   $ch1.
      @11    acct11   $ch1.
      @12    acct12   $ch1.
      @13    acct13   $ch1.
      @14    acct14   $ch1.
      @15    acct15   $ch1.
      @16    acct16   $ch1.
      @17    acct17   $ch1.
      @18    acct18   $ch1.
      @19    acct19   $ch1.
      @20    rest     $30.
      ;
informat acct7 acct8 acct9 acct10 acct11
       acct12 acct13 acct14 acct15 acct16 acct17 acct18 acct19 $ch1.; 
 *put acct_bin acct7 acct8 acct9 acct10 acct11 acct12 acct13 acct14 acct15 acct16
     acct17 acct18 acct19 rest;          
put   @1     acct_bin $6.
      @7     acct7    $1.
      @8     acct8    $1.
      @9     acct9    $1.
      @10    acct10   $1.
      @11    acct11   $1.
      @12    acct12   $1.
      @13    acct13   $1.
      @14    acct14   $1.
      @15    acct15   $1.
      @16    acct16   $1.
      @17    acct17   $1.
      @18    acct18   $1.
      @19    acct19   $1.
      @20    rest     $30.
      ;
run;
