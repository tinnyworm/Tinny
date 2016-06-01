/*
################################################################################
# $Source: /work/gold/projects/src/sas/fdrdata/RCS/read_fdrsmf9801.sas,v $
# $Id: read_fdrsmf9801.sas,v 1.8 2001/03/06 18:46:31 jag exp $
################################################################################
# $Log: read_fdrsmf0207.sas,v $
# Revision 1.9  2002/12/05  18:46:31  jwb
# Layout position for frdind,fdrind,typlos changed by FDR.
#
# $Log: read_fdrsmf9801.sas,v $
# Revision 1.8  2001/03/06  18:46:31  jag
# Additional comments added.
#
# Revision 1.7  2001/01/10  20:45:48  jag
# Updated to work with SAS v7+.
# Corrected auto conversion/comparisions between numeric and character fields.
#
# Revision 1.6  2000/12/26  23:44:01  jag
# Corrected Y2K bug that would not handle transaction dates in 2000 correctly.
# Cleaned-up code for readability.
#
# Revision 1.5  1999/04/15  08:00:00  rgd
# Adjustments made for FDR layout changes.
#
# Revision 1.4  1997/08/10  08:00:00  tjh
# Massive clean-up.
# Remove silly dloss vs doff stats.
#
# Revision 1.3  1996/06/24  08:00:00  tjh
# Clean-up code for 1995 FDR data reading.
#
# Revision 1.2  1995/08/14  08:00:00  hst_&_tjh
# frauds.dat needs to be sorted by acct_id.
# Correct WARNING about format being too long.
#
# Revision 1.1  1995/08/11  08:00:00  hst_&_hst
# Fix total bug (i.e. total=amount, not 0).
#
# Revision 1.0  1995/08/09  08:00:00  hst_&_tjh
# Initial revision
#
################################################################################

NOTE: Layout changes in FDR Technical Document will require changes
*/

options errors=5;

title1 "Security Master File";

proc format;
  value $typflg
    '00'  = 'L'  /* Lost        */
    '01'  = 'S'  /* Stolen      */
    '02'  = 'N'  /* Non-Receipt */
    '03'  = 'A'  /* Application */
    '04'  = 'C'  /* Counterfeit */
    '05'  = 'O'  /* Other       */
    '06'  = 'M'  /* Mail Order  */
    other = 'U'  /* Unknown     */
  ;
  value $chgtyp
    '0'   = '0: Pending'
    '1'   = '1: Fraud'
    '2'   = '2: Made by Cardholder'
    '3'   = '3: Charged Back to Interchange'
    '4'   = '4: Charged Back to Merchant'
    '5'   = '5: Counterfeit'
    '6'   = '6: Phone Fraud'
    other = 'Unknown'
  ;
run;

data base trans;
  infile STDIN recfm=s370vb lrecl=1000 missover;
  input
    @001 system   $EBCDIC4.0   /* System               */
    @005 prin     $EBCDIC4.0   /* Prin                 */
    @009 agent    $EBCDIC4.0   /* Agent                */
    @013 acctid   $EBCDIC16.0  /* Account id           */ 
    @029 rectyp   $EBCDIC2.0   /* 1 = base, 4 = detail */
    @                          /* Hold input record    */
  ;

  select (rectyp);
    when ('01') do;  /* BASE (master) RECORD */
      input 
        @239 status   $EBCDIC1.0  /* SMW1-STAT                              */
        @599 Tdloss   S370FPD4.0  /* SMW2-DATE-LOSS (DOFF if this is fraud) */
        @604 Tfraddt  S370FPD4.0  /* SMW2-FRAUD-DATE                        */
        @620 Tfrdate  S370FPD4.0  /* SMW2-FR-DATE                           */
	/*** changed following positions, jwb 12/5/02 ***/
        @630 frdind   $EBCDIC1.0  /* SMW1-FRAUD                             */
        @631 fdrind   $EBCDIC1.0  /* SMW1-FDR-IND                           */
        @876 typlos   $EBCDIC2.0  /* SMW2-TYPE-LOSS                         */
      ;
    end;
    when ('04') do;  /* DETAIL (transaction) RECORD */
      input
        @058 amount  S370FPD7.2 /* SMW4-TRAN-AMT                               */
        @135 Ttrdate S370FPD4.0 /* SMW4-TRAN-DATE  (sometimes maybe post date) */
        @155 chgtyp  $EBCDIC1.0 /* SMW4-CHARGE-TYPE                            */
      ;
    end;
    otherwise;
  end;

  /* Convert date fields into SAS dates */
  dloss  = input(put(Tdloss,z6.),??yymmdd6.);
  fraddt = input(put(Tfraddt,z6.),??yymmdd6.);
  frdate = input(put(Tfrdate,z6.),??yymmdd6.);
  trdate = input(put(Ttrdate,z6.),??yymmdd6.);
  drop Tdloss Tfraddt Tfrdate Ttrdate;

  /* Eliminate chargeback to cardholder and pendings */
  select (rectyp);
    when ('01') output base;
    when ('04') do;
      if (chgtyp ne '02') & (chgtyp ne '00') then output trans;
    end;
    otherwise;
  end;

  attrib
    system   format=$char4.    label='System'
    prin     format=$char4.    label='Prin'
    agent    format=$char4.    label='Agent'
    acctid   format=$char16.   label='Account ID'
    rectyp   format=$char2.    label='Record Type'
    status   format=$char1.    label='SMW1-STAT'
    dloss    format=yymmdd10.  label='SMW2-DATE-LOSS'
    fraddt   format=yymmdd10.  label='SMW2-FRAUD-DATE'
    frdate   format=yymmdd10.  label='SMW2-FR-DATE'
    frdind   format=$char1.    label='SMW1-FRAUD'
    fdrind   format=$char1.    label='SMW1-FDR-IND'
    typlos   format=$typflg.   label='SMW2-TYPE-LOSS'
    amount   format=dollar13.2 label='SMW4-TRAN-AMOUNT'
    trdate   format=yymmdd10.  label='SMW4-TRAN-DATE'
    chgtyp   format=$char1.    label='SMW4-CHARGE-TYPE'
  ;
