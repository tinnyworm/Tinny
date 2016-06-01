#! /usr/local/bin/perl -w

#open ($CODE1_zero,    " | stats.pl >CODE1_zero.log")    || die "couldn't get the CODE1_zero.log";
#open ($CODE1_nonzero, " | stats.pl >CODE1_nonzero.log") || die "couldn't get the CODE1_nonzero.log";
#open (CODE2_zero,    " | stats.pl >CODE2_zero.log")    || die "couldn't get the CODE2_zero.log";
#open (CODE2_nonzero, " | stats.pl >CODE2_nonzero.log") || die "couldn't get the CODE2_nonzero.log";
#open (CODE3_zero,    " | stats.pl >CODE3_zero.log")    || die "couldn't get the CODE3_zero.log";
#open (CODE3_nonzero, " | stats.pl >CODE3_nonzero.log") || die "couldn't get the CODE3_nonzero.log";

while (<STDIN>){
   chomp;
   $code1=substr($_,223,2);
   $code2=substr($_,225,2);
   $code3=substr($_,227,2);
   $score=substr($_,159,4);
   if (($score >= 300) && ($code1 =~ /^[0]*$/ |$code2  =~ /^[0]*$/ | $code3  =~ /^[0]*$/)) {
#      print "CODE1_zero\n";
      print ("$_\n");
   }
   #else {
#      print "CODE1_nonzero\n";
#      print $CODE1_nonzero ("$score\n");
#   }
#   if ($code2 =~ /^[0]*$/) {
#      print "CODE2_zero\n";
#      print CODE2_zero ("$score\n");
#   }else {
#      print "CODE2_nonzero\n";
#      print CODE2_nonzero ("$score\n");
#   }
#   if ($code3 =~ /^[0]*$/) {
#      print "CODE3_zero\n";
#      print CODE3_zero ("$score\n");
#   }else {
#      print "CODE3_nonzero\n";
#      print CODE3_nonzero ("$score\n");
#   }
}


