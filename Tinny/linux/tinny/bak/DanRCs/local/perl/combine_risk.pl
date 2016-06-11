#!/usr/bin/env perl
use Getopt::Long;

$DEFAULT_MIN_THRESHOLD=5;
$DEFAULT_ACCURACY=2;

$usage="
Combine risktables (all but SICxAMT).
Usage: $0 [options] <table file list>  > combined_table_file
Options:
        -a <accuracy>           Print accuracy for floating point numbers.
                                Default: 2
        -M <Min_threshold>      Table records will be drooped, if number of 
                                transactions in it < Min_threshold.
                                Default: 5
        -n                      NO stddev fixing. Otherwise, if (0<stddev<1), 
                                the stddev will be set to 0.0.

The risktable file in the list should content exactly the same tables.
";

@optl = ("a=f","M=i","n");
die $usage unless GetOptions(@optl);

$accuracy=$opt_a?$opt_a:$DEFAULT_ACCURACY;
$min_threshold=$opt_M?$opt_M:$DEFAULT_MIN_THRESHOLD;
$fix_stddev=$opt_n?0:1;

$file=shift or die $usage;

while($file){
  open FL, "<$file" or die "Cannot read file $file\n";

  $i=-1;
  while(<FL>){
    chomp;
    next if (/^\s*\/\//);
    if(/^\s*TABLE\s+(\S+)\s+OF\s+(\S+)\s*$/){
      $i++;
      if(!defined $TABLES[$i]){
        $TABLES[$i]={name=>$1,type=>$2};
      }else{
        die "RiskTable files should content the same tables.\n"
                if($TABLES[$i]->{name} ne $1);
      }
    }elsif(/^\s*DEFAULT/){
      $nl=<FL>;
      ($dkey,@defdata)=split(/,/,$nl);
      $dkey=~s/"//g;
      if (defined $TABLES[$i]->{def}){
        $newdefdata=rec_comb($TABLES[$i]->{def},\@defdata);
        $TABLES[$i]->{def}=[ @{ $newdefdata } ];
      }else{
        $TABLES[$i]->{def}=[ @defdata ];
      }
    }elsif(/NUM_ROWS/){
      next;
    }elsif(/DATA/){
      $TABLES[$i]->{data}={} unless defined $TABLES[$i]->{data};
    }else{
      ($key,@rdata)=split(/,/,$_);
      $key=~s/"//g;
      if(defined $TABLES[$i]->{data}->{$key}){
        $newdata=rec_comb($TABLES[$i]->{data}->{$key},\@rdata);
        $TABLES[$i]->{data}->{$key}=[ @{ $newdata } ];
      }else{
        $TABLES[$i]->{data}->{$key}=[ @rdata ];
      }
    }
  }

  close FL;
  $file=shift;
}

foreach $table (@TABLES){
  cleanup($table->{name},$table->{def},$table->{data});
  table_print($table->{name},$table->{type},$table->{def},$table->{data});
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
    foreach my $i (qw(4 5 8 9)){
      if($def->[$i]>0 && $def->[$i]<1){
        print STDERR "WARNING: $name, DEFAULT, stddev$i=",$def->[$i]," is set to 0.0\n";
        $def->[$i]=0;
      }
    }
  }

  foreach my $k (keys %{ $ref }){
    if ($name !~ /HOW/ && $ref->{$k}->[0] < $min_threshold){
      delete $ref->{$k};
    }elsif($fix_stddev){
      foreach my $i (qw(4 5 8 9)){
        if($ref->{$k}->[$i]>0 && $ref->{$k}->[$i]<1){
          print STDERR "WARNING: $name, KEY=$k, stddev$i=",$ref->{$k}->[$i]," is set to 0.0\n";
          $ref->{$k}->[$i]=0;
        }
      }
    }
  }
}


sub table_print{
  my ($title,$type,$def,$ref)=@_;

  print "TABLE $title OF $type\n";
  print "DEFAULT =\n";
  if ($type =~ /SIC/){
    printf "%d,%d,%d" . ",%.${accuracy}f" x 8 .",%d,%d\n", 0, @{ $def };
    delete $ref->{0};
  }elsif($type =~ /POST/){
    printf "\"%3.3s\",%d,%d" . ",%.${accuracy}f" x 8 ."\n", "   ", @{ $def };
    delete $ref->{"   "};
  }elsif($type =~ /HOW/){
    printf "%d,%d,%d" . ",%.${accuracy}f" x 8 ."\n", 0, @{ $def };
  }else{
    printf "%d,%d,%d" . ",%.${accuracy}f" x 8 ."\n", 0, @{ $def };
    delete $ref->{0};
  }
  print "NUM_ROWS = ", scalar(keys %{ $ref }), "\n";
  print "DATA =\n";

  my @klist;
  if ($type =~ /POST/){
    @klist=sort {$a cmp $b} keys %{ $ref };
  }else{
    @klist=sort {$a <=> $b} keys %{ $ref };
  }

  foreach $k (@klist){
    if($type =~ /SIC/){
      printf "%d,%d,%d" . ",%.${accuracy}f" x 8 .",%d,%d\n",$k, @{ $ref->{$k} };
    }elsif($type =~ /POST/){
      printf "\"%3.3s\",%d,%d" . ",%.${accuracy}f" x 8 ."\n",$k, @{ $ref->{$k} };
    }else{
      printf "%d,%d,%d" . ",%.${accuracy}f" x 8 ."\n",$k, @{ $ref->{$k} };
    }
  }
}

