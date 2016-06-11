#! /usr/local/bin/perl  

##############################################################
#  This script to count auth(F) transactions within the      #
#  fraud window. 													       #
#                                                            #
#  yxc: created on 6/2005	                                  #
#                                                            #
##############################################################

$usage = 
  "zcat frauds file | $0 auths file  \n\n".
  "\n\n ";

### >>>>>>>> Change here if the directory is different<<<<<<<<<<<<<####
#######################################################################
$auth=$ARGV[0]; 

if ($auth =~ /[gz]$/){
		open (AUTH, "/usr/local/bin/zcat $auth | ") || die "Couldn't open $auth\n";
} else {
		open (AUTH, "/usr/local/bin/cat $auth | ") || die "Couldn't open $auth\n";
  }
  
while (<STDIN>){
	 $key = substr($_, 0, 19); 
	 $ffd = substr($_, 20, 8);
	 $fft = substr($_,29,6); 
	 $fdd = substr($_, 35, 8); 
	 $fdt = substr($_, 43,6);
	
	    $fdt =~ s/ /9/g; 
       $fft =~ s/ /0/g;
		 $ffdt=$ffd . $fft; 
		 $fddt= $fdd . $fdt; 
		 $keys{$key} = 1; 
		 $dates{$key}[0]=$ffdt*1; # ffd + fft
		 $dates{$key}[1]=$fddt*1; # fdd + fdt
	
#	 print $fddt, "\n"; 
}


$allInWin=0; 
$nonDecline=0;
%fauth = (); 
$lastAcct=" "; 
######### Seperate Auths
  while (defined($line=<AUTH>)){
		$authAcct = substr($line, 0, 19);
		
		$auths{$authAcct}[0] = substr($line, 19, 8)*1; # auth date
		$authDate = substr($line, 19, 8);
		$authTime = substr($line, 27, 6);

#		$authTime =~ s/\D/0/g; 
		if (!($authDate =~ /^[0-9 ]*$/ || $authTime =~ /^[0-9 ]*$/)){
		  next; 
		}
		$authTime =~ s/^\s+//;
		$authTime =~ s/\s+$//;
		
		$len= length $authTime;
		if ( $len ne 6) {
			  $authTime0 = "0"x(6-$len) . $authTime; 
		 } else { 
			 $authTime0=$authTime; 
		 }
	  #	  print STDERR $authTime0, "\n"; 	
	   $both=$authDate . $authTime0; 
	  	$auths{$authAcct}[1] = $both*1;  # auth date + auth time
		$auths{$authAcct}[2] = substr($line, 62,1);  # auth decision
	 if (!(defined($keys{$authAcct}))){
	 	next; 
	 }  else {
			if ($authAcct ne $lastAcct){
				 $allInWin=0;
				 $nonDecline=0;
				 $seqNon=0; 
			} else {
			#	print STDERR "$authAcct: $dates{$authAcct}[0] : $dates{$authAcct}[1]\n"; 
		  		if ($auths{$authAcct}[1] >= $dates{$authAcct}[0] && $auths{$authAcct}[1] <= $dates{$authAcct}[1]){
						  $allInWin ++; 
						  $fauth{$authAcct}[0] = $allInWin;
						  # print STDERR $fauth{$authAcct}[0] . "\n"; 
						 if ($auths{$authAcct}[2] !~ /^(D|P|R)$/) {
							  $nonDecline ++; 
							 $fauth{$authAcct}[1] = $nonDecline; 
						 }
			  
				 } elsif($auths{$authAcct}[1] < $dates{$authAcct}[0]) {
			 		
						 $fauth{$authAcct}[0] = 0;
				 } else {
			
						   if ( $auths{$authAcct}[2] !~ /^(D|P|R)$/){
			
								$seqNon ++; 
								$fauth{$authAcct}[2] = $seqNon;
							 }
					
				 }
				}
					 $lastAcct=$authAcct;  
		} 
	}

	close(AUTH); 
	
$numTran=0;
$numDecTran=0;
$seqNoDecTran=0; 

printf("%19s | %10s |  %10s | %10s\n", "Fraud Acct      ", "#.Tran",
	 "#.NonDecline.Tran", "#.Acct.NonDeclineAfterBlock"); 
foreach (sort keys %fauth){
#	 $fauth{$_}[0] =~ s/^[ ]*$/0/g; 
#	 $fauth{$_}[1] =~ s/^[ ]*$/0/g;
	printf("%19s | %10d | %10d | %10d \n",  $_, $fauth{$_}[0],
	 $fauth{$_}[1],$fauth{$_}[2]
	 );
	
	 if ($fauth{$_}[0] <=0){
	 	$numTran ++;
	  }
	  if ($fauth{$_}[1] <=0) {
	  	$numDecTran ++; 
  		}
  		if ($fauth{$_}[2] >0) {
	  	$seqNoDecTran ++; 
  		}

  }
	  
@totalfOrig=keys %keys;
$totalfO=@totalfOrig; 
@totalfaL=keys %fauth; 
$totalfa=@totalfaL;
$r_no1=sprintf("%.2f%%", 100*$numTran/$totalfa); 
$r_no2=sprintf("%.2f%%", 100*$numDecTran/$totalfa);
$r_no3=sprintf("%.2f%%", 100*$seqNoDecTran/$totalfa);
		 		 
print STDERR "Summary of issues\n"; 
print STDERR "# of frauds accounts within the coverage  : $totalfO\n";	  
print STDERR "# of the fraud accounts in auth           : $totalfa\n";
print STDERR "# of frauds acct with no trans in fraud window or fdd > ffd: $numTran	 \($r_no1\)\n";
print STDERR "# of frauds acct with no approved trans in fraud window    : $numDecTran	 \($r_no2\)\n";
print STDERR "# of frauds acct with non declined trans after block date  : $seqNoDecTran	 \($r_no3\)\n";
