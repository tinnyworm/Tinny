#!/usr/bin/env perl
use Getopt::Long;

$DEFAULT_MIN_THRESHOLD=5;
$DEFAULT_ACCURACY=2;

$usage= "
Generate risktables (all but SICxAMT) from input data file. 
The following input data file format is assumed:
  IS_FRAUD, IS_KEY, HOW, SIC, POSTAL, CNTRY, CURR, AMT, DIST

Usage: zcat input_data.gz|$0 [options] > risktable
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

#Load SIC Group 97
open SICF, "</work/price/falcon/database/sic_riskgen_93_97.tbl" or die;
while(<SICF>){
  next if (/^\s*#/);
  @line=split(/,/,$_);
  $SICGRP97{$line[0]*1}=[$line[3],$line[4]]; # SIC => [VISA97, FACTOR97]
}

@TABLES=( {name=>'A_KEY_HOW_RISK_DATA',   type=>'HOW_RISK_DATA',   data=>\%key_how_table},
          {name=>'A_SWP_HOW_RISK_DATA',   type=>'HOW_RISK_DATA',   data=>\%swp_how_table},
          {name=>'A_CHP_HOW_RISK_DATA',   type=>'HOW_RISK_DATA',   data=>\%chp_how_table},
          {name=>'A_KEY_SIC_RISK_DATA',   type=>'SIC_RISK_DATA',   data=>\%key_sic_table},
          {name=>'A_SWP_SIC_RISK_DATA',   type=>'SIC_RISK_DATA',   data=>\%swp_sic_table},
          {name=>'A_CHP_SIC_RISK_DATA',   type=>'SIC_RISK_DATA',   data=>\%chp_sic_table},
          {name=>'A_KEY_POST_RISK_DATA',  type=>'POSTAL_RISK_DATA',data=>\%key_postal_table},
          {name=>'A_SWP_POST_RISK_DATA',  type=>'POSTAL_RISK_DATA',data=>\%swp_postal_table},
          {name=>'A_CHP_POST_RISK_DATA',  type=>'POSTAL_RISK_DATA',data=>\%chp_postal_table},
          {name=>'A_KEY_CNTRY_RISK_DATA', type=>'CNTRY_RISK_DATA', data=>\%key_cntry_table},
          {name=>'A_SWP_CNTRY_RISK_DATA', type=>'CNTRY_RISK_DATA', data=>\%swp_cntry_table},
          {name=>'A_CHP_CNTRY_RISK_DATA', type=>'CNTRY_RISK_DATA', data=>\%chp_cntry_table},
          {name=>'A_KEY_CURR_RISK_DATA',  type=>'CNTRY_RISK_DATA', data=>\%key_curr_table},
          {name=>'A_SWP_CURR_RISK_DATA',  type=>'CNTRY_RISK_DATA', data=>\%swp_curr_table},
          {name=>'A_CHP_CURR_RISK_DATA',  type=>'CNTRY_RISK_DATA', data=>\%chp_curr_table} );

while(<STDIN>){
  chomp;
  ($IS_FRAUD, $IS_KEY, $HOW, $SIC, $POSTAL, $CNTRY, $CURR, $AMT, $DIST)
        =split(/,/,$_);

  $IS_FRAUD=($IS_FRAUD > 0.5);

  $SIC=($SIC>=0)?$SIC:0;

  $CNTRY=sprintf("%d",$CNTRY);
  $CURR=sprintf("%d",$CURR);

  if($IS_KEY =~ /K/){
    update(\$key_how_table{$HOW});
    update(\$key_sic_table{$SIC});
    update(\$key_postal_table{$POSTAL});
    update(\$key_cntry_table{$CNTRY});
    update(\$key_curr_table{$CURR});
  }elsif($IS_KEY =~ /C/){
    update(\$chp_how_table{$HOW});
    update(\$chp_sic_table{$SIC});
    update(\$chp_postal_table{$POSTAL});
    update(\$chp_cntry_table{$CNTRY});
    update(\$chp_curr_table{$CURR});
  }elsif($IS_KEY =~ /S/){
    update(\$swp_how_table{$HOW});
    update(\$swp_sic_table{$SIC});
    update(\$swp_postal_table{$POSTAL});
    update(\$swp_cntry_table{$CNTRY});
    update(\$swp_curr_table{$CURR});
  }else{
    die ("Bad IS_KEY value!");
  }
}

foreach $table (@TABLES){
  $table->{def}=summarize($table->{data});
  cleanup($table->{name},$table->{def},$table->{data});
  table_print($table->{name},$table->{type},$table->{def},$table->{data});
}

sub update {
  my $refref = shift;
  $$refref=[0,0,0,0,0,0,0,0,0,0] unless (defined $$refref);
  my $ref=$$refref;
  $ref->[0]+=1;
  $ref->[1]+=($IS_FRAUD)?1:0;
  if($IS_FRAUD==1){
    $ref->[3]+=$AMT;
    $ref->[5]+=$AMT*$AMT;
    $ref->[7]+=$DIST;
    $ref->[9]+=$DIST*$DIST;
  }elsif($IS_FRAUD==0){
    $ref->[2]+=$AMT;
    $ref->[4]+=$AMT*$AMT;
    $ref->[6]+=$DIST;
    $ref->[8]+=$DIST*$DIST;
  }
}

sub summarize {
  my $hashref=shift;
  my @def=();

  foreach my $k (keys %{$hashref}){
    my $ref=$hashref->{$k};

    my $i=0;
    while(defined ($ref->[$i])){
      $def[$i]+=$ref->[$i];
      $i++;
    }
    
    my $f=$ref->[1];
    my $nf=$ref->[0]-$f;

    $ref->[2]=eval{$ref->[2]/$nf;} || 0;
    $ref->[4]=eval{($ref->[4]-$ref->[2]*$ref->[2]*$nf)/($nf-1);} || 0;
    $ref->[4]=eval{sqrt($ref->[4]);} || 0;

    $ref->[3]=eval{$ref->[3]/$f;} || 0;
    $ref->[5]=eval{($ref->[5]-$ref->[3]*$ref->[3]*$f)/($f-1);} || 0;
    $ref->[5]=eval{sqrt($ref->[5]);} || 0;

    $ref->[6]=eval{($nf<=0)?0:$ref->[6]/$nf;} || 0;
    $ref->[8]=eval{($ref->[8]-$ref->[6]*$ref->[6]*$nf)/($nf-1);} || 0;
    $ref->[8]=eval{sqrt($ref->[8]);} || 0;

    $ref->[7]=eval{$ref->[7]/$f;} || 0;
    $ref->[9]=eval{($ref->[9]-$ref->[7]*$ref->[7]*$f)/($f-1);} || 0;
    $ref->[9]=eval{sqrt($ref->[9]);} || 0;
  }

  $f=$def[1];
  $nf=$def[0]-$f;

  $def[2]=eval{$def[2]/$nf;} || 0;
  $def[4]=eval{($def[4]-$def[2]*$def[2]*$nf)/($nf-1);} || 0;
  $def[4]=eval{sqrt($def[4]);} || 0;

  $def[3]=eval{$def[3]/$f;} || 0;
  $def[5]=eval{($def[5]-$def[3]*$def[3]*$f)/($f-1);} || 0;
  $def[5]=eval{sqrt($def[5]);} || 0;

  $def[6]=eval{$def[6]/$nf;} || 0;
  $def[8]=eval{($def[8]-$def[6]*$def[6]*$nf)/($nf-1);} || 0;
  $def[8]=eval{sqrt($def[8]);} || 0;

  $def[7]=eval{$def[7]/$f;} || 0;
  $def[9]=eval{($def[9]-$def[7]*$def[7]*$f)/($f-1);} || 0;
  $def[9]=eval{sqrt($def[9]);} || 0;

  return \@def;
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
    printf "%d,%d,%d" . ",%.${accuracy}f" x 8 .",%d,%d\n", 0, @{ $def }, @{ $SICGRP97{-1} };
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
      printf "%d,%d,%d" . ",%.${accuracy}f" x 8 .",%d,%d\n",$k, @{ $ref->{$k} }, @{ $SICGRP97{$k*1} };
    }elsif($type =~ /POST/){
      printf "\"%3.3s\",%d,%d" . ",%.${accuracy}f" x 8 ."\n",$k, @{ $ref->{$k} };
    }else{
      printf "%d,%d,%d" . ",%.${accuracy}f" x 8 ."\n",$k, @{ $ref->{$k} };
    }
  }
}

