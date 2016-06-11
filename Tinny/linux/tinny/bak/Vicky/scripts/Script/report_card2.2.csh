#!/bin/csh -f
# SCRIPT GENERATES DATA REPORT CARD BASED ON 3.10 DATA CONVENTIONS
# WRITTEN BY JBLUE ON 8/27/2002

alias sas8 /export/home/Applications/sas/sas

# USAGE MESSAGE PRINTS FOR 0 ARGS

if ( $#argv == 0 ) then
	echo " "
	echo "  USAGE: report_card2.2.csh "
	echo "         [ -c <cards_file>   ] [ -a <auths_file>     ]   [ -n <nonmons_file> ] "
	echo "         [ -t <posts_file>   ] [ -p <payments_file>  ]   [ -f <frauds_file>  ] "
	echo "         [ -i <inquiry_file> ] [ -x <intersect_file> ]   [ -w <full SAS temp partition> ]"
	echo "         [ -d/j ]  "
  	echo " "
	echo "  NOTE:    -d option produces debit report card. "
	echo "           -j option produces Japan credit report card."
	echo "              (Otherwise, a US credit report card is produced.)"
	echo " "
	exit 1
endif

set MSGBIN = /work/gold/projects/bin
set BASE = "$MSGBIN/CCDS/3.10"
set AUTODIR = "$BASE/AUTO_2.2"
set UFALUNIQ = /work/price/falcon/bin/ufaluniq
set UFALCUT = /work/price/falcon/bin/ufalcut
set BMSORT = /work/price/falcon/bin/bmsort
set UNIQ = /usr/local/bin/uniq
set CUT = /usr/local/bin/cut
set SORT = /usr/local/bin/sort


   echo "=========================================================="
   echo " Starting report card at `date` "
   echo "=========================================================="
   echo " "

cp $BASE/MACROS/2.2/sasmacr.sas7bcat .

if (! -e date.file) then
	cp /work/gold/projects/bin/CCDS/3.10/AUTO_2.2/date.file .
endif

set start = `grep Start date.file | cut -c8-15`
set stop = `grep Stop date.file | cut -c8-15`
set expire = `grep Expire date.file | cut -c8-15`

   echo "  Date used for Past Date comparisons is $start"
   echo "  Date used for Future Date comparisons is $stop"
   echo "  Date used for Expire Date comparisons is $expire"


# CHECKING FOR ARGUMENTS

while ($#argv)

   # CHECK FOR DEBIT CARD

   if ( "$argv[1]" =~ -[Dd] ) then
	set sysparm = D
   endif

   # CHECK FOR JAPAN CARD

   if ( "$argv[1]" =~ -[Jj] ) then
	set sysparm = J
   endif


   # CHECK FOR CARD FILE

   if ( "$argv[1]" =~ -[Cc] ) then
	set card = 1
	shift
	set card_file = $argv[1]
	if (! -e $card_file ) then
	    echo "$0 : $card_file card file does not exist."
	    unset card 
	endif
   endif

   
   # CHECK FOR AUTH FILE

   if ( "$argv[1]" =~ -[Aa] ) then
	set auth = 1
	shift
	set auth_file = $argv[1]
	if (! -e $auth_file ) then
	    echo "$0 : $auth_file auth file does not exist."
	    unset auth 
	endif
   endif

   
   # CHECK FOR NONMON FILE

   if ( "$argv[1]" =~ -[Nn] ) then
	set nonmon = 1
	shift
	set nonmon_file = $argv[1]
	if (! -e $nonmon_file ) then
	    echo "$0 : $nonmon_file nonmon file does not exist."
	    unset nonmon 
	endif
   endif

   
   # CHECK FOR POST FILE

   if ( "$argv[1]" =~ -[Tt] ) then
	set post = 1
	shift
	set post_file = $argv[1]
	if (! -e $post_file ) then
	    echo "$0 : $post_file post file does not exist."
	    unset post 
	endif
   endif

   
   # CHECK FOR PAYMENT FILE

   if ( "$argv[1]" =~ -[Pp] ) then
	set pay = 1
	shift
	set pay_file = $argv[1]
	if (! -e $pay_file ) then
	    echo "$0 : $pay_file pay file does not exist."
	    unset pay 
	endif
   endif

   
   # CHECK FOR FRAUD FILE

   if ( "$argv[1]" =~ -[Ff] ) then
	set fraud = 1
	shift
	set fraud_file = $argv[1]
	if (! -e $fraud_file ) then
	    echo "$0 : $fraud_file fraud file does not exist."
	    unset fraud 
	endif
   endif


   # CHECK FOR INQUIRY FILE

   if ( "$argv[1]" =~ -[Ii] ) then
	set inquiry = 1
	shift
	set inquiry_file = $argv[1]
	if (! -e $inquiry_file ) then
	    echo "$0 : $inquiry_file inquiry file does not exist."
	    unset inquiry 
	endif
   endif


   # CHECK FOR INTERSECT REPORT

   if ( "$argv[1]" =~ -[Xx] ) then
	set int = 1
	shift
	set intersect_file = $argv[1]
	if (! -e $intersect_file ) then
	    echo "$0 : $intersect_file file does not exist."
	    unset int 
	endif
   endif

   # CHECK FOR SAS TEMP SPACE

   if ( "$argv[1]" =~ -[Ww] ) then
	set wk = 1
	shift
	set work = $argv[1]
	if (! -e $work ) then
	    echo "$0 : can't write to $work."
	    unset wk 
	endif
   endif

   shift

end


# IF NO FILES SPECIFIED NO REPORT IS PRODUCED
if (! $?card && ! $?auth && ! $?nonmon && ! $?post && ! $?pay && ! $?fraud && ! $?inquiry && ! $?int) then
  echo " No files specified. Nothing to do."
  exit 1
endif


# INITIALIZE SYSPARM TO C IF D or J WAS NOT SUPPLIED
if (! $?sysparm ) then
	set sysparm = C
endif

# INITIALIZE WORK TO nobackup1_tmp1 if not specified
if (! $?wk ) then
	set work = /work/Nobackup2_tmp
endif

echo "  Disk used for SAS temp space: $work"



# PROCESS CARD FILE

echo    "------------------ CARD FILE -----------------------------"
if ( $?card ) then
   echo " CARDHOLDER FILE: $card_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set crate = `$AUTODIR/samp_rate.pl $card_file $fraud_file`;
   if ( $card_file =~ *.gz ) then
   	gzip -dc $card_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M  $AUTODIR/cards_2
   else
   	cat $card_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/cards_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif


# PROCESS AUTH FILE

echo    "------------------ AUTH FILE -----------------------------"
if ( $?auth ) then
   echo " AUTH FILE: $auth_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set arate = `$AUTODIR/samp_rate.pl $auth_file $fraud_file`;
   if ( $auth_file =~ *.gz ) then
        if (! -e authdates.tmp) then
 	      gzip -dc $auth_file | $UFALCUT -c20-27 | $BMSORT -m100 -S | $UFALUNIQ > authdates.tmp
	endif 
   	gzip -dc $auth_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/auths_2
   else
        if (! -e authdates.tmp) then
 	   cat $auth_file | $UFALCUT -c20-27 | $BMSORT -m100 -S | $UFALUNIQ > authdates.tmp
	endif 
   	cat $auth_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/auths_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif



# PROCESS NONMON FILE

echo    "---------------- NONMON FILE -----------------------------"
if ( $?nonmon ) then
   echo " NONMON FILE: $nonmon_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set nrate = `$AUTODIR/samp_rate.pl $nonmon_file $fraud_file`;
   if ( $nonmon_file =~ *.gz ) then
   	gzip -dc $nonmon_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M  $AUTODIR/nonmons_2
   else
   	cat $nonmon_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/nonmons_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif



# PROCESS POST FILE

echo    "------------------ POST FILE -----------------------------"
if ( $?post ) then
   echo " POSTING FILE: $post_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set trate = `$AUTODIR/samp_rate.pl $post_file $fraud_file`;
   if ( $post_file =~ *.gz ) then
   	gzip -dc $post_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M  $AUTODIR/posts_2
   else
   	cat $post_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/posts_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif



# PROCESS PAYMENT FILE

echo    "---------------- PAYMENT FILE ----------------------------"
if ( $?pay ) then
   echo " PAYMENT FILE: $pay_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set prate = `$AUTODIR/samp_rate.pl $pay_file $fraud_file`;
   if ( $pay_file =~ *.gz ) then
        gzip -dc $pay_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M  $AUTODIR/payments_2
   else 
        cat $pay_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/payments_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif



# PROCESS FRAUD FILE

echo    "----------------- FRAUD FILE -----------------------------"
if ( $?fraud ) then
   echo " FRAUD FILE: $fraud_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   if ( $fraud_file =~ *.gz ) then
        gzip -dc $fraud_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/frauds_2
   else 
        cat $fraud_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/frauds_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif



# PROCESS INQUIRY FILE

echo    "---------------- INQUIRY FILE ----------------------------"

if ( $?inquiry ) then
   echo " INQUIRY FILE: $inquiry_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   #set irate = `$AUTODIR/samp_rate.pl $inquiry_file $fraud_file`;
   if ( $inquiry_file =~ *.gz ) then
        gzip -dc $inquiry_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/inqs_2
   else 
        cat $inquiry_file | sas8 -work $work -sysparm  "$sysparm|$start|$stop|$expire" -memsize 2000M $AUTODIR/inqs_2
   endif
   echo " COMPLETED SAS PROGRAM:  `date` "
endif


# PROCESS POSTAL CODE INFORMATION

if ( $?card && $?fraud ) then
   cp /work/gold/projects/bin/CCDS/3.10/AUTO_2.2/postal.file.list .
   echo    "---------------- POSTAL CODE ----------------------------"
   echo "Creating Cardholder Postal Code information..."

   if ( $card_file =~ *.gz && $fraud_file =~ *.gz ) then
      fish.pl -b frauds.dat.gz -c1-19 -s1 cards.dat.gz | $UFALCUT -c1-28 | $UNIQ | $UFALCUT -c1-19 | $UFALUNIQ | $CUT -c1-10 | $SORT | $UFALUNIQ >> postal.file.list
   else
      fish.pl -u -b frauds.dat.gz -c1-19 -s1 cards.dat.gz | $UFALCUT -c1-28 | $UNIQ | $UFALCUT -c1-19 | $UFALUNIQ | $CUT -c1-10 | $SORT | $UFALUNIQ >> postal.file.list
   endif

endif


# PROCESS INTERSECT FILE

echo    "---------------- INTERSECT FILE ----------------------------"

if ( $?int ) then
   echo " INTERSECT FILE: $intersect_file "
   echo " BEGINNING SAS PROGRAM:  `date` "
   cat $intersect_file | sas8 $AUTODIR/intersect
   echo " COMPLETED SAS PROGRAM:  `date` "
endif


echo    "-------------- BUILDING REPORT.RTF FILE ------------------"
echo " "


if (-e report.rtf) then 
   rm report.rtf
endif

foreach rtf (cards_int auths_int cards auths nonmons posts payments frauds inquiry)
   if (! -e $rtf.rtf) then
	touch $rtf.rtf
   endif

   cat $rtf.rtf | $AUTODIR/seds.sh >> null.rtf

end

echo "}" >> null.rtf
sed '1,1s/\\rtf1/{\\rtf1/g' < null.rtf >> report.rtf


echo    "---------------- CLEANING UP ZERO SIZE FILES -----------------"
echo " "


foreach file (`ls auths.* cards.* posts.* nonmons.* frauds.* payments.* inquiry.*`)
  set SIZE = `wc -c $file | sed 's/^ *//g' | cut -f1 -d' '`
  if ( $SIZE == 0 ) then
     rm $file
  endif
end
rm null.rtf
rm sasmacr.sas7bcat 

   echo "=========================================================="
   echo " Finished report card at `date` "
   echo "=========================================================="
