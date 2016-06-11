#! /usr/local/bin/perl


## put file type based on DSN
##
@list = ("RIB", "CARD", "PAY", "NONM");
@list1 = ("00223", "00200", "00108", "00053");
@list2 = ("AUTH", "CARD", "PAYS", "NONM");

open ($element, "<filelist")
or die ("cannot open filelist.");

while ($line = <$element>){
  chomp($line);  
    $month = substr($line, 0, 4); ## catch month
    $linep = $line;
    $linep =~ tr/A-Z/L/;
    $linep =~ tr/a-z/L/;
    $linep =~ tr/0-9/#/;
    $invloc = index($linep, '######L');
    $inv = substr($line,  $invloc, 7);


for ($i=0; $i < 4; $i++) {
$tell = index($line, $list[$i]);
if ($tell >0) {
  $newfile = join(".", $month, "DISCOVER",$list2[$i],$list1[$i],$inv, "gz");
   $newfinal= join("/",$month, $newfile);
rename ( $line, $newfinal) or warn "Couldn't rename $line to $newfinal:$!\n" ;
 print ($line,"\t  ", "$newfinal\n");
  } 
 }
}

close ($element);
