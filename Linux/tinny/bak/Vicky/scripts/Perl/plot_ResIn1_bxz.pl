#!/usr/local/bin/perl

use Getopt::Long;

$usage=
"Usage: $0 [OPTIONS] res_fileName1 res_fileName2 ...
 Options:  --color,-c           Plot with color.
           --eps,-e             Output eps files instead of ps files.(No print)
           --nops,-n            No ps outputs.
           --printonly,-p       No X output, only print.
";

sub changePlotScalesUpper {
  my ($xRangeMax,$yRangeMax,$xRangeMin,$yRangeMin,$xRangeInc,
      $yRangeInc,$xInc,$yInc)=@_;
  print STDERR "Wait for gnuplot ... \n";
  print STDERR "'.' OR ',' followed by ENTER to rescale X, '<' OR '>'",
               "followed by ENTER to rescale Y, any other key to CONTINUE...\n";
  while(1){
    chomp($ch=<STDIN>);
    if ( $ch eq "\." ) {
      $xRangeMax=$xRangeMax + $xInc;
    } elsif ( $ch eq "," ) {
      $xRangeMax=$xRangeMax - $xInc;
    } elsif ( $ch eq ">" ) {
      $yRangeMax=$yRangeMax + $yInc;
    } elsif ( $ch eq "<" ) {
      $yRangeMax=$yRangeMax - $yInc;
    } else {
      last;
    }

    print GNU "set xrange \[$xRangeMin:$xRangeMax\]
               set xtics $xRangeMin,$xRangeInc,$xRangeMax
               set yrange \[$yRangeMin:$yRangeMax\]
               set ytics $yRangeMin,$yRangeInc,$yRangeMax
               replot\n";
  }
}
 
sub changePlotScalesLower {
  my ($xRangeMax,$yRangeMax,$xRangeMin,$yRangeMin,$xRangeInc,
      $yRangeInc,$xInc,$yInc)=@_;
  print STDERR "Wait for gnuplot ... \n";
  print STDERR "'.' OR ',' followed by ENTER to rescale X, '<' OR '>'",
               "followed by ENTER to rescale Y, any other key to CONTINUE...\n";
  while(1){
    chomp($ch=<STDIN>);
    if ( $ch eq "\." ) {
      $xRangeMin=$xRangeMin + $xInc;
    } elsif ( $ch eq "," ) {
      $xRangeMin=$xRangeMin - $xInc;
    } elsif ( $ch eq ">" ) {
      $yRangeMax=$yRangeMax + $yInc;
    } elsif ( $ch eq "<" ) {
      $yRangeMax=$yRangeMax - $yInc;
    } else {
      last;
    }

    print GNU "set xrange \[$xRangeMin:$xRangeMax\]
               set xtics $xRangeMin,$xRangeInc,$xRangeMax
               set yrange \[$yRangeMin:$yRangeMax\]
               set ytics $yRangeMin,$yRangeInc,$yRangeMax
               replot\n";
  }
}
  
sub plotGraph {
  my $name=$_[0];
  print "$name\n";
  unless($opt_nops){
    print GNU "set terminal postscript $tmn
               set output \"$name.$sfx\"
               replot\n";
  }
  if ($opt_printonly){
    print GNU "set terminal dumb
               set output \'/dev/null\'\n";
  }else{
    print GNU "set output
               set terminal x11\n";
  }
}

sub plotX { #$xRange $yRange $yLabel $yWhich
  ($xRange,$yRange,$yLabel,$yWhich)=@_;
  print GNU "set xlabel \"Account False Positive (AFPR)\"
             set ylabel \"$yLabel\"
             set xrange \[0:$xRange\]
             set yrange \[0:$yRange\]
             set xtics 0,10,$xRange
             set ytics 0,10,$yRange
             plot ";
  for ($i=0;$i<$nbr;$i++){
  $X=2+$i*$noOfC;
  $Y=$yWhich+$i*$noOfC;
  my $tt=$fname[$i];
  $tt=~s/_/\\\\_/g if $opt_color;
  print GNU "\'dataout\' using $X:$Y title \"$tt\" with l";
  if ($i<$nbr-1){print GNU ", ";}
  }
  print GNU "\n";
}

sub plotX2 { #$xRange $yRange $yLabel $yWhich
  ($xRange,$yRange,$yLabel,$yWhich)=@_;
  print GNU "set xlabel \"Score\"
             set ylabel \"$yLabel\"
             set xrange \[$xRange:1000\]
             set yrange \[0:$yRange\]
             set xtics $xRange,100,1000
             set ytics 0,$yRange>200?20:10,$yRange
             plot ";
  for ($i=0;$i<$nbr;$i++){
  $Y=$yWhich+$i*$noOfC;
  my $tt=$fname[$i];
  $tt=~s/_/\\\\_/g if $opt_color;
  print GNU "\'dataout\' using 1:$Y title \"$tt\" with l";
  if ($i<$nbr-1){print GNU ", ";}
  }
  print GNU "\n";
}


sub plotX3 { #$xRange $yRange $yLabel $yWhich
  ($xRange,$yRange,$yLabel,$yWhich)=@_;
  print GNU "set xlabel \"%GOOD\"
             set ylabel \"$yLabel\"
             set xrange \[$xRange:1000\]
             set yrange \[0:$yRange\]
             set xtics $xRange,100,1000
             set ytics 0,$yRange>200?20:10,$yRange
             plot ";
  for ($i=0;$i<$nbr;$i++){
  $Y=$yWhich+$i*$noOfC;
  my $tt=$fname[$i];
  $tt=~s/_/\\\\_/g if ($opt_color);
  print GNU "\'dataout\' using 10:$Y title \"$tt\" with l";
  if ($i<$nbr-1){print GNU ", ";}
  }
  print GNU "\n";
}



@optl = ("color","eps","nops","printonly");
die $usage unless GetOptions(@optl);
$tmn="";
$sfx="ps";
if($opt_eps){
  $tmn.="eps ";
  $sfx="eps";
}
if($opt_color){
  $tmn.="enhanced color ";
}

