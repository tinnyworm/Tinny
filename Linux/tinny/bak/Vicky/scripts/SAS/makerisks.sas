filename varfile  '../variables/RiskVars.Auth2001';
filename ivarfile './input_sasvars.inc';
filename lvarfile './length_sasvars.inc';

filename indata   './dataRisks.vars';

%macro procfreq(varnamep = , varname0 = , varname = , varlen = , clstrnxt = );
  options
    nocenter
    ps = 50
    ls = 75
    pageno = 1;
  title "Frequency by %upcase(&varname) * tagF";

  %let index = %eval(1 * %substr(&varname0, 4, 4));
  %put "macro index = " &index;
  proc freq data = riskvar1;
    tables &varname0 * tagF / out = risk&index;
  run;
  proc transpose data = risk&index
                 out  = risk&index._t (drop = _name_ _label_
                                       rename = (_0 = cnt_0
                                                 _1 = cnt_1));
    by  &varname0;
    var count;
    id  tagF;
  run;
  data risk&index._t;
    set risk&index._t;
    if cnt_0 eq . then
      cnt_0 = 0;
    if cnt_1 eq . then
      cnt_1 = 0;

    length riskntry 8;
    riskntry = (cnt_1 + 0.5) /
               (cnt_0 + cnt_1 + round(1 / (2 * &avfrdrat), 1));

    length keylngth 8 keystrng $200;
    if substr("&varlen", 1, 1) eq '$' then do;
      keylngth = 1 * substr("&varlen", 2);
      keystrng = 'Key : string[' || trim(left(keylngth)) || '];';
    end;
    else do;
      keylngth = length(left(&varname0));
      keystrng = 'Key : int32;';
    end;

    if
      abs(&avfrdrat - riskntry) lt (2 * sqrt(riskntry*(1-riskntry)/(cnt_0 + cnt_1)))
    then
      delete;
  run;
  data risk&index._t;
    set risk&index._t;

    call symput('numkeys', trim(left(_n_)));
  run;

  *** Related to Cluster Variable - Calculate Risk_Entry Differently ***;
  %put 'macro clstrnxt = ' &clstrnxt;
  %let index2 = %eval(&index - 1);
  %put 'macro index2 = ' &index2;

  %if &clstrnxt eq 2 %then %do;
    data cluster (keep = cluster new_ave);
      set risk&index2._t;

      length cluster $1;
      cluster = &varnamep;

      length new_ave 8;
      new_ave = cnt_1 / (cnt_0 + cnt_1);
    run;

    data clstrnxt (keep = &varname0 cnt_0 cnt_1 keylngth keystrng cluster);
      set risk&index._t;

      length cluster $1;
      cluster = put(&varname0, $cc_cont.);
    run;

    proc sort data = cluster;
      by cluster;
    run;
    proc sort data = clstrnxt;
      by cluster;
    run;

    data new;
      merge
        clstrnxt (in = a)
        cluster;
      by cluster;
      if a;
    run;
    proc sort data = new;
      by &varname0;
    run;
    data risk&index._t (keep = &varname0
                               cnt_0 cnt_1
                               riskntry
                               keylngth keystrng);
      retain &varname0 cnt_0 cnt_1 riskntry keylngth keystrng;
      set new;

      length riskntry 8;
      riskntry = (cnt_1 + 0.5) /
                 (cnt_0 + cnt_1 + round(1 / (2 * new_ave), 1));
    run;
  %end;

  proc print data = risk&index._t;
  run;

  filename type&index "Auth2001RiskT&index";
  data _null_;
    set risk&index._t end = eof;
    file type&index;
    if _n_ eq 1 then do;
      put @1 "Type &varname._risk = record";
      put @3 keystrng;
      put @3 'Risk_Entry : float;';
      put @1 'End;';
      put @1 "Table &varname._risk of &varname._risk";
      put @1 'Default = ';
      if substr("&varlen", 1, 1) eq '$' then do;
        put
          @1 '"'
          @(keylngth + 2) '"'
          @(keylngth + 3) ", &avfrdrat";
        ;
      end;
      else do;
        put @1 "0, &avfrdrat";
      end;
      put @1 "Num_Rows = &numkeys";
      put @1 'Data = ';
    end;

    if substr("&varlen", 1, 1) eq '$' then do;
      put
        @1 '"' &varname0 +(-1) '", ' riskntry
      ;
    end;
    else do;
      put
        @1 &varname0 +(-1) ', ' riskntry
      ;
    end;

    if eof then
      put @1 ;
  run;
%mend procfreq;

