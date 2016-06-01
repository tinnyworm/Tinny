#!/usr/local/bin/perl -w
#
# Plots (using gnuplot) some curves from a number of falcon FALB920 files.
#
# 
# Output is:
#   (a) to the screen (always)
#   (b) to postscript file (user queried)
#   (c) to cgm (MS-Word readable) file (user queried)
#
# Postscript curves from different runs should be of the same scale ---
# hold them up to the light and see the curves on the same scales.
#
#
# More detailed usage below (to see, execute without any command line
# args).
#



########################################

if ( -r "data.$$" ) {
   print "Cannot create temp data file: data.$$ exists!\n";
   exit 20;
}

sub GracefulExit($) {
   my ($exit_val) = @_;
   unlink "data.$$";
   print "\n...tmp files removed\n";
   if ($exit_val !~ m/^\d+$/o) {
      $exit_val = 1;
   }
   exit $exit_val;
}

$SIG{HUP}  = \&GracefulExit;
$SIG{INT}  = \&GracefulExit;
$SIG{QUIT} = \&GracefulExit;
$SIG{ALRM} = \&GracefulExit;
$SIG{TERM} = \&GracefulExit;



########################################
# 
# Definitions of the axis for different plots
# Accessed by the plot number.
#
# $GRAPH[1]{xaxis}
# $GRAPH[1]{yaxis}

@GRAPH = (
   { xaxis => "scoreAUTH", yaxis => "ScoreDistAUTH" },    
   { xaxis => "score900AUTH", yaxis => "ScoreDist900AUTH" }, 
   { xaxis => "score980AUTH", yaxis => "ScoreDist980AUTH" }, 
    
);

@IMP = (0, 1, 2);

########################################
# 
# Define where in the data file you will find each of the above items.
#
# This must change if your data file output changes!!!!!

%COL = (
   "scoreAUTH"             => 1,
   "ScoreDistAUTH"         => 2,
   "score900AUTH"          => 3,
   "ScoreDist900AUTH"      => 4,
   "score980AUTH"          => 5,
   "ScoreDist980AUTH"      => 6,
);

########################################
# 
# Define a label which described each of the above items

%LABEL = (
   "scoreAUTH"          => "authorization transaction score",
   "ScoreDistAUTH"      => "Fraction of authorizations below threshold",
   "score900AUTH"       => "authorization transaction score from 900",
   "ScoreDist900AUTH"   => "Fraction of authorizations below threshold",
   "score980AUTH"       => "authorization transaction score from 980",
   "ScoreDist980AUTH"   => "Fraction of authorizations below threshold",
);

########################################
# 
# Define a range for each of the above items

%RANGE = (

   "scoreAUTH"          => "[0:1000]",
   "ScoreDistAUTH"      => "[0.9:1]",
   "score900AUTH"       => "[900:1000]",
   "ScoreDist900AUTH"   => "[0.0:1]",
   "score980AUTH"       => "[980:1000]",
   "ScoreDist980AUTH"   => "[0.0:1]",
);

########################################
# 
# Define the key location for each of the above items when it is the x-axis

%KEY_LOCATION = (

   "scoreAUTH"            => "bottom right",
   "score900AUTH"         => "bottom right",
   "score980AUTH"         => "bottom right",
);
$clear_key = 0;

########################################

$DATA = "> data.$$";

select DATA   ; $| = 1;
select STDOUT;
open DATA    or die "Could not open $DATA!\n";



########################################
# 
# Defaults and them options...

@GRAPH_CHOICE = ();
$STYLE="linespoints";
$HELP="N";
$YRANGE="";
$XRANGE="";
$TITLE=" ";
$KEY="";
$QUIET = 0;
$ANSWER_CGM = "n";
$ANSWER_PS = "n";
$DO_ALL = 0;

while ( (scalar @ARGV) > 0 ) {
   $i=shift @ARGV;
   if ( "$i" eq "-h" ) {
      $HELP="Y";
   } elsif ( $i =~ m/^\-([0-9]+)$/ ) {
      push @GRAPH_CHOICE, $1 - 1;
   } elsif ( "$i" eq "-nl" ) {
      $STYLE="points";
   } elsif ( "$i" eq "-np" ) {
      $STYLE="lines";
   } elsif ( "$i" eq "-ur" ) {
      $KEY="$KEY top right"
   } elsif ( "$i" eq "-br" ) {
      $KEY="$KEY bottom right";
   } elsif ( "$i" eq "-s" ) {
      $XRANGE="[ * : * ]";
      $YRANGE="[ * : * ]";
   } elsif ( "$i" eq "-q" ) {
      $QUIET = 1;
   } elsif ( "$i" eq "-Q" ) {
      $QUIET = 2;
   } elsif ( "$i" eq "-c" ) {
      $ANSWER_CGM = "y";
   } elsif ( "$i" eq "-p" ) {
      $ANSWER_PS = "y";
   } elsif ( "$i" eq "-xr" ) {
      $j = shift @ARGV;
      $XRANGE=$j;
   } elsif ( "$i" eq "-yr" ) {
      $j = shift @ARGV;
      $YRANGE=$j;
   } elsif ( "$i" eq "-title" ) {
      $j = shift @ARGV;
      $TITLE=$j;
   } elsif ( "$i" eq "-all" ) {
      $DO_ALL = 1;
   } elsif ( "$i" eq "-imp" ) {
      push @GRAPH_CHOICE, @IMP;
   } else {
      $IN = "$i";
      if (!(open IN)) {
         print "Could not open $IN\n";
         &GracefulExit(2);
      }
      print "Reading $i ...\n";

      # Clear out old values...
      if (defined (@scoreAUTH)) {
          undef @scoreAUTH;
          undef @ScoreDistAUTH;
          undef @score900AUTH;
          undef @ScoreDist900AUTH;
          undef @score980AUTH;
          undef @ScoreDist980AUTH;
      }
      for ($k=0; $k<21; $k++) {
          $scoreAUTH[$k] = 0;
          $countsScoreA[$k] = 0;
          $ScoreDistAUTH[$k] = 0;
          $score900AUTH[$k] = 0;
          $countsScore900A[$k] = 0;
          $ScoreDist900AUTH[$k] = 0;
          $score980AUTH[$k] = 0; 
          $countsScore980A[$k] = 0; 
          $ScoreDist980AUTH[$k] = 0;  
      
      }

      my ($hdr);
      while (defined ($line = <IN>)) {
          if ($line =~ m/^\(q35\) Total authorizations in 50 point bands/io)  {               
      # falb920 v1.2 files q35 for auth will match here
              for ($m = 0; $m < 2; $m ++) {
                  $in = <IN>;
              }   #headers
                  $in = <IN>;
                  @F = split " ", $in;
                  $scoreAUTH[0] = $F[0];
                  $countsScoreA[0] = $F[1];
                  $totalAUTH1 = $countsScoreA[0]; 
                  $ScoreDistAUTH[0] = $totalAUTH1 ;
              for ($k=1; $k<21; $k++) {
                  $in = <IN>;
                  chop $in;
                  @l = split " ", $in;
                  $scoreAUTH[$k]  = $l[2];  
                  $countsScoreA[$k] = $l[3];
                  $totalAUTH1 += $countsScoreA[$k];
                  $ScoreDistAUTH[$k] = $totalAUTH1;
              }
          } elsif ($line =~ m/^\(q36\) Total authorizations with scores greater than or equal to 900 in 10 point bands by TRN_AUT/io) {

              # falb920 v1.2 files q36 for auth will match here
              # The data points start from k=10 to avoid extral line
              for ($m = 0; $m < 2; $m ++) {
                  $in = <IN>;
              }   #headers
                  $in = <IN>;
                  @F = split " ", $in;
                  $score900AUTH[10] = $F[0];  
                  $countsScore900A[10] = $F[1];
                  $totalAUTH2 = $countsScore900A[0];
                  $ScoreDist900AUTH[10] = $totalAUTH2;
  
              for ($k=11; $k<21; $k++) {
                  $in = <IN>;
                  chop $in;
                  @l = split " ", $in;
                  $score900AUTH[$k] = $l[2];  
                  $countsScore900A[$k] = $l[3];
                  $totalAUTH2 += $countsScore900A[$k];
                  $ScoreDist900AUTH[$k] = $totalAUTH2; 
              }
          } elsif ($line =~ m/^\(q37\) Total authorizations with scores greater than or equal to 980 in 1 point bands by TRN_AUTH/io) {                           
              # falb920 v1.2 files q37 for auth will match here                 
              for ($m = 0; $m < 2; $m ++) {
                  $in = <IN>;
              }   #headers
              
              $totalAUTH3 = 0;
   
              for ($k=0; $k<21; $k++) {
                  $in = <IN>;
                  chop $in;
                  @l = split " ", $in;
                  $score980AUTH[$k] = $l[0];
                  $countsScore980A[$k] = $l[1];
                  $totalAUTH3 += $countsScore980A[$k]; 
                  $ScoreDist980AUTH[$k] = $totalAUTH3;
              }
          }
      }
      for ($j=0; $j<21; $j++) {
         if ($totalAUTH1 != 0) { 
            printf DATA " %4d",   $scoreAUTH[$j];
            printf DATA " %9.5f", $ScoreDistAUTH[$j]/$totalAUTH1;
         }
         if ($totalAUTH2 != 0) {
            printf DATA " %4d", $score900AUTH[$j];
            printf DATA " %9.5f", $ScoreDist900AUTH[$j]/$totalAUTH2;
         }
         if ($totalAUTH3 != 0) {
            printf DATA " %4d", $score980AUTH[$j]; 
            printf DATA " %9.5f", $ScoreDist980AUTH[$j]/$totalAUTH3;
         }
    

         printf DATA "\n";
      }
      print DATA "\n";
      print DATA "\n";
      push @FILES, $i;
   }
}



########################################

if ( !"@FILES" || $HELP eq "Y" ) {
   print "No files found on the command line!

Usage:  $0 \\
             [-[1-21]] [-np] [-nl] \\
             [-q] [-Q] [-p] [-c] \\
             [-s] [-ur] [-br] [-xr <range>] \\
             [-all] [-imp] \\
             [-yr <range> ] <file-1> [ <file-2> [...] ]


 Plots (using gnuplot) some curves from a number of output files from
 FCgraph using the dat.* files.

 The default curve is \"$LABEL{$GRAPH[0]{yaxis}}\" vs. \"$LABEL{$GRAPH[0]{xaxis}}\".

 Output is:
   (a) to the screen (always)
   (b) to postscript file (user queried)
   (c) to cgm (MS-Word readable) file (user queried)

 Postscript curves from different runs should be of the same scale ---
 hold them up to the light and see the curves on the same scales.

 Author: rxz 9/22/03 


$rcsID


 -q          Quiet.  Do not ask user anything.
 -Q          Quiet!  Don't even display the graph!
 -p          Automaticall put out postcript.
 -c          Automaticall put out cgm.
 -np         Do not put individul points on the graph
 -nl         Do not put lines on the graph
 -s          Autoscale both axes
 -ur         Legend in the upper right
 -br         Legend in the bottom right
 -xr <range> Use <range> as the x-axis range
 -yr <range> Use <range> as the y-axis range
             <range> = [<lo>:<hi>], and needs to be quoted in most shells
 -title <t>  Use <t> as the graph title (quote as needed for your shell).
\n";

   for ($i = 0; $i< scalar(@GRAPH) ; $i++) {
      printf " -%-2d      Plot %s vs %s\n", $i+1,
         $LABEL{$GRAPH[$i]{xaxis}}, $LABEL{$GRAPH[$i]{yaxis}};
   }

   print "\n";

   &GracefulExit(3);
}



########################################

if (!$QUIET) {
   print "\n\nPlease input the graph title : ";
   $NEWTITLE = <STDIN>;
   chop $NEWTITLE;

   if ($NEWTITLE) {
      $TITLE="$NEWTITLE ";
   }
}

########################################

# print "$X, $Y\n";
#
if ($DO_ALL) {
    for ($i = scalar (@GRAPH) - 1; $i >= 0; $i--) {
        &doPlot($i);
        sleep 1;
    }
} else {
    if (scalar (@GRAPH_CHOICE) == 0) {
        push @GRAPH_CHOICE, @IMP;
    }
    # print "GRAPH_CHOICE =  @GRAPH_CHOICE\n";

    for ($i = scalar (@GRAPH_CHOICE) - 1; $i >= 0; $i--) {
        &doPlot($GRAPH_CHOICE[$i]);
        sleep 1;
    }
}
&GracefulExit(0);


