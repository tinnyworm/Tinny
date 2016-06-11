#! /usr/local/bin/perl
# Scans template file + variable defs for 4.0 models to produce profile map
# JAS (2001)

$usage = "$0 template_file\n".
  "Reads from RL3.0 variable defs STDIN, writes to STDOUT in CSV format\n";

$template = shift @ARGV
  or die $usage;

# Read in profile map from template file
open(FP, "< $template")
  or die "Opening $template";

while(<FP>){
  last if(/^PROF/);
}
/^PROF \. (\d+)/;
$cnt = $1;
for($i=1; $i<=$cnt; $i++){
  $map_name = <FP>; chomp $map_name;
  $map{$map_name}{type} = <FP>; chomp $map{$map_name}{type};
  $map{$map_name}{letter} = <FP>;
  $map{$map_name}{offset} = <FP>*1;
  $map{$map_name}{size} = <FP>*1;
}
close(FP);

print STDERR "Read info for $cnt positions on the profile map\n";

# Scan variable files (from STDIN) for additional info
while(<>){
  if(/^VAR\s+(\S+)\s+\:\s+(\S+)/){
    $last_var = $1;
    $last_type = $2;
  }
  if(/^\s*MAP\s*\=\s*PROF\.(\w+);*/){
    if(exists($map{$1})){
      $map{$1}{name} = $last_var;
      $map{$1}{type} = $last_type;
    } else {
      print STDERR "PROF.$1 not found in template!\n";
    }
  }
}

# Print out results
print "Offset, Map, Name, Type\n";
foreach $key (sort {$map{$a}{offset} <=> $map{$b}{offset}} keys %map){
  print "$map{$key}{offset}, PROF.$key, $map{$key}{name}, $map{$key}{type}\n";
}
