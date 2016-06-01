#!/usr/local/bin/perl

$usage = 
    "\n" .
    "Usage: $0 keyposition (starting from 0) statposition1 statposition2...\n" .
    " Author: Gregory Gancarz\n" .
    " Date: 12/9/2004\n" .
    "\n" .
    "\n";

$a=-1;
while (scalar @ARGV) {
  $arg = shift (@ARGV);
  if ($arg =~ /[0-9]+/o) {
    if ($a==-1){
      $keypos = $arg;
    }
    else{
      $statpos[$a]=$arg;
    }
  }
  else {
    print STDERR $usage;
    exit 1;
  }
  $a++;
}

sub day90{
  local($indate)=@_;
  @months = (31,28,31,30,31,30,31,31,30,31,30,31);
  $yr=1*substr($indate,0,4);
  $mm=1*substr($indate,4,2);
  $dd=1*substr($indate,6,2);
  $sum=0;
  $add=0;
  for ($ii=0;$ii<($mm-1);$ii++){
    $sum=$sum+1*$months[$ii];
  }
  for ($iii=1990;$iii<$yr;$iii++){
    if ((1*$iii%4)==0){
      $add++;
    }
  }
  if($mm>2 && (1*$yr%4)==0){
    $add++;
  }
  $outnum=365*($yr-1990)+$sum+$dd+$add-1;
  $outnum;
}

while(<>){
  chomp;
  @tran=split(" ",$_);
  #$key=$tran[$keypos];
  $tmp=`echo $tran[$keypos] | unpack_zips`;
  #print "tmp=$tmp";
  $key=unpack("a3",$tmp);
  #print "key=$key\n\n";
  $tranisfrd=$tran[0]*1;
  for ($s=0;$s<$a;$s++){
    $val=$tran[$statpos[$s]]*1;
    if ($tranisfrd==1){
      $frdcnt[$s]{$key}++;
      $frdsumval[$s]{$key}+=$val;
      $frdsumvalsqrd[$s]{$key}+=$val**2;
      $deffrdcnt[$s]++;
      $deffrdsumval[$s]+=$val;
      $deffrdsumvalsqrd[$s]+=$val**2;
    }
    else{
      $nfcnt[$s]{$key}++;
      $nfsumval[$s]{$key}+=$val;
      $nfsumvalsqrd[$s]{$key}+=$val**2;
      $defnfcnt[$s]++;
      $defnfsumval[$s]+=$val;
      $defnfsumvalsqrd[$s]+=$val**2;
    }
    $allvals[$s]{$key}++;
  }
}

$keycnt=0;
foreach $key (sort {$a<=>$b} keys %{$allvals[0]}){
  $keycnt++;
}

print("TABLE \n");
print("DEFAULT =\n");

printf("%d,%d,%d","0",$defnfcnt[0]+$deffrdcnt[0],$deffrdcnt[0]);
for ($s=0;$s<$a;$s++){
  $avgfrdval=eval{$deffrdsumval[$s]/$deffrdcnt[$s];} || 0;
  $avgnfval=eval{$defnfsumval[$s]/$defnfcnt[$s];} || 0;
  $nfvalstd=eval{ sqrt(($defnfsumvalsqrd[$s]-$defnfsumval[$s]**2/$defnfcnt[$s])/($defnfcnt[$s]-1));} || 0;
  $frdvalstd=eval{ sqrt(($deffrdsumvalsqrd[$s]-$deffrdsumval[$s]**2/$deffrdcnt[$s])/($deffrdcnt[$s]-1));} || 0;
  printf(",%.2f,%.2f,%.2f,%.2f",$avgnfval,$avgfrdval,$nfvalstd,$frdvalstd);
}
printf("\n");

printf("NUM_ROWS =   %d\n",$keycnt);
print("DATA =\n");

foreach $key (sort {$a<=>$b} keys %{$allvals[0]}){
  printf("%d,%d,%d",$key,$allvals[0]{$key},$frdcnt[0]{$key});
  for ($s=0;$s<$a;$s++){
    $avgfrdval=eval{ $frdsumval[$s]{$key}/$frdcnt[$s]{$key}; } || 0;
    $avgnfval=eval{ $nfsumval[$s]{$key}/$nfcnt[$s]{$key}; } || 0;
    $nfvalstd=eval{ sqrt(($nfsumvalsqrd[$s]{$key}-$nfsumval[$s]{$key}**2/$nfcnt[$s]{$key})/($nfcnt[$s]{$key}-1)); } || 0;
    $frdvalstd=eval{ sqrt(($frdsumvalsqrd[$s]{$key}-$frdsumval[$s]{$key}**2/$frdcnt[$s]{$key})/($frdcnt[$s]{$key}-1)); } || 0;
    printf(",%.2f,%.2f,%.2f,%.2f",$avgnfval,$avgfrdval,$nfvalstd,$frdvalstd);
  }
  printf("\n");
}
