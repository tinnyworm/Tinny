#! /usr/local/bin/perl
# ---------------------------------------------------------------------------
#
# Input:
#        Falcon style .cfg and .mod file
#
# Output:
#        nnet style .newcfg file for training
#
# Author: XXS, July, 1999
# ---------------------------------------------------------------------------
# REVISION HISTORY:
#
# $Id: cfg2nn.pl,v 1.20 2003/07/10 09:09:39 mxs Exp $
#
# $Log: cfg2nn.pl,v $
# Revision 1.20  2003/07/10 09:09:39  mxs
# Bug fix - 1.19 failed to set num_node from .mod file.
#
# Revision 1.19  2003/06/11 03:34:26  mxs
# Added more command line options (learn, gain, epochs)
# .mod file made optional
#
# Revision 1.18 2001/06/25  01:11:49  alex
# Changed FalconSclFile and FalconTagFile to DMWSclFile and DMWTagFile
#
# Revision 1.17  2001/01/25  20:02:11  jas
# Changed default for 'force_last_weights' to 'false'
#
# Revision 1.16  2001/01/25  19:59:53  jas
# No idea...
#
# Revision 1.15  2000/12/28  20:04:25  jas
# Added "--momentum" option
#
# Revision 1.14  2000/10/13  17:22:42  jas
# Added default backprop option: force_last_weights        true
#
# Revision 1.13  2000/10/13  17:20:48  jas
# Corrected spelling in backprop options: mimimum -> minimum
#
# Revision 1.12  2000/08/18  23:33:43  axs
# Changed it so that backtrack= false is the default.
# Added corresponding options.
#
# Revision 1.11  2000/08/04 20:49:42  xxs
# Add back the min-max-mean-stdev info to .nncfg file.
#
# Revision 1.10  2000/07/21  23:26:23  jas
# Fixed --version response
#
# Revision 1.9  2000/07/21  23:16:54  jas
# Added 2 options:
#          --batch, -b=<Batching Epoch>
#          --increase, -i=<allowable number of RMS increase epochs>
#
# Revision 1.8  2000/07/11  23:26:45  jas
# Added option "random" to set random seed during training.
#
# Revision 1.7  2000/06/01  00:01:03  axs
# Changed options syntax and added new options-processing code.
#
# Revision 1.6  2000/04/19 23:48:21  jas
# Correctly responds to --help and --version
# Reads min hidden node number from mod file, not slab[0]
# Command-line options for stepsize and cost function file
#
# Revision 1.5  2000/04/18 21:06:38  jas
# Set scalefn to "NONE"
# add line to backprop options: stepsize 1
#
# Revision 1.4  2000/01/10 20:09:09  xxs
# cfg2nn.pl  set randomseed
# nn2wta.pl recognize C-float scientific notation
#
# Revision 1.1  2000/01/06 21:59:56  xxs
# Initial revision
#
# Revision 1.3  1999/11/30 23:53:17  xxs
# fixed : FalconScaleFile (not FalconSclFile)
#
# Revision 1.2  1999/11/29 22:03:53  xxs
# make FalconSclFile FalconTagFile default.
#
# Revision 1.1  1999/11/29 22:02:25  xxs
# Initial revision
#
# ---------------------------------------------------------------------------
#
use Getopt::Long;

$sclfile = 1;

$version = "cfg2nn.pl version 1.10\n";

$ErrWrongFormat = "\nError parsing .cfg file\n\n";
$ErrWrongCount = "\nError counting vars in .cfg file\n\n";

# --------------------------------------------------------------------
# Get and process options:
# --------------------------------------------------------------------
$usage = 
  "\n"
  . "Usage:\t $0 [OPTIONS] <basename>\n\n" 
  . "\t --help, -h\n"
  . "\t --version, -v\n"
  . "\t --random, -r=<RandomSeed>\n"
  . "\t --nodes, -n=<numberofnodes>\n"
  . "\t --learn, -l=<learning rate (alpha)>\n"
  . "\t --acce, -a=<learning acceleration rate >\n"
  . "\t --dece, -d=<learning deceleration rate >\n"
  . "\t --momentum, -m=<momentum term (beta)>\n"
  . "\t --gain, -g=<interlayer gain> (Default: 4)\n"
  . "\t --epochs, -e=<maximum training epochs>\n"
  . "\t --stepsize, -s=<stepsize> (Default: 1)\n"
  . "\t --increase, -i=<allowable number of RMS increase epochs>\n"
  . "\t --batch, -bat=<Batching Epoch>\n"
  . "\t --cost, -c\n"
  . "\t --backtrack, -bac\n"
  . "\nCommand line options will quietly override .mod file (-e, -n, -l, -m, -g).\n"
  . "If no DMW .mod file, -e, -n, -l, -m are required.\n\n"
;

$Getopt::Long::autoabbrev= 1;
@optl= ("batch=i", "increase=i", "random=i", "stepsize=i", "cost", "nodes=i",
	"help", "version", "backtrack", "momentum=f", "learn=f", "gain=f", 
	"epochs=i","acce=f","dece=f");

