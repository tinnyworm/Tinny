#! /usr/local/bin/perl -w

#$has_approved =0;

open (OUT, "| gzip -c > new_bigScore.gz" ) || die "Couldn't write to new_bigScore.gz";

while (defined($line= <STDIN>)){
#   chomp;
   $acct    = substr($line, 0 ,19);
   $fradTag = substr($line, 59, 1);
   $result  = substr($line, 88, 1);
   
   $total ++;
   
   if ($fradTag ne "1") {
     print OUT "$line";
#     $count ++;
   }
   else {
#      if ($result eq "A"){
         push (@{$accthash{$acct}}, $line);
#         $a_count ++;
#      } 
#      else 
#      {
#         print OUT "$line";
#         $o_count ++;
#      }
   }

}

foreach $acct (keys %accthash) {

    $num=@{$accthash{$acct}};
          if ($num eq "1") {
#           $onlyOneAppro ++;
          } 
           
           $j=0;
              for ($i=0; $i< $num; $i++){
                 $result_F = substr($accthash{$acct}[$i],88,1);
                 if ($result_F eq "A"){
                     $j=$i;
                 }
              }

              for ($i=0; $i <=$j; $i++){
                 print OUT "$accthash{$acct}[$i]";   
              }

              for ($k=$j+1; $k<$num; $k++){
                 print "$accthash{$acct}[$k]";
              }
           $remove = $num-$j-1;
           $c_rm += $remove;

}

print "Total\t\t\t\t: $total\n";
#print "Count for NF\t\t\t: $count\n";
#print "Count for F+A\t\t\t: $a_count\n";
#print "Count for F+NA\t\t\t: $o_count\n";
#print "Count for Removed\t\t: $remove\n";
#print "Count for OnlyOneApprove Acct\t: $onlyOneAppro\n";
#print "Sum of above except last\t:", $count+$a_count+$o_count, "\n";
#print "Final Record  \t\t\t:", $count+$a_count+$o_count-$remove, "\n";

print "Removed trans: $c_rm\n";
