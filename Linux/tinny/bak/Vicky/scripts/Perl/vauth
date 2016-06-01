#!/usr/bin/perl

#
#   Purpose:  Provides histograms and statistics for variables in the 
#             Falcon authorizations file
#
#   20040823-24 gg initial coding, some code borrowed from krh's code;
#               non object oriented to improve speed
#   20040825    gg added numeric stats, some code borrowed from pcd's code
#   20040915    gg added (binned) to titles, and formatted print on freqs
#               so columns line up in a freq table
#   20040924    gg added negative num check on numeric sqrt std calc since
#               was getting errors due to rounding / precision
#   20050613    gg fixed minor bug with total_lines & put eval around std calc
#               and added some extra code on numerics
#   20071129    mws added 4.1 fields

$usage = "Usage: zcat auths.dat.gz | vauths.pl >& vauths.out&\n";
if ($#ARGV>=0) { die "$usage"; }

# Define template and titles
$template = "a1 X a6 " . "x"x13 . " a6 XXXXXX a8 a2 xxxx a13 a3 a13 a1 a1 a10 a10 a4 a3 XXX a5 xxxx a3 a1 a1 a1 a6 XXXXXX a8 a6 a5 a1 a16 xxxxxx a1 a13 a4 x a30 a13 a3 a1 a1 a1 a1 a1 a12 a3 a16 a1 a1 a2 a1 a1 a6 a10 a10 a1 a5 a5 a2 a2 a2 a2 a8 a3 a15 a1";

#make sure numeric fields have a variable identifying them
$i = 0;
$title[$i] = "Histogram of card association (first byte of account number)";$ASSOC = $i;$i++;
$title[$i] = "Histogram of 6 digit account bins";$i++;
$title[$i] = "Histogram of auth month";$i++;
$title[$i] = "Histogram of auth date";$i++;
$title[$i] = "Histogram of auth hour";$i++;
$title[$i] = "Histogram of auth amount (binned)";$amount = $i;$i++;
$title[$i] = "Histogram of auth curr code";$curr_code = $i;$i++;
$title[$i] = "Histogram of auth curr rate";$curr_rate = $i;$i++;
$title[$i] = "Histogram of auth decision";$i++;
$title[$i] = "Histogram of auth tran type";$tran_type = $i;$i++;
$title[$i] = "Histogram of auth CREDIT avail cred or DEBIT avail bal (binned)";$avail_cred = $i;$i++;
$title[$i] = "Histogram of auth CREDIT cred line or DEBIT total bale (binned)";$cred_line = $i;$i++;
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
$title[$i] = "Histogram of auth merchant name length";$MRCH_NAME = $i;$i++;
$title[$i] = "Histogram of auth merchant city length";$MRCH_CITY = $i;$i++;
$title[$i] = "Histogram of auth merchant state";$i++;
$title[$i] = "Histogram of auth CVV2 present";$i++;
$title[$i] = "Histogram of auth CVV2 response";$i++;
$title[$i] = "Histogram of auth AVS response";$i++;
$title[$i] = "Histogram of auth Tran category";$i++;
$title[$i] = "Histogram of auth fraud transaction";$i++;
$title[$i] = "Histogram of auth ACQ ID";$i++;
$title[$i] = "Histogram of auth ACQ country";$i++;
$title[$i] = "Histogram of auth terminal ID length";$terminal_id = $i;$i++;
$title[$i] = "Histogram of auth terminal type";$TERM_TYPE = $i;$i++;
$title[$i] = "Histogram of auth terminal entry";$TERM_ENTRY = $i;$i++;
$title[$i] = "Histogram of auth point-of-sale condition code";$POS_COND = $i;$i++;
$title[$i] = "Histogram of auth card media";$i++;
$title[$i] = "Histogram of auth ATM network";$i++;
$title[$i] = "Histogram of auth CREDIT kcheck number";$i++;
$title[$i] = "Histogram of auth tvr";$i++;
$title[$i] = "Histogram of auth cvr";$i++;
$title[$i] = "Histogram of auth ARQC valid";$i++;
$title[$i] = "Histogram of auth ATC card";$i++;
$title[$i] = "Histogram of auth ATC host";$i++;
$title[$i] = "Histogram of auth last online ATC";$i++;
$title[$i] = "Histogram of auth online limit incoming";$lim_incoming = $i;$i++;
$title[$i] = "Histogram of auth online limit to card";$lim_tocard = $i;$i++;
$title[$i] = "Histogram of auth recurring frequency";$i++;
$title[$i] = "Histogram of auth recurring expire date";$i++;
$title[$i] = "Histogram of auth item code";$i++;
$title[$i] = "Histogram of auth address code";$address_code = $i;$i++;
$title[$i] = "Histogram of auth payment type";$i++;
$title[$i] = "Histogram of auth SIC 6011 & PIN verify";$SICxPIN = $i;$i++;
$title[$i] = "Histogram of auth POS entry & CVV";$POSxCVV = $i;$i++;
$title[$i] = "Histogram of auth POS entry & PIN verify";$POSxPIN = $i;$i++;
$title[$i] = "Histogram of auth tran type & SIC";$tran_typexSIC = $i;$i++;
$title[$i] = "Histogram of auth curr code & curr rate";$curr_codexcurr_rate = $i;$i++;
$title[$i] = "Histogram of auth terminal type & association";$TERM_TYPExASSOC = $i;$i++;
$title[$i] = "Histogram of auth terminal entry & association";$TERM_ENTRYxASSOC = $i;$i++;
$title[$i] = "Histogram of auth POS code & association";$POS_CONDxASSOC = $i;$i++;
$num_analysis = $i;

# Specify the numeric fields
@numeric = ($amount , $curr_rate , $avail_cred , $cred_line , $cashback_amount , $MRCH_ID, $MRCH_NAME, $MRCH_CITY, $terminal_id, $lim_incoming, $lim_tocard, $address_code );

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
  $data[$tran_typexSIC] = $data[$tran_type] . " & " . $data[$SIC]; 
  $data[$curr_codexcurr_rate] = $data[$curr_code] . " & " . $data[$curr_rate];
  $data[$TERM_TYPExASSOC] = $data[$ASSOC] . " & " . $data[$TERM_TYPE];
  $data[$TERM_ENTRYxASSOC] = $data[$ASSOC] . " & " .$data[$TERM_ENTRY];
  $data[$POS_CONDxASSOC] = $data[$ASSOC] . " & " . $data[$POS_COND];

  #trim spaces from merch id and count length for histogram
  while(substr($data[$MRCH_ID],0,1) eq " "){substr($data[$MRCH_ID],0,1) = "";}
  while(substr($data[$MRCH_ID],-1,1) eq " "){chop $data[$MRCH_ID];}
  $data[$MRCH_ID] = length $data[$MRCH_ID];

  #repeat for merchant name & city, acq id, terminal id
  while(substr($data[$MRCH_NAME],0,1) eq " "){substr($data[$MRCH_NAME],0,1) = "";}
  while(substr($data[$MRCH_NAME],-1,1) eq " "){chop $data[$MRCH_NAME];}
  $data[$MRCH_NAME] = length $data[$MRCH_NAME];
  while(substr($data[$MRCH_CITY],0,1) eq " "){substr($data[$MRCH_CITY],0,1) = "";}
  while(substr($data[$MRCH_CITY],-1,1) eq " "){chop $data[$MRCH_CITY];}
  $data[$MRCH_CITY] = length $data[$MRCH_CITY];
  while(substr($data[$terminal_id],0,1) eq " "){substr($data[$terminal_id],0,1) = "";}
  while(substr($data[$terminal_id],-1,1) eq " "){chop $data[$terminal_id];}
  $data[$terminal_id] = length $data[$terminal_id];
  while(substr($data[$address_code],0,1) eq " "){substr($data[$address_code],0,1) = "";}
  while(substr($data[$address_code],-1,1) eq " "){chop $data[$address_code];}
  $data[$address_code] = length $data[$address_code];

  #calculate sum for basic stats of numeric fields
  foreach $i (@numeric) {
    if ($data[$i] !~ /^\s*$/){
      $total_num_lines[$i]++;
      $sum[$i] += $data[$i];
      $sumsq[$i] += $data[$i]**2;
      if ($total_num_lines[$i] == 1) {
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
  if ($data[$amount]!~ /^\s*$/){
    if ($data[$amount] < 500){$data[$amount] = 10 * int($data[$amount]/10);}
    elsif ($data[$amount] < 5000){$data[$amount] = 50 * int($data[$amount]/50);}
    else {$data[$amount] = $data[$amount] = 100 * int($data[$amount]/100);}
  }
  if ($data[$avail_cred] !~ /^\s*$/){$data[$avail_cred] = 100 * int($data[$avail_cred]/100 )};
  if ($data[$cred_line] !~ /^\s*$/){$data[$cred_line] = 100 * int($data[$cred_line]/100)};
  if ($data[$lim_incoming] !~ /^\s*$/){$data[$lim_incoming] = 100 * int($data[$lim_incoming]/100)};
  if ($data[$lim_tocard] !~ /^\s*$/){$data[$lim_tocard] = 100 * int($data[$lim_tocard]/100)};

  #count number of each unique value for histogram
  for ($i = 0; $i < $num_analysis; $i++) {
    $hists[$i]{$data[$i]}++;
    if ($hists[$i]{$data[$i]}>$max[$i]){
      $max[$i]=$hists[$i]{$data[$i]};
    }
    if ((length $data[$i])>$maxlength[$i]){
      $maxlength[$i]=(length $data[$i]);
    }
  }

  #number of records
  $total_lines++;
}

#output histograms.  the max number of '*' is $chart_length
$chart_length = 20;
for ($i = 0; $i < $num_analysis; $i++)  {
  print $title[$i],"\n";
  $cumfreq=0;

  #print histograms
  foreach $x (sort {$is_num[$i]? $a<=>$b:$a cmp $b} keys %{$hists[$i]}){
    $pct=100*$hists[$i]{$x}/$total_lines;
    $cumfreq+=$hists[$i]{$x};
    $cumpct=100*$cumfreq/$total_lines;
    $chart_line = "*"x(int($chart_length*($hists[$i]{$x}/$max[$i])));
    printf("%-${maxlength[$i]}s |%-${chart_length}s %9d %9d %6.2f%% %6.2f%%\n",$x,$chart_line,$hists[$i]{$x},$cumfreq,$pct,$cumpct);
  }
  print "\n\n";

  #print stats for numeric data
  if ($is_num[$i]){
    print "Number of non-missing numeric entries: $total_num_lines[$i]\n";
    print "Sum: ", $sum[$i], "\n";
    print "Mean: ", eval{$sum[$i]/$total_num_lines[$i]} || 0, "\n";
    print "Std Dev: ", eval{sqrt(($sumsq[$i] - $sum[$i]**2/$total_num_lines[$i])/($total_num_lines[$i] - 1))} || 0, "\n";
    print "Min: ", $nummin[$i], "\n";
    print "Max: ", $nummax[$i], "\n";
    print "\n\n";
  }
}
