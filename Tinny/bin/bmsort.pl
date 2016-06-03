#!/usr/bin/env perl

use warnings; 
use strict;
use Getopt::Long;

#
# Variables which will control the construction of the
# sort command
#
my $sort_memory_megs=2000;
my $sort_order=''; # 0 - default, 1 means descending. 
my @tempdirsdefault=("."); 
# Oct 23-09: "/tmp" removed from default list. 
my @tempdirs;
my $delimiter = "";
my $compress_intermediate='';
my $is_sort = "";

my $gzip_path = "/usr/bin/gzip";
my $sort_path; 
my $silent;
my $unique_keys;

my $macharch = `uname -p`; 
chomp($macharch);   # return value of `` can include stupid \n
if ($macharch eq "x86_64") {
    $sort_path = "/apps/usr/local64/bin/sort";
} elsif ($macharch eq "i686") {
    $sort_path = "/apps/usr/local/bin/sort"; 
} elsif ($macharch eq "sparc") {
    $sort_path = "/apps/usr/local64/bin/sort";
}

if (!(-x $sort_path)) {
    $sort_path = "/bin/sort"; 
}



#@ARGV_original = @ARGV;
@ARGV = cook_arguments(@ARGV); 

#print "Original args: ".join(" ",@ARGV_original)."\n";
#print "Cooked   args: ".join(" ",@ARGV)."\n";

my @keyfields;
my $dry_run;
#
# first check for obsolete input options
#

Getopt::Long::Configure("no_ignore_case");
my $ret = GetOptions("i" => \&Obsolete_option,
		     "o" => \&Obsolete_option,
		     "x" => \&Obsolete_option,
		     "M" => \&Obsolete_option,
		     "key=s" => \@keyfields,
		     "D" => \$sort_order,
		     "d=s" => \$delimiter,
		     "compress-intermediate" => \$compress_intermediate,
		     "gzip-path=s" => \$gzip_path,
		     "sort-path=s"=> \$sort_path,
                     "is-sort" => \$is_sort,
		     "m=i" => \$sort_memory_megs,
		     "t|T=s" => \@tempdirs,
		     "dry-run"=> \$dry_run,
		     "S" => \$silent,
		     "x" => \$unique_keys,
		     "h" => \&Help    );

if (!$ret) {
    die "$0: error: invalid option used.  See -h for help\n"
}

#
# Now check features regarding sort.
# 
# First, is it GNU sort?
#
my $cmdout=`${sort_path} --version 2>&1`; 
my $gnu_sort = 0;
$gnu_sort = 1 if ($cmdout =~ m/coreutils/);

#
# if it's gnu sort, check for modern enough
# version to support --compress-prog
#   
my $compress_program=0;
if ($gnu_sort) {
    $cmdout = `${sort_path} --compress-program=/bin/cat </dev/null 2>&1`;
    chomp($cmdout);
    $compress_program = 1 if (length($cmdout) == 0);
}
#print STDERR "GNU sort on=$gnu_sort, has compress=$compress_program\n"; 

if (($compress_program==0) && ($compress_intermediate)) {
    print STDERR "Warning! --compress-intermediate asked for, but didn't find GNU sort which supports it, so it's turned off\n" ; 
    $compress_intermediate=0; 
}


@tempdirs = @tempdirsdefault if (!@tempdirs) ; 

#&Dump_parameters;

my $cmd = $sort_path;
$cmd .= " -s" if $gnu_sort;  # add -s for stable sort with GNU sort

$cmd .=" -r" if ($sort_order);  # reverse
if (!($is_sort)) {
    foreach my $dir (@tempdirs) {
	$cmd .= " -T ".$dir; 
    }
    $cmd .= " -S ".$sort_memory_megs."M";
}
foreach my $key (@keyfields) {
    $cmd .= " ".&key_to_sort_option($key); 
}
if (!($is_sort)) {
    if ($compress_intermediate)  {
	$cmd .= " --compress-program=".$gzip_path;
    }
    if ($unique_keys) {
	$cmd .=" -u"; 
    }
}
else {
    $cmd .=" -c";
}

$cmd .= " -";
unless ($silent) {
    print STDERR "Sort command to be executed:\n".$cmd."\n"; 
}
if (!$dry_run) {
    $ENV{'LC_ALL'} = 'C'; 
    if ($is_sort) {
	my $sorted =`$cmd 2>&1`;
	if ($sorted =~ /-:(\d+):/) {
	    print "\nUnsorted record at line $1\n\n";
	}
	else {
	    print "\nFile is sorted\n\n";
	}
	exit(0);
    }
    else {
	exec($cmd); 
    }
}
die "$0: exec() of underlying sort failed\n";
exit(0);

