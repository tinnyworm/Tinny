#! /usr/local/bin/perl -w

while (<STDIN>)
{
	chomp;

	@tmp=split(/\,\s+/, $_);

	print "$tmp[0]\t$tmp[1]\t$tmp[2]\t$tmp[3]\t$tmp[4]\n"; 

}
