#! /usr/local/bin/perl  -I/home/yxc/scripts/Perl/PM 


use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;

#$template = "\@0 A6 \@19 A14 \@34 A6 \@52 A2 \@59 A1 \@61 A8 \@70 A8 \@79 A1 \@81 A1 \@83 A4 \@88 A1 \@90 A1 \@92 A1 \@94 A4 \@99 A7 \@107 A7 \@116 A89 \@127 A3 \@138 A3 \@142 A16"; 

$template = "\@0 A6 \@19 A14 \@34 A6 \@52 A2 \@59 A1 \@61 A8 \@70 A8 \@79 A1 \@81 A1 \@83 A4 \@88 A1 \@90 A1 \@92 A1 \@94 A4 \@127 A3 \@138 A3 \@142 A16";

$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of auth amount"; $amount=$i; $i++;
$title[$i] = "Histogram of auth date"; $i++;
$title[$i] = "Histogram of auth hour"; $i++;
$title[$i] = "Histogram of Fraud tag";$fraudTag = $i;$i++;
$title[$i] = "Histogram of First Fraud Date"; $i++;
$title[$i] = "Histogram of Detection Date";$i++;
$title[$i] = "Histogram of auth fraud type";$i++;
$title[$i] = "Histogram of Post/Auth flag";$i++;
$title[$i] = "Histogram of score";$score = $i ; $i++;
$title[$i] = "Histogram of Decision";$i++;
$title[$i] = "Histogram of Trans Type";$i++;
$title[$i] = "Histogram of Keyed/Swipped";$i++;
$title[$i] = "Histogram of SIC"; $SIC= $i; $i++;
$title[$i] = "Histogram of Merchant zip"; $i++;
$title[$i] = "Histogram of Merchant country";$i++;
## Add more variables for frauds transactions
$title[$i] = "Histogram of SIC by score";$SICxSCORE = $i ; $i++;
$title[$i] = "Histogram of Amount by score";$amtxscore = $i; $i++;


$num_analysis =$i ;

#@numeric = ($amount, $score, $SICxSCORE, $amtxscore);
@numeric = ($amount, $score); 

foreach $i (@numeric) {
  $is_num[$i] = 1;
  $stats[$i] = Statistics::Statistics->new();
}



for ($i = 0; $i < $num_analysis; $i++) {
  $hists[$i] = Histogram::Histogram->new();
}

#next LABEL 

while(<STDIN>) {
  chomp;
  @data = unpack($template,$_);
#  print $data[$fraudTag], "\n"; 
## ERROR here<<<

#  next LINE if $data[$fraudTag] == 0 ; 
#    
#  Define additional variables
#  print "$data[$amount]\n"; 

### ERROR END >>>>>


  $data[$SICxSCORE] = $data[$SIC] . " & " . $data[$score];
  $data[$amtxscore] = $data[$amount] . " & " . $data[$score]; 


  foreach $i (@numeric) {
    $stats[$i]->add_data($data[$i]);
  }


#  Perform any binning of numeric values
   if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10 );}
   elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50 );}
   else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100 );}
   $data[$score] = 100 * int($data[$score]/100 );
 
  $data[$SICxSCORE] = $data[$SIC] . " & " . $data[$score];
  $data[$amtxscore] = $data[$amount] . " & " . $data[$score]; 

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