sub Dump_parameters {
    print "gzip-path: $gzip_path\n";
    print "sort_path: $sort_path\n";
    print "temp dirs: ".join(":",@tempdirs)."\n";
    print "memory megs: $sort_memory_megs\n";
    if ($delimiter) {
	print "delimiter : \"$delimiter\" \n";
    }
    print "sort order: ".(($sort_order)?"descending":"ascending")."\n";
    print "keys: ".join(" ",@keyfields)."\n"

}

sub Obsolete_option {
    print "It appears you used a bmsort option no longer supported.\nPlease review the help.\n"; 
    &print_usage;
    exit(1); 
}

sub Help {
    &print_usage;
    exit(0);
}

sub key_to_sort_option {
    # translate a key in internal specification
    # to sort option
    #
    my ($key) = @_; 
   
    my $keyargs = substr($key,2);
    my $keyopt;
    if (substr($key,0,1) eq "C") {
	if ($keyargs =~ m/([0-9]+)-([0-9]+)/) {
	    $keyopt = sprintf("1.%d,1.%d",$1,$2);
	} elsif ($keyargs =~ m/([0-9]+)/) {
	    $keyopt = sprintf("1.%d,1.%d",$1,$1);
	} else {
	    die "error: non-integer -c argument : $keyargs\n"; 
	}
    } elsif (substr($key,0,1) eq "F") {
	if ($keyargs =~ m/([0-9]+)-([0-9]+)/) {
	    $keyopt = sprintf("%d,%d",$1,$2);
	} elsif ($keyargs =~ m/([0-9]+)/) {
	    $keyopt = sprintf("%d,%d",$1,$1);
	} else {
	    die "error: non-integer -f argument : $keyargs\n"; 
	}
    } else {
	die "error: key char invalid. Internal error.\n"; 
    }
    $keyopt .= "n" if (substr($key,1,1) eq "N");
    return("-k ".$keyopt);
}

#
# Arguments regarding sort keys are internally preprocessed into an alternate 
# form before using long GetOptions code
#
# 
sub cook_argument {
    #
    # given an argument in $_[0], and
    # a 1/0 in $_[1] specifying "column" or "field"< 
    # and 1/0 in $_[2] specifying 
    my ($arg,$key_is_field,$is_numerical) = @_; 
    my @out = (); 
#    print "debug: cook_arugument, arg=->$arg<- \n";
    my @pieces = split(',',$arg);  
    
    my $prefix = $key_is_field ? "F":"C"; 
    $prefix .= $is_numerical ? "N":"L"; 
    foreach my $piece (@pieces) {
	push(@out,"--key");
	push(@out,$prefix.$piece);
    }
    return(@out); 
}

sub cook_arguments {
    # The problem that we have in using standard GetOptions
    # is correctly parsing the -n and -l options, as they
    # affect only the key specifications which occur on the
    # command line AFTER the -n and -l options.
    #
    # Furthermore the ordering of the keys matter. 
    #
    # Hence the strategy here is to first run through
    # @ARGV looking only for -n and -l which
    # toggle between numerical and lexical parsing.
    # This will change -c and -k options into 
    # compound --key options with a leading [C|F][L|N]
    #

    my $is_numerical=0;  # 0 is lexical, 1 is numerical.
    my @cooked_options;
    my @argsnew;

    for (my $i=0; $i < scalar(@ARGV); $i++) {
	my $arg = $ARGV[$i];
	my $argnext = ""; 
	$argnext = $ARGV[$i+1] unless ($i+1) > $#ARGV; 
#	$argnext = "" if (substr($argnext,0,1) eq "-"); # don't allow -c -x -c -f

	my $arg2 = substr($arg,0,2);  # positions 01
	my $argpast="";
	if (length($arg) > 2) {
	    $argpast = substr($arg,2); # positions 2 on
	}

	@argsnew = (); 
	# check for -n or -l
	if ($arg eq "-n") {
	    $is_numerical = 1; 
	} elsif ($arg eq "-l") {
	    $is_numerical = 0; 
	} elsif (($arg2 eq "-c") || ($arg2 eq "-f")) {
	    my $object;
	    if (length($argpast) > 0)  {
		$object = $argpast; 
	    } else {
		die "$0: must provide key specification argument to -c or -f\n" if (length($argnext) == 0);
		$object = $argnext;
		$i++;
	    }
	    my @cooked_args = cook_argument(  $object, ($arg2 eq "-f"), $is_numerical);
	    push(@argsnew,@cooked_args); 
	} else { 
	    @argsnew = ($arg); 
	}
	push(@cooked_options,@argsnew); 
    }
    return(@cooked_options); 
}



sub print_usage {
    print
"\n$0: sort STDIN according to fields, calling GNU sort, or other compatible sort.\n\n".
"Example:\n" .
"     gunzip -c bigScrSeg1.gz bigScrSeg2.gz | bmsort.pl -c1-19,35-58 | gzip > bigScrCombined.gz\n".
"\n".
"Options controlling sort ordering:\n".
"     -c     Specify the columns for sort keys.  Format should be\n".
"            'a-b,c,d-e,etc.' where a,b,c,d,e represent columns in the input\n".
"            file, counting from 1.\n".
"     -n     Sort in numeric order.  Only affects keys that follow it.\n".
"     -l     Sort in lexical order (default).  Only affects keys that follow it.\n".
"     -f     Specify the fields for sort keys. (instead of usual -c).\n".
"            Format should be like 'a-b,c,d-'\n".
"     -d ch  Specify the delimiter for the fields, for -f option. Default is any white space.\n".
"     -D     Sort in descending order (default is ascending)\n".
"     --is-sort\n".
"            Check if input is sorted. Do not sort\n".
"     -x     Remove all but first lines with equal keys.  Note this is NOT uniq\n".
"            as it tests only subsequent lines with equal keys.\n".
"\n".
"     Note: LC_ALL=C will be set internally by this script before calling sort.\n".
"     This means that, among other things, upper ASCII (>=128) will be treated as sorting after\n".
"     lower ASCII, like nearly all other tools.\n".
"     Legacy bmsort did NOT do that, and upper ASCII sorted before regular\n\n".
"Options controlling sort performance/diagnostics:\n".
"     -m mem Specify the amount of RAM to use in buffer, in megabytes.  Default=2000.\n".
"     -t dir\n".
"     -T dir Specify directories to hold temporary data. -t and -T here have identical meaning.\n".
"            Multiple directories can be specified with multiple -t options.\n".
"            and the sort will use all of them simultaneously. Useful for performance when they are on\n".
"            distinct hardware, and useful for very large sorts when you need lots of temp space\n".
"            Default: -t . /\n".
"            Note that locally attached disk partitions like /tmp or /apps/* are substantially faster\n".
"            than network attached storage (NAS, e.g. /fraudNN/* ), but often have less free space.\n".
"            Sorting will crash and may not clean up intermediate files if you run out of space on any drive.\n".
"            Unfortunately it does not appear that GNU sort will intelligently manage use of free space\n".
"            when you specify multiple directories with -t, it just splits it roughly evenly across all\n".
"            of them, and free space limitations still apply.\n". 
"            Of course increasing RAM usage with -m option will lower disk usage.\n".
"     -S     Run in silent mode\n".
"\n".
"Control options not in legacy bmsort:\n".
"     --compress-intermediate\n".
"            If provided, pass --compress-program option to GNU sort using path\n".
"            in next option.  This will use gzip on the internal intermediate files\n".
"            used by sort. Note, this is only supported by very recent GNU sort\n".
"            implementations, probably in /apps/usr/local/bin/ or /apps/usr/local64/bin/\n".
"            If you are sorting very large files then you may need to use this to prevent\n".
"            filesystem exhaustion.  Note also -t option. \n".
"     --gzip-path=<path to gzip executable>\n".
"            Provide the path to the executable to use for compressing intermediate files.\n".
"            Default: /usr/bin/gzip\n".
"     --sort-path=<path to GNU sort or syncsort (GNU compatibility)\n".
"            Path to underlying GNU sort executable or syncsort executable in GNU sort compat mode\n".
"            Default:   Linux 64-bit:   /apps/usr/local64/bin/sort\n".
"                       Linux 32-bit:   /apps/usr/local/bin/sort\n".
"                       SunOS 64-bit:   /apps/usr/local64/bin/sort\n".
"            If these defaults do not exist, the next default is /bin/sort.\n".
"            --sort-path argument overrides default.\n". 
"     --dry-run\n".
"            Do not actually perform the sort, just emit to stdout the command which would\n".
"            be executed\n".
"\n".
"Options in legacy bmsort not supported:\n".
"\n".
"     -i <input file>\n".
"     -o <output file>\n".
"            Workaround: use standard in and standard out.\n".
"     -M     bmsort: Multiple delimiters count as multiple-zero length fields.\n".
"            Workaround: none. Multiple delimiter interpretation will be as per GNU sort, not bmsort\n".
"     -R     bmsort: replace NULL characters in input records.\n".
"            Workaround: none needed.  GNU sort works will null characters\n".
"\n";

}
