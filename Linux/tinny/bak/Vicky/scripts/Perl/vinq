#!/usr/local/bin/perl 

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon cardholder file
#     
#   20040901 gg made non object oriented to improve speed
#   20040915 gg fixed formatting on print so columns line up in a table
#   20041014 yxc for inquiry file analysis only


$usage = "Usage: zcat payment.gz | $0 >& vpaymnts.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a6 xxxxxxxxxxxxx a1 a6 XXXXXX a8 a2 xxxx a16 a1 a1 a1 ";
$i = 0;
$title[$i] = "Hist of 6 digit account bins";$i++;
$title[$i] = "Hist of Type";$i++;
$title[$i] = "Hist of Month of transactions"; $i++;
$title[$i] = "Hist of Date of transactions"; $i++;
$title[$i] = "Hist of Time of transactions";$i++;
$title[$i] = "Hist of Ani-Number";$i++;
$title[$i] = "Hist of Connect-mode";$i++;
$title[$i] = "Hist of Response-mode";$i++;
$title[$i] = "Hist of Fraud Trx";$i++;
$num_analysis = $i;

# read stdin data
while(<>) {
  chomp;
  @data = unpack($template,$_);

# get hash array for each feed

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
     printf ("%26s %9s %9s %6s %6s\n","$title[$i]","Freq", "Cum_freq", "Percent", "Cum_per");
     $cumfreq=0;
     
# for sort purpose     
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

  
# print out the statistics for fields of interest  
foreach $x (sort {sortfunc} keys %{$hists[$i]}){
    $pct=100*$hists[$i]{$x}/$total_lines;
    $cumfreq+=$hists[$i]{$x};
    $cumpct=100*$cumfreq/$total_lines;
    $chart_line = "*"x(int($chart_length*($hists[$i]{$x}/$max[$i])));
    printf("%-${maxlength[$i]}s |%-${chart_length}s %9d %9d %6.2f%% %6.2f%%\n",$x,$chart_line,$hists[$i]{$x},$cumfreq,$pct,$cumpct);
}
    print "\n\n";
}
