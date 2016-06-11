#! /usr/local/bin/perl -w

while (<>){
	 $year=$_; 
	 if (!($year % 400) || (($year % 100) && !($year % 4))) {
			print 29;
		} else {
			print 28;
		}
}
