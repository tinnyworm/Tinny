#!/usr/local/bin/perl -w

use strict;
use warnings;
use Getopt::Long qw(:config no_ignore_case pass_through)
  ;    # pass_through to allow processing of -1 -2 -3 -4 type of options
use Perf;
use PlotUtils;
use PlotUtils::Res;
use PlotUtils::Tran;
use FileUtils;

my $MAX_LEGEND_LENGTH = 50;
my $DEFAULT_PLOTS_STR = "1";

my $PLOT_NUM_STR = "Use -plot <list of plot nums>, 
where plot num can be one of the following for Research files:\n";
$PLOT_NUM_STR = $PLOT_NUM_STR . getResPlotNumStr() . "\n\n";
$PLOT_NUM_STR = $PLOT_NUM_STR . "for Tran files use:\n";
$PLOT_NUM_STR = $PLOT_NUM_STR . getTranPlotNumStr() . "\n";

my $usage = "FICO Fraud Analytics Tools version 1.2.1\n
Usage: $0    [ -r ] [ -t] [ -plot [1-N] ]
             [ -q ] 
             [ -np ] [ -nl ] 
             [ -fname ] [ -p ] [ -P ] [ -c ] [ -E ] [ -png ] [ -svg ] [ -fsize ]
             [ -s ] [ -ur ] [ -br ] [ -xr <range> ] [ -yr <range> ] 
             [ -title <title> ] [ -nts ] [ -legend <legends> ]
             [ -k ] 
              <file-1> [ <file-2> [...] ]

Options:
   -h        :  Display this message

   -r         :  files are research performance files. Default
   -t         :  files are tran performance files

   -plot <plotNums> :  Comma separated list of plots to output, 
                       see listing below to find out what plot numbers mean.
             The older syntax of individual options for a given number,
             e.g. $0 -1 -2 -23 is still supported.

   -q        :  Quiet.  Do not even display the graph.

   -np       :  Do not put individual points on the graph
   -nl       :  Do not put lines on the graph
   
   -fname <nm>: Send output to file nm.[ps|cgm|png etc] instead of default. 
   -p        :  Automatically put out Postscript.
   -P        :  Automatically put out color Postscript.
   -E        :  Automatically put out (color) EPS.
   -c        :  Automatically put out CGM (insert into Word/PPT)
   -fsize <n>:  Fontsize for PS/EPS/CGM (default=14)
   -svg      :  Automatically put out SVG 
   -png      :  Automatically put out PNG (640x480)
   
   -s        :  Autoscale both axes
   -ur       :  Legend in the upper right
   -br       :  Legend in the bottom right
   -xr <range>:  Use <range> as the x-axis range
   -yr <range>:  Use <range> as the y-axis range
               <range> = [<lo>:<hi>], and needs to be quoted in most shells
   -title <t> :  Use <t> as the graph title (quote as needed for your shell).
   -nts       :  No time stamp on lower left
   -legend    :  'legend1,legend2,...,legendN'
                 use those strings as key for graph legend instead of filenames.
   -k        :  Do not delete the temporary data file.  Also copies the data to ./

 Output is:
   (a) to the screen (unless -q flag is specified)
   (b) to Postscript/EPS file (if user specified )
   (c) to CGM (MS-Word readable) file (if user specified)
   (d) to bitmap PNG (if user specified)
   (e) to SVG (if user specified)

 Postscript curves from different runs should be of the same scale ---
 hold them up to the light and see the curves on the same scales.

Note that plotPerf.pl uses gnuplot under the covers and that it will look
for GNUPLOT_EXEC env var and if it is not present then it will use whatever
gnuplot is first in your path.
" . $PLOT_NUM_STR;

our $help;
our $plotNumStr;
our $quiet           = 0;
our $postscript      = 0;
our $colorPostscript = 0;
our $colorEPS        = 0;
our $cgm             = 0;
our $fsize           = 14;
our $svg             = 0;
our $png             = 0;
our $outFileName;
our $noPoints = 0;
our $noLines  = 0;
our $autoScale;
our $upRightLegend;
our $botRightLegend;
our $xRange;
our $yRange;
our $keepData    = 0;
our $title       = " ";
our $noTimeStamp = 0;
our $legendStr;
our $resFiles  = 1;
our $tranFiles = 0;
my @opt = (
    "h"        => \$help,
    "q"        => \$quiet,
    "plot=s"   => \$plotNumStr,
    "p"        => \$postscript,
    "P"        => \$colorPostscript,
    "E"        => \$colorEPS,
    "c"        => \$cgm,
    "fsize=i"  => \$fsize,
    "svg"      => \$svg,
    "png"      => \$png,
    "fname=s"  => \$outFileName,
    "np"       => \$noPoints,
    "nl"       => \$noLines,
    "s"        => \$autoScale,
    "ur"       => \$upRightLegend,
    "br"       => \$botRightLegend,
    "xr=s"     => \$xRange,
    "yr=s"     => \$yRange,
    "k"        => \$keepData,
    "title=s"  => \$title,
    "nts"      => \$noTimeStamp,
    "legend=s" => \$legendStr,
    "r"        => \$resFiles,
    "t"        => \$tranFiles
);