run;

*******************************************************************************;

/* Summary statistics and sample observations */
proc freq data=base;
  tables typlos;
  title2 "Frequency of Type of Loss";
run;

proc freq data=base;
  tables status;
  title2 "Frequency of Status";
run;

proc freq data=base;
  tables rectyp*chgtyp;
  format chgtyp $chgtyp.;
  title2 "Frequency of Record Format by Charge Type";
run;

proc print label data=base(obs=10);
  title2 "Base Records";
run;

proc print label data=trans(obs=10);
  title2 "Transaction Records";
run;

proc sort data=trans;
  by acctid trdate;
run;

*******************************************************************************;

/* Create a data set with only the first transaction for each account */
data firstrx;
  set trans;
  by acctid;

  keep acctid system prin agent total earliest amount;
  retain total earliest;

  if first.acctid then do;
    total=amount;
    earliest=trdate;
  end;
  else total=total+amount;

  if last.acctid then output;

  attrib
    total    format=dollar13.2 label='Total amount lost to fraud'
    earliest format=yymmdd10.  label='Date of Earliest Fraud Transaction'
  ;
run;

proc print label data=firstrx(obs=10);
  title2 "Detail Records: First Transactions Only";
run;

proc sort data=base;
  by acctid;
run;

proc print label data=base(obs=10);
  title2 "Base Records: Sorted";
run;

*******************************************************************************;

/* Merge transaction and base records  */
data combo;
  merge base(in=a) firstrx(in=b);
  by acctid;

  ind = a + 2*b;
  
  doff = input("691228",yymmdd6.);   /* default condition */

  select (ind);
    when (1) doff = dloss;    /* base only: doff is dloss or set fraud date */
    when (3) doff = earliest; /* both: doff is date of earliest tran        */
    otherwise;
  end;

  attrib doff format=yymmdd10. label='Date of First Fraud';
 
  keep doff ind acctid system prin agent fraddt dloss total earliest typlos status;
run;

proc print label data=combo(obs=10);
  title2 "Merged Transaction and Base Records ";
run;

/* Keep only transactions with populated SMW2-FRAUD-DATE fields */
data tmp;
  set combo;
  by acctid;

  if doff ne 0 
    then doff_dt = compress(put(doff,yymmdd10.),"-");
    else doff_dt = repeat(' ',6);

  if (fraddt > 0) then output tmp; 
run;

proc sort data=tmp;
  by acctid doff;
run;

proc print label data=tmp(obs=100);
  title2 "Fraud file DataSet";
run;

options missing=' ';

data _null_ ;
  file STDOUT;
  set tmp;

  put
    @01  acctid
    @20  typlos
    @21  doff_dt 
    @50  system
    @54  prin
    @58  agent
    @62  total 13.2
  ;
run;
