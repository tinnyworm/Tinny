#! /usr/local/bin/perl

## generate BIN by month distribution for indicating the file
## yxc 01/30/04

$usage = "\nUsage: bin_x_mth.pl [ -a <auths_file> ] [ -c <cards_file> ] [ -f <frauds_file> ]\n"
         . "                       [ -n <nonmons_file> ] [ -p <paymnts_file> ] \n"
         . "                       [ -t <posts_file> ] \n"
         . "                       [ -h ] \n"
         . " where:\n"
         . "    -h    :   show this message\n"
         . "    LIMITATION: The files must already be sorted according to account IDs.\n\n";

require "getopts.pl";

&Getopts('a:c:f:n:p:s:t:h');

%binHash      =qw();

$auths_file   = $opt_a;
$cards_file   = $opt_c;
$frauds_file  = $opt_f;
$nonmons_file = $opt_n;
$paymnts_file = $opt_p;
$posts_file   = $opt_t;

$cardBIN = "/usr/local/bin/zcat $cards_file|";

#### catch the BIN list from the cards
open(BIN, "$cardBIN") || die "Couldn't read $cards_file \n";
while (<BIN>){
   $bin= substr($_,3,6);
   ($binHash{$bin}++) or ($binHash{$bin}=1);
   
}
close(BIN);

%monthHash_a  =qw();
%monthHash_c  =qw();
%monthHash_f  =qw();
%monthHash_n  =qw();
%monthHash_p  =qw();
%monthHash_t  =qw();

  if ($opt_a){
     open (AUTH, "/usr/local/bin/zcat $auths_file |") || die ("Couldn't open $auths_file\n");
      while (<AUTH>){
       $bin_auth=substr($_,0,6);
       $authmonth=substr($_,19,6);
       $binmonth_auth=$bin_auth . $authmonth;
       ($monthHash_a{$binmonth_auth} ++) or ($monthHash_a{$binmonth_auth}=1);
       
     } 
     close(AUTH);
   }

  if ($opt_c){
     open (CARD, "/usr/local/bin/zcat $cards_file |") || die ("Couldn't open $cards_file\n");
      while (<CARD>){
       $bin_card=substr($_,3,6);
        $cardmonth=substr($_,19,6);
         $binmonth_card=$bin_card . $cardmonth;   
	  ($monthHash_c{$binmonth_card} ++) or ($monthHash_c{$binmonth_card}=1);
      
     } 
     close(CARD);
   }

  if ($opt_t){
     open (POST, "/usr/local/bin/zcat $posts_file |") || die ("Couldn't open $posts_file\n");
      while (<POST>){
       $bin_post=substr($_,0,6);
       $postmonth=substr($_,48,6);
       $binmonth_post=$bin_post . $postmonth; 
            
	  ($monthHash_t{$binmonth_post} ++) or ($monthHash_t{$binmonth_post}=1);
       
     } 
     close(POST);
   }  
       
    if ($opt_f){
     open (FRAUD, "/usr/local/bin/zcat $frauds_file |") || die ("Couldn't open $frauds_file\n");
      while (<FRAUD>){
       $bin_fraud=substr($_,0,6);
       $fraudmonth=substr($_,35,6);
       $binmonth_fraud=$bin_fraud . $fraudmonth; 
       ($monthHash_f{$binmonth_fraud} ++) or ($monthHash_f{$binmonth_fraud}=1);
       }
     close(FRAUD);
     }

#### catch max

$max=0;
foreach $binmonthcards (keys %monthHash_c){
#print "binmonthcards=$binmonthcards , $monthHash_c{$binmonthcards}\n";
         if ($max < $monthHash_c{$binmonthcards}) 
	{$max = $monthHash_c{$binmonthcards};}
	}
#print "max=$max\n";

### print 
@bin_cards= sort {$binHash{$b} <=> $binHash{$a}} keys %binHash;
#### obtain the cards, auths, posts, frauds hashes
print "  Bin by month analysis \n";
printf ("%8s   %8s  %60s  %10s  %10s %10s %10s \n","BIN", "Month", " ", "card_freq");