*********************************************************************;
*** Read in variable names and create new SAS variable names      ***;
*** since non-SAS variable names may have length greater than 8.  ***;
*********************************************************************;
data riskvar0;
  infile
    varfile
    delimiter = '|'
    missover;
  length varname vartype $200 vartype2 $1;
  input varname vartype;

  *** Convert periods (.) in varname to underscores (_). ***;
  varname = translate(varname, '_', '.');

  length varnamep varname0 $8 varnum varlen $4;
  varnum = put(_n_, z4.);
  if _n_ ne 1 then
    varnamep = 'VAR' || put(_n_ - 1, z4.);
  varname0 = 'VAR' || varnum;

  call symput('numvars', trim(left(_n_)));

  *** For sorting purposes, we want numerics to be first. ***;
  if vartype eq 'N' then do;
    vartype2 = '0';
    varlen   = '8';
  end;
  else do;
    vartype2 = '1';
    varlen   = '$' || substr(vartype, 2);
  end;

  length cluster clstrnxt $1;
  retain cluster clstrnxt '0';
  if index(upcase(varname), '_CLUSTER') gt 0 then do;  /* Cluster Variable */
    cluster = '1';
    clstrnxt = '1';
  end;
  else do;
    cluster = '0';
    if cluster eq '0' and clstrnxt eq '1' then
      clstrnxt = '2';  /* Related to (Previous) Cluster Variable */
    else
      clstrnxt = '0';
  end;
run;

%put 'macro numvars = ' &numvars;

options
  nocenter;
proc print data = riskvar0;
run;

*********************************************************************;
*** Create a file that has the length statement defining the      ***;
*** lengths of the variables.                                     ***;
*********************************************************************;
proc sort data = riskvar0
          out  = riskvarl;
  by vartype2 vartype;
run;

data _null_;
  set
    riskvarl
    end = eof;
  by vartype2 vartype;

  file lvarfile;
  if _n_ eq 1 then do;
    put @03 'length';
    put @05 'tagF     $1;';
    put @03 'length';
  end;

  put   @05 varname0 $8.;
  if last.vartype then do;
    put @14 varlen;
  end;

  if eof then do;
    put @03 ';';
  end;
run;

*********************************************************************;
*** Create a file that has the input statement for the variables. ***;
*********************************************************************;
data _null_;
  set
    riskvar0
    end = eof;

  file ivarfile;
  if _n_ eq 1 then do;
    put @3 'input';
    put @5 'tagF';
  end;
  put @5 varname0 $8.;
  if eof then do;
    put @3 ';';
  end;
run;

*********************************************************************;
*** Read in the data.                                             ***;
*********************************************************************;
data riskvar1;
  infile
    indata
    delimiter = '|'
    missover
    lrecl = 5000;

  *** Define lengths of variables ***;
  %include lvarfile;

  *** Read in data to variables ***;
  %include ivarfile;
run;

/*
data varlist;
  set sashelp.vcolumn;
  where
    libname eq 'WORK' and
    memname eq 'RISKVAR1'
  ;
run;
proc print data = varlist;
run;
*/

*********************************************************************;
*** Calculate the average fraud rate.                             ***;
*********************************************************************;

data avfraudr;
  set riskvar1;
  keep tagF;
run;

proc sort data = avfraudr;
  by tagF;
run;

data avfraudr;
  set avfraudr;
  by tagF;

  retain count_f;
  if first.tagF then
    count_f = 1;
  else
    count_f = count_f + 1;
run;

data avfraudr;
  set avfraudr;
  by tagF;
  if last.tagF;
run;

proc transpose data = avfraudr
               out  = avfrdr_t (drop   = _name_
                                rename = (_0 = cnt_0
                                          _1 = cnt_1));
  id  tagF;
  var count_f;
run;

data avfrdr_t;
  set avfrdr_t;

  length avfrdrat 8;
  avfrdrat = cnt_1 / (cnt_0 + cnt_1);
  call symput('avfrdrat', avfrdrat);
run;

%put 'macro avfrdrat = ' &avfrdrat;

options
  nocenter;
proc print data = avfrdr_t
           noobs;
run;

*********************************************************************;
*** Calculate the frequencies.                                    ***;
*********************************************************************;
options
  nocenter;

proc print data = riskvar0 noobs;
  title 'Variable Names';
  var varname0 varname vartype;
run;

data _null_;
  set riskvar0;

  param = '%procfreq(varnamep = ' ||
          varnamep                ||
          ', varname0 = '         ||
          varname0                ||
          ', varname = '          ||
          trim(varname)           ||
          ', varlen = '           ||
          trim(varlen)            ||
          ', clstrnxt = '         ||
          trim(clstrnxt)          ||
          ');';
  call execute(param);
run;

%macro concat;
  x "/bin/cat Auth2001RiskT1 Auth2001RiskT2 > Auth2001RiskT";
  %do i = 3 %to &numvars;
    %let j = %eval(&i - 1);
    x "/bin/cat Auth2001RiskT Auth2001RiskT&i > Auth2001RiskT0";
    x "/bin/cat Auth2001RiskT0 > Auth2001RiskT";
  %end;

  *** Delete temporary files ***;
  %do i = 1 %to &numvars;
    x "/bin/rm -f Auth2001RiskT&i";
  %end;
  x "/bin/rm -f Auth2001RiskT0";
%mend concat;

%concat;

