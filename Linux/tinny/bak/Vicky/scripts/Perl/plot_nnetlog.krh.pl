#!/usr/local/bin/perl

$i = 0; 

open(OUT, "| /home/yxc/bin/mxs_bin/xgraph -bg black -fg white") || die "Cannot open output pipe: $!  "; 

while (<>) { 
  $line = $_; 
  if (/^Epoch/) { 
    if ($line =~ /Epoch\s+(\d+).+MSE Train: (0.\d+).*/) { 
      $s = "$1  $2\n"; 
      push(@train, $s); 
    }   
    
    if ($line =~ /Epoch\s+(\d+).+MSE Test: (0.\d+).*/) {
      $s = "$1  $2\n"; 
      push(@test, $s); 
    } 
    $i++;  
  } 
} 

print OUT "\"Train:\"\n"; 

foreach $t (@train) {
  print OUT $t;
} 

print OUT "\n\n\"Test:\"\n"; 

foreach $t (@test) {
  print OUT $t;
} 
