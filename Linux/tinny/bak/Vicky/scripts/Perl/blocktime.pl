#! /usr/local/bin/perl -w

#
# perl replacement for blockdate.sas
# 2003     mxs initial coding
# 20040723 gg  cosmetic edits
#

$usage = "Typical Use:\n".
  "zcat frauds.dat.gz | $0 auths.dat.gz | gzip >&frauds.blcktme.dat.gz&\n".
  "Finds the last approved transaction for the account, sets the block\n".
  "date and time one second after that. Records in the frauds file\n".
  "that have no transactions are unchanged.\n";

$auth_name = shift
  or die $usage;
open(AUTH, "zcat $auth_name |")
  or die "Can't zcat $auth_name\n";

$auth = <AUTH>;
while(<>){  # For each fraud record
  chomp;
  # Progress
  #if(!($. % 1000)){
  #  print STDERR "$.\r";
  #}
  # Get acct nr
  $acct_nr = substr($_, 0, 19);
  $ff_date = substr($_, 20, 8);
  # Find date and time of last approved tran for acct
  $found = 0;
  $has_approved = 0;
  while ($auth){
    ($acc, $date, $time, $result) = unpack("a19 a8 a6 \@62 a1", $auth);
    if($acc gt $acct_nr){
      last;
    } elsif($acc eq $acct_nr){
      $found = 1;
      $last_date = $date;
      if($result eq "A" or $result eq " "){
			$has_approved = 1;
			$last_apprv_date = $date;
			$last_time = $time;
      }
    }
    $auth = <AUTH>;
  } 
  # Found?
  if($found){
    # Yes - write new fraud line
    # Only prefrauds - spit out line as is
    if ($last_date < $ff_date){
			print substr($_,0,49) . "\n";
    }
    # Got approved transactions?
    elsif($has_approved){
      # Yes - write new block date and time
      ($hour, $minute, $seconds) = unpack "a2 a2 a2", $last_time;
      if($seconds < 59){
			$seconds++;
      } else {
			$seconds = 0;
			if($minute < 59){
			  $minute++;
			} else {
			  $minute = 0;
			  if($hour < 23){
			    $hour++;
			  } else {
	   		 ($hour, $minute, $seconds) = unpack "a2 a2 a2", $last_time;
	  		  }
			}
	  }
        printf("%s%s%02d%02d%02d\n",
	     substr($_,0,35), $last_apprv_date, $hour, $minute, $seconds);
	 } else {
       # No - write same fraud line but make type Z. Most likely old frauds
     	 substr($_,19,1) = "Z";
       $_ .= " " x 49;  ## Pad with spaces to ensure constant record length
       print substr($_,0,49),"\n";
    }
  }else{
  	 # No - spit out the line as is
     print substr($_,0,49) . "\n";
  } 
}
#print STDERR "$.\n";

close(AUTH);
