proc format;

  value yesno 0='No' 1='Yes';

  value $Ftype
    'A'   = 'A: Application'
    'C'   = 'C: Counterfeit'
    'L'   = 'L: Lost'
    'N'   = 'N: Non-Receipt'
    'O'   = 'O: Other'
    'S'   = 'S: Stolen'
    'T'   = 'T: Takeover'
    'M'   = 'M: Mail/Phone Order'
    'K'   = 'K: Convenience Check'
    'F'   = 'F: Friendly'
    'Z'   = 'Z: Zero Loss'
    ' '   = 'Blank: Unknown'
    other = 'Invalid Data'
  ;

  value $Fmethod
    'C'   = 'C: Customer Reported'
    'F'   = 'F: HNC System'
    'V'   = 'V: CVV/CVC Check'
    'P'   = 'P: PVV Check'
    'S'   = 'S: Non-HNC System'
    ' '   = 'Blank: Unknown'
    other = 'Invalid Data'
  ;

run;


data fraud; infile STDIN lrecl=49 pad missover;
  input @1 bin $6.
	@20 type $1.
	@21 ffd $8.
	@29 detect $1.
	@36 fdd $8.
	;

  ffdate = input (ffd, yymmdd8.);
  fddate = input (fdd, yymmdd8.);

  if ffdate=. then has_date=0; else has_date=1;
  if ffdate=fddate & ffdate^=. then eq_date=1; else eq_date=0;

  format ffdate fddate mmddyy8. type Ftype. detect Fmethod.;
run;

/*
proc print;
  var type ffd ffdate detect fdd fddate;
run;
*/


title1 "ARGENCARD FRAUD DATE ANALYSIS";
title2 "Historical data: 9911-0010";

proc freq data=fraud;
  label has_date='Valid Fraud Date' 
	type='Fraud Type'
	detect='Fraud Method';
  tables has_date has_date*type has_date*detect / list;
  format has_date yesno.;
run;

proc freq data=fraud; where ffdate^=.;
  label eq_date='Fraud Date equals detect' 
	type='Fraud Type'
	detect='Fraud Method';
  tables eq_date eq_date*type eq_date*detect / list;
  format eq_date yesno.;
run;
