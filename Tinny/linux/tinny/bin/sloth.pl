#!/usr/bin/env perl
use warnings;
use strict;
use Getopt::Long;

my $VERSION = 5;

my $OS = `uname`;
my $PROC = `uname -p`;

# Location of scale files (*.cfg, *.scl, *.tag)
my $dir;

# Model name stem for most files
my $stem;

# Choose a name for this model segment
my $modelId;

# Number of hidden nodes
my $nhidCsv;

# Desired non-fraud to fraud ratio. Not the actual ratio of the input.
my $nffCsv;

# Number of noise variables
my $noisevars = 0;

# Seed value for Nnet08
my $seed = 100;

my $runCap = 0; # Run capping flag
my $runNnet = 1; # Train neural network
my $runRrcd = 0; # Generate reason codes flag
my $runSens = 0; # Run sensitivity analysis flag
my $runGrl = 1; # Generate GRL for neural network

# Generate nets and sensitivity analysis files using specified wta type
# err, wta, lauc
my @wtaTypeA = qw(err AUC LAUC);

# Select which .wta file to use for reason code generation and capping
my $wtaSelect = "err";

my $help;

my $USAGE = "
Version $VERSION

$0 --dir --stem --modelId --nhid [options]

-h or --help            Print usage.
--dir <string>          Location of scale files (*.cfg, *.scl, *.tag).
                        If using current directory ( ./ ) then no copying or
                        linking will be done.
--stem <string>         Model name stem for most files.
--modelId <string>      Name for this model segment.
--nhid <string>         Number of hidden nodes. For multiple values use comma
                        delimited list with no whitespace.
--seed <int>            Seed value required by Nnet08.
                        Default: $seed
--cap <int>             Run capping (0=no, 1=yes).
                        Default: $runCap
--nnet <int>            Train neural network (0=no, 1=yes).
                        Default: $runNnet
--rrcd <int>            Generate reason codes (0=no, 1=yes).
                        Default: $runRrcd
--sens <int>            Run sensitivity analysis (0=no, 1=yes).
                        Default: $runSens
--grl <int>             Generate GRL for neural network (0=no, 1=yes).
                        Default: $runGrl
--wta <string>          Select which .wta file to use for reason code generation
                        and capping. Select from 'err', 'AUC' or 'LAUC'.
                        Default: $wtaSelect
--nff <string>          Set a desired effective NF:F ratio. For multiple
                        values use comma delimited list with no whitespace.
                        Default: Natural ratio
--noisevars <int>       Number of noise variables to add to configuration
                        file.
                        Default: $noisevars
";

die $USAGE unless GetOptions(
    "h" => \$help,
    "help" => \$help,
    "dir=s" => \$dir,
    "stem=s" => \$stem,
    "modelId=s" => \$modelId,
    "nhid=s" => \$nhidCsv,
    "seed=i" => \$seed,
    "cap=i" => \$runCap,
    "nnet=i" => \$runNnet,
    "rrcd=i" => \$runRrcd,
    "sens=i" => \$runSens,
    "grl=i" => \$runGrl,
    "wtaSelect=s" => \$wtaSelect,
    "nff=s" => \$nffCsv,
    "noisevars=i" => \$noisevars,
);

die $USAGE if (defined($help));
die "--dir required\n" unless (defined($dir));
die "--stem required\n" unless (defined($stem));
die "--modelId required\n" unless (defined($modelId));
die "--nhid required\n" unless (defined($nhidCsv));
$dir =~ s/\/+$//;

