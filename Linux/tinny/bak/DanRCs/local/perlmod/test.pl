#!/usr/local/bin/perl

use lib '/home/dxl/local/perlmod';
use Binning;
use warnings;
use strict;

my @var = (0.4, 0.3, 0.6, 0.4, 0.3, 0.6, 0.4, 0.3, 0.6, 0.4, 0.3, 0.6);
my @tag = (0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0);

binning(\@var, \@tag, 1, 1);
