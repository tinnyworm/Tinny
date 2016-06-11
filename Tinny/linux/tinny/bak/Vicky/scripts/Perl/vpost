#!/usr/local/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon cardholder file
#     
#   20040901 gg made non object oriented to improve speed
#   20040915 gg fixed formatting on print so columns line up in a table
#

$usage = "Usage: zcat posts.dat.gz | vposts.pl >& vposts.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a6 xxxxxxxxxxxxx a13 a3 a13 a8 a8 a6 a10 a10 a1 a1 a4 a9 a3 xxxxxx xxxxxx a16 xxxxxx a1 a13 a4";

$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of amount (binned)";$amount = $i;$i++;
$title[$i] = "Histogram of currency code";$curr_code = $i;$i++;
$title[$i] = "Histogram of currency rate";$curr_rate = $i;$i++;
$title[$i] = "Histogram of post date";$i++;
$title[$i] = "Histogram of tran date";$i++;
$title[$i] = "Histogram of tran time";$i++;
$title[$i] = "Histogram of available credit (binned)";$avail_cred = $i;$i++;
$title[$i] = "Histogram of credit line (binned)";$cred_line = $i;$i++;
$title[$i] = "Histogram of transaction type";$tran_type = $i;$i++;
$title[$i] = "Histogram of authed";$i++;
$title[$i] = "Histogram of sic";$SIC = $i;$i++;
$title[$i] = "Histogram of merchant postal code";$i++;
$title[$i] = "Histogram of merchant country";$i++;
#$title[$i] = "Histogram of authorization id";$i++;
#$title[$i] = "Histogram of posting id";$i++;
$title[$i] = "Histogram of merchant id length";$MRCH_ID = $i;$i++;
#$title[$i] = "Histogram of terminal id";$i++;
$title[$i] = "Histogram of which card";$i++;
$title[$i] = "Histogram of cashback amount";$cashback_amount = $i;$i++;
$title[$i] = "Histogram of falcon score";$falscr = $i;$i++;
$title[$i] = "Histogram of auth tran type & SIC";$tran_typexSIC = $i;$i++;
$title[$i] = "Histogram of auth curr code & curr rate";$curr_codexcurr_rate = $i;$i++;
$num_analysis = $i;

# Specify the numeric fields
@numeric = ($amount , $curr_rate , $avail_cred , $cred_line , $MRCH_ID , $cashback_amount , $falscr);
foreach $i (@numeric) {
  $is_num[$i] = 1;
}

$total_lines=0;
while(<>) {
  chomp;
  @data = unpack($template,$_);

  #Define additional variables
  while(substr($data[$MRCH_ID],0,1) eq " "){substr($data[$MRCH_ID],0,1) = "";}
  while(substr($data[$MRCH_ID],-1,1) eq " "){chop $data[$MRCH_ID];}
  $data[$MRCH_ID] = length $data[$MRCH_ID];
  $data[$tran_typexSIC] = $data[$tran_type] . " & " . $data[$SIC]; 
  $data[$curr_codexcurr_rate] = $data[$curr_code] . " & " . $data[$curr_rate];

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
  if ($data[$amount] !~ /^\s*$/){
    if ($data[$amount]<500){
    $data[$amount]=10*int($data[$amount]/10);
    }
    elsif ($data[$amount]<5000){
      $data[$amount]=50*int($data[$amount]/50);
    }
    else {
      $data[$amount]=$data[$amount]=100*int($data[$amount]/100);
    }
  }
  if ($data[$avail_cred] !~ /^\s*$/){
    $data[$avail_cred] = 100 * int($data[$avail_cred]/100 );
  }
  if ($data[$cred_line] !~ /^\s*$/){
    $data[$cred_line] = 100 * int($data[$cred_line]/100 );
  }

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
      if ($a =~ /^\s*$/){
        $result=-1;
      }
      elsif ($b =~ /^\s*$/){
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
    print "Number of non-missing numeric entries: " . $total_num_lines[$i]*1 . "\n";
    if ($total_num_lines[$i]>0){
      print "Sum: ", $sum[$i], "\n";
      print "Mean: ", $sum[$i]/$total_num_lines[$i], "\n";
      if (($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i] - 1)>0 && $nummin[$i]!=$nummax[$i]){
        print "Std Dev.: ", sqrt(($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i]-1)), "\n";
      }
      else{
        print "Std Dev.: 0\n";
      }
      print "Min: ", $nummin[$i], "\n";
      print "Max: ", $nummax[$i], "\n";
    }
    print "\n\n";
  }
}
