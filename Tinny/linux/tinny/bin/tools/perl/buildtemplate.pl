#!/usr/local/bin/perl -w

$offset=1036;

for ($i=1; $i<=70; $i++) {
	 $offset += 4;
	 print "MP$i\n11\nW\n$offset\n4\n";
}
