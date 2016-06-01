#!/usr/local/bin/perl
use Getopt::Long;

$DEFAULT_MIN_THRESHOLD=5;
$DEFAULT_ACCURACY=2;

$usage="
Combine SICxAMT risktables.
Usage: $0 [options] <table file list>  > combined_table_file
Options:
        -a <accuracy>           Print accuracy for floating point numbers.
                                Default: 2
        -M <Min_threshold>      Table records will be drooped, if number of 
                                transactions in it < Min_threshold.
                                Default: 5
        -n                      NO stddev fixing. Otherwise, if (0<stddev<1), 
                                the stddev will be set to 0.0.
";

@optl = ("a=f","M=i","n");
die $usage unless GetOptions(@optl);

$accuracy=$opt_a?$opt_a:$DEFAULT_ACCURACY;
$min_threshold=$opt_M?$opt_M:$DEFAULT_MIN_THRESHOLD;
$fix_stddev=$opt_n?0:1;

$file=shift or die $usage;

while($file){
  open FL, "<$file" or die "Cannot read file $file\n";

  while(<FL>){
    chomp;
    next if (/^\s*\/\//);
    last if(/A_SICxAMT_RISK_DATA/);

    if(/^\s*TABLE\s+A_SICxAMT_(\d+).*AMT_RISK_DATA/){
      $sic=$1;
      $TABLES[$sic]={} unless defined $TABLES[$sic];
    }elsif(/^\s*TABLE +DEF_A_SICxAMT/){
      $sic=10000;
      $TABLES[$sic]={} unless defined $TABLES[$sic];
    }elsif(/^\s*DEFAULT/){
      $nl=<FL>;
      ($dkey,@defdata)=split(/,/,$nl);
      $dkey=~s/"//g;
      if (defined $TABLES[$sic]->{def}){
        $newdefdata=rec_comb($TABLES[$sic]->{def},\@defdata);
        $TABLES[$sic]->{def}=[ @{ $newdefdata } ];
      }else{
        $TABLES[$sic]->{def}=[ @defdata ];
      }
    }elsif(/NUM_ROWS/){
      next;
    }elsif(/DATA/){
      $TABLES[$sic]->{data}={} unless defined $TABLES[$sic]->{data};
    }else{
      ($key,@rdata)=split(/,/,$_);
      $key=~s/"//g;
      if(defined $TABLES[$sic]->{data}->{$key}){
        $newdata=rec_comb($TABLES[$sic]->{data}->{$key},\@rdata);
        $TABLES[$sic]->{data}->{$key}=[ @{ $newdata } ];
      }else{
        $TABLES[$sic]->{data}->{$key}=[ @rdata ];
      }
    }
  }

  close FL;
  $file=shift;
}

print << '_END';
//
//************************************************
//* Auth SIC x AMT risk tables (4.0)
//************************************************
//
_END

$t_count=0;
for ($sic=0;$sic<10000;$sic++){
  next unless defined $TABLES[$sic];
  if ($TABLES[$sic]->{def}->[0]<$min_threshold){
    delete $TABLES[$sic];
  }else{
    $t_count++;
    cleanup("A_SICxAMT_$sic",$TABLES[$sic]->{def},$TABLES[$sic]->{data});
    SICXAMT_print("A_SICxAMT_$sic","AMT_RISK_DATA",$TABLES[$sic]->{def},$TABLES[$sic]->{data});
  }
}
SICXAMT_print("DEF_A_SICxAMT","AMT_RISK_DATA",$TABLES[10000]->{def},$TABLES[10000]->{data});
delete $TABLES[10000];

print << '_END';
TABLE A_SICxAMT_RISK_DATA OF A_SICxAMT_RISK_DATA
DEFAULT =
12345, DEF_A_SICxAMT
_END

print "NUM_ROWS = ", $t_count,"\n";
print "DATA =\n";
for ($sic=0;$sic<10000;$sic++){
    next unless defined $TABLES[$sic];
    print $sic,", A_SICxAMT_",$sic,"\n";
}


sub rec_comb {
  my ($ref1,$ref2)=@_;
  my @rt=();
  my ($n1,$n2,$f1,$f2);

  $rt[0]=$ref1->[0]+$ref2->[0];
  $rt[1]=$ref1->[1]+$ref2->[1];

  $n1=$ref1->[0]-$ref1->[1];
  $f1=$ref1->[1];
  $n2=$ref2->[0]-$ref2->[1];
  $f2=$ref2->[1];

  ($rt[2],$rt[4])=combine($n1,$ref1->[2],$ref1->[4],$n2,$ref2->[2],$ref2->[4]);
  ($rt[3],$rt[5])=combine($f1,$ref1->[3],$ref1->[5],$f2,$ref2->[3],$ref2->[5]);

  if(defined $ref1->[6]){
    ($rt[6],$rt[8])=combine($n1,$ref1->[6],$ref1->[8],$n2,$ref2->[6],$ref2->[8]);
    ($rt[7],$rt[9])=combine($f1,$ref1->[7],$ref1->[9],$f2,$ref2->[7],$ref2->[9]);
  }

  if(defined $ref1->[10]){
    $rt[10]=$ref1->[10];
    $rt[11]=$ref1->[11];
  }

  return \@rt;
}

sub combine {
  my ($nx,$ax,$sx,$ny,$ay,$sy)=@_;
  my ($n,$a,$s);

  $n=$nx+$ny;
  $a=eval{($ax*$nx+$ay*$ny)/$n;} || 0;
  $sx=$sx*$sx*($nx-1);
  $sy=$sy*$sy*($ny-1);
  $s=eval{($sx+$sy+$ax*$ax*$nx+$ay*$ay*$ny-$a*$a*$n)/($n-1);} || 0;
  $s=eval{sqrt($s);} || 0;
  return ($a,$s);
}

sub cleanup {
  my ($name,$def,$ref)=@_;

  if($fix_stddev){
    foreach my $i (qw(4 5)){
      if($def->[$i]>0 && $def->[$i]<1){
        print STDERR "WARNING: $name, DEFAULT, stddev$i=",$def->[$i]," is set to 0.0\n";
        $def->[$i]=0;
      }
    }
  }

  foreach my $k (keys %{ $ref }){
    if($fix_stddev){
      foreach my $i (qw(4 5)){
        if($ref->{$k}->[$i]>0 && $ref->{$k}->[$i]<1){
          print STDERR "WARNING: $name, KEY=$k, stddev$i=",$ref->{$k}->[$i]," is set to 0.0\n";
          $ref->{$k}->[$i]=0;
        }
      }
    }
  }
}


sub SICXAMT_print{
  my ($title,$type,$def,$ref)=@_;

  print "TABLE $title OF $type\n";
  print "DEFAULT =\n";

  my $avg_ac=$accuracy+2;
  printf("%.${avg_ac}f,%d,%d,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f\n",
        $def->[1]/$def->[0],@{ $def });
  print "NUM_ROWS = ",scalar(keys %{ $ref }),"\n";
  print "DATA =\n";
  foreach my $i (sort {$a <=> $b} keys %{ $ref }){
    printf("%.2f,%d,%d,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f,%.${accuracy}f\n",
        $i,@{ $ref->{$i} });
  }
}

