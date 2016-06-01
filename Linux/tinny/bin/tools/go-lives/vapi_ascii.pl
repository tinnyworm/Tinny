#!/usr/local/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon api file
#     
#   20040902 gg made non object oriented to improve speed
#   20040915 gg fixed formatting on print so columns line up in a table
#

$usage = "Usage (ascii): zcat api.dat.gz | vapi_ascii.pl >& vapi_ascii.out&\nUsage (cics): zcat api.dat.gz | X | vapi_ascii.pl >& vapi_ascii.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a8 a4 \@36 a19 a1 a9 a3 a8 a8 a1 a8 a8 a10 a10 a1 a8 a3 a10 a1
a1 a8 a6 a13 a3 a13 a1 a1 a4 a9 a3 a1 a1 a1 a8 a1 \@337 a8 \@407 a16";

$i = 0;
$title[$i] = "Histogram of Applid Name";$i++;
$title[$i] = "Histogram of Score (50 Binned)";$score=$i;$i++;
#$title[$i] = "Histogram of reason 1";$res1= $i; $i++;
#$title[$i] = "Histogram of reason 2";$res2= $i; $i++;
#$title[$i] = "Histogram of reason 3";$res3= $i; $i++;
#$title[$i] = "Histogram of return code";$i++;
$title[$i] = "Histogram of 6 digit account bins";$acctid=$i;$i++;
$title[$i] = "Histogram of Auth-Post flag";$i++;
$title[$i] = "Histogram of card postal code (first 3 digits)";$cpost=$i;$i++;
$title[$i] = "Histogram of card country code";$i++;
$title[$i] = "Histogram of account open date";$i++;
$title[$i] = "Histogram of new plastic date";$i++;
$title[$i] = "Histogram of plastic issue type";$i++;
$title[$i] = "Histogram of account expire date";$i++;
$title[$i] = "Histogram of card expire date";$i++;
$title[$i] = "Histogram of available credit (binned)";$avail_cred=$i;$i++;
$title[$i] = "Histogram of credit line (binned)";$cred_line=$i;$i++;
$title[$i] = "Histogram of gender";$i++;
$title[$i] = "Histogram of birthdate";$i++;
$title[$i] = "Histogram of number of cards";$i++;
$title[$i] = "Histogram of income";$income=$i;$i++;
$title[$i] = "Histogram of card type";$i++;
$title[$i] = "Histogram of card use";$i++;
$title[$i] = "Histogram of transaction date";$i++;
$title[$i] = "Histogram of transaction time (hr)";$trantime=$i;$i++;
$title[$i] = "Histogram of amount (binned)";$amount=$i;$i++;
$title[$i] = "Histogram of currency code";$curr_code = $i;$i++;
$title[$i] = "Histogram of currency conversion";$curr_rate=$i;$i++;
$title[$i] = "Histogram of auth result";$i++;
$title[$i] = "Histogram of transaction type";$tran_type = $i;$i++;
$title[$i] = "Histogram of sic code";$SIC = $i;$i++;
$title[$i] = "Histogram of merchant postal code (first 3 digits)";$mpost=$i;$i++;
$title[$i] = "Histogram of merchant country code";$i++;
$title[$i] = "Histogram of pin verify";$PIN = $i;$i++;
$title[$i] = "Histogram of cvv verify";$CVV = $i;$i++;
$title[$i] = "Histogram of keyed swiped";$POS = $i;$i++;
$title[$i] = "Histogram of post date";$i++;
$title[$i] = "Histogram of post authed code";$i++;
$title[$i] = "Histogram of model segments used";$i++;
$title[$i] = "Histogram of merchant id length";$MRCH_ID=$i;$i++;
#$title[$i] = "Histogram of zero Score"; $zeroScr=$i; $i++; 
$title[$i] = "Histogram of auth SIC 6011 & PIN verify";$SICxPIN = $i;$i++;
$title[$i] = "Histogram of auth POS entry & CVV";$POSxCVV = $i;$i++;
$title[$i] = "Histogram of auth POS entry & PIN verify";$POSxPIN = $i;$i++;
$title[$i] = "Histogram of auth tran type & SIC";$tran_typexSIC = $i;$i++;
$title[$i] = "Histogram of auth curr code & curr rate";$curr_codexcurr_rate = $i;$i++;
$num_analysis = $i;

if ($score =~ /^[ 0]*$/){
		$data[$score]=$data[$score]; 
}

# Specify the numeric fields
@numeric = ($score, $res1, $res2, $res3, $amount, $curr_rate, $income, $avail_cred, $cred_line, $falscr);
foreach $i (@numeric) {
  $is_num[$i] = 1;
}

$total_lines=0;
while(<>) {
  chomp;
  @data = unpack($template,$_);

  #Define additional variables
    #set acct num to acct bin, trantime to hr, and set postal codes to first 3
    $data[$acctid]=substr($data[$acctid],0,6);
    $data[$cpost]=substr($data[$cpost],0,3);
    $data[$mpost]=substr($data[$mpost],0,3);
    $data[$trantime]=substr($data[$trantime],0,2);
    $data[$SICxPIN] = ($data[$SIC] eq "6011" ? "6011" : "Other") . " & " . $data[$PIN];
    $data[$POSxCVV] = $data[$POS] . " & " . $data[$CVV];
    $data[$POSxPIN] = $data[$POS] . " & " . $data[$PIN];
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
#  $data[$score] =~ s/\s+0/ /g; 
  if ($data[$score] !~ /^[ 0]*$/){
    $data[$score] = 50 * int($data[$score]/50);
} else {
	$data[$zeroScr]=$data[$score]; 
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
        print "Std Dev.: ", sqrt(($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i] - 1)), "\n";
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
