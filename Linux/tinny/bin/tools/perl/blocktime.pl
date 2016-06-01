#! /usr/local/bin/perl -w

$usage = "$0 auths.dat.gz\n".
  "Typical Use:\n".
  " ~/> zcat frauds.dat.gz | $0 auths.dat.gz\n".
  "     | gzip -c > frauds-new.dat.gz\n".
  "Finds the last approved transaction for the account, sets the block\n".
  "date and time one second after that. Tosses records in the frauds file\n".
  "that have no transactions.\n";

$auth_name = shift
  or die $usage;
open(AUTH, "zcat $auth_name |")
  or die "Can't zcat $auth_name\n";

$auth = <AUTH>;
while(<>){  # For each fraud record
  chomp;
  # Progress
  if(!($. % 100)){
    print STDERR "$.\r";
  }
  # Get acct nr
  $acct_nr = substr($_, 0, 19);
  # Find date and time of last approved tran for acct
  $found = 0;
  $has_approved = 0;
  $done = 0;
  do {
    ($acc, $date, $time, $result) = unpack("a19 a8 a6 \@62 a1", $auth);
    if($acc gt $acct_nr){
      $done = 1;
    } elsif($acc eq $acct_nr){
      $found = 1;
      if($result eq "A" or $result eq " "){
	$has_approved = 1;
	$last_date = $date;
	$last_time = $time;
      }
    }
  } while(!$done and $auth = <AUTH>);
  # Found?
  if($found){
    # Yes - write new fraud line
    # Got approved transactions?
    if($has_approved){
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
	     substr($_,0,35), $last_date, $hour, $minute, $seconds);
    } else {
      # No - write same fraud line but make type Z
      substr($_,19,1) = "Z";
      $_ .= " " x 49;  ## Pad with spaces to ensure constant record length
      print substr($_,0,49),"\n";
    }
  }
  # No - skip
  
}
print STDERR "$.\n";

close(AUTH);
