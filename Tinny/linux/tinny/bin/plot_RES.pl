#!/usr/bin/perl -w

#
# Plots (using gnuplot) some curves from a number of falcon research files.
#
# The research files may be from drv_falcon, nawkRESEARCH, ot falconer4.
#
#
$rcsID='$Id: plot_RES.pl,v 1.15 2004/08/03 16:30:20 pcd Exp pcd $';
# 
# Output is:
#   (a) to the screen (always)
#   (b) to EPS/postscript file (user queried)
#   (c) to cgm (MS-Word readable) file (user queried)
#   (d) to SVG (command option)
#   (e) to PNG (command option)
#
# Postscript curves from different runs should be of the same scale ---
# hold them up to the light and see the curves on the same scales.
#
# Author: pcd 10/26/01
# Modified: Matt Kennel, 2008, adding 1 and 10 bin, -nts and legend
#
# More detailed usage below (to see, execute without any command line
# args).
#


########################################

my $datafname = "/tmp/data.$$";

if ( -r "$datafname" ) {
    print "Cannot create temp data file: $datafname already exists!\n";
    exit 20;
}

sub GracefulExit($) {
    my ($exit_val) = @_;
    if ($KEEP_DATA && $exit_val == 0) {
	print "\nCopying data file $datafname to current directory...";
	$status = system ("/bin/cp $datafname ./data.$$"); 
	if ($status != 0) {
	    warn "copying failed: $?";
	}
    }
    unlink "$datafname";
    print "$datafname removed\n";

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
	  { xaxis => "afpr",           yaxis => "adr"            },
	  { xaxis => "afpr",           yaxis => "rtddr"          },
	  { xaxis => "afpr",           yaxis => "olddr"          },
	  { xaxis => "afpr",           yaxis => "bddr"           },
	  { xaxis => "score",          yaxis => "adr"            },
	  { xaxis => "score",          yaxis => "afpr"           },
	  { xaxis => "score",          yaxis => "rtddr"          },
	  { xaxis => "score",          yaxis => "olddr"          },
	  { xaxis => "score",          yaxis => "bddr"           },
	  { xaxis => "caseCreation",   yaxis => "adr"            },
	  { xaxis => "score",          yaxis => "caseCreation"   },
	  { xaxis => "afpr",           yaxis => "caseCreation"   },
	  { xaxis => "acctHitRate",    yaxis => "adr"            },
	  { xaxis => "acctHitRate",    yaxis => "rtddr"          },
	  { xaxis => "acctHitRate",    yaxis => "olddr"          },
	  { xaxis => "acctHitRate",    yaxis => "bddr"           },
	  { xaxis => "score",          yaxis => "tfpr"           },
	  { xaxis => "score",          yaxis => "legitScoreDist" },
	  { xaxis => "score",          yaxis => "fraudScoreDist" },
	  { xaxis => "score",          yaxis => "totalScoreDist" },
	  { xaxis => "afpr",           yaxis => "tfpr"           },
	  { xaxis => "tfpr",           yaxis => "fraudScoreDist" },
	  { xaxis => "legitScoreDist", yaxis => "fraudScoreDist" },
	  { xaxis => "adr",            yaxis => "rtddr"          },
	  { xaxis => "apg",            yaxis => "adr"            },
	  { xaxis => "apg",            yaxis => "rtddr"          },
	  { xaxis => "apg",            yaxis => "olddr"          },
	  { xaxis => "apg",            yaxis => "bddr"           },
	  { xaxis => "score",          yaxis => "apg"            },
	  { xaxis => "afpr",           yaxis => "apg"            },
	  );

@IMP = (0, 1, 2, 3, 5);

########################################
# 
# Define where in the data file you will find each of the above items.
#
# This must change if your data file output changes!!!!!

%COL = (
	"score"          => 1,
	"afpr"           => 2,
	"adr"            => 3,
	"rtddr"          => 6,
	"olddr"          => 7,
	"bddr"           => 8,
	"caseCreation"   => 9,
	"legitScoreDist" => 10,
	"fraudScoreDist" => 11,
	"totalScoreDist" => 12,
	"tfpr"           => 13,
	"acctHitRate"    => 14,
	"apg"            => 15,
	);

########################################
# 
# Define a label which described each of the above items

%LABEL = (
	  "acctHitRate"    => "Account-Level Hit Rate",
	  "adr"            => "Fraud-Account Detection Rate",
	  "afpr"           => "Account False Positive Ratio",
	  "caseCreation"   => "Case Creation Multiplier [(AFPR + 1)*ADR]",
	  "rtddr"          => "Real-Time Value Detection Rate",
	  "olddr"          => "On-line Value Detection Rate",
	  "bddr"           => "Batch Value Detection Rate",
	  "score"          => "Falcon Score",
	  "tfpr"           => "Transaction-level False Positive Rate",
	  "legitScoreDist" => "Legitimate Transactions Score Distribution",
	  "fraudScoreDist" => "Fraudulent Transactions Score Distribution (TDR)",
	  "totalScoreDist" => "Total Score Distribution",
	  "apg"            => "Account-level Percent Good",
	  );

########################################
# 
# Define a range for each of the above items

%RANGE = (

	  "acctHitRate"    => "[0:100]",
	  "adr"            => "[0:100]",
	  "afpr"           => "[0:100]",
	  "caseCreation"   => "[0:100]",
	  "rtddr"          => "[0:100]",
	  "olddr"          => "[0:100]",
	  "bddr"           => "[0:100]",
	  "score"          => "[500:1000]",
	  "tfpr"           => "[0:100]",
	  "legitScoreDist" => "[0:2]",
	  "fraudScoreDist" => "[0:100]",
	  "totalScoreDist" => "[0:2]",
	  "apg"            => "[0:10]",

	  );

########################################
# 
# Define the key location for each of the above items when it is the x-axis

%KEY_LOCATION = (

		 "acctHitRate"    => "top right",
		 "adr"            => "bottom right",
		 "afpr"           => "bottom right",
		 "caseCreation"   => "bottom right",
		 "rtddr"          => "bottom right",
		 "olddr"          => "bottom right",
		 "bddr"           => "bottom right",
		 "score"          => "top right",
		 "tfpr"           => "bottom right",
		 "legitScoreDist" => "bottom right",
		 "fraudScoreDist" => "bottom right",
		 "totalScoreDist" => "bottom right",
		 "apg"            => "bottom right",
		 );
$clear_key = 0;

########################################

$DATA = "> $datafname";

select DATA   ; $| = 1;
select STDOUT;
open DATA    or die "Could not open $DATA!\n";
for $i (sort {$COL{$a} <=> $COL{$b}} keys %COL) {
    print DATA "# Column $COL{$i} : $LABEL{$i}\n";
}



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
$QUIET = 1;
$ANSWER_CGM = "n";
$ANSWER_PS = "n";
$ANSWER_PS_COLOR = "n";
$ANSWER_EPS = "n";
$SVG="n";
$PNG="n";
$FSIZE="14";
$FNAMEARG="";
$DO_ALL = 0;
$KEEP_DATA = 0;

