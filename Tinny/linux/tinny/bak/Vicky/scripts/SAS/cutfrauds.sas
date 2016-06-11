options linesize = 79 nocenter ;

data _null_ ;
	infile stdin  ;
	file   stdout ;
	input 
		@01 left  $20.
		@21 date  $8.
		@29 right $20.
		;
*	if date < "&sysparm" then date = "&sysparm" ;
	if date >= "&sysparm" ;
	put 
		@01 left  $20.
		@21 date  $8.
		@29 right $20.
		;

	