die $usage unless GetOptions(@opt);
die $usage if ($help);

$resFiles = 0 if ($tranFiles);

my $keyLoc;
if ($upRightLegend) {
    $keyLoc = "top right";
} elsif ($botRightLegend) {
    $keyLoc = "bottom right";
}

my @legends;
if ($legendStr) {
    @legends = split ",", $legendStr;
}

my @plots;
@plots = split ",", $plotNumStr if ($plotNumStr);

my @inFiles = ();
my $cnt     = 0;
my $tmpFile;
while ( defined( $tmpFile = shift(@ARGV) ) ) {
    if ( $tmpFile =~ m/^-([1-9][0-9]?)$/ ) {
        #  handle old-school,cis` plot_RES -1 -2 -3 type arguments
        push( @plots, $1 );
    } else {

        # set up legends if not defined on command line
        # also truncate them to max legend length
        my $noLegendDefined = !defined($legends[$cnt]);
        $legends[$cnt] = $tmpFile if ( $noLegendDefined );
        if ( $noLegendDefined && length( $legends[$cnt] ) > $MAX_LEGEND_LENGTH ) {
            $legends[$cnt] =
              "..." . substr( $legends[$cnt], -( $MAX_LEGEND_LENGTH + 3 ) );
        }

        my $baseName = FileUtils::getFileName($tmpFile);
        my $dataFile = genTempFile($baseName);
        
        if ($resFiles) {
            my $plotData = genResPerfData($tmpFile);
            printResPerfData( $dataFile, $plotData );
        } else {
            #handle fpMetrics/Tran perf files which are fortunately much easier to handle!
            die
"plotPerf.pl ERROR: This looks like a research file but you specified -t (tran file)"
              if isResFile($tmpFile);
            `cp $tmpFile $dataFile`;
        }
        push( @inFiles, $dataFile );
        $cnt++;
        $tmpFile = undef;
    }
}
die "There must be at least one input res file!\n$usage"
  if ( scalar(@inFiles) == 0 );

@plots = split ",", $DEFAULT_PLOTS_STR if(scalar(@plots) == 0);

my $noOutFile = !$outFileName;
foreach my $plotNum (@plots) {
    my ($plot, $xIndex, $yIndex);
    if ($resFiles) {
        ($plot,$xIndex,$yIndex) = createBaseResPlot($plotNum);        
    } elsif ($tranFiles) {
        ($plot,$xIndex,$yIndex) = createBaseTranPlot($plotNum);
    } else {
        die
"Should never happen that we don't have either research or tran flags specified!";
    }
    #print STDERR "Found plotNum $plotNum with xindex = $xIndex and yindex = $yIndex\n";
    $outFileName = "plotPerf.${xIndex}.${yIndex}.$$" if $noOutFile;
    $plot->setOutFileName($outFileName);

    $plot->setXRange($xRange) if $xRange;
    $plot->setYRange($yRange) if $yRange;
    $plot->setAutoScale($autoScale) if $autoScale;
    $plot->setKeyLocation($keyLoc) if $keyLoc;
    $plot->setQuiet() if $quiet;
    
    $plot->setNoLines() if $noLines;
    $plot->setNoPoints() if $noPoints;
    
    $plot->setTitle($title) if $title;
    $plot->setTimestamp(!$noTimeStamp);
    $plot->setFontSize($fsize);
    $plot->setCgm() if $cgm;
    $plot->setPs() if $postscript;
    $plot->setCps() if $colorPostscript;
    $plot->setEps() if $colorEPS;
    $plot->setPng() if $png;
    $plot->setSvg() if $svg;
    
    my @yIndices = ($yIndex);
    $plot->addFiles(\@inFiles, $xIndex, \@yIndices, \@legends);
    $plot->plot();
}

if ( !$keepData ) {
    unlink @inFiles;
} else {
    my $files = join " ", @inFiles;
    `mv $files .`;
}

sub isResFile {
    my ($file) = @_;
    open FILE, "<$file";

    my $found1 = 0;
    my $found2 = 0;
    my $found3 = 0;
    while (<FILE>) {
        chomp;
        last if $found3;

        $found1 = 1
          if (/^Created by '[\w\.\/\-]+' using the following parameters/);
        $found2 = 1 if (/^TRANSACTION-BASED INFORMATION:/);
        $found3 = 1 if (/^ACCOUNT BASED INFORMATION /);
    }
    close FILE;
    return $found1 && $found2 && $found3;
}


sub genTempFile {
    my ($baseName) = @_; 
    my $tmpFile = "/tmp/plotData_${baseName}_$$.tmp";
    my $cnt = 1;
    while(-e $tmpFile) {
        $tmpFile = "/tmp/plotData_${baseName}_${cnt}_$$.tmp"; 
        $cnt++; 
    }
    return $tmpFile;
}