die "\nUsage:  $0 $usage" unless (GetOptions @optl);
if($opt_version){
  die $version;
}
$basename = shift(@ARGV) or die $usage;
print "Basename: $basename\n";

# -------------------------------------------------------------------
# Read default parameters from .mod file, change it if specified by
# input options
# -------------------------------------------------------------------

# Parse .mod file information to get default parameters
if (open(MOD, "< $basename.mod")){
	while (<MOD>) {
		if (/^Minimum number of hidden nodes for ANN *: *(\d+)/) {
    		$num_node = $1;
		}
		elsif (/^random seed *: *(\d+)/) {
    	    $randomSeed = $1;
		}
		elsif (/^learning rate constant \(alpha\) *: *(-?\d+\.?\d*)/) {
    	    $learn_rate = $1;
		}
		elsif (/^smoothing constant \(beta\) *: *(-?\d+\.?\d*)/) {
    	    $smooth_beta = $1;
		}
		elsif (/^gain ratio *: *(-?\d+\.?\d*)/) {
    	    $gain_ratio = $1;
		}
		elsif (/^training epochs *: *(\d+)/) {
    	    $epochs = $1;
		}
  }
  print "Done parsing and converting the .mod file\n\n";
  close(MOD);
}else{
  warn "Can't open $basename.mod for reading, relying on command line options...\n";
}

# Parse command line options
$randomSeed = ($opt_random) ? $opt_random : $randomSeed;
print "RandomSeed: $randomSeed\n";

$num_node = ($opt_nodes) ? $opt_nodes : $num_node;
print "Number of Nodes: $num_node\n";

$learn_rate = ($opt_learn) ? $opt_learn : $learn_rate;
print "learn_rate:  $learn_rate\n";

$lr_acce = ($opt_acce) ? $opt_acce : 1.0;
print "lr_acceleration:  $lr_acce\n";

$lr_dece = ($opt_dece) ? $opt_dece : 0.952380997732;
print "lr_deceleration:  $lr_dece\n";

$RMSincrease = ($opt_increase) ? $opt_increase : 100;
print "RMSincrease: ", ($opt_increase) ? $opt_increase: "Not set. Default 100", "\n";

$stepsize = ($opt_stepsize) ? $opt_stepsize : 1 ;
print "Stepsize: $stepsize\n";

$costs = ($opt_cost)? "true":"false";
print "Costs: $costs\n";

$backtrack = ($opt_backtrack)? "true":"false";
print "Backtrack: $backtrack\n";

$gain_ratio = ($opt_gain) ? $opt_gain : $gain_ratio; 
print "Gain ratio: $gain_ratio\n";

$smooth_beta = ($opt_momentum) ? $opt_momentum : $smooth_beta;
print "momentum: $smooth_beta\n";

$epochs = ($opt_epochs) ? $opt_epochs : $epochs;
print "epochs: $epochs\n";

$batchEpoch = ($opt_batch) ? $opt_batch : -1;
print "batchEpoch: ", ($opt_batch) ? $opt_batch: "Not set. Defaults to number of epochs", "\n";

if($batchEpoch < 0){
  $batchEpoch = $epochs;
}

die "One or more required options not set, exiting...\n" unless (defined $num_node and defined $learn_rate and defined $smooth_beta and defined $epochs);

# ---------------------------------------------------------------------
# Open nncfg file to write
# ---------------------------------------------------------------------
if ($basename =~ /^([-\@\w.]+)$/) {
	$basename = $1; 			# $data now untainted
} else {
	die "Bad data in $basename"; 	# log this somewhere
}  

open(NEWCFG, "> $basename.nncfg")
    or die "Can't open $basename.newcfg for writing: $!\n";

print NEWCFG "#!\\bin\\false\n";
print NEWCFG "# This is auto-generated from $basename.cfg and".
    " $basename.mod\n\n";

# output neural networks information
if ($sclfile == 1) {
	print NEWCFG "set DMWScaleFile    true\n";
	print NEWCFG "set DMWTagFile      true\n";
    }
if ($randomSeed >= 0) {
    print NEWCFG "set RandomSeed      $randomSeed\n";
}
    print NEWCFG "set MissingDataChar\t\".\"\n";
    print NEWCFG "set NumberOfLayers  2\n";
    print NEWCFG "set TrainModeName   \"backprop\"\n";
    print NEWCFG "set use_costs       $costs\n\n";