$StartWord="ACCOUNT BASED INFORMATION";
$StartWord2="----\n";
$EndingWord="^\n";
$EndingWord2="^\n";
die $usage if ($#ARGV < 0);

$nbr=0;
for ($ac=0; $ac<=$#ARGV; $ac++){
# if ($ARGV[$ac] =~ /^-/){
# }else{
    $fname[$nbr]=$ARGV[$ac];
    die "Can't open file $fname[$nbr]\n$usage" unless open(RES, "<$fname[$nbr]");
    $nbr++;
    $i=0;
    #
    # Skip till 1st match
    while ( <RES> !~ /$StartWord/ ) { 0 }
    # Skip till 2nd match
    while ( <RES> !~ /$StartWord2/ ) { 0 }
    #
    # Read table
    while (<RES>) {
      if (/$EndingWord/){
        <RES>;
        last if (/$EndingWord2/);
      }
      next if /^\s*$/;
      chomp($line[$i]=$_);
      @f= split(/ +/);
      $AFPR[$i]=$f[2];
      $ADR[$i]=$f[3];
#     print "$AFPR[$i] $ADR[$i] \n";
      $i++;
    }
    die "AFPR section of this research file does not contain 101 lines\n" if ($i != 101);
    close (RES);
    @t=split(' ',$line[100]);
    $totalFLoss=$t[3];
#   print "$line[100],$totalFLoss\n";
    for ($i=0;$i<=100;$i++)
    {
      @t=split(' ',$line[$i]);
      $pg=$t[1]*$t[2]/$AFPR[0];
      $rtVDR=100*($totalFLoss-$t[3])/$totalFLoss;
      $olVDR=100*($totalFLoss-$t[4])/$totalFLoss;
      $baVDR=100*($totalFLoss-$t[5])/$totalFLoss;
      $ak=($t[1]+1)*$t[2]/100;
      $k=$t[2]/($t[1]+1);
      $dk=$rtVDR/($t[1]+1);
      $aa=1/($t[1]+1);
      if ($nbr == 1) {$outline[$i] = "$t[0] ";}
      $outline[$i] .= "$t[1] $t[2] $rtVDR $olVDR $baVDR $ak $k $dk $aa $pg ";
    }
# } #end of else
} #end of loop

$noOfC=10;
die "Can't open file: dataouti\n$usage" unless open(DOUT,">dataout");
for ($i=0;$i<=100;$i++){print DOUT "$outline[$i]\n";}

die "Can't find gnuplot.\n$usage" unless 
    open(GNU,"|gnuplot -background white -fn 7x13bold -geometry 800x600");
$oldfh=select(GNU);$|=1;select($oldfh);

if ($opt_printonly){
  print GNU "set terminal dumb
             set output \'/dev/null\'\n";
}else{
  print GNU "set output
             set terminal x11\n";
}
print GNU "set autoscale
           set grid\n";

##############################################################################
# PLOT ADR vs AFPR
##############################################################################

plotX(100,80,"Account Detection Rate (ADR %)",3);
changePlotScalesUpper($xRange,$yRange,0,0,10,10,10,5) unless $opt_printonly;
plotGraph "gnu01_afpr_adr";

##############################################################################
# PLOT REAL-TIME VDR vs AFPR
##############################################################################

plotX(100,80,"Real-Time Value Detection Rate (RT-VDR %)",4);
changePlotScalesUpper($xRange,$yRange,0,0,10,10,10,5) unless $opt_printonly;
plotGraph "gnu02_afpr_rt";

##############################################################################
# PLOT AFPR vs ONLINE VDR 
##############################################################################

plotX(100,80,"Online Value Detection Rate (OL-VDR %)",5);
changePlotScalesUpper($xRange,$yRange,0,0,10,10,10,5) unless $opt_printonly;
plotGraph "gnu03_afpr_ol";

##############################################################################
# PLOT AFPR vs SCORE
##############################################################################

plotX2(100,300,"Account False Positive (AFPR)",2);
changePlotScalesLower(1000,$yRange,$xRange,0,100,20,100,50) unless $opt_printonly;
plotGraph "gnu04_scr_afpr";

##############################################################################
# PLOT ADR vs SCORE
##############################################################################

plotX2(100,100,"Account Detection Rate (ADR %)",3);
changePlotScalesLower(1000,$yRange,$xRange,0,100,10,100,5) unless $opt_printonly;
plotGraph "gnu05_scr_adr";

##############################################################################
# PLOT CASE MULTIPLIER vs SCORE
##############################################################################

plotX2(100,150,"Case Multiplier (K)",7);
changePlotScalesLower(1000,$yRange,$xRange,0,100,10,100,25) unless $opt_printonly;
plotGraph "gnu06_scr_k";

#plotX3(100,150,"%BAD",3);
#changePlotScalesLower(1000,$yRange,$xRange,0,100,10,100,25) unless $opt_printonly;
##############################################################################
# PRINT ALL PLOTS IF DESIRED 
##############################################################################
unless($opt_eps){
  if ($opt_printonly) {$ch="p";}
  else {
    print "Press 'p' to PRINT plots, any other key to EXIT...\n";
    chomp($ch=<STDIN>);
  }
  if ($ch eq "p"){
    print "Printing plots to default printer...\n";
    sleep 2;
    system("cat gnu*.ps >tt.ps")==0 or die "1";
    system("ps2ps tt.ps tt1.ps")==0 or die "2";
    system("psnup -l -pletter -Pletter -6 tt1.ps tt.ps &>/dev/null")==0 or die "3";
    system("lpr -Psdoprt07 tt.ps")==0 or die "4";
    system("rm -f tt.ps tt1.ps");
    system("rm -f gnu*.ps");
  }
}
close(GNU);
system("rm -f dataout");

