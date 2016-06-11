#!/usr/local/bin/perl -w

$usage = "Usage: gzcat bigscr_file.gz |  $0 <segment names>\n" .
  "  Where <segment names> are the model names of the segments. Calibration tables (with or without\n" .
  "     rule language structure) must have segment names in the file name(e.g.g1_genfcr20_apf10_calib for genfcr20).\n" .
  "  Reads a big score file from stdin, and outputs to stdout.\n" .
  "\n" .
  "  **** ASSUMES that the table is in increments of 1. ****\n" .
  "\n";

if (scalar @ARGV < 1 || $ARGV[0] eq "-h" || $ARGV[0] eq "--help") {
  print $usage;
  exit 1;
}



foreach $arg (@ARGV) {
  
  #  $file = <*$arg*>;
  ($file) = glob("*$arg*");
  
  if (!defined $file) {
    print STDERR "ERROR: there is no file containing $arg in the filename in the current directory\n";
  }
  else {
    
    open(CALIB, $file) || die "Cannot open file $file:$!\n";
    
    
    $nLines = 0;
    $nEntries = 0;
    
    while (defined($line = <CALIB>)) {
      chomp $line;
      $nLines++;
      if ($line =~ m/^(\d+),(\d+)$/) {
        $nEntries++;
        $from = $1;
        $to = $2;
        $newScore{$arg}[$from] = $to;
      }
    }
    
    close(CALIB);
    
  }
  
  print STDERR "Calibration table $file has $nLines lines with $nEntries entries\n";
  if ($nEntries != 1001) {
    print STDERR "Error!  Table must have 1001 entries [0,1000]\n", $usage;
    exit 1;
  }
  
}


$nLines = 0;
while (defined ($line = <STDIN>)) {
  
  $oldScore             = substr ($line, 83, 4);
  $segment              = substr ($line, 166, 8);
  substr ($line, 83, 4) = sprintf "%4d", $newScore{$segment}[$oldScore];
  print $line;
  $nLines++;
  

  if ($nLines % 1000 == 0) { print STDERR "\b" x length($nLines), $nLines;}

}



print STDERR "$nLines lines processed\n";

# vi:cindent:
