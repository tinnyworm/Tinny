#! /usr/bin/perl -w

$str = "/app1/app2/app3";
$str =~ m%/(\w+)/(\w+)$%;

print $1;

