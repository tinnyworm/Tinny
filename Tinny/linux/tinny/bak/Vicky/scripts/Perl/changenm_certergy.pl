#! /usr/local/bin/perl -w

## change filename based on DSN
## yxc 02/09/04

## put file type based on DSN
##
@list = ("AUTH", "CARD", "NMON", "INQ", "PAY");
@list1 = ("00164", "00186",  "00047", "00034","000108");
@list2 = ("00324", "00291",  "00063");
#@list3 = ("AUTH", "CARD",  "NONM", "INQ", "PAY");
@monthlist = ("Feb", "Mar");
@newmthlst = ("200401", "200402");
@type = ("DEBIT", "CREDIT");

$temp = `ls -l *equ*gz |grep -v PI |  grep -v FRAUD > filelist` ;
open ($element, "<filelist")
or die ("cannot open filelist.");

while ($line = <$element>){
  chop $line;
    $linep = $line;
    $linep =~ tr/[A-Z][a-z]/L/;
#    $linep =~ tr/a-z/L/;
    $linep =~ tr/0-9/#/;
    $invloc = index($linep, '######L');
    $inv = substr($line,  $invloc, 7);

## catch month   
   @arraytmp=split(/ +/, $line);
   $month=$arraytmp[4];
   $file=$arraytmp[7];
 
for ($i=0; $i<2; $i++){
   $mthtmp=index($month,$monthlist[$i]);
   #print "$mthtmp\n";
   if ($mthtmp==0){
    $newmonth=$newmthlst[$i];
#    print "$mthtmp $month  $newmonth\n";
   }
  }

for ($i=0; $i <5; $i++) {
$tell_list = index($file, $list[$i]);

if ($tell_list ==1) {
  $newfile = join(".", $newmonth, $list[$i],$list2[$i],$inv,"$type[0].dat.gz");
#   $newfinal= join("/",$newmonth, $newfile);
 rename ( $file, $newfile) or warn "Couldn't rename $line to $newfile:$!\n" ;
 print ($file,"   ", "$newfile\n");
} elsif ($tell_list >=2){
  $newfile = join(".", $newmonth, $list[$i],$list1[$i],$inv,"$type[1].dat.gz");
#   $newfinal= join("/",$newmonth, $newfile);
 rename ( $file, $newfile) or warn "Couldn't rename $line to $newfile:$!\n" ;
print ($file,"   ", "$newfile\n");
      }
   }
}

close ($element);
