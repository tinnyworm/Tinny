#!/usr/local/bin/perl  -I/home/krh/bin/PM  -I/home/oxb/bin/PM


##############################################################################################
#   Purpose:  Provides histograms and statistics for variables in the Falcon cardholder file
#   
#   Common Usage:  gzcat cards.dat.gz | vcards.pl > vcards.out 
#     
#   Author: krh
#
#
#










use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;


# Define template and titles
$template = "a6 " . "x"x13 . " a3 XXX a5 xxxx a3 a6 XXXXXX a8 a6 XXXXXX a8 a1 a6 XXXXXX a8 a10 a1 a8 a10 a1 a1 a3 a8 " . "x"x57 . " a1 a1 a19 a2 a8";
$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of card 3 digit postal code";$i++;
$title[$i] = "Histogram of card 5 digit postal code";$i++;
$title[$i] = "Histogram of card country";$i++;
$title[$i] = "Histogram of card open month";$i++;
$title[$i] = "Histogram of card open date";$i++;
$title[$i] = "Histogram of card issue month";$i++;
$title[$i] = "Histogram of card issue date";$i++;
$title[$i] = "Histogram of card issue type";$i++;
$title[$i] = "Histogram of card expiration month";$i++;
$title[$i] = "Histogram of card expiration date";$i++;
$title[$i] = "Histogram of card credit line";$cred_line = $i;$i++;
$title[$i] = "Histogram of card gender";$i++;
$title[$i] = "Histogram of card birthdate";$i++;
$title[$i] = "Histogram of card income";$income = $i;$i++;
$title[$i] = "Histogram of card type";$i++;
$title[$i] = "Histogram of card use";$i++;
$title[$i] = "Histogram of card num cards";$num_cards = $i;$i++;
$title[$i] = "Histogram of card record date";$i++;
$title[$i] = "Histogram of card association";$i++;
$title[$i] = "Histogram of card incentive";$i++;
$title[$i] = "Histogram of card xref prev acct";$i++;
$title[$i] = "Histogram of card status";$i++;
$title[$i] = "Histogram of card status date";$i++;
$num_analysis = $i;


# Specify the numeric fields
@numeric = ( $cred_line , $income , $num_cards );



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


  foreach $i (@numeric) {
    $stats[$i]->add_data($data[$i]);
  }


#  Perform any binning of numeric values
   $data[$cred_line] = 100 * int($data[$cred_line]/100 );
   $data[$income] = 10000 * int($data[$income]/10000);




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
