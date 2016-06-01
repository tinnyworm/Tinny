options linesize = 79 nocenter ;
%let zz=/usr/local/bin/zcat ;
*filename cardhold pipe "&zz cards.withdups.gz" ;
*filename cardhold pipe "&zz /work/pelagius/falcon_data/840/mbna/credit/1pct/200201_200212/cards.dat.gz" ;
libname  here     "./" ;
libname  library  "./" ;

data cardhold ;
	infile stdin ;
	input 
		@01 account  $char19. 
		@32 opendate $char8. 
		@91 recdate  $char6. ;


proc sort  ; by account opendate ;

data counter1 ;
	retain hasdup ;
	set cardhold ;
	by account opendate ;
	
	if first.account then do ;
		hasdup = 0 ;
		end ;
		
	if last.opendate and not last.account then hasdup = 1 ;
	if last.account then output ;	
		
proc freq data = counter1 ; 
	tables hasdup ;

proc sort data = cardhold ;
	by account recdate ;

data counter2 ;
	retain hasdup ;
	set cardhold ;
	by account recdate ;
	
	if first.account then do ;
		hasdup = 0 ;
		end ;
		
	if last.recdate and not first.recdate then hasdup = 1 ;
	if last.account then output ;	
		
proc freq data = counter2 ; 
	tables hasdup ;

data compare ;
    merge 
	counter1 (rename=(hasdup=hasdup1)) 
	counter2 (rename=(hasdup=hasdup2)) ;
	by account ;
proc freq  data = compare ;
	tables hasdup1 * hasdup2 ;
/*
run ; endsas ;
*/
data diff1 diff2 toformat ;
    set compare ;
	if     hasdup1 and not hasdup2 then output diff1 ;
	if not hasdup1 and     hasdup2 then output diff2 ;

	if hasdup1 or hasdup2          then output toformat ;
data toformat ;
	set toformat (rename = (account=start)) ;
	label = "Y" ;
	fmtname = "$mbnadup" ;
	
proc sort nodupkey data = toformat ; by start ;
proc format cntlin = toformat library = here ;

proc print data = diff1 (obs=100);
proc print data = diff2 (obs=100);

proc print data = toformat (obs=100);
