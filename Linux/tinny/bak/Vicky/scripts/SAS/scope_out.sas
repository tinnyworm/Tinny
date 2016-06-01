/*
################################################################################
# $Source:$
# $Id:$
################################################################################
# scope_out.sas
# Program uses output of scope (written by rdl) as input.
# Graphic representation of modeling machine usage over time.
################################################################################
# $Log:$
################################################################################
*/

filename scope  '/work/gold/projects/.scope/scope_output';
filename hosts  '/work/gold/projects/.scope/scope_hosts';
filename odsout '/work/gold/projects/.scope/JAG/WWW';

goptions
  reset=all
  xpixels=650
  ypixels=500
;

goptions
  reset=global
  gunit=pct
  cback=white
  ctext=black
  ftitle=zapfb
  ftext=swissb
  htitle=6
  htext=5
  noborder
  nocharacters
  transparency
  device=gif
  nodisplay
;

title1 'Machine Load';
footnote height=3 justify=right "Last Updated: &sysdate &systime";
symbol1 color=blue interpol=join value=dot height=.5;

proc format;
  value $groups
    'ux58'   = 0
    'priceclub'   = 0

    'fraud11'  = 2
    'fraud15'  = 2

    other      = 3
  ;
run;

data group0 group1 group2 group3;
  infile scope lrecl=77 firstobs=5 missover;
  attrib
    host     informat=$char11.
    date1    informat=yymmdd8.
    time1    informat=time8.
    load1    informat=6.2
    load5    informat=6.2
    load15   informat=6.2
    CPU_USER informat=percent6.2
    CPU_Idle informat=percent6.2
    IO_Wait  informat=percent6.2
    FreeMEM  informat=$char7.
  ;

  input host date1 time1 load1 load5 load15 CPU_USER CPU_Idle IO_Wait FreeMEM;

  if (_error_) then
    do;
      put 'Error in record Number' _n_;
      put _infile_;
      delete;
  end;

  if host not in ('ux58','priceclub',
    'fraud11','fraud15') then delete;

  if date1 >= '01jan2001'd;
  date2=put(put(date1,date9.),$9.);
  time2=put(put(time1,time8.),$8.);
  date =input(input(compress(date2||":"||time2),$18.),datetime18.);

  format date1 date9.;
  format time1 time8.;
  format date  datetime8.;

  machine_group = put(host,$groups.);
  select (machine_group);
    when (0) output group0;
    when (1) output group1;
    when (2) output group2;
    when (3) output group3;
  end;
run;

%macro sortem(dsn);
  proc print data=&dsn(obs=20);

  proc sort data=&dsn;
    by host date;
  run;
%mend sortem;

%sortem(group0);
%sortem(group1);
%sortem(group2);
%sortem(group3);

ods listing close;

ods html
  body='load.html'
  contents='loadCont.html'
  frame='loadFram.html'
  path=odsout
  newfile=output
;

axis1
  order=(0 to 15 by 1)
  value=(height=3)
  label=(height=3.5 font=swiss angle=90 color=black 'Load')
  major=(color=black width=1.5)
  minor=none
;

axis2
  value=(height=3)
  label=(height=3.5 font=swiss color=black 'Date')
  major=(color=black width=1.5)
  minor=none
;

%macro plotem(dsn);
  proc gplot gout=gout data=&dsn;
    plot load15*date /
      noframe
      vaxis=axis1
      haxis=axis2
      vref=(4)
      cvref=red
      lvref=1
    ;
    by host;
    label host='00'x;
    format date datetime8.;
  run;
%mend plotem;

%plotem(group0);
%plotem(group1);
%plotem(group2);
%plotem(group3);

goptions display;

proc greplay nofs;
  igout gout;
  tc tempcat;

  tdef fourby des='4x4 plots'

  3/llx=0    lly=0
    ulx=0    uly=50
    urx=50   ury=50
    lrx=50   lry=0

  4/llx=50   lly=0
    ulx=50   uly=50
    urx=100  ury=50
    lrx=100  lry=0

  1/llx=0    lly=50
    ulx=0    uly=100
    urx=50   ury=100
    lrx=50   lry=50

  2/llx=50   lly=50
    ulx=50   uly=100
    urx=100  ury=100
    lrx=100  lry=50;

  template fourby;

  /* group0 */
  treplay 1:gplot   2:gplot1  3:gplot2  4:gplot3;
  treplay 1:gplot4  2:gplot5  3:gplot6  4:gplot7;
  treplay 1:gplot8;

  /* group1 */
  treplay 1:gplot10 2:gplot11 3:gplot12 4:gplot9;

  /* group2 */
  treplay 1:gplot13 2:gplot14 3:gplot16 4:gplot15;

  /* group3 */
  treplay 1:gplot17 2:gplot18 3:gplot19 4:gplot20;
  treplay 1:gplot21 2:gplot22 3:gplot23 4:gplot24;
  treplay 1:gplot25 2:gplot26 3:gplot27 4:gplot28;
  treplay 1:gplot29 2:gplot30 3:gplot31 4:gplot32;
  treplay 1:gplot33 2:gplot34 3:gplot35 4:gplot36;
run;

ods html close;

ods listing;
