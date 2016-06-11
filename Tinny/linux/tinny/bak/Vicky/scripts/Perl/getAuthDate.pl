#! /usr/local/bin/perl -w

while (<STDIN>){
	if ($_ =~ m/^Histogram of auth date\s+/o) {
		$hdr = <STDIN>;  # header line
	              for ($k=0; $k<101; $k++) {
        	          $in = <STDIN>;
                	  chop $in;
	                  @l = split " ", $in;
        	          $date[$k]       = $l[0];  # * score_fact + th
                  $afpr[$k]         = $l[1];
                  $adr[$k]          = $l[2]; # * ENVIRON["FAC"]
                  $case1[$k]        = sprintf("%.2f", ($l[1]+1)*$l[2]/100);
                 
                  $research{$thresh[$k]}[0]=$afpr[$k];
                  $research{$thresh[$k]}[1]=$adr[$k];
                  $research{$thresh[$k]}[2]=$case1[$k];
               }


}
