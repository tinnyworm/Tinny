#!/usr/local/bin/perl

#thresholdtag.pl - Filters records under specified threshold from tag file
#Scores should be obtained from nnet-output file.
#Threshold should be specified in the range (-1, 1)
#Should be used with thresholdscl that similarly filters the scale file
#Spits out to standard out

#Usage: threshold.pl <tagfile> <nnet output file> <threshold> <new tagfile>]

if ($#ARGV < 3){
    print STDERR "thresholdtag.pl - Filters records under specified threshold from tag file\n";
    print STDERR "Usage: threshold.pl [-r] <tagfile> <nnet output file> <threshold> <new tagfile>\n";
    exit;
}
$lt = 0; 
if ($ARGV[0] eq "-r"){$lt=1; shift @ARGV;}
$tagfile = shift @ARGV;
$nnetfile = shift @ARGV;
$thres = shift @ARGV;
$newtag = shift @ARGV;

if ($thres<=-1 || $thres>=1){
    print STDERR "Threshold should be specified in the range (-1,1).\n";
    print STDERR "exiting...\n";
    exit;
}

print STDERR "tagfile: $tagfile, nnet output: $nnetfile, threshold: $thres\n";

die "Failed to open $tagfile" unless open(TAG, $tagfile);
die "Failed to open $nnetfile" unless open(NNET, $nnetfile);
die "Failed to open $newtag" unless open(NEWTAG, "> $newtag");

#First two lines of the tag file is the test/train count
$line = <TAG>; print NEWTAG $line;
$line = <TAG>; print NEWTAG $line;

$cnt{"train"}=0;
$cnt{"test"}=0;
while(<NNET>){
    @nnet_rec=split;
    @tag_rec =split(' ',<TAG>);
    last if ($#tag_rec!=1);
    if ($nnet_rec[2] >= $thres xor $lt==1){
	print NEWTAG ($cnt{"train"}+$cnt{"test"} . " $tag_rec[1]\n");
	$cnt{$tag_rec[1]}++;
    }
}
close(TAG);
close(NNET);
close(NEWTAG);

#patch the first two lines of the new tagfile with correct test/train count
system("head -2 $tagfile > /tmp/threshold.$$.1");
open(DIFF, "| diff /tmp/threshold.$$.1 - | patch $newtag");
print DIFF "test: $cnt{'test'}\n";
print DIFF "train: $cnt{'train'}\n";
close(DIFF);

unlink "/tmp/threshold.$$.1";
