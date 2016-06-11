%let minlen = 35 ;
options linesize = 109 pagesize = 32760 ;

data temp ;
   infile stdin  lrecl = 32760 delimiter = "|" length = lrecl ;
   input @ ;
   if lrecl < &minlen then do ;
      input ;
      delete ;
      end ;
   else do ;
   linesize = lrecl ;  
   length 
      client 
      product_code 
      barcode 
      volser 
      seq 
      media_type 
      location 
      slot 
      content 
      dsn_id 
      dsn 
      seq_total 
      received_date 
      log_date 
      coverage_begin 
      coverage_end 
      test_set 
      data_files_count 
      label_type 
      encoding 
      record_length 
      block_size 
      portfolio_category 
      qa_level 
      first_dump_date 
      dsn_remarks 
      shipment_id 
      waybill 
      carrier 
      sender 
      origin 
      ship_remarks 
      tape_remarks
      $ 40 ;
   
   input 
      client 
      product_code 
      barcode 
      volser 
      seq 
      media_type 
      location 
      slot 
      content 
      dsn_id 
      dsn 
      seq_total 
      received_date 
      log_date 
      coverage_begin 
      coverage_end 
      test_set 
      data_files_count 
      label_type 
      encoding 
      record_length 
      block_size 
      portfolio_category 
      qa_level 
      first_dump_date 
      dsn_remarks 
      shipment_id 
      waybill 
      carrier 
      sender 
      origin 
      ship_remarks 
      tape_remarks
      ;
      end ;
proc freq ;
   tables client ;
*   tables product_code ;
   endsas ;
/*	  
*/



/*
data sbsa ;
	set temp ;
	if client = "sbsa" ;
*/
data merrill ; set temp ; if client = "merrill" ;
   proc sort ;
   by coverage_begin received_date content ;
   

   proc print noobs ;
   var content coverage_begin received_date volser dsn ; 
   format coverage_begin received_date $10. ;

   

   endsas ;



/*
data sentry ;   
   set temp ;
   if product_code = "sentry" ;

proc freq ;
   tables client ;
   endsas ;
data efx ;
   set sentry ;
   if client = "equifax" ;
   
   proc sort ;
   by coverage_begin received_date content client ;
   

   proc print noobs ;
   var client content coverage_begin received_date volser dsn ; 
   format coverage_begin received_date $10. ;

   

   endsas ;
*/



data ufal ;
   set temp ;
   if product_code = "ufal" ;
   proc freq ;
   tables client ;
   endsas ;
   
/*
*/
data boaeast ;
   set ufal ;
   if client in("boa_east","nations","boa") ;
/*
   proc freq ;
   tables received_date ;
*/

data badrdate ;
   set boaeast ;
   if received_date  >= "2000-09-01 00:00:00" or 
      coverage_begin >= "2000-10-01 00:00:00"  ;
   
   proc sort ;
   by coverage_begin received_date content client ;
   

   proc print noobs ;
   var client content coverage_begin received_date volser dsn ; 
   format coverage_begin received_date $10. ;

   

   endsas ;
   

   
data sears ;
   set temp ;
   if client="sears" and content in("auths","frauds") and 
      product_code = "ufal" ;
   if substr(dsn,1,6) = "DO_NOT" ;
   
   proc sort ;
   by volser ;
   proc print ;
   endsas ;
   
   (substr(received_date,1,10) > "2001-06-07") ;
   proc sort ;
   by content received_date ;
   

   proc print noobs ;
   var content coverage_begin received_date volser dsn ; 
   format coverage_begin received_date $10. ;

