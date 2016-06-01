options  nodate nonumber nocenter linesize=122 pagesize=122 errors=500 missing=' ';

filename myin  '/ana/mds_files1/PROFITMAX/CHASE_RETRO/volser_coverage_month.dat'; *237 records;
/*--OFF
Barcode    Volser#     Seq #  Data_Set_Name                          Coverage_Start 
---------  ---------   ------ ------------------------------------   -------------- 
300079158  A67329        1    PCPD.FRM7020.MTHCHMAS.CUST.G0037V00    Mar  1 2003 
300079159  A67345        2    PCPD.FRM7020.MTHCHMAS.CUST.G0037V00    Mar  1 2003 
300079160  A67349        3    PCPD.FRM7020.MTHCHMAS.CUST.G0037V00    Mar  1 2003 
300079161  A67354        4    PCPD.FRM7020.MTHCHMAS.CUST.G0037V00    Mar  1 2003 
*--OFF*/ 


data temp9 (compress=yes);  
  infile myin delimiter=','  pad dsd  missover lrecl=80 end=eof firstobs=3;
  input @1
    barcode    :$9. 
    volser     :$6. 
    seqno      :$2.    
    dsn        :$50. 
    cov_mon    :$3.
    ;


*-- format code table --*;
proc sort data=temp9 out=temp9s nodupkey; *--unique sorted volser;
  by volser cov_mon;
 run;

filename fmt_mon '/ana/mds_files1/PROFITMAX/CHASE_RETRO/fmt_month_to_file.inc';
 data _null_;
  set temp9s end=eof;
  file fmt_mon;
  if _n_ eq 1 then do;
    put @1 '*-- fmt_month_to_file.inc : to assign month to volser --*;';
    put @1 'value $vol2mon';
  end;
 *put @3 '"' ordid $char8. '" = "Y"';
  put @3 '"' volser $char6. '" = "'cov_mon $3. '"';
  if eof then do;
    put @3 'other = "?"';
    put @3 ';';
    end;
   run;

