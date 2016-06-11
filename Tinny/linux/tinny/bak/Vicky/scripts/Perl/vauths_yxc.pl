#!/usr/local/bin/perl  -I/home/krh/bin/PM  -I/home/oxb/bin/PM

############################################################################################
#   Purpose:  Provides histograms and statistics for variables in the Falcon authorizations file
#   
#   Common Usage:  gzcat auths.dat.gz | vauths.pl > vauths.out 
#     
#   Author: krh
#   Modifier: yxc for new spec
#
#






use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;


# Define template and titles
$template = "a6 " . "x"x13 . " a6 XXXXXX a8 a2 xxxx a13 a3 a13 a1 a1 a10 a10 a4 a3 XXX a5 xxxx a3 a1 a1 a1 a6 XXXXXX a8 a6 a5 a1 a16 xxxxxx a1 a13 a4 a30 a13 a3 a1 a1 a1 a1 a1 a12 a3 a16 a1 a1 a2 a1 a1 a6 a10 a10 a1 a5 a5 a2 a2 a2 a2 a8 a3 a15 a1";

#$template = "\@0 A19 \@19 A8 \@27 A6 \@33 A13 \@46 A3 \@49 A13 \@62 A1 \@63 A1 \@64 A10 \@74 A10 \@84 A4 \@88 A9 \@97 A3 \@100 A1 \@101 A1 \@102 A1 \@103 A8 \@111 A6 \@117 A5 \@122 A1 \@123 A16 \@139 A6 \@145 A1 \@146 A13 \@159 A4 \@163 A1 \@164 A30 \@194 A13 \@207 A3 \@210 A1 \@211 A1 \@212 A1 \@213 A1 \@214 A1 \@215 A12 \@227 A3 \@230 A16 \@246 A1 \@247 A1 \@248 A2 \@250 A1 \@251 A1 \@252 A6 \@258 A10 \@268 A10 \@278 A1 \@279 A5 \@284 A5 \@289 A2 \@291 A2 \@293 A2 \@295 A2 \@297 A8 \@305 A3 \@308 A15 \@323 A1 "

$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of auth month";$i++;
$title[$i] = "Histogram of auth date";$i++;
$title[$i] = "Histogram of auth hour";$i++;
$title[$i] = "Histogram of auth amount";$amount = $i;$i++;
$title[$i] = "Histogram of auth curr code";$curr_code = $i;$i++;
$title[$i] = "Histogram of auth curr rate";$curr_rate = $i;$i++;
$title[$i] = "Histogram of auth decision";$i++;
$title[$i] = "Histogram of auth tran type";$tran_type = $i;$i++;
$title[$i] = "Histogram of auth avail cred";$avail_cred = $i;$i++;
$title[$i] = "Histogram of auth cred line";$cred_line = $i;$i++;
$title[$i] = "Histogram of auth SIC";$SIC = $i;$i++;
$title[$i] = "Histogram of auth 3 digit postal code";$i++;
$title[$i] = "Histogram of auth 5 digit postal code";$i++;
$title[$i] = "Histogram of auth merchant country";$i++;
$title[$i] = "Histogram of auth PIN verify";$PIN = $i;$i++;
$title[$i] = "Histogram of auth CVV";$CVV = $i;$i++;
$title[$i] = "Histogram of auth POS entry";$POS = $i;$i++;
$title[$i] = "Histogram of auth MSG expire month";$i++;
$title[$i] = "Histogram of auth MSG expire date";$i++;
$title[$i] = "Histogram of auth ID";$i++;
$title[$i] = "Histogram of auth reason code";$i++;
$title[$i] = "Histogram of auth advice";$i++;
$title[$i] = "Histogram of auth merchant ID length";$MRCH_ID = $i;$i++;
$title[$i] = "Histogram of auth which card";$i++;
$title[$i] = "Histogram of auth cashback amount";$cashback_amount = $i;$i++;
$title[$i] = "Histogram of auth merchant name"; $MRCH_NM = $i; $i++;
$title[$i] = "Histogram of auth merchant city"; $MRCH_CT = $i; $i++;
$title[$i] = "Histogram of auth merchant state";$i++;
$title[$i] = "Histogram of auth CVV2 present";$i++;
$title[$i] = "Histogram of auth CVV2 response";$i++;
$title[$i] = "Histogram of auth tran category";$i++;
$title[$i] = "Histogram of auth fraud transaction";$i++;
$title[$i] = "Histogram of auth acq ID"; $ACQ_ID = $i; $i++;
$title[$i] = "Histogram of auth acq country";$i++;
$title[$i] = "Histogram of auth terminal ID"; $TERM_ID =$i ; $i++;
$title[$i] = "Histogram of auth falcon score";$i++;
$title[$i] = "Histogram of auth falcon score";$i++;
$title[$i] = "Histogram of auth falcon score";$i++;
$title[$i] = "Histogram of auth falcon score";$i++;
$title[$i] = "Histogram of auth falcon score";$i++;
$title[$i] = "Histogram of auth falcon score";$i++;


