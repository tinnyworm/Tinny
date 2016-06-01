#! /usr/local/bin/perl -w

## Reads nnw file, inserts dummy min and max values where needed

while(<>){
  if(/^set Field\((\d+)\)/){
    $fieldnum = $1;
  }
  if(/^\s+default_value\s+(-*\d+\.*\d*)/){
    $meanv = $1;
    $minv = $meanv - .01;
    $maxv = $meanv + .01;
    print;
    $_ = <>;
    if(/^\s+group_size/){ ## No min/max info
      print STDERR "Fixing field $fieldnum\n";
      print "    max                 $maxv\n";
      print "    min                 $minv\n";
      print "    mean                $meanv\n";
      print "    stdev               .01\n";
    }
  }
  print;
}
