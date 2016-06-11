#!/usr/local/bin/perl

#use strict;

if (@ARGV == 0) {
  print "plots training and test data errors by using xgraph\n";
  print "\n";
  print "plotnnw.pl <nnw file> [alpha|mean|mabs|msq|rms|rsq]\n";
  print "\n";
  print "rms is default\n";
  print "\n";
  print "e.g. plotnnw genmod_nn_10.nnw msq\n\n";
  exit;
}


my $in_file = $ARGV[0];
my $err = $ARGV[1];


my %error = (alpha => 2,      
	     mean => 3,     
	     mabs => 4,     
	     msq => 5,     
	     rms => 6, 
	     rsq => 7
	    );


if ($err eq "") {
  $err = "rms";
}

#print "|", $error{$err}, "|\n";

my $i;


open(IN, $in_file) || die "Cannot open input file  $in_file: $!" ;
open(OUT, "| xgraph -bg black -fg white") || die "Cannot open output pipe: $!  "; 
#open(OUT, ">-");

@arr = <IN>;


print OUT "\"Training $err:\"\n";

#for($i = 0; $i < @arr; $i++) {


$i = 0;


while ($arr[$i] !~ /stats_train/) {
  $i++;
}

$i++;
$i++;

while ($arr[$i] !~ /\}/) {
  @sarr = split(/\s+/, $arr[$i]);

  print  OUT $sarr[1], " ",  $sarr[$error{$err}], "\n";
  $i++;
}

while ($arr[$i] !~ /stats_tests/) {
  $i++;
}


$i++;
$i++;

print OUT "\n";
print OUT "\"Test $err:\"\n";


while ($arr[$i] !~ /\}/) {
  @sarr = split(/\s+/, $arr[$i]);
  
  print  OUT $sarr[1], " ",  $sarr[$error{$err}], "\n";
  $i++;
}

