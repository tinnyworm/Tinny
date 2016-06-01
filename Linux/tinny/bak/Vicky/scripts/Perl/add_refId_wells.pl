#! /usr/local/bin/perl 

$usage = "\nUsage: $0 -e -d [-h]\n"
         . " where:\n"
         . "    -h    :   show this message\n"
         . "    -e    :   encrypt the reference ID\n"
         . "    -d    :   decrypt the reference ID\n\n";

require "getopts.pl";

&Getopts('hed');
$index=0; 

%liste= (
		"0" => "4",
		"1" => "0",
		"2" => "8",
		"3" => "1",	
		"4" => "2",
		"5" => "7",
		"6" => "5",
		"7" => "3",
		"8" => "9",
		"9" => "6"
		);
  
%listd= (
		4 => 0,
		0 => 1,
		8 => 2,
		1 => 3,	
		2 => 4,
		7 => 5,
		5 => 6,
		3 => 7,
		9 => 8,
		6 => 9
		);


if ($opt_e) {
	while (<STDIN>){
		chomp;
		$acct = $_; 
		$bin=substr($_, 0, 6); 
		$index ++; 
		$temp2 = ''; 
		$temp1=$bin . $index;
		$len = length $temp1;
	   @temp = split '', $temp1; 	
		for ($i=0; $i<$len; $i++){
			if (defined($liste{$temp[$i]})) {
				 $cov[$i]=$liste{$temp[$i]};
				 $temp2= $temp2 . $cov[$i]  
			}
	  }
	   $final = "A"x(16-$len) . $temp2 . "   "; 
		print $acct, "   ", $final , "\n"; 
	   print STDERR "$bin $index\n"; 
	
	}

}

if ($opt_d) {
	while (<STDIN>){
		 chomp;
		 $ref=substr($_,19,16);
		 $ref =~ s/^A//g;
	    $temp2 = '';
		 $temp1=$ref; 
		 $len = length $temp1;
		 @temp = split '', $temp1;
	 	 for ($i=0; $i<$len; $i++){
			if (defined($listd{$temp[$i]})) {
				 $cov[$i]=$listd{$temp[$i]};
				 $temp2=$temp2 . $cov[$i]; 
			}
	  }
		$bin=substr($temp2, 0, 6); 
		$index= substr($temp2, 6 , $len-6); 
		print $bin, " ", $index , "\n"; 	
	
	}



}