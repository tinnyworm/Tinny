#!/usr/local/bin/perl  -I/home/krh/bin/PM  -I/home/oxb/bin/PM

use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;

## for PTLF file#

#@k=(170,174,26,27,176,57,60,8,18,44,29,47,29,138);
#@name=("Tran Date", "Tran Time", "Tran Amount 1", "Tran Amount 2", "Tran Type", "Postcode","Account Expire Date", "Card Expire Date", "Tran Curr Code", "SIC Code", "PIN Verify", "CVV Verify", "Keyed Swiped", "Merchant ID");

## for TLF file##
@k=(143,147,140,139,46,49,37,14,30);
@name=("Tran Date", "Tran Time", "Tran Amount", "Tran Type", "Postcode","Account Expire Date", "Card Expire Date", "Tran Curr Code", "CVV Verify");


# Define template and titles
for ($i=0; $i<@k; $i++){ 
$title[$k[$i]] = "Histogram of $name[$i]";

}
$num_analysis = $i;


# Specify the numeric fields
@numeric = ($k[2] , $k[3]);



foreach $i (@numeric) {
  $is_num[$i] = 1;
  $stats[$i] = Statistics::Statistics->new();
}

for ($i = 0; $i < @k; $i++) {
  $hists[$k[$i]] = Histogram::Histogram->new();
}
while(<>) {
  chomp; 
  @data = split("\t", $_);

#  @data = unpack($template,$_);


 foreach $j (@numeric) {
    $stats[$j]->add_data($data[$j]);
  }


#  Perform any binning of numeric values
#   if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10 );}
#   elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50 );}
#   else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100 );}
#   $data[$avail_cred] = 100 * int($data[$avail_cred]/100 );
#   $data[$cred_line] = 100 * int($data[$cred_line]/100 );

  for ($i = 0; $i < @k ; $i++) {
    $hists[$k[$i]]->add_data($data[$k[$i]]);
  }
}


for ($i = 0; $i < @k; $i++)  {
  print $title[$k[$i]],"\n";
  if (defined $is_num[$k[$i]]) { $hists[$k[$i]]->chart(n);$stats[$k[$i]]->print();}
  else { $hists[$k[$i]]->chart();}  
  print "\n\n";
}
