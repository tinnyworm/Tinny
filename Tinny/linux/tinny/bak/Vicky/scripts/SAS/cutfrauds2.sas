options linesize = 79 nocenter ;
data _null_ ;
	*twodates=input("&sysparm",$17.) ;
	*date1=substr(twodates,1,8) ;
	*date2=substr(twodates,10,8) ;
	date1="20001001";
	date2="20020801";
	call symput("date1", date1) ;
	call symput("date2", date2) ; 
	%put &sysparm ;
	%put &date1 ;
	%put &date2 ;
	
data _null_ ;
	infile stdin  ;
	file   stdout ;
	input 
        	@01 left  $20.
		@21 date  $8.
		@29 right $20.
		;
*	if substr(date,5,2)^='00';
*	if date >= "&date1";
        if date < "&date1" then date = "&date1" ;
	if date > "&date2" then date = "&date2" ;
	put 
		@01 left  $20.
		@21 date  $8.
		@29 right $20.
		;

	
