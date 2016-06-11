#!/usr/local/bin/perl


#this script reads the output from the last table in mixin spread sheet and 
# converts count select files to rgen select files.
# desired parameters:
$set_train_to_start = 1;
$numfraudays = 100;
$oldfrauds = 0;
$stopatblck = 1;
$stopatpckup = 1;

$setpctone = 1;


while (<>) {

  if (/res./) {
    @arr = split(/\s+/, $_);
  }


  #find the extension

  ($ext) =  $arr[0] =~ /.+\..+\.(.+)/;

#  $pctfraud = $arr[-2];
#  $pctnfraud = $arr[-1];

  $pctfraud = $arr[7];
  $pctnfraud = $arr[8];
  if ($setpctone == 1) {$pctfraud = 1; $pctnfraud = 1;}
  open(RGEN,  "> select.rgen.$ext") || die "cannot open file select.rgen.$ext: $!\n";
  open(COUNT, "select.count.$ext") || die "cannot open file select.count.$ext: $!\n";
  
  $startdate = 0;  
  while (<COUNT>) {

    chomp;
    if (/PCTFRAUD/) {
      print RGEN " PCTFRAUD=$pctfraud\n";
    }
    elsif (/PCTNOFRAUD/) {
      print RGEN " PCTNOFRAUD=$pctnfraud\n";
    }
    elsif (/STARTDATE/) {
      ($tmp,$startdate) = split /=/;
      print RGEN $_, "\n";
    }
    elsif (/TRAINDATE/) {
      if ($set_train_to_start == 0) {print RGEN $_, "\n";}
      else {print RGEN " TRAINDATE=$startdate\n";}
    }
    elsif (/NUMFRAUDAYS/) {
      print RGEN " NUMFRAUDAYS=$numfraudays\n";
    }  
    elsif (/OLDFRAUDS/) {
      print RGEN " OLDFRAUDS=$oldfrauds\n";
    }
    elsif (/STOPATBLCK/) {
      print RGEN " STOPATBLCK=$stopatblck\n";
    }
    elsif (/STOPATPCKUP/) {
      print RGEN " STOPATPCKUP=$stopatpckup\n";
    }
    else{
      print RGEN $_, "\n";
    }
    
  }
  close RGEN;
  close COUNT;

}

