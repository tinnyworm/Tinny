#! /usr/local/bin/perl 
#######################################################
#
#   This program is for mergeing two different sources
#   fraud file.
#
#



$temp = "\@0a19 \@19a1 \@20a8 \@28a6 \@34a1 \@35a8 \@43a6 \@49a1 \@50a8 \@58a6 \@64a1 \@65a8 \@73a6";

open (MATCHALL, "| gzip > allMatch.gz") || die  "couldn't write Match\n";
open (MATCHFD,  "| gzip > onlyMatchTwoDates.gz") ||  die  "couldn't write MatchFD\n";

#$total=0;
#$straiEqual=0;

while (<STDIN>){
  chomp;
  ($acct, $ftype, $ffd, $fft, $fmthd, $fdd, $fdt, $rftype, $rffd, $rfft,$rfmthd, $rfdd, $rfdt) = unpack $temp, $_;
  $total ++;
  
  $jregular=join("", $ftype, $ffd, $fdd);
  $jreferen=join("", $rftype,$rffd,$rfdd);
#  print $jregular, "\n";  
#  if ($ftype eq $rftype and $ffd eq $rffd and $fdd eq $rfdd){
  if ($jregular eq $jreferen){
  $straiEqual ++;
  } 
  
  else { 
  
     if (($jregular =~ /^[ ]*$/) and ($jreferen !~ /^[ ]*$/)) {
     $nonregular ++;
     } 
     elsif ($jreferen =~ /^[ ]*$/)  {
          $nonreference ++;
          #$inregular ++;
     } else {
     
         if  ($ftype ne $rftype) { 
          $noteqType ++;
         } 
      
         if (($ffd eq $rffd) and ($fdd eq $rfdd)){
           $twoDayEqual ++; 
         } 
         elsif ($ffd eq $rffd) {
           $notfdd ++;
         } 
         elsif ($fdd eq $rfdd) {
          $notffd ++;
         }
         else {
          $notall ++; 
         }
     }
  
#     if ($jreferen =~ /^[ ]*$/) {
#      $nonreference ++;
#     } else{ 
#        $inreference ++;
#     } 
}  
#     
#  if ($ftype ne $rftype) { 
#     $noteqType ++;
#  }
#
#  if ($ffd ne $rffd) {
#     $notffd ++;
#  }
#  
#  if ($fdd ne $rfdd) {
#     $notfdd ++;
#  }
}
$ratio_se = sprintf("%.2f", 100*$straiEqual/$total);
$ratio_nreg = sprintf("%.2f", 100*$inregular/$total);
$ratio_nref = sprintf("%.2f", 100*$inreference/$total);
$ratio_tdsE = sprintf("%.2f", 100*$twoDayEqual/$total);
$intersect = $total-$nonregular-$nonreference ;
$ratio_nt = sprintf("%.2f", 100*$noteqType/$intersect);
$ratio_nf = sprintf("%.2f", 100*$notffd/$intersect);
$ratio_nd = sprintf("%.2f", 100*$notfdd/$intersect);
$ratio_na = sprintf("%.2f", 100*$notall/$intersect);

print "Total records   : ", $total, "\n"; 
print "Intersection    : ", $intersect, "\n"; 
print "Not in regular  : ", $nonregular, "\n";
print "Not in reference: ", $nonreference, "\n";
print "\n"; 
print "The results for comparing the two frauds file", "\t", "Count", "\t", "Percent", "\n" ;
print "100% match each other                        :", "\t", $straiEqual, "\t", $ratio_se,  "\n";

#print "Only in regular not in reference             :", "\t", $inregular, "\t", $ratio_nreg,  "\n";
#print "Only in reference not in regular             :", "\t", $inreference, "\t", $ratio_nref,  "\n";
print "Not match for FRAUD_TYPE                     :", "\t", $noteqType, "\t", $ratio_nt,  "\n";
print "Match Two dates each other                   :", "\t", $twoDayEqual, "\t", $ratio_tdsE,  "\n";
print "Match FIRST_FRAUD but not BLOCK_DATE         :", "\t", $notffd, "\t", $ratio_nf,  "\n";
print "Match BLOCK_FRAUD but not FIRST_DATE         :", "\t", $notfdd, "\t", $ratio_nd,  "\n";
print "Not match both BLOCK_FRAUD and FIRST_DATE    :", "\t", $notall, "\t", $ratio_nd,  "\n";