# output NNet model parameter
    print NEWCFG "set NNetModelParams\t{\n";
    print NEWCFG "    momentum                  $smooth_beta\n";
    print NEWCFG "    learning_rate             $learn_rate\n";
    print NEWCFG "    lr_acceleration           $lr_acce\n";
    print NEWCFG "    lr_deceleration           $lr_dece\n";
    print NEWCFG "    interlayer_gain           $gain_ratio\n";
    print NEWCFG "    retry_on_local_minimum    false\n";
    print NEWCFG "    sample_with_replacement   false\n";
    print NEWCFG "    backtrack                 $backtrack\n";
    print NEWCFG "    smoothing                 true\n";
    print NEWCFG "    epochs                    $epochs\n";
    print NEWCFG "    stepsize                  $stepsize\n";
    print NEWCFG "    use_costs                 $costs\n";
    print NEWCFG "    batching_epoch            $batchEpoch\n";
    print NEWCFG "    rms_epoch_increase        $RMSincrease\n";
    print NEWCFG "    force_last_weights        false\n";
    print NEWCFG "}\n\n\n";

# output hidden layer description
    print NEWCFG "set NNetSlab(0) {\n";
    print NEWCFG "    function    tanh\n";
    print NEWCFG "}\n\n";
    print NEWCFG "set NNetSlab(1) {\n";
    print NEWCFG "    function    tanh\n";
    print NEWCFG "}\n\n";

open(CFG, "< $basename.cfg")
    or die "Can't open $basename.cfg for reading: $!\n";
# skip the first 2 lines
$dummy = <CFG>;
$dummy = <CFG>;

$varcnt=0;
$numVarActive=0;
$start=0;
$length=3;
$length=4 if ($sclfile==1);
while (<CFG>) {
	# Parse all information about a variable here
	chop;
	$_ =~ m/([-\d]*)/;
	$varnum = $1;
	last if ($varnum == -1);
	print $ErrWrongCount if ($varnum != $varcnt);
	
	($line = <CFG>) =~ m/^Name= (\w*)/;
	$name = $1;
	
	($line = <CFG>) =~ m/^Type= (\w*)/;
	$type = $1;
	
	($line = <CFG>) =~ m/^Slab= (\w*)/;
	$slab = $1;
	$type = "exclude" if ($slab eq "NONE");
	$numVarActive = $numVarActive - 1 if ($slab eq "NONE");
	
	$line = <CFG>;		# Size=...
	$line = <CFG>;		# Start=... Length=...
	$line = <CFG>;		# RecCnt=...
	($line = <CFG>) =~ m/^Min= *([\d+-e]+) *Max= *([\d\+-e]+)/;
	$min = $1; $max = $2;
	($line = <CFG>) =~ m/^MissingValue= ([\d\+-e]+)/;
	$default = $1;
	$line = <CFG>;		# Sum=...
	($line = <CFG>) =~ m/^Mean= ([\d\+-e]+)/;
	$mean = $1;
	($line = <CFG>) =~ m/^StdDev= ([\d\+-e]+)/;
	$stddev = $1;
	$line = <CFG>;		# Derivative=...
	$line = <CFG>;		# TimeSlice=...
	$line = <CFG>;		# NbrOfSymbols=...
	($line = <CFG>) =~ m/^Symbolic= (\w*)/;
	$numeric = ($1 eq "NUMERIC") ? "true" : "false";
	($line = <CFG>) =~ m/^ScaleMode= (\w*) *ScaleFn= (\w*)/;
	$scalefn = $2;
	$line = <CFG>;		# Divisor=... Range=...
	$line = <CFG>;		# empty line
	
	# print "$name && $type && $slab && $numeric && $scalefn\n";
	die $ErrWrongFormat unless $name && $type && $slab && $numeric && $scalefn;

	if ( ($slab eq "INPUT") && abs($mean-0.0)<1e-6 && abs($stddev-1.0)<1e-6 ) {
		 # binary variable
		 $type = 'BINARY';
#		 $scalefn = 'BINARY';
	} 
	 # XXS: set scalefn default to NONE
	 $scalefn = 'NONE';
	
	# output to newcfg
	print NEWCFG "set Field($varnum) {\n";
	print NEWCFG "    name             $name\n";
	print NEWCFG "    type             $type\n";
	print NEWCFG "    slab             $slab\n";
	print NEWCFG "    start            $start\n";
	print NEWCFG "    length           $length\n";
	print NEWCFG "    numeric          $numeric\n";
	print NEWCFG "    scalefn          $scalefn\n";
	print NEWCFG "    default_value    $default\n";
	print NEWCFG "    mean             $mean\n";
	print NEWCFG "    stdev            $stddev\n";
	print NEWCFG "    min              $min\n";
	print NEWCFG "    max              $max\n";
	print NEWCFG "}\n\n";
	
	if ($sclfile==1) {
	    $start+=4;
	}
	else {
	    if ($slab eq "TARGET") {
		$start+=4;
	    } else {
		$start+=13;
    	}
    	$length=12;
	}
	$varcnt++;
	$numVarActive++;
} # while(<CFG>)

print $ErrWrongCount if ($varnum!=-1);
close(CFG);

print "Done parsing and converting the .cfg file\n\n";

print NEWCFG "set NumberOfFields\t$varcnt\n";
$numVarActive--;
print NEWCFG "set NNetLayers      \"$numVarActive"."x"."$num_node"."x1\"\n";

close(NEWCFG);

# End of file: fal2nn.pl -------------------------------------------------
