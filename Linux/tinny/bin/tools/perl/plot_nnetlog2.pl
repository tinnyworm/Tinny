#!/usr/local/bin/perl

$i = 0; 

open(OUT, "| xgraph -bg black -fg white") || die "Cannot open output pipe: $!  "; 

while (<>) { 
  $line = $_; 
  if (/^Epoch/) { 
    if ($line =~ /.+MSE Train: (0.\d+).*/) { 
#      $s = "$i  $1\n"; 
      push(@train, $1); 
    }   
    
    if ($line =~ /.+MSE Test: (0.\d+).*/) {
#      $s = "$i  $1\n"; 
      push(@test, $1); 
    } 
    $i++;  
  } 
} 

$size = scalar(@train);

print OUT "\"Train:\"\n"; 

for ($k = 0; $k < $size; $k++) {
  if ($k < 2 || $k > $size - 3) {
#    print  OUT $k, " ",  $train[$k], "\n";
  }
  else {
    print OUT $k,  " ",  ($train[$k - 2] + $train[$k - 1] + $train[$k] + $train[$k + 1] + $train[$k + 2])/5, "\n";
  }
}

$size = scalar(@test);

print OUT "\n\n\"Test:\""; 

for ($k = 0; $k < $size; $k++) {
 if ($k < 2 || $k > $size - 3) {
#    print  OUT $k, " ",  $test[$k], "\n";
  }
  else {
    print OUT $k,  " ",  ($test[$k - 2] + $test[$k - 1] + $test[$k] + $test[$k + 1] + $test[$k + 2])/5, "\n";
  }
}
