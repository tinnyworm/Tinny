#!/usr/local/bin/perl

$USAGE=<< "xxxEOFxxx";
#
#	$0 *HW*
#
#
xxxEOFxxx


open(OUT, "| /home/mxs/bin/xgraph -bg black -fg white ") || die "Cannot open output pipe: $!  "; 
#open(OUT_temp, "| more");

$i = 1;
while ($input = shift){
  print STDOUT "\t$input\n";
  open(IN, "< $input") || die "Could not open the input file $input\n"; 
  push(@{$table[$i]},"\n\n\"$input:\n");
  while (defined($_=<IN>)) { 
    if (/DATA =/) { 
       while(defined($_=<IN>)){
         if(/A/){goto ENDLOOP;}
         @data = split/,/;
	 $risk = $data[2]/$data[1];
         $s = "$data[0]  $risk\n"; 
         push(@{$table[$i]}, $s); 
       } 
       ENDLOOP: 
       $i++;
    } 
  } 
}
for($k = 1;$k < $i;$k++){
  foreach $t (@{$table[$k]}) {
    print OUT $t;
    print STDERR $t; 
  }  
}

close(IN);
close(OUT); 