################################################################################

sub doPlot {
   
   if ( -x "/usr/local/bin/gnuplot" ) {
       $GNUPLOT = "| /usr/local/bin/gnuplot -persist";
   } elsif ( -x "/usr/bin/gnuplot" ) {
       $GNUPLOT = "| /usr/bin/gnuplot -persist";
   }
   open GNUPLOT or die "Could not open $GNUPLOT!\n";
   select GNUPLOT; $| = 1;
   select STDOUT;

   my ($CHOICE) = @_;
   # print "$X, $Y\n";
########################################


    $X=$GRAPH[$CHOICE]{'xaxis'};
    $Y=$GRAPH[$CHOICE]{'yaxis'};

    $XCOL=$COL{$X};
    $XLABEL=$LABEL{$X};
    if (! $XRANGE) {
        $XR = $RANGE{$X};
    } else {
        $XR = $XRANGE;
    }


    $YCOL=$COL{$Y};
    $YLABEL=$LABEL{$Y};
    if (! $YRANGE) {
        $YR = $RANGE{$Y};
    } else {
        $YR = $YRANGE;
    }

    if (! $KEY) {
        $KEY = $KEY_LOCATION {$X};
        $clear_key = 1;
    }

########################################

    print GNUPLOT "set grid\n";
    print GNUPLOT "set data style $STYLE\n";
    print GNUPLOT "set xrange $XR\n";
    print GNUPLOT "set yrange $YR\n";
    print GNUPLOT "set xlabel '$XLABEL'\n";
    print GNUPLOT "set ylabel '$YLABEL'\n";
    print GNUPLOT "set title \"$TITLE\"\n";
    print GNUPLOT "set timestamp\n";
    print GNUPLOT "set key $KEY\n";
    if ($QUIET > 1) {
        print GNUPLOT "set term dumb\n";
        print GNUPLOT "set out '/dev/null'\n";
    }
    print GNUPLOT "plot \\\n";
    $NUM=0;
    $FIRST=1;
    foreach $i (@FILES) {
        if ($FIRST) {
            $FIRST=0;
        } else {
            print GNUPLOT ", \\\n";
        }
        print GNUPLOT
            "'data.$$' index $NUM:$NUM using $XCOL:$YCOL title '$i'";
        $NUM++;
    }
    print GNUPLOT "\n";



########################################

    $C = sprintf "%3.3d", $CHOICE + 1;

    if (!$QUIET) {
        print "Do you desire a postscript output? [n] : ";
        $ANSWER_PS = <STDIN>;
    }
    if ( $ANSWER_PS  =~ /^y/ || $ANSWER_PS  =~ /^Y/ ) {
        print GNUPLOT "set terminal postscript\n";
        print GNUPLOT "set out 'plot_FALB920.$C.$Y.$X.$$.ps'\n";
        print GNUPLOT "replot\n";

        print "\n";
        print "Postscript output to plot_FALB920.$C.$Y.$X.$$.ps\n";
        print "\n";
    }



########################################

    if (!$QUIET) {
        print "Do you desire a cgm (MS-Word readable) output? [n] : ";
        $ANSWER_CGM = <STDIN>;
    }
    if ( $ANSWER_CGM =~ /^y/ || $ANSWER_CGM =~ /^Y/ ) {
        print GNUPLOT "set terminal cgm\n";
        print GNUPLOT "set out 'plot_FALB920.$C.$Y.$X.$$.cgm'\n";
        print GNUPLOT "replot\n";

        print "";
        print "CGM output to plot_FALB920.$C.$Y.$X.$$.cgm\n";
        print "\n";
    }


########################################

    close GNUPLOT;

# $sys_return=system ("/usr/local/bin/gnuplot -persist < gnuplot.$$");

    if ($? != 0) {
        print "gnuplot returned $?\n";
    }

    $KEY="" if ($clear_key);


}
## print quiting time... ; read ANSWER


# vim:tw=78: