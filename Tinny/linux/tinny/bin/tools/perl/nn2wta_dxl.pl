#! /usr/bin/env perl

#
# Input:
#        nnet model file
#
# Output:
#        Falcon .wta file
#        Falcon .cta file
#
# XXS: last updated Jan, 2000
# (now recognizes C-float in scientific notation)
#

    $usage = "\nnn2wta.pl <nnet_model_file> <falcon_basename>\n" .
             "\n    Input:  <nnet_model_file>\n" .
	     "\n    Output: <falcon_basename>.wta" .
	     "\n            <falcon_basename>.cta\n\n";
    $err_nnet = "Error reading NNet information.\n";

    $modelfile = shift(@ARGV);
    $basename = shift(@ARGV);
    die $usage unless $modelfile && $basename;
	 
	 # following piece of code is used to untaint the input name
	 # the issue is only raised due to use gmake, not so clear to me why for
	 # now.
	 if ($basename =~ /^([-\@\w.]+)$/) {
			$basename = $1; 			# $data now untainted
	 } else {
			die "Bad name in $basename"; 	# log this somewhere
	 }  

    open(MODEL, "< $modelfile")
	or die "Can't open $modelfile for reading: $!\n";

    open(WTA, "> $basename.wta")
	or die "Can't open $basename.wta for writing: $!\n";
	
    while (<MODEL>) {
	last if (/^set NNetSlab/);
	if (/^set NNetLayers.*\"(\d+)x(\d+)x(\d+)\"/) {
	    $input=$1;
	    $hidden=$2;
	    $output=$3;
	}
    }
    die $err_nnet if ($output != 1);

    &makeCTA("$basename.cta", $input, $hidden);
    print "Falcon CTA file: $basename.cta generated\n";

	# print wta file header
    $num1 = ($input+1)*$hidden;
    $num2 = $hidden+1;
    print WTA "wts\t\tmbpn\n2\n\n2\n0\n$num1\n";

    while (<MODEL>) {
	last if (/weights/);
    }
    $i=0;
    while (<MODEL>) {
	last if (/^set/);
	while (/(([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?)/g) {
	    $weights[$i]=$1;
	    $i ++;
	}
	if ($i>0 && /}/) {
	    $i --;
	    print WTA " " if ($weights[$i]>=0.0);
	    print WTA "$weights[$i] ";
	    for ($j=0; $j<$i; $j++) {
		print WTA " " if ($weights[$j]>=0.0);
		print WTA "$weights[$j] ";
		print WTA "\n" if ((($j+2) % 5)==0 && $j>0);
	    }
	    print WTA "\n";
	    $i=0;
	}
    }

    print WTA "\n5\n0\n$num2\n";
    while (<MODEL>) {
	last if (/weights/);
    }
    $i=0;
    while (<MODEL>) {
	while (/(([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?)/g) {
	    $weights[$i]=$1;
	    $i ++;
	}
	if ($i>0 && /}/) {
	    $i --;
	    print WTA " " if ($weights[$i]>=0.0);
	    print WTA "$weights[$i] ";
	    for ($j=0; $j<$i; $j++) {
		print WTA " " if ($weights[$j]>=0.0);
		print WTA "$weights[$j] ";
		print WTA "\n" if ((($j+2) % 5)==0 && $j>0);
	    }
	    print WTA "\n";
	    $i=0;
	}
    }
    print WTA "\n";

    close(MODEL);
    close(WTA);
    print "Falcon WTA file: $basename.wta generated.\n";

#
# A subroutine the generates the CTA file
#
# syntax: &makeCTA($filename, $inputs, $nodes)
#
sub makeCTA
{
    local($filename, $Inputs, $Nodes) = @_;

    open(CTA, "> $filename") or die "Can't open $filename for writing: $!\n";

#    $, = ' ';		# set output field separator
#    $\ = "\n";		# set output record separator

    print CTA  "cts		mbpn\n";
    print CTA  "\n";
    print CTA  $Inputs . " \n";
    print CTA  "1 \n";
    print CTA  "1 \n";
    print CTA  $Nodes . " \n";
    print CTA  "1 \n";
    print CTA  "1 \n";
    print CTA  "119 \n";
    print CTA  " 4.9999999e-03 \n";
    print CTA  "2 \n";
    print CTA  "0 \n";
    print CTA  "0 \n";
    print CTA  " 1.0000000e+00 \n";
    print CTA  " 1.0000000e+00 \n";
    print CTA  "-1.0000000e+00 \n";
    print CTA  " 6.1315552e-39 \n";
    print CTA  " 1.9618179e-44 \n";
    print CTA  "2048 \n";
    print CTA  "-1.6000000e+01 \n";
    print CTA  " 1.6000000e+01 \n";
    print CTA  "\n";
    print CTA  "\n";
    print CTA  " 3.9999999e-04 \n";
    print CTA  " 6.1309274e-39 \n";
    print CTA  " 0.0000000e+00 \n";
    print CTA  " 9.9999997e-05 \n";
    print CTA  " 8.9999998e-01 \n";
    print CTA  "-1.5838515e+29 \n";
    print CTA  " 6.1124415e-39 \n";
    print CTA  " 8.9999998e-01 \n";
    print CTA  " 0.0000000e+00 \n";
    print CTA  "2 \n";
    print CTA  "1 \n";
    print CTA  "0 \n";
    print CTA  "0 \n";
    print CTA  "1 \n";
    print CTA  "1 \n";
    print CTA  "\n";

    close(CTA);
}
