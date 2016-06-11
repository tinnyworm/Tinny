#!/usr/local/bin/perl 

$usage = 
"res2scrdist.pl: Extract transaction based information from research file 
                into csv (comma separated values) format
     Usage: res2scrdist.pl <input research file> <output csv file>
        Specify \"-\" as file name to use standard input (or output)\n";

die $usage if ($#ARGV < 1);

if ($ARGV[0] eq '-'){
    $in = STDIN;
}else{
    die "Can't open file $ARGV[0]\n$usage" unless open(RES, $ARGV[0]);
    $in = RES;
}
if ($ARGV[1] eq '-'){
    $out = STDOUT;
}else{
    die "Can't open file $ARGV[1]\n$usage" unless open(CSV, "> $ARGV[1]");
    $out = CSV;
}

while($line = <$in>){
    last if ($line =~ /TRANSACTION-BASED INFORMATION/);
}

while($line = <$in>){
    last if ($line =~ /Threshold/);
}
$line = <$in>;
while($line = <$in>){
    $line =~ s/%/ /g;
    @data = split(' ', $line);
    $trisk = $data[3] if ($data[0]+0 == 0);
    last if ($#data != 3);
    printf $out ("%.3f,%.3f,%.3f,%.3f\n",$data[1], $data[2], $data[3], ($data[1]*$trisk+$data[2])/($trisk+1));
#    printf $out ("$data[1]\%,$data[2]\%,$data[3],%.3f\%\n",($data[1]*$trisk+$data[2])/($trisk+1)*100);
}

close $in;
close $out;
