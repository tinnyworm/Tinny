#! /usr/local/bin/perl 

$usage =
  "$0 sample_stats1 [sample_stats2 ...]\n\n".
  "Get the information from sample stats files.";


$fname = shift(@ARGV);
die $usage if(!$fname or $fname eq "-h" or $fname eq "--help");
unshift @ARGV, $fname;

while($fname = shift(@ARGV)) {
    
    $month = `echo $fname | /home/yxc/scripts/Perl/rmdir.pl | cut -c1-4`;
    chop($month);  
    open (IN, "<$fname") || die "Couldn't open $fname:$!"; 
    
    $totrec=0;
    $totacc=0;
    $nfrec=0;
    $frec=0;
    $nfacc=0;
    $facc=0;
    $srrec=0;
    $sracc=0;
    
    while(<IN>) {
        
       if (/Total number of records seen          =\s+(\d+)/){
                    $totrec=$1*1;
       } 
       elsif (/Number of fraud records               =\s+(\d+)/){
                    $frec=$1*1; 
       } 
       elsif (/Number of non-fraud records in sample =\s+(\d+)/){
                    $nfrec=$1*1; 
       } 
       elsif (/Total number of accounts seen         =\s+(\d+)/){
                    $totacc=$1*1; 
       }
       elsif (/Number of fraud accounts              =\s+(\d+)/){
                    $facc=$1*1; 
       }
       elsif (/Number of non-fraud accounts in sample=\s+(\d+)/){
                    $nfacc=$1*1; 
       }
       elsif (/Requested sampling rate               =\s+(\d+)/){
                    $reqacc=$1*1; 
		    print "$reqacc\n"; 
       }
       elsif (/Actual record sampling rate (%)\s+=\s+(\d+)/){
                    $srrec=$1*1; 
       } 
       elsif (/Actual account sampling rate (%)\s+=\s+(\d+)/){
                    $sracc=$1*1; 
       } 
        else {
         next;
       }
    $monthHash{$month}[0]+=$totrec;
    $monthHash{$month}[1]+=$totacc;
    $monthHash{$month}[2] = $nfrec;
    $monthHash{$month}[3] = $frec;
    $monthHash{$month}[4] = $nfacc;
    $monthHash{$month}[5] = $facc;
    $monthHash{$month}[6] = $srrec;
    $monthHash{$month}[7] = $sracc;
  }
}

printf ("%5s %5s %12s %12s %10s %10s %10s %10s %5s %5s\n", "Index", "Month", "#ofTotalRec", "#ofTotalAcct", "#of$reqacc%.NF Rec", "#ofFradRec", "#of$reqacc%.NFAcct", "#ofFradAcct", "RecSamRate", "AcctSamRate");

@month = sort {$a cmp $b} keys %monthHash;
for ($i=0; $i<=$#month; $i++){
  $index=$i+1;
  printf ("%5s %5s %12d %12d %10d %10d %10d %10d %1.2f% %1.2f%\n", $index, $month[$i], $monthHash{$month[$i]}[0], $monthHash{$month[$i]}[1], $monthHash{$month[$i]}[2], $monthHash{$month[$i]}[3], $monthHash{$month[$i]}[4], $monthHash{$month[$i]}[5], $monthHash{$month[$i]}[6], $monthHash{$month[$i]}[7]); 
}  
