libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

/*proc catalog catalog=macrolib.credit;
  contents ;
 run;
 
proc catalog catalog=macrolib.debit;
  contents ;
 run;

proc catalog catalog=macrolib.retail;
  contents ;
 run;

proc catalog catalog=macrolib.formats;
  contents ;
 run;

proc catalog catalog=macrolib.sasmacr;
  contents ;
 run;
 
/* proc datasets library=library;
  contents directory;
 run;*/
 
 /*proc format 
    library = macrolib.credit fmtlib;
 select  $type $status ;
 run; */
 
 /*proc format 
    library = macrolib.formats fmtlib;
 select  $amounts $bins $credit $days $timernge $weekdays $acvv $advice $apin
         $assoc $atype $blank $cashsic $country $decisn $empty $fmethod $ftype
	 $gender $incentv $issue $itype $method $missing  $ntype $posmode $ptype
	 $reverse $sicfmt $ssn5dig $ttype $use $yes_no;
 run;*/
 
  /******************** Create/update a new entry to cover the old one ***********/
 /*proc format library = macrolib.formats;
     value $Fmethod   ' ' = 'Blank: Unknown'
                      'C' = 'C: Customer Reported'
                      'F' = 'F: FI System'
		      'P' = 'P: PVV Check'
		      'S' = 'S: Non-FI System'
		      'V' = 'V: CVV/CVC Check'
                    other = [$CHAR1.];
 run;
 
 proc format library= macrolib.formats fmtlib;
   select $fmethod ;
   run;

/********************** Delete the entries in the format catalog*/
/*proc catalog catalog=macrolib.formats;
  delete fmethd.formatc ;
 run;
 */
/*proc catalog catalog=macrolib.formats;
 contents;
 run; */
   
   
   proc format library= macrolib.formats fmtlib;
   select  $fmethod ;
   run;
 
 
 
