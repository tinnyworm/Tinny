libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats/yxc';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

/************************* Add new entries into macrolib.formats **************************/
proc format library= library.credit;
   
   value $payhist    ' ' = 'Blank'
                     'Z' = 'No statement'
		     0-7 = 'No postings'
		     'I' = 'Not delinquent'
		     'J' = '1 cycle delinquent'
		     'K' = '2 cycles delinquent'
		     'L' = '3 cycles delinquent'
		     'M' = '4 cycles delinquent'
		     'N' = '5 cycles delinquent'
		     'O' = '6 cycles delinquent'
		     'P' = '7 cycles delinquent'
		     'Q' = 'Not delinquent'
		     'R' = '1 cycle delinquent'
		     'S' = '2 cycles delinquent'
		     'T' = '3 cycles delinquent'
		     'U' = '4 cycles delinquent'
		     'V' = '5 cycles delinquent'
		     'W' = '6 cycles delinquent'
		     'X' = '7 cycles delinquent'
		     '%' = 'Credit balance with no activity'
		     '#' = 'Credit balance with debit and credit activity'
		     '+' = 'Credit balance with debit activity only'
		     '-' = 'Credit balance with credit activity only'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
run;
		   	   	   
proc format library= library.debit;
   value $payhist    ' ' = 'Blank'
                     'Z' = 'No statement'
		     0-7 = 'No postings'
		     'A' = 'Not delinquent'
		     'B' = '1 cycle delinquent'
		     'C' = '2 cycles delinquent'
		     'D' = '3 cycles delinquent'
		     'E' = '4 cycles delinquent'
		     'F' = '5 cycles delinquent'
		     'G' = '6 cycles delinquent'
		     'H' = '7 cycles delinquent'
		     'Q' = 'Not delinquent'
		     'R' = '1 cycle delinquent'
		     'S' = '2 cycles delinquent'
		     'T' = '3 cycles delinquent'
		     'U' = '4 cycles delinquent'
		     'V' = '5 cycles delinquent'
		     'W' = '6 cycles delinquent'
		     'X' = '7 cycles delinquent'
		     '%' = 'Credit balance with no activity'
		     '#' = 'Credit balance with debit and credit activity'
		     '+' = 'Credit balance with debit activity only'
		     '-' = 'Credit balance with credit activity only'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
run;  

proc catalog catalog=macrolib.credit;
contents;
run;

proc catalog catalog=macrolib.debit;
contents;
run;
		   		   		       
/*proc format 
    library = library.newcard fmtlib;
 select  $Caipsta $Caipdyn $Caipver $Caiprsk $Caipiaa $Cmedia $country $issue;
 run;
