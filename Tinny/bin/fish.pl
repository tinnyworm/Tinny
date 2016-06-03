#!/usr/bin/perl

use Getopt::Long;

$usage = "$0 -d delimiter -b baitfile -p pondfile [-s bait_key_field] [-l bait_key_length] [-t pond_key_field] [-v] [-h] [-m bait_key_offset (base 1)] [-n pond_key_offset]\n".
"\t fish out records specified by BAIT from POND. \n".
"\t -h help. \n".
"\t -v fish records that are in POND but not in BAIT.\n".
"\t -d delimiter that is used for both BAIT and POND.\n".
"\t Please note that Getopt::Std have some internal bug to parse continous missing paratmeters, example fish.pl -p file1 -b file2 -s -t 1 -v  generates \$opt_s=-t; thus Getopt::Long is used instead \n";

GetOptions (
            "bait:s"  => \$opt_b,
            "pond:s"  => \$opt_p,
            "s:s"     => \$opt_s,
            "t:s"     => \$opt_t,
	    "l:s"     => \$opt_l,
	    "m:s"     => \$opt_m,
	    "n:s"     => \$opt_n,
	    "d:s"     => \$opt_d,
            "v"       => \$opt_v,
            "help"    => \$opt_h,
            );


die $usage if ($opt_h);
die $usage unless (($opt_b)&&($opt_p));

$opt_s = 1 unless ($opt_s);
$opt_l = -1 unless ($opt_l);
$opt_t = 1 unless ($opt_t);
$opt_v = 0 unless ($opt_v);
$opt_d = hexcode($opt_d);

if ($opt_b =~ /.gz/) {
  open(B,"zcat $opt_b |") || die $usage;
} else {
  open(B,$opt_b) || die $usage;
}

if ($opt_p =~ /.gz/) {
  open(P,"zcat $opt_p |") || die $usage;
} else {
  open(P,$opt_p) || die $usage;
}

while (<B>) {
  chomp;
  @a = split(/\x$opt_d/);
  $key = $a[$opt_s-1];
  $key = substr($key, $opt_m-1) if ($opt_m); 
  if ($opt_l > 0) {
      $nkey = substr($key,0,$opt_l);
  } else {
      $key =~ s/ //g;
      $nkey = $key;
  }
  $b_key{$nkey} = 1;
}

close(B);

while (<P>) {
  chomp;
  @a = split(/\x$opt_d/);
  $key = $a[$opt_t-1];

  $key = substr($key, $opt_n-1) if ($opt_n); 
  if ($opt_l > 0) {
    $key = substr($key,0,$opt_l);
  }
  $key =~ s/ //g;

  if (defined($b_key{$key})) {
    print "$_\n" unless ($opt_v);
  } else {
    if ($opt_v){
      print "$_\n";
    }
  }
}

close(P);

sub hexcode {
	my ($d) = @_;
	$d =~ s#\%#25#g;
	$d =~ s#\+#2B#g;
	$d =~ s#\s#\+#g;
	$d =~ s#\!#21#g;
	$d =~ s#\"#22#g;
	$d =~ s#\##23#g;
	$d =~ s#\$#24#g;	
	$d =~ s#\&#26#g;
	$d =~ s#\'#27#g;
	$d =~ s#\(#28#g;
	$d =~ s#\)#29#g;
	$d =~ s#\*#2A#g;	
	$d =~ s#\,#2C#g;
	$d =~ s#\-#2D#g;
	$d =~ s#\.#2E#g;
	$d =~ s#\/#2F#g;
	$d =~ s#\:#3A#g;
	$d =~ s#\;#3B#g;
	$d =~ s#\<#3C#g;
	$d =~ s#\=#3D#g;
	$d =~ s#\>#3E#g;
	$d =~ s#\?#3F#g;
	$d =~ s#\@#40#g;
	$d =~ s#\`#60#g;
	$d =~ s#\|#7C#g;
	return $d;
}
