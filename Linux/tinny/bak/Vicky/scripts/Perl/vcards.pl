#!/usr/local/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon cardholder file
#     
#   2004    krh initial coding
#   20040723 gg minor edits, usage output
#   20040901 gg made non object oriented to improve speed
#   20040915 gg fixed formatting on print so columns line up in a table
#

$usage = "Usage: zcat cards.dat.gz | vcards.pl >& vcards.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a6 " . "x"x13 . " a3 XXX a5 xxxx a3 a6 XXXXXX a8 a6 XXXXXX a8 a1 a6 XXXXXX a8 a10 a1 a8 a10 a1 a1 a3 a8 " . "x"x57 . " a1 a1 xxxxxxxxxxxxxxxxxxx a2 a8";
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
$title[$i] = "Histogram of card credit line (binned)";$cred_line = $i;$i++;
$title[$i] = "Histogram of card gender";$i++;
$title[$i] = "Histogram of card birthdate";$i++;
$title[$i] = "Histogram of card income (binned)";$income = $i;$i++;
$title[$i] = "Histogram of card type";$i++;
$title[$i] = "Histogram of card use";$i++;
$title[$i] = "Histogram of card num cards";$num_cards = $i;$i++;
$title[$i] = "Histogram of card record date";$i++;
$title[$i] = "Histogram of card association";$i++;
$title[$i] = "Histogram of card incentive";$i++;
$title[$i] = "Histogram of card status";$i++;
$title[$i] = "Histogram of card status date";$i++;
$num_analysis = $i;

# Specify the numeric fields
@numeric = ( $cred_line , $income , $num_cards );
foreach $i (@numeric) {
  $is_num[$i] = 1;
}

$total_lines=0;
while(<>) {
  chomp;
  @data = unpack($template,$_);

  foreach $i (@numeric) {
    if ($data[$i] !~ /^\s*$/){
      $total_num_lines[$i]++;
      $sum[$i] += $data[$i];
      $sumsq[$i] += $data[$i]**2;
      if ($total_lines == 1) {
        $nummin[$i] = $data[$i];
        $nummax[$i] = $data[$i];
      } 
      else {
        $nummin[$i] = $data[$i] if ($data[$i] < $nummin[$i]);
        $nummax[$i] = $data[$i] if ($data[$i] > $nummax[$i]);
      }
    }
  }

  #Perform any binning of numeric values
  if ($data[$cred_line] !~ /^\s*$/){$data[$cred_line] = 100 * int($data[$cred_line]/100 )};
  if ($data[$income] !~ /^\s*$/){$data[$income] = 10000 * int($data[$income]/10000)};

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
  sub sortfunc {
    if ($is_num[$i]){
      $result=$a<=>$b;
      if ($a eq " "){
        $result=-1;
      }
      elsif ($b eq " "){
        $result=1;
      }
    }
    else{
      $result=$a cmp $b;
    }
    $result;
  }
  foreach $x (sort {sortfunc} keys %{$hists[$i]}){
    $pct=100*$hists[$i]{$x}/$total_lines;
    $cumfreq+=$hists[$i]{$x};
    $cumpct=100*$cumfreq/$total_lines;
    $chart_line = "*"x(int($chart_length*($hists[$i]{$x}/$max[$i])));
    printf("%-${maxlength[$i]}s |%-${chart_length}s %9d %9d %6.2f%% %6.2f%%\n",$x,$chart_line,$hists[$i]{$x},$cumfreq,$pct,$cumpct);
  }
  print "\n\n";
  if ($is_num[$i]){
    print "Number of non-missing numeric entries: $total_num_lines[$i]\n";
    print "Sum: ", $sum[$i], "\n";
    print "Mean: ", $sum[$i]/$total_num_lines[$i], "\n";
    if (($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i] - 1)>0 && $nummin[$i]!=$nummax[$i]){
      print "Std Dev.: ", sqrt(($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i] - 1)), "\n";
    }
    else{
      print "Std Dev.: 0\n";
    }
    print "Min: ", $nummin[$i], "\n";
    print "Max: ", $nummax[$i], "\n";
    print "\n\n";
  }
}
