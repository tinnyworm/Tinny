#! /usr/local/bin/perl -w

$total = $ARGV[0] ;

while (<STDIN>) {
  chomp;

   $len=length($_);
   $line = $_; 
   $inv=$total-$len;
   $blank=" "x$inv;
 
   $new = $line . $blank;
  
  print "$new\n";
}
