#! /usr/local/bin/perl  -I/home/yxc/scripts/Perl/PM 


use Histogram::Histogram;
use Statistics::Statistics;
use Date::Manip;

#$template = "\@0 A6 \@17 A10 \@28 A4 \@33 A2 \@36 A2 \@39 A2 \@42 A10"; 

#$template = "\@0 A6 \@19 A14 \@34 A6 \@52 A2 \@59 A1 \@61 A8 \@70 A8 \@79 A1 \@81 A1 \@83 A4 \@88 A1 \@90 A1 \@92 A1 \@94 A4 \@127 A3 \@138 A3 \@142 A16";

$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$bin =$i ; $i++;
$title[$i] = "Histogram of Error code"; $err=$i; $i++;
$title[$i] = "Histogram of Score"; $scr = $i;  $i++;
$title[$i] = "Histogram of Reason code 1"; $i++;
$title[$i] = "Histogram of Reason code 2"; $i++;
$title[$i] = "Histogram of Reason code 3"; $i++;
$title[$i] = "Histogram of Model fired "; $seg = $i; $i++;
## Add more variables for frauds transactions
$title[$i] = "Histogram of Error by seg";$errxseg = $i ; $i++;
$title[$i] = "Histogram of Score by seg";$scrxseg = $i ; $i++;

$num_analysis =$i ;

#@numeric = ($amount, $score, $SICxSCORE, $amtxscore);
#@numeric = ($scr); 

#foreach $i (@numeric) {
#  $is_num[$i] = 1;
#  $stats[$i] = Statistics::Statistics->new();
#}



for ($i = 0; $i < $num_analysis; $i++) {
  $hists[$i] = Histogram::Histogram->new();
}

#next LABEL 

while(<STDIN>) {
  chomp;
  @data = split(" ",$_);
#  print $data[$fraudTag], "\n"; 
## ERROR here<<<

#  next LINE if $data[$fraudTag] == 0 ; 
#    
#  Define additional variables
#  print "$data[$amount]\n"; 

### ERROR END >>>>>

  $data[$bin] = substr($data[$bin],0,6); 


 # foreach $i (@numeric) {
 #   $stats[$i]->add_data($data[$i]);
 # }


#  Perform any binning of numeric values
#   if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10 );}
#   elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50 );}
#   else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100 );}
   $data[$scr] = 50 * int($data[$scr]/50 );
 
  $data[$scrxseg] = $data[$seg] . " & " . $data[$scr];
  $data[$errxseg] = $data[$seg] . " & " . $data[$err];

  for ($i = 0; $i < $num_analysis; $i++) {
    $hists[$i]->add_data($data[$i]);
  }
}


for ($i = 0; $i < $num_analysis; $i++)  {
  print $title[$i],"\n";
  if (defined $is_num[$i]) { $hists[$i]->chart(n);$stats[$i]->print();}
  else { $hists[$i]->chart(n);}  
  print "\n\n";
}
