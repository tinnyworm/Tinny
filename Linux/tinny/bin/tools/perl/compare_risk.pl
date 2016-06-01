#!/usr/bin/perl

use warnings;
use strict;

my $usage = "$0 new_risk, old_risk";

my $new_risk = shift
	or die $usage;

my $old_risk = shift
	or die $usage;

open NEW $new_risk
	or die "Can NOT open $new_risk!";

open OLD $old_risk
	or die "Can NOT open $old_risk!";


