options linesize = 79 nocenter ;

libname  here     "./" ;
libname  library  "./" ;

data _null_ ; 
*data temp ;
	infile stdin  ;
	file   stdout ;
	input @01 account $char19. ;
	isdup = put(account,$mbnadup1.) ;
	if isdup in("4","5") then put _infile_ ;

*	proc freq ;
*	tables isdup ;

