options linesize = 79 nocenter ;
%let nojunk=egrep -v '(^-|FRAUD-DATA)' | sed 's:| ::' | sed 's:|::' ;

filename rawdata  pipe "dog frauds.200204_200205.raw | &nojunk" ;
filename jcldata           "frauds.200204_200205.jcl"           ;
filename whatsits          "frauds.200204_200205.whatsits"      ;
data _null_ ;
	infile rawdata length = linesize ;
	file   jcldata ;
	input  
		@01 accountid   $char19. 
		@23 fraudtype   $char1.  @ ;
	if fraudtype in('0','1','2','3','4','5','6','7','8','9') then do ;
		fraudtype = " " ;
		input  
		@23 frauddate   $char8. 
		@31 findmethod  $char1.
		@38 datedetect  $char8. 
		;
		end ;
	else do ;
		input 
		@24 frauddate   $char8. 
		@32 findmethod  $char1.
		@39 datedetect  $char8. 
		;
		end ;
	put
		@01 accountid   $char19. 
		@20 fraudtype   $char1.
		@21 frauddate   $char8.
		@29 findmethod  $char1. 
		@36 datedetect  $char8. 
		@44 "      " ;
	if linesize > 49 then do ;
		file whatsits ;
		put _infile_ ;
		end ;
		