foreach $bin_cards (@bin_cards){

### construct a 2-D array
$i=0; 
### store the cards into 2-D array
@cbinmonth= sort {$a <=> $b} keys %monthHash_c;
foreach $key_c (@cbinmonth){
    $cbin=substr($key_c,0,6);
    $cmonth=substr($key_c,6,6);
    if ($cbin eq $bin_cards)
    {
    @array = ($cmonth, $monthHash_c{$key_c});
    $order_array[$i]=[@array];
    $i++;
    }
#  print "$monthHash_c{$key_c}\n";
}
### store the auths into 2-D array
#print "$i\n";
#foreach $key_a (keys %monthHash_a){
# $abin=substr($key_a,0,6);
#    $amonth=substr($key_a,6,6);
#    if ($abin eq $bin_cards)
#    {
#     $j=0; 
#      if (exists $monthHash_c{$key_a}){  # if the month appears in cards file, add the auth freq; otherwise, add a new line
#       while ($j<$i) {
#     if ($amonth==$order_array[$j][0]){
#        $order_array[$j][2]= $monthHash_a{$key_a};}
#       $j++;}
#     } else {
#       @array=($amonth, " ", $monthHash_a{$key_a}, " ", " ");
#       $order_array[$i]=[@array];
#       $i=$i+1;
#     }
#     }
# }
#print "$i\n";
### store the posts into 2-D array
#foreach $key_t (keys %monthHash_t){
#$tbin=substr($key_t,0,6);
#    $tmonth=substr($key_t,6,6);
#    if ($tbin eq $bin_cards)
#    {
#$j=0; 
#   if (exists $monthHash_c{$key_t} || exists $monthHash_a{$key_t} ){
#   while ($j<$i) {
#     if ($tmonth==$order_array[$j][0]){
#        $order_array[$j][3]= $monthHash_t{$key_t};}
#       $j++;}
#     } else {
#       @array=($tmonth, " ",  " ",$monthHash_t{$key_t, " ");
#       $order_array[$i]=[@array];
#       $i=$i+1;
#     }
#     }
#print "$monthHash_t{$key_t}\n";
 #}

### store the frauds into 2-D array
#foreach $key_f (keys %monthHash_f){
#$fbin=substr($key_f,0,6);
#    $fmonth=substr($key_f,6,6);
#    if ($fbin eq $bin_cards)
#    {
#$j=0; 
#   if (exists $monthHash_c{$key_f} || exists $monthHash_a{$key_f} ||exists $monthHash_t{$key_f}){
#   while ($j<$i) {
#     if ($fmonth==$order_array[$j][0]){
#        $order_array[$j][4]= $monthHash_f{$key_f};}
#       $j++;}
#     } else {
#       @array=($fmonth, " ", " ", " ", $monthHash_f{$key_f});
#       $order_array[$i]=[@array];
#       $i=$i+1;
#     }
#     }
# }

### print order the freq and plot
@order_array= sort {$a->[0] <=> $b->[0]} @order_array;
#order the array according to month
$firstmonth=$order_array[0][0];
#$lastmonth=$order_array[$i-1][0];
$k=$firstmonth;
#print "@order_array\n";
for ($j=0;$j<$i; $j++){
       if ($k % 100 >12) {$k+=88; }
      if ($order_array[$j][0]==$k) #find the freq of the month
                       {
                   $chart_line="*"x(int(60*($order_array[$j][1]/$max)));
           if ($j==0) {
                printf("%8s   %8s | %-60s  %10d  %10d %10d %10d \n", $bin_cards,
                $order_array[$j][0], $chart_line, $order_array[$j][1] );
           } else {
                printf("%8s   %8s | %-60s  %10d  %10d %10d %10d \n", "",
                $order_array[$j][0], $chart_line, $order_array[$j][1] );
                #,$order_array[$j][2],$order_array[$j][3],$order_array[$j][4] );
           }
           $k++;
     } else # not find the related month then print zero
     {
           if ($j==0) {
                printf("%8s   %8s | %-60s  %10d  %10d %10d %10d \n", $bin_cards,
                $k, "", 0 );
           } else {
                printf("%8s   %8s | %-60s  %10d  %10d %10d %10d \n", "",
                $k, "", 0 );
           }
	  $j--;

    }
 }
print "\n";
}

     
