#! /usr/local/bin/perl


use lib "/home/yxc/scripts/Perl/PM";
use Date::Simple (':all');
open (outfile, "| gzip > frauds_withdiff.gz ");
#$frauds=$1;
#open (FRAUD, "/usr/local/bin/zcat $frauds | ") || die "Couldn't open $frauds\n";;
#while (<FRAUD>){
 while (<STDIN>){
  chomp;
### format the date variable in the d8 format ##
if (! (substr($_,20,8) =~ /^[ 0]*$/) && ! (substr($_,35,8) =~ /^[ 0]*$/)){
$dt_ffd = d8(substr($_,20,8));
$dt_fdd = d8(substr($_,35,8));
#$acctType= substr($_,0,20);
$count ++;
$diff_ffd_fdd = $dt_fdd - $dt_ffd;
print outfile "$_ $diff_ffd_fdd\n";
   if ($diff_ffd_fdd > 30 ){
       $mt30 ++;
    } elsif ($diff_ffd_fdd <0) {
       $st0 ++;
    } else {
       $mt0 ++;
#      (%histBin{$bin} ++; )
    } 
 }
}

#close(FRAUD);
$mt30_ratio=sprintf("%.2f", 100*$mt30/$count);
$mt0_ratio=sprintf("%.2f", 100*$mt0/$count);
$st0_ratio=sprintf("%.2f", 100*$st0/$count);

print ("Summary of the difference b/w detect date and first date\n");
printf ("%30s \t %8d \t %6.2f%% \n", "Ratio of more than 30 days:", $mt30, $mt30_ratio) ;
printf ("%30s \t %8d \t %6.2f%% \n", "Ratio of more than  0 days:", $mt0 , $mt0_ratio) ;
printf ("%30s \t %8d \t %6.2f%% \n", "Ratio of negative days:", $st0, $st0_ratio) ;



