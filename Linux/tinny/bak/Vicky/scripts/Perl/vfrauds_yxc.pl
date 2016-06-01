#!/usr/local/bin/perl  -I/home/krh/bin/PM  -I/home/oxb/bin/PM

#####################################################################################
#   Purpose:  Provides histograms and statistics for variables in the Falcon frauds file
#   
#   Common Usage:  gzcat frauds.dat.gz | vfrauds.pl > vfrauds.out 
#     
#   Author: krh
#
#
#










use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;


# Define template and titles
$template = "a6 " . "x"x13 . " a1 a6 XXXXXX a8 a1 a2 xxxx a6 XXXXXX a8 a2";
@weekday = ("1:Monday","2:Tuesday","3:Wednesday","4:Thursday","5:Friday","6:Saturday","7:Sunday"); 
$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of fraud type";$fraud_type = $i;$i++;
$title[$i] = "Histogram of month of first fraud";$first_fraud_month = $i;$i++;
$title[$i] = "Histogram of date of first fraud";$first_fraud_date = $i;$i++;
$title[$i] = "Histogram of fraud find method";$i++;
$title[$i] = "Histogram of hour of first fraud";$i++;
$title[$i] = "Histogram of fraud detection month";$detect_month = $i;$i++;
$title[$i] = "Histogram of fraud detection date";$detect_date = $i;$i++;
$title[$i] = "Histogram of hour of fraud detection";$i++;
$title[$i] = "Histogram of fraud run days";$run_days = $i;$i++;
$title[$i] = "Histogram of first fraud day of week";$first_fraud_day = $i;$i++;
$title[$i] = "Histogram of fraud detection day of week";$detect_day = $i;$i++;
$title[$i] = "Histogram of first fraud month by fraud type";$first_fraud_month_by_type = $i;$i++;
$title[$i] = "Histogram of detection month by fraud type";$detect_month_by_type = $i;$i++;
$num_analysis = $i;


# Specify the numeric fields
@numeric = ($run_days);



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
  if (!(ParseDate($data[$detect_date]) && ParseDate($data[$first_fraud_date]))) {$data[$run_days] = " ";}
  else {$data[$run_days] = $data[$detect_date] - $data[$first_fraud_date];} 
  $data[$first_fraud_month_by_type] = $data[$fraud_type] . " - " . $data[$first_fraud_month];
  $data[$detect_month_by_type] = $data[$fraud_type] . " - " . $data[$detect_month];
  if (!(ParseDate($data[$first_fraud_date]))) {$data[$first_fraud_day] = " ";}
  else {$data[$first_fraud_day] = $weekday[Date_DayOfWeek(unpack("xxxx a2 a2 XXXXXXXX a4",$data[$first_fraud_date])) - 1];}
  if (!(ParseDate($data[$detect_date]))) {$data[$detect_day] = " ";}
  else {$data[$detect_day] = $weekday[Date_DayOfWeek(unpack("xxxx a2 a2 XXXXXXXX a4",$data[$detect_date])) - 1];}


  for ($i = 0; $i < $num_analysis; $i++) {
    $hists[$i]->add_data($data[$i]);
  }
  foreach $i (@numeric) {
    $stats[$i]->add_data($data[$i]);
  }
}
for ($i = 0; $i < $num_analysis; $i++)  {
  print $title[$i],"\n";
  if (defined $is_num[$i]) { $hists[$i]->chart(n);$stats[$i]->print();}
  else { $hists[$i]->chart();}  
  print "\n\n";
}
