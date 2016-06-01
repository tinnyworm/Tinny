#! /usr/bin/perl

$bads = 0;

while (<>) {

    chomp;
    @data = split /\t/;

    ++$bads if $data[1] == 1;

    print "$_\t$bads\n";

}
