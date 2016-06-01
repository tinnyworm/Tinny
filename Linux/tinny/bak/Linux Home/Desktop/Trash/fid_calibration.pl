#! /usr/bin/perl -w

my $r  = 0.3;
my $rt = 0.5;

for ( my $i = 0; $i < 100; ++$i ) {

    my $raw_s = 0.0 + $i * 0.01;

    my $den = $raw_s * ( $rt - $r ) + $r * ( 1 - $rt );

    my $t_s = $raw_s * $rt * ( 1 - $r ) / $den;

    #print "$t_s\t$raw_s\n";
    printf "%5.4f\t%5.4f\n", $raw_s, $t_s;

}
