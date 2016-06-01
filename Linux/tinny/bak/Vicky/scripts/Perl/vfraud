#!/usr/local/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#   Falcon frauds file
#     
#   20040428 krh initial coding
#   20040628 krh fixed frd2blck/rundays bug
#   20040628 gg reduced width of charts to fit 80 width windows
#   20040629 gg prints out first 10 records
#   20040721 gg added usage output
#   20040901 gg made non object oriented to improve speed
#   20040914 gg fixed formatting on freq print for const width
#               and improved invalid date code
#

$usage = "Usage: zcat frauds.dat.gz | vfrauds.pl >& vfrauds.out&\n";
if ($#ARGV>=0) { die "$usage"; }

sub day90{
  local($indate)=@_;
  @months = (31,28,31,30,31,30,31,31,30,31,30,31);
  $yr=1*substr($indate,0,4);
  $mm=1*substr($indate,4,2);
  $dd=1*substr($indate,6,2);
  $sum=0;
  $add=0;
  for ($ii=0;$ii<($mm-1);$ii++){
    $sum=$sum+1*$months[$ii];
  }
  for ($iii=1990;$iii<$yr;$iii++){
    if ((1*$iii%4)==0){
      $add++;
    }
  }
  if($mm>2 && (1*$yr%4)==0){
    $add++;
  }
  $outnum=365*($yr-1990)+$sum+$dd+$add-1;
  $outnum;
}

sub okdate{
  local($okdate)=@_;
  if ($okdate!~/\d{8}/ || substr($okdate,0,4)*1<1900 || substr($okdate,0,4)*1>2100 || substr($okdate,4,2)*1<1 || substr($okdate,4,2)*1>12 || substr($okdate,6,2)*1>31){
    $ok=0;
  }
  else{
    $ok=1;
  }
  $ok;
}

# Define template and titles
$template = "a6 " . "x"x13 . " a1 a6 XXXXXX a8 a1 xxxxxx a6 XXXXXX a8 xxxxxx";

$i = 0;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of fraud type";$fraud_type = $i;$i++;
$title[$i] = "Histogram of month of first fraud";$first_fraud_month = $i;$i++;
$title[$i] = "Histogram of date of first fraud";$first_fraud_date = $i;$i++;
$title[$i] = "Histogram of fraud find method";$i++;
$title[$i] = "Histogram of fraud detection month";$detect_month = $i;$i++;
$title[$i] = "Histogram of fraud detection date";$detect_date = $i;$i++;
$title[$i] = "Histogram of fraud run days";$run_days = $i;$i++;
$title[$i] = "Histogram of first fraud month by fraud type";$first_fraud_month_by_type = $i;$i++;
$title[$i] = "Histogram of detection month by fraud type";$detect_month_by_type = $i;$i++;
$num_analysis = $i;

# Specify the numeric fields
@numeric = ($run_days);
foreach $i (@numeric) {
  $is_num[$i] = 1;
}

$n=0;
$total_lines=0;
while(<>) {
  chomp;
  @data = unpack($template,$_);
  if ($n<10){
    $savedline[$n]=$_;
    $n++;
  }

  #Define additional variables
  if (!(&okdate($data[$detect_date]) && &okdate($data[$first_fraud_date]))){
    $data[$run_days]=" ";
  }
  else{
    $data[$run_days] = &day90($data[$detect_date])-&day90($data[$first_fraud_date]);
  }
  $data[$first_fraud_month_by_type] = $data[$fraud_type] . " - " . $data[$first_fraud_month];
  $data[$detect_month_by_type] = $data[$fraud_type] . " - " . $data[$detect_month];

  foreach $i (@numeric) {
    if ($data[$i] ne " "){
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

#print out the first ten observations
printf(" Obs      ACCOUNT       FRAUDTYP  FRDDATE   FFIND  FRDTIME  DETDATE   DETTIME\n");
for ($i=0;$i<$n;$i++){
  printf("  %2s  %s  %s      %s    %s    %s   %s  %s\n",$i+1,substr($savedline[$i],0,19),substr($savedline[$i],19,1),substr($savedline[$i],20,8),substr($savedline[$i],28,1),substr($savedline[$i],29,6),substr($savedline[$i],35,8),substr($savedline[$i],43,6));
}
printf("\n\n");

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
    printf("%-${maxlength[$i]}s |%-${chart_length}s %7d %7d %6.2f%% %6.2f%%\n",$x,$chart_line,$hists[$i]{$x},$cumfreq,$pct,$cumpct);
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