my @nhidA = split /,/, $nhidCsv;
my $nhid = $nhidA[0];
my @nffA = (defined($nffCsv)) ? split(/,/, $nffCsv) : ();
my $nff = ($#nffA>-1) ? $nffA[0] : undef;

print STDERR "
    Version = $VERSION
    dir = $dir
    stem = $stem
    modelId = $modelId
    nhid = $nhid
    seed = $seed
    cap = $runCap
    nnet = $runNnet
    rrcd = $runRrcd
    sens = $runSens
    grl = $runGrl
    wtaSelect = $wtaSelect
    nff = $nff
    noisevars = $noisevars

";

######################################################################
# Settings for NN training
######################################################################
my $TRN_OPT = 
"-train ".
"-bp ".
"-scl $stem.scl ".
"-tag $stem.tag ".
"-cfg $stem.cfg ".
"-wtaout $stem.wta ".
"-learning_log $stem.learning.log ".
"-nhid $nhid ".
"-noisevars $noisevars ".
"-alpha 0.002 ".
"-beta 0.1 ".
"-interlayer_gain 4.0 ".
"-lr_accel 1.02 ".
"-lr_decel 0.96 ".
"-bsize 10000 ".
"-nepochs 1000 ".
"-seed $seed ".
"-bound1 0.0";
$TRN_OPT .= " -nff $nff" if (defined($nff));

######################################################################
# Settings for NN recall
######################################################################
my $RCL_OPT =
"-recall ".
"-log $stem.log ".
"-scl $stem.scl ".
"-cfg $stem.cfg ".
"-wtain $stem.wta ".
"-nhid $nhid";

######################################################################
# Settings for sensitivity analysis
######################################################################
my $SENS_OPT =
"-sens ".
"-scl $stem.scl ".
"-cfg $stem.cfg ".
"-tag $stem.tag ".
"-nhid $nhid";

######################################################################
# Tools
######################################################################
#NNET = nnet08.${PROC}
my $NNET = 'nnet08';
my $REASBIN = 'reasbin4';
my $CAPCFG = 'capcfg5';

######################################################################
# Rules
######################################################################

# Link files
&myExec("if [ ! -f $stem.cfg ]; then cp $dir/$stem.cfg $stem.cfg; else echo \"Using $stem.cfg in current directory\"; fi");
die "Cannot find: $dir/$stem.scl\n" unless (-f "$dir/$stem.scl");
&myExec("if [ ! -f $stem.scl ]; then ln -s $dir/$stem.scl $stem.scl; else echo \"Using $stem.scl in current directory\"; fi");
die "Cannot file: $dir/$stem.tag\n" unless (-f "$dir/$stem.tag");
&myExec("if [ ! -f $stem.tag ]; then ln -s $dir/$stem.tag $stem.tag; else echo \"Using $stem.tag in current directory\"; fi");

# Train neural network
die "Training requires: $stem.cfg $stem.scl $stem.tag\n" unless (-f "$stem.cfg" and -f "$stem.scl" and -f "$stem.tag");
if ($runNnet) {
    &myExec("${NNET} ${TRN_OPT} > nnet08.log 2>&1");
}

# Recall and reason bins
if ($runRrcd) {
    die "Reason code generation requires: $stem.test_${wtaSelect}.wta\n" unless (-f "$stem.test_${wtaSelect}.wta");
    &myExec("ln -s $stem.test_${wtaSelect}.wta $stem.wta");
    &myExec("${NNET} ${RCL_OPT}");
    &myExec("${REASBIN} -m$stem -R");
}

# Cap
if ($runCap) {
    die "Capping requires: $stem.cfg\n" unless (-f "$stem.cfg");
    &myExec("cp $stem.cfg $stem.orig.cfg");
    &myExec("${CAPCFG} -i$stem.orig.cfg -o$stem.cfg -m3 -d6");
}

# Convert NNet files to GRL compliant nets file
if ($runGrl) {
    my $rrcd = '';
    if (-f "$stem.rrcd") {
        die "GRL generation requires: $stem.rrcd\n" unless (-f "$stem.rrcd");
        die "GRL generation requires: $stem.cfg\n" unless (-f "$stem.cfg");
        $rrcd = "--rrcd $stem.rrcd";
    }
    for my $type (@wtaTypeA) {
        die "GRL generation requires: $stem.test_$type.wta\n" unless (-f "$stem.test_$type.wta");
        my $cmd =
        "generateNetsGRL.pl ".
        "--model_id $modelId ".
        "--wta $stem.test_$type.wta ".
        "--cfg $stem.cfg ".
        $rrcd .
        "> ${modelId}_$type ".
        "2>  generateNetsGRL_$type.log ";
        &myExec($cmd);
    }
}

# Sensitivity analysis
if ($runSens) {
    for my $type (@wtaTypeA) {
        die "Sensitivity analysis requires: $stem.test_$type.wta\n" unless (-f "$stem.test_$type.wta");
        my $cmd =
        "${NNET} ${SENS_OPT} ".
        "-wtain $stem.test_$type.wta ".
        "-sens_output $stem.test_$type.sens ".
        "> sens_$type.log 2>&1";
        &myExec($cmd);
    }
}

sub myExec {
    my $ret;
    my $cmd = shift;

    # Format command
    $cmd =~ s/ -/ \\\n -/g;

    print $cmd,"\n";
    my $str = `$cmd`;
    print $str;

    $ret = $? >> 8;
    print "Return value: $? >> 8 = $ret\n\n";
    return $ret;
}
