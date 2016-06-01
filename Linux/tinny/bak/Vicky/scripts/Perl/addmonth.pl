#! /usr/local/bin/perl

$filetemp = `ls 02*/* > filelist`; 
open($filetemp, "<filelist") 
|| die ("cannot open filelist");

#print ("$element\n");

while ($line = <$filetemp>){
  chop $line;
  $month = substr($line,0,4);
  @array = split('[/]',$line);
  $filenm = $array[1];
  $newfile= join(".", $month, $filenm);
  $newfinal = join("/", $month, $newfile);
  $tmp= `mv $line $newfinal`;
  print ($line, "     ", "$newfinal\n");
}

close ($file);