$title[$i] = "Histogram of auth SIC 6011 & PIN verify";$SICxPIN = $i;$i++;
$title[$i] = "Histogram of auth POS entry & CVV";$POSxCVV = $i;$i++;
$title[$i] = "Histogram of auth POS entry & PIN verify";$POSxPIN = $i;$i++;
$title[$i] = "Histogram of auth tran type & SIC";$tran_typexSIC = $i;$i++;
$title[$i] = "Histogram of auth curr code & curr rate";$curr_codexcurr_rate = $i;$i++;

$num_analysis = $i;


# Specify the numeric fields
@numeric = ($amount , $curr_rate , $avail_cred , $cred_line , $cashback_amount , $MRCH_ID );



foreach $i (@numeric) {
  $is_num[$i] = 1;
  $stats[$i] = Statistics::Statistics->new();
}

for ($i = 0; $i < $num_analysis; $i++) {
  $hists[$i] = Histogram::Histogram->new();
}
while(<>) {
  chomp;
  @data = unpack($template,$_);


#  Define additional variables
  $data[$SICxPIN] = ($data[$SIC] eq "6011" ? "6011" : "Other") . " & " . $data[$PIN];
  $data[$POSxCVV] = $data[$POS] . " & " . $data[$CVV];
  $data[$POSxPIN] = $data[$POS] . " & " . $data[$PIN];
  while(substr($data[$MRCH_ID],0,1) eq " "){substr($data[$MRCH_ID],0,1) = "";}
  while(substr($data[$MRCH_ID],-1,1) eq " "){chop $data[$MRCH_ID];}
  $data[$MRCH_ID] = length $data[$MRCH_ID];
  while(substr($data[$MRCH_NM],0,1) eq " "){substr($data[$MRCH_NM],0,1) = "";}
  while(substr($data[$MRCH_NM],-1,1) eq " "){chop $data[$MRCH_NM];}
  $data[$MRCH_NM] = length $data[$MRCH_NM];
 $data[$tran_typexSIC] = $data[$tran_type] . " & " . $data[$SIC]; 
  $data[$curr_codexcurr_rate] = $data[$curr_code] . " & " . $data[$curr_rate];


  foreach $i (@numeric) {
    $stats[$i]->add_data($data[$i]);
  }


#  Perform any binning of numeric values
   if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10 );}
   elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50 );}
   else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100 );}
   $data[$avail_cred] = 100 * int($data[$avail_cred]/100 );
   $data[$cred_line] = 100 * int($data[$cred_line]/100 );




  for ($i = 0; $i < $num_analysis; $i++) {
    $hists[$i]->add_data($data[$i]);
  }
}
for ($i = 0; $i < $num_analysis; $i++)  {
  print $title[$i],"\n";
  if (defined $is_num[$i]) { $hists[$i]->chart(n);$stats[$i]->print();}
  else { $hists[$i]->chart();}  
  print "\n\n";
}
