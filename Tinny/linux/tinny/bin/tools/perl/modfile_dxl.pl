#! /usr/local/bin/perl

# Usage:
# 	modfile <basename> <min_node> <max_node> <step> 
#           <train_epoch> <seed> <file_name>
#
# In *.mod file:
#	change Minimum hidden node
#	change Maximum hidden node
#	change step for ANN
# 	substitude basename
#
# The modified .mod file is intended for psnap training.
#----------------------------------------------------

    $basename = shift(@ARGV);
    $min_node = shift(@ARGV);
    $max_node = shift(@ARGV);
    $step = shift(@ARGV);
    $train_epoch = shift(@ARGV);
	$seed = shift(@ARGV);
    die "Variables not found" unless \
    	$basename && $min_node && $max_node && $step && $train_epoch && $seed;

    while (<>) {
	if (/BASENAME/) {
	   $_ =~ s/BASENAME/$basename/;
	   print $_;
	}
	elsif (/training epochs/) {
	    @array = split(/:/, $_);
	    print @array[0], ":", $train_epoch, "\n";
	    print STDERR "\tNumber of training epochs : $train_epoch\n";
	}
	elsif (/Minimum number of hidden nodes/) {
	    @array = split(/:/, $_);
	    print @array[0], ":", $min_node, "\n";
	    print STDERR "\tMinimum number of hidden node : $min_node\n";
	}
	elsif (/Maximum number of hidden nodes/) {
	    @array = split(/:/, $_);
	    print @array[0], ":", $max_node, "\n";
	    print STDERR "\tMaximum number of hidden node : $max_node\n";
	}
	elsif (/Step size for ANN/) {
	    @array = split(/:/, $_);
	    print @array[0], ":", $step, "\n";
	    print STDERR "\tStep size : $step\n";
	}
	elsif (/random seed/) {
		@array = split(/:/, $_);
		print @array[0], ":", $seed, "\n";
		print STDERR "\tRandom seed : $seed\n";
	}
	else {
	    print $_;
	}
    }
