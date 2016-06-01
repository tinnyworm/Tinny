#! /usr/local/bin/perl

@dateall=("20020718","20020723", "20020724", "20020725", "20021225", "20030625",
"20030716","20030723","20030813", "20030827", "20030910", "20030917",
"20030924");
#$date = <STDIN> ;
#$date = "20030625";

#%amountHash=qw();
%accountHash=qw();
#$scalar=@date;

open (out, "> authdate_check/dateall_amt.list") || die ("cannot write to dateall_amt.list");
#$count=0;

foreach $date (@dateall){
%amountHash=qw();
$count=0;
open (auth, "/usr/local/bin/zcat auths.dat.gz  |") || die ("cannot open
auths.dat.gz") ;
while ($_= <auth>) {
  chomp;
  $authdate=substr($_,19,8);
   if ($authdate eq $date){
    $amount=substr($_,33,13);
    ($amountHash{$amount}++) or ($amountHash{$amount}=1);
     $count ++;
    #print "authdat\n";
      }
 }

close(auth);
@m_sort = sort {$amountHash{$b} cmp $amountHash{$a}} keys %amountHash;
print  out "$date\n";
printf out "%15s | %10s | %6s \n", "amount", "frequency", "percent" ;
foreach $amount (@m_sort) {
   $freq = $amountHash{$amount};
   $freq_per = sprintf("%.2f", 100*$freq/$count);
   if ($freq_per >= "20%"){
   printf out "%15s | %10s | %6s  \n", $amount, $freq , $freq_per  ;
   print out "\n";
         }
  
     }
 }

close(out);

#open (amt, "/bin/cat date_amt.list |") || die ("cannot open date_amt.list");
#while ($_= <auth>) {
#  chomp;

#($accountHash{$account}++) or ($accountHash{$account}=1);


