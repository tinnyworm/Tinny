# -*- Perl -*-
# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl (testname).t'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..9\n"; }
END {print "not ok 1\n" unless $loaded;}
use Number::Format qw(:subs);
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

print "not " unless (format_number(123456.51) eq '123,456.51');
print "ok 2\n";

print "not " unless (format_number(1234567.509, 2) eq '1,234,567.51');
print "ok 3\n";

print "not " unless (format_number(12345678.5, 2) eq '12,345,678.5');
print "ok 4\n";

print "not " unless (format_number(123456789.51, 2) eq '123,456,789.51');
print "ok 5\n";

print "not " unless (format_number(1.23456789, 6) eq '1.234568');
print "ok 6\n";

print "not " unless (format_number("1.2300", 7, 1) eq '1.2300000');
print "ok 7\n";

print "not " unless (format_number(.23, 7, 1) eq '0.2300000');
print "ok 8\n";

print "not " unless (format_number(-100, 7, 1) eq '-100.0000000');
print "ok 9\n";
