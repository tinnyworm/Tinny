#! /usr/local/bin/perl -w
#
## check the percentage of zero or negative amount in the auths and posts
## yxc 02/13/04
$usage = "\nUsage: $0 [ -t <posts_file> ] [ -a <auths_file> ]  \n" ;


require "getopts.pl";
&Getopts('t:a:');
$auths_file   = $opt_a;
$posts_file   = $opt_t;

die $usage unless $opt_a || $opt_t;

$count1=0;
$nega= 0; 
if ($opt_t){
open (POST, "/usr/local/bin/zcat $posts_file | ") || die ("Couldn't open $posts_file\n");
open (OUTP, "| /usr/local/bin/gzip -c > nega_amt_post.gz ") || die ("Couldn't write nega_amt_post.gz\n");

while (<POST>){
   chomp;
   $sign=substr($_,19,13);
   $count1 ++;
   if ($sign <0 ) {
#  if ($sign =~ /^[-]*$/){
  $nega ++; 
   print OUTP "$_\n"; 
  }
}
close(POST);
close(OUTP); 
print ("percent of the negative sign: \n");
$percent = sprintf("%.5f", 100*$nega/$count1);
$month2=`ls $posts_file | cut -c1-6 `;
chomp($month2);
printf (" %6s   %6.5f%%\n",  $month2 , "$percent");
print "\n";
}

######## auths checking section ###########

$count2=0;
$zero=0;
$nega=0;
if ($opt_a) {
open (AUTH, "/usr/local/bin/zcat $auths_file | ") || die ("Couldn't open $auths_file\n");
open (OUT, "| /usr/local/bin/gzip -c > nega_amt_auth.gz ") || die ("Couldn't write nega_amt_auth.gz\n");
while (<AUTH>){
   chomp;
   $amt=substr($_,33,13);
   $count2 ++;
 #  $type=substr($_,63,1);
 #  $month=substr($_,19,6);
   if ($amt =~ /^[ 0.+]*$/) {
    $zero ++;
#   print "$_\n";
   } elsif ($amt =~ /^-/ ) {
     $nega ++;
     print OUT "$_\n"; 
   }
}
close (AUTH);
close (OUT); 
#print "$count2\n";
#print "$zero\n";
#$month=`ls $auths_file | cut -c1-6 `;
#chomp ($month);
$percent_a = sprintf("%.2f", 100*$zero/$count2);
$percent_n = sprintf("%.2f", 100*$nega/$count2);
print "percent of the zero amount or negative amount: \n";
printf ("%6s   %6s\n",  "zero_amt", "negative_amt");
printf ("%6.5f%%   %6.5f%%\n",  "$percent_a", "$percent_n");
#printf (" %6s   %6.2f%%   %6.2f%%\n",  , "$month" , "$percent_a", "$percent_n");
print "\n";
}
