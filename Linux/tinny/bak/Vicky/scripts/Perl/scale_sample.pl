#!/usr/local/bin/perl

#Used for sampling the scale file. samp_rate is between 0 and 1. FOr example if samp_rate is 0.2 then 
#approximately %20  of the records in scale file will be sampled and will be written to the new file.


#use Math::Random;

$samp_rate = shift;

$in_base = shift;
$out_base = shift;

$in_scl_fn = $in_base . ".scl";
$in_tag_fn = $in_base . ".tag";

$out_scl_fn = $out_base . ".scl";
$out_tag_fn = $out_base . ".tag";


if (!defined $out_fn) {
  ($fn, $ext) = split(/\./, $in_fn); 
  $out_fn = $fn . "_sampled.". $ext;
}

open (IN_SCL,  "$in_scl_fn")     || die "cannot read file $in_scl_fn:$!\n";
open (OUT_SCL, "> $out_scl_fn")  || die "cannot create file $out_scl_fn:$!\n";

open (IN_TAG,  "<$in_tag_fn")     || die "cannot read file $in_tag_fn:$!\n";
open (OUT_TAG, "> $out_tag_fn")  || die "cannot create file $out_tag_fn:$!\n";


read(IN_SCL, $buff, 4);


$n = unpack("I4", $buff);
#$n = unpack("l4", $buff);

#print $n, "\n";

print OUT_SCL $buff;
#print OUT $n;

$i = 0;

#First two lines of the tag file is the test/train count
$line = <IN_TAG>; print OUT_TAG $line;
$line = <IN_TAG>; print OUT_TAG $line;

$cnt{"train"}=0;
$cnt{"test"}=0;

while (read(IN_SCL, $buff, 4*$n)) {
  
  #  $rand = random_uniform(); 
  
  $tag_line = <IN_TAG>;

  @tag_rec =split(' ',$tag_line);

  $rand = rand(); 
  
  if ($rand <= $samp_rate) {
    print OUT_SCL $buff;
    #print OUT_TAG $tag_line;
    print OUT_TAG ($cnt{"train"}+$cnt{"test"} . " $tag_rec[1]\n");
    $cnt{$tag_rec[1]}++;
  }
  
  
  if ($i % 1000 == 0) { print "\b" x length($i), $i;}
  $i++;
  
}




close(IN_SCL);
close(OUT_SCL);
close(IN_TAG);
close(OUT_TAG);


#patch the first two lines of the new tagfile with correct test/train count
system("head -2 $in_tag_fn > /tmp/threshold.$$.1");
open(DIFF, "| diff /tmp/threshold.$$.1 - | patch $out_tag_fn");
print DIFF "test: $cnt{'test'}\n";
print DIFF "train: $cnt{'train'}\n";
close(DIFF);

unlink "/tmp/threshold.$$.1";