#
$NOTIMESTAMP=0;
@LEGEND = ();

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
	} elsif ( "$i" eq "-v" ) {
	    $QUIET = 0;
	} elsif ( "$i" eq "-q" ) {
	    $QUIET = 1;
	} elsif ( "$i" eq "-Q" ) {
	    $QUIET = 2;
	} elsif ( "$i" eq "-c" ) {
	    $ANSWER_CGM = "y";
	} elsif ( "$i" eq "-P" ) {
	    $ANSWER_PS_COLOR = "y";
	} elsif ( "$i" eq "-p" ) {
	    $ANSWER_PS = "y";
	} elsif ( "$i" eq "-E" ) {
	    $ANSWER_EPS = "y";
	} elsif ( "$i" eq "-svg" ) {
	    $SVG = "y";
	} elsif ( "$i" eq "-png" ) {
	    $PNG = "y";
	} elsif ( "$i" eq "-fsize" ) {
	    $j = shift @ARGV;
	    $FSIZE=$j;
	} elsif ( "$i" eq "-fname" ) {
	    $j = shift @ARGV;
	    $FNAMEARG=$j;
	} elsif ( "$i" eq "-xr" ) {
	    $j = shift @ARGV;
	    $XRANGE=$j;
	} elsif ( "$i" eq "-yr" ) {
	    $j = shift @ARGV;
	    $YRANGE=$j;
	} elsif ( "$i" eq "-title" ) {
	    $j = shift @ARGV;
	    $TITLE=$j;
	} elsif ( "$i" eq "-d" ) {
	    $KEEP_DATA = 1;
	} elsif ( "$i" eq "-all" ) {
	    $DO_ALL = 1;
	} elsif ( "$i" eq "-imp" ) {
	    push @GRAPH_CHOICE, @IMP;
	} elsif ( "$i" eq "-nts") {
	    $NOTIMESTAMP=1; 
	} elsif ( "$i" eq "-legend") {
	    $j = shift @ARGV;
	    @LEGEND = split(/,/,$j); # MBK: legend strings are split with commas
	} else {
	    $IN = "$i";
	    if (!(open IN)) {
		print "Could not open $IN\n";
		&GracefulExit(2);
	    }
	    print "Reading $i ...\n";

	    # Clear out old values...
	    if (defined (@thresh)) {
		undef @thresh;
		undef @afpr;
		undef @adr;
		undef @raw_rtddr;
		undef @raw_olddr;
		undef @raw_bddr;
	    }

	    $NUMBINS = 100;
	    for ($k=0; $k< 1001; $k++) {
		$tfpr[$k] = 0;
		$legitScoreDist[$k] = 0;
		$fraudScoreDist[$k] = 0;
	    }

	    my ($hdr);
	    while (defined ($line = <IN>)) {
		if ($line =~ m/^ Threshold.*AFPR.*ADR.*Case1/o) {
		    # drv_falcon30 (and falconer4?) research files will match here
		    $hdr = <IN>;  # header line
		    for ($k=0; $k<$NUMBINS+1; $k++) {
			$in = <IN>;
			chop $in;
			@l = split " ", $in;
			$thresh[$k]       = $l[0];  # * score_fact + th
			$afpr[$k]         = $l[1];
			$adr[$k]          = $l[2]; # * ENVIRON["FAC"]
			$raw_rtddr[$k]    = $l[3];
			$raw_olddr[$k]    = $l[4];
			$raw_bddr[$k]     = $l[5];
			$raw_bddr[$k]     =   1.0   if ($raw_bddr[$k] <= 0 && $k > 0);
			if ($k == 1) {
			    if ($l[0] == 1) {
				$NUMBINS = 1000;
			    }
			}
		    }
		} elsif ($line =~ m/SCORE.*AFPR.*ADR.*savings/o) {
		    # nawkRESEARCH RES files will match here
		    for ($k=0; $k<$NUMBINS+1; $k++) {
			$in = <IN>;
			chop $in;
			@l = split " ", $in;
			$thresh[$k]       = $l[0]; # * score_fact + th
			$afpr[$k]         = $l[1];
			$adr[$k]          = $l[2] * 100.; # * ENVIRON["FAC"]
			$raw_rtddr[$k]    = $l[3];
			$raw_olddr[$k]    = $l[4];
			$raw_bddr[$k]     = $l[5];
			if ($k == 1) {
			    if ($l[0] == 1) {
				$NUMBINS = 1000;
			    }
			}
		    }
		} elsif ($line =~ m/Threshold.*False Assigns.*Valid Assigns.*TFPR/o) {
		    # falconer4 research files Transaction level table will match here
		    $hdr = <IN>;  # header line
		    for ($k=0; $k<$NUMBINS+1; $k++) {
			$in = <IN>;
			chop $in;
			@l = split " ", $in;
			$l[1] =~ s/%//o;
			$l[2] =~ s/%//o;
			$legitScoreDist[$k] = $l[1];
			$fraudScoreDist[$k] = $l[2];
			$tfpr[$k]           = $l[3];
			$totalScoreDist[$k] = ($fraudScoreDist[$k] + $tfpr[0] * $legitScoreDist[$k]) / ($tfpr[0] + 1);
			if ($k == 1) {
			    if ($l[0] == 1) {
				$NUMBINS = 1000;
			    }
			}

		    }
		}
	    }
	    for ($j=0; $j<$NUMBINS+1; $j++) {
		printf DATA " %4d",   $thresh[$j];
		printf DATA " %9.5f", $afpr[$j];
		printf DATA " %9.5f", $adr[$j];
		printf DATA " %9.5f", $raw_rtddr[$j];
		printf DATA " %9.5f", $raw_rtddr[$NUMBINS] - $raw_rtddr[$j];
		if($raw_rtddr[$NUMBINS] > 0) {
		  printf DATA " %9.5f", 100.*($raw_rtddr[$NUMBINS] - $raw_rtddr[$j])/$raw_rtddr[$NUMBINS];
		} else {
		  printf DATA " %9.5f", 0.0;		    
		}
		if($raw_olddr[$NUMBINS] > 0) {
		  printf DATA " %9.5f", 100.*($raw_olddr[$NUMBINS] - $raw_olddr[$j])/$raw_olddr[$NUMBINS];
		} else {
		  printf DATA " %9.5f", 0.0;		    		    
		}
		if($raw_bddr[$NUMBINS] > 0) {
		  printf DATA " %9.5f", 100.*($raw_bddr[$NUMBINS]  - $raw_bddr[$j]) /$raw_bddr[100];
		} else {
		  printf DATA " %9.5f", 0.0;		    
		}
		printf DATA " %9.5f", ($afpr[$j] + 1 ) * $adr[$j]/100.;
		printf DATA " %9.5f", $legitScoreDist[$j];
		printf DATA " %9.5f", $fraudScoreDist[$j];
		printf DATA " %9.5f", $totalScoreDist[$j];
		if ($tfpr[$j] < 100000000) {
		    printf DATA " %9.5f", $tfpr[$j];
		} else {
		    printf DATA " .        ";
		}
		if ($adr[$j] > 0) {
		    printf DATA " %9.5f", 100./(1+ $afpr[$j]);
		} else {
		    printf DATA " .        ";
		}
		if ($afpr[0] > 0) {
		    printf DATA " %9.5f", $adr[$j]*$afpr[$j]/$afpr[0];
		} else {
		    printf DATA " .        ";
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
             [-v] [-q] [-Q] [-P] [-p] [-c] \\
             [-s] [-ur] [-br] [-xr <range>] \\
             [-all] [-imp] \\
             [-d] \\
             [-yr <range> ] <file-1> [ <file-2> [...] ]

$rcsID

 The default curve is \"$LABEL{$GRAPH[0]{yaxis}}\" vs. \"$LABEL{$GRAPH[0]{xaxis}}\".

 Output is:
   (a) to the screen (always)
   (b) to Postscript/EPS file (user queried)
   (c) to CGM (MS-Word readable) file (user queried)
   (d) to bitmap PNG (option only)
   (e) to SVG (option only)

 Postscript curves from different runs should be of the same scale ---
 hold them up to the light and see the curves on the same scales.

 Author: pcd 6/04/01.  

Enhanced, Matt Kennel 2008:
   EPS, PNG, SVG output. Better quality CGM. 
   -fsize to change fontsize (for PS/CGM based outputs)
   -fname to specify output graphics image filename
   -nts to eliminate timestamp
   -legend to specify legend text manually
   Able to process output from big2res_size1bin for better resolution.

 -v          Verbose.  Annoy the user as much as possible.
 -q          Quiet.  Do not ask user anything. (default)
 -Q          Quiet!  Don't even display the graph!
 -p          Automatically put out Postscript.
 -P          Automatically put out color Postscript.
 -E          Automatically put out (color) EPS.
 -c          Automatically put out CGM (insert into Word/PPT)
 -fsize <n>  Fontsize for PS/EPS/CGM (default=14)
 -svg        Automatically put out SVG 
 -png        Automatically put out PNG (640x480)
 -fname <nm> Send output to file nm.[ps|cgm|png etc] instead of default. 
 -np         Do not put individual points on the graph
 -nl         Do not put lines on the graph
 -s          Autoscale both axes
 -ur         Legend in the upper right
 -br         Legend in the bottom right
 -d          Leave the temp data file in place
 -xr <range> Use <range> as the x-axis range
 -yr <range> Use <range> as the y-axis range
             <range> = [<lo>:<hi>], and needs to be quoted in most shells
 -title <t>  Use <t> as the graph title (quote as needed for your shell).
 -nts        No time stamp on lower left
 -legend     'legend1,legend2,...,legendN'
              use those strings as key for graph legend instead of filenames.\n";

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
    #$GNUPLOT = ">plot_res.gnuplot";
    print STDERR "Using gnuplot '$GNUPLOT'\n";
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
    print STDERR "set data style $STYLE\n";
    print GNUPLOT "set xrange $XR\n";
    print STDERR "set xrange $XR\n";
    print GNUPLOT "set yrange $YR\n";
    print STDERR "set yrange $YR\n";
    print GNUPLOT "set xlabel '$XLABEL'\n";
    print STDERR "set xlabel '$XLABEL'\n";
    print GNUPLOT "set ylabel '$YLABEL'\n";
    print STDERR "set ylabel '$YLABEL'\n";
    print GNUPLOT "set title \"$TITLE\"\n";
    print STDERR "set title \"$TITLE\"\n";
    print GNUPLOT "set timestamp\n" unless $NOTIMESTAMP; 
    print GNUPLOT "set key $KEY\n";
    print STDERR "set key $KEY\n";
    if ($QUIET > 1) {
        print GNUPLOT "set term dumb\n";
        print GNUPLOT "set out '/dev/null'\n";
    }
    print GNUPLOT "plot \\\n";
    $NUM=0;
    $FIRST=1;
    foreach $i (0..$#FILES) {
        if ($FIRST) {
            $FIRST=0;
        } else {
            print GNUPLOT ", \\\n";
        }
	my $legendstr = $FILES[$i]; # default;
	if (defined($LEGEND[$i])) {
	    $legendstr = $LEGEND[$i];
	}
        print GNUPLOT
            "'$datafname' index $NUM:$NUM using $XCOL:$YCOL title '$legendstr'";
         print STDERR
            "'$datafname' index $NUM:$NUM using $XCOL:$YCOL title '$legendstr'\n";
        $NUM++;
    }
    print GNUPLOT "\n";



########################################

    $C = sprintf "%3.3d", $CHOICE + 1;


#
# set the base name (before extension) of output.
# 
    if (length($FNAMEARG) > 0) {
	$FNAMEBASE = $FNAMEARG; 
    } else {
	$FNAMEBASE = "plot_RES.$C.$Y.$X.$$"; 
    }

    if (!$QUIET) {
        print "Do you desire a postscript output? [n] : ";
        $ANSWER_PS = <STDIN>;
    }
    if ( uc($ANSWER_PS)  =~ /^Y/ ) {
	print GNUPLOT "set terminal postscript ${FSIZE}\n";
	$FNAME = $FNAMEBASE.".ps"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "\n";
	print "Postscript output to $FNAME\n";
	print "\n";
    }

    if (!$QUIET) {
	print "Do you desire a color postscript output? [n] : ";
	$ANSWER_PS_COLOR = <STDIN>;
    }
    if ( uc($ANSWER_PS_COLOR) =~ /^Y/ ) {
	print GNUPLOT "set terminal postscript color ${FSIZE}\n";
	$FNAME = $FNAMEBASE.".color.ps"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "\n";
	print "Postscript output to $FNAME\n";
	print "\n";
    }
    if (!$QUIET) {
	print "Do you desire a (color) EPS output? [n] : ";
	$ANSWER_EPS = <STDIN>;
    }
    if ( uc($ANSWER_EPS)  =~ /^Y/ ) {
	print GNUPLOT "set terminal postscript eps color ${FSIZE}\n";
	$FNAME = $FNAMEBASE.".eps"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "\n";
	print "EPS (color) output to $FNAME\n";
	print "\n";
    }
    if ( $SVG eq "y") {
	print GNUPLOT "set terminal svg dynamic\n";
	$FNAME = $FNAMEBASE.".svg"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "\n";
	print "SVG output to $FNAME\n";
	print "\n";
    }
    if ( $PNG eq "y") {
	print GNUPLOT "set terminal png small picsize 640 480 color\n";
	$FNAME = $FNAMEBASE.".png"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "\n";
	print "PNG output to $FNAME\n";
	print "\n";
    }


########################################

    if (!$QUIET) {
	print "Do you desire a cgm (MS-Word readable) output? [n] : ";
	$ANSWER_CGM = <STDIN>;
    }
    if ( $ANSWER_CGM =~ /^y/ || $ANSWER_CGM =~ /^Y/ ) {
	# print GNUPLOT "set terminal cgm\n";
	print GNUPLOT "set key $KEY Left reverse width 3.0 spacing 1.5\n";
	print GNUPLOT "set terminal cgm width 640 'Helvetica' ${FSIZE}\n";
	$FNAME = $FNAMEBASE.".cgm"; 
	print GNUPLOT "set out '$FNAME'\n";
	print GNUPLOT "replot\n";

	print "";
	print "CGM output to $FNAME\n";
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
