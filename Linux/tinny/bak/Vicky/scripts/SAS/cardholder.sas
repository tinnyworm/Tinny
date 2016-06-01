data temp;
infile './temp' /*firstobs=200*/ obs=100;
input @1   ID                     $ 19.
      @20  CODE                      9.
      @29  CNTRY                     3.
      @32  open_date           yymmdd8.
      @40  issue_date          yymmdd8.
      @48  issue_type               $1.
      @49  expire_date         yymmdd8.
      @57  cred_line                10.
      @67  gender                   $1.
      @68  birthday            yymmdd8.
      @76  income                   10.
      @86  type                      1.
      @87  use                      $1.
      @88  num_cards                 3.
      @91  rec_date            yymmdd8.
      @99  addr_line_1             $26.
      @125 addr_line_2             $26.
      @151 ssn_5                     5.
      @156 assoc                    $1.
      @157 incentive                $1.
      @158 xref_prev_acct         $ 19.
      @177 status                   $2.
      @179 status_date         yymmdd8.;
format open_date issue_date expire_date birthday rec_date status_date yymmdd8.;
run;

/*proc print data=temp;
run;*/

proc contents data=temp fmtlen;
run;
