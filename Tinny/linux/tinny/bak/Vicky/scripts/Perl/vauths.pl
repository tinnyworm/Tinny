#!/usr/local/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon authorizations file
#
#   20040823-24 gg initial coding, some code borrowed from krh's code;
#               non object oriented to improve speed speed
#   20040825    gg added numeric stats, some code borrowed from pcd's code
#   20040915    gg added (binned) to titles, and formatted print on freqs
#               so columns line up in a freq table
#   20040924    gg added negative num check on numeric sqrt std calc since
#               was getting errors due to rounding / precision
#

$usage = "Usage: zcat auths.dat.gz | vauths.pl >& vauths.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a6 " . "x"x13 . " a6 XXXXXX a8 a2 xxxx a13 a3 a13 a1 a1 a10 a10 a4 a3 XXX a5 xxxx a3 a1 a1 a1 a6 XXXXXX a8 a6 a5 a1 a16 xxxxxx a1 a13 a4 ";
$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of auth month";$i++;
$title[$i] = "Histogram of auth date";$i++;
$title[$i] = "Histogram of auth hour";$i++;
$title[$i] = "Histogram of auth amount (binned)";$amount = $i;$i++;
$title[$i] = "Histogram of auth curr code";$curr_code = $i;$i++;
$title[$i] = "Histogram of auth curr rate";$curr_rate = $i;$i++;
$title[$i] = "Histogram of auth decision";$i++;
$title[$i] = "Histogram of auth tran type";$tran_type = $i;$i++;
$title[$i] = "Histogram of auth avail cred (binned)";$avail_cred = $i;$i++;
$title[$i] = "Histogram of auth cred line (binned)";$cred_line = $i;$i++;
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
}

$total_lines=0;
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
  $data[$tran_typexSIC] = $data[$tran_type] . " & " . $data[$SIC]; 
  $data[$curr_codexcurr_rate] = $data[$curr_code] . " & " . $data[$curr_rate];

foreach $i (@numeric) {
  $sum[$i] += $data[$i];
  $sumsq[$i] += $data[$i]**2;
  if ($total_lines == 1) {
    $nummin[$i] = $data[$i];
    $nummax[$i] = $data[$i];
  } else {
    $nummin[$i] = $data[$i] if ($data[$i] < $nummin[$i]);
    $nummax[$i] = $data[$i] if ($data[$i] > $nummax[$i]);
  }
}

#  Perform any binning of numeric values
   if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10 );}
   elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50 );}
   else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100 );}
   $data[$avail_cred] = 100 * int($data[$avail_cred]/100 );
   $data[$cred_line] = 100 * int($data[$cred_line]/100 );

  for ($i = 0; $i < $num_analysis; $i++) {
    $hists[$i]{$data[$i]}++;
    if ($hists[$i]{$data[$i]}>$max[$i]){
      $max[$i]=$hists[$i]{$data[$i]};
    }
    if ((length $data[$i])>$maxlength[$i]){
      $maxlength[$i]=(length $data[$i]);
    }
  }
  $total_lines++;
}

$chart_length = 20;
for ($i = 0; $i < $num_analysis; $i++)  {
  print $title[$i],"\n";
  $cumfreq=0;
  foreach $x (sort {$is_num[$i]? $a<=>$b:$a cmp $b} keys %{$hists[$i]}){
    $pct=100*$hists[$i]{$x}/$total_lines;
    $cumfreq+=$hists[$i]{$x};
    $cumpct=100*$cumfreq/$total_lines;
    $chart_line = "*"x(int($chart_length*($hists[$i]{$x}/$max[$i])));
    printf("%-${maxlength[$i]}s |%-${chart_length}s %9d %9d %6.2f%% %6.2f%%\n",$x,$chart_line,$hists[$i]{$x},$cumfreq,$pct,$cumpct);
  }
  print "\n\n";
  if ($is_num[$i]){
    print "Number of entries: $total_lines\n";
    print "Sum: ", $sum[$i], "\n";
    print "Mean: ", $sum[$i]/$total_lines, "\n";
    if (($sumsq[$i] - $sum[$i]**2/$total_lines)/($total_lines - 1)>0 && $nummin[$i]!=$nummax[$i]){
      print "Std Dev.: ", sqrt(($sumsq[$i] - $sum[$i]**2/$total_lines)/($total_lines - 1)), "\n";
    }
    else{
      print "Std Dev.: 0\n";
    }
    print "Min: ", $nummin[$i], "\n";
    print "Max: ", $nummax[$i], "\n";
    print "\n\n";
  }
}
