#!/usr/local/bin/perl

$i = 0; 

open(OUT, "| /home/mxs/bin/xgraph -bg black -fg white") || die "Cannot open output pipe: $!  "; 

while (<>) { 
  $line = $_; 
  if (/^Epoch/) { 
    if ($line =~ /.+MSE Train: (0.\d+).*/) { 
      $s = "$i  $1\n"; 
      push(@train, $s); 
    }   
    
    if ($line =~ /.+MSE Test: (0.\d+).*/) {
      $s = "$i  $1\n"; 
      push(@test, $s); 
    } 
    $i++;  
  } 
} 

print OUT "\"Train:\"\n"; 

foreach $t (@train) {
  print OUT $t;
} 

print OUT "\n\n\"Test:\""; 

foreach $t (@test) {
  print OUT $t;
} 
