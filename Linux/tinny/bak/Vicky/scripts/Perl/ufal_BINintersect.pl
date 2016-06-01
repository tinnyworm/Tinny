#!/usr/local/bin/perl
#
# ufal_intersect.pl
# PURPOSE:
#    Finds intersection between FALCON consortium frauds, cards, auths,
#    nonmons, and paymnts.
#
# AUTHOR: kmh
#
# CREATIONE DATE: 10-15-1996
#
# $Source: /price/falcon/dvl/scripts/RCS/ufal_intersect.pl,v $
#
# 
#

$usage = "\nUsage: ufal_intersect.pl [ -a <auths_file> ] [ -c <cards_file> ] [ -f <frauds_file> ]\n"
         . "                       [ -n <nonmons_file> ] [ -p <paymnts_file> ] \n"
         . "                       [ -t <posts_file> ] [ -s <sampling_rate> ]\n"
         . "                       [ -h ] [ -u ] [ -v ]\n"
         . " where:\n"
         . "    -h    :   show this message\n"
         . "    -u    :   assume all files are uncompressed\n"
         . "    -v    :   verbose\n\n"
         . "    NOTE: if -s not specified, default sampling rate is assumed to be 0.01\n\n"
	 . "    LIMITATION: The files must already be sorted according to account IDs.\n\n";

require "getopts.pl";

&Getopts('a:c:f:n:p:s:t:huv');

use FileHandle;
STDOUT->autoflush(1);

$auths_file = $opt_a;
$cards_file = $opt_c;
$frauds_file = $opt_f;
$nonmons_file = $opt_n;
$paymnts_file = $opt_p;
$posts_file = $opt_t;
$sampling_rate = $opt_s;
if ( ! $sampling_rate ) { $sampling_rate = 0.01; } 
$dont_uncompress = $opt_u;
if ( $dont_uncompress ) { $compress_flag = "-u"; }
#print "Hello $opt_u $compress_flag\n";

die "$usage" unless $opt_a || $opt_c || $opt_f || $opt_n || $opt_p;

#if ( $opt_h ) { die "$usage"; } 

#########################################
# PRINT START TIME AND CURRENT DIRECTORY
#########################################

printf "STARTED: "; system ( "date" );

print "==========================================================================\n";
printf "Directory: "; system ( "pwd" );

$elapsed_start_time = time;
$user_start_time = (times)[0];
$system_start_time = (times)[1];
$cuser_start_time = (times)[2];
$csystem_start_time = (times)[3];

if ( $opt_a ) { 
$auths_file_str = $dont_uncompress ? "cat $auths_file |"
                                : "/usr/local/bin/zcat $auths_file |";
}
if ( $opt_c ) {
$cards_file_str = $dont_uncompress ? "cat $cards_file |"
                                : "/usr/local/bin/zcat $cards_file |";
}
if ( $opt_f ) {
$frauds_file_str = $dont_uncompress ? "cat $frauds_file |"
                                : "/usr/local/bin/zcat $frauds_file |";
}
if ( $opt_n ) {
$nonmons_file_str = $dont_uncompress ? "cat $nonmons_file |"
                                : "/usr/local/bin/zcat $nonmons_file |";
}
if ( $opt_p ) {
$paymnts_file_str = $dont_uncompress ? "cat $paymnts_file |"
                                : "/usr/local/bin/zcat $paymnts_file |";
}
if ( $opt_t ) {
$posts_file_str = $dont_uncompress ? "cat $posts_file |"
                                : "/usr/local/bin/zcat $posts_file |";
}

########################################
# GENERATE UNIQUE ACCOUNT ID TEMP FILES
########################################
 if ( $dont_uncompress ) {
     $auths_file_tmp = "afile.tmp.tmp";
     $cards_file_tmp = "cfile.tmp.tmp";
     $frauds_file_tmp = "ffile.tmp.tmp";
     $nonmons_file_tmp = "nfile.tmp.tmp";
     $paymnts_file_tmp = "pfile.tmp.tmp";
     $posts_file_tmp = "postfile.tmp.tmp";
     $temp_file = "tempfile.tmp.tmp";
 } else {
     $auths_file_tmp = "afile.tmp.tmp.gz";
     $cards_file_tmp = "cfile.tmp.tmp.gz";
     $frauds_file_tmp = "ffile.tmp.tmp.gz";
     $nonmons_file_tmp = "nfile.tmp.tmp.gz";
     $paymnts_file_tmp = "pfile.tmp.tmp.gz";
     $posts_file_tmp = "postfile.tmp.tmp.gz";
     $temp_file = "tempfile.tmp.tmp.gz";
 }

###################
# PRINT FILE NAMES
###################

#print "auths: $auths_file_str ; cards: $cards_file_str ; frauds: $frauds_file_str \n";
#print "auths_tmp: $auths_file_tmp ; cards_tmp: $cards_file_tmp ; frauds_tmp: $frauds_file_tmp \n";

if ( $opt_a ) {
	if ( $opt_v ) { print "Creating $auths_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$auths_file_str ufalcut -c1-6 | uniq > $auths_file_tmp" ); 
	} else {
		system ( "$auths_file_str ufalcut -c1-6 | uniq | gzip > $auths_file_tmp" ); 
	}
}
if ( $opt_c ) {
	if ( $opt_v ) { print "Creating $cards_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$cards_file_str ufalcut -c1-6 | uniq > $cards_file_tmp" ); 
	} else {
		system ( "$cards_file_str ufalcut -c1-6 | uniq | gzip > $cards_file_tmp" ); 
	}
}
if ( $opt_f ) {
	if ( $opt_v ) { print "Creating $frauds_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$frauds_file_str ufalcut -c1-6 | uniq > $frauds_file_tmp" ); 
	} else {
		system ( "$frauds_file_str ufalcut -c1-6 | uniq  | gzip > $frauds_file_tmp" ); 
	}
}
if ( $opt_n ) {
	if ( $opt_v ) { print "Creating $nonmons_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$nonmons_file_str ufalcut -c1-6 | uniq > $nonmons_file_tmp" ); 
	} else {
		system ( "$nonmons_file_str ufalcut -c1-6 | uniq | gzip > $nonmons_file_tmp" ); 
	}
}
if ( $opt_p ) {
	if ( $opt_v ) { print "Creating $paymnts_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$paymnts_file_str ufalcut -c1-6 | uniq > $paymnts_file_tmp" ); 
	} else {
		system ( "$paymnts_file_str ufalcut -c1-6 | uniq | gzip > $paymnts_file_tmp" ); 
	}
}
if ( $opt_t ) {
	if ( $opt_v ) { print "Creating $posts_file_tmp...\n"; }
	if ( $dont_uncompress ) {
		system ( "$posts_file_str ufalcut -c1-6 | uniq > $posts_file_tmp" ); 
	} else {
		system ( "$posts_file_str ufalcut -c1-6 | uniq | gzip > $posts_file_tmp" ); 
	}
}

###################################################
# DETERMINE NUMBER OF UNIQUE ACCOUNTS IN EACH FILE
###################################################

if ( $opt_a ) {
	if ( $opt_v ) { print "Counting unique accounts in $auths_file_tmp...\n"; }
	$auths_file_tmp_str = $dont_uncompress ? "cat $auths_file_tmp |"
                                : "/usr/local/bin/zcat $auths_file_tmp |";
	$temp_str = `$auths_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$auth_accounts = $1;
}

if ( $opt_c ) {
	if ( $opt_v ) { print "Counting unique accounts in $cards_file_tmp...\n"; }
	$cards_file_tmp_str = $dont_uncompress ? "cat $cards_file_tmp |"
                                : "/usr/local/bin/zcat $cards_file_tmp |";
	$temp_str = `$cards_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$card_accounts = $1;
}

if ( $opt_f ) {
	if ( $opt_v ) { print "Counting unique accounts in $frauds_file_tmp...\n"; }
	$frauds_file_tmp_str = $dont_uncompress ? "cat $frauds_file_tmp |"
                                : "/usr/local/bin/zcat $frauds_file_tmp |";
	$temp_str = `$frauds_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$fraud_accounts = $1;
}

if ( $opt_n ) {
	if ( $opt_v ) { print "Counting unique accounts in $nonmons_file_tmp...\n"; }
	$nonmons_file_tmp_str = $dont_uncompress ? "cat $nonmons_file_tmp |"
                                : "/usr/local/bin/zcat $nonmons_file_tmp |";
	$temp_str = `$nonmons_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$nonmon_accounts = $1;
}

if ( $opt_p ) {
	if ( $opt_v ) { print "Counting unique accounts in $paymnts_file_tmp...\n"; }
	$paymnts_file_tmp_str = $dont_uncompress ? "cat $paymnts_file_tmp |"
                                : "/usr/local/bin/zcat $paymnts_file_tmp |";
	$temp_str = `$paymnts_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$paymnt_accounts = $1;
}

if ( $opt_t ) {
	if ( $opt_v ) { print "Counting unique accounts in $posts_file_tmp...\n"; }
	$posts_file_tmp_str = $dont_uncompress ? "cat $posts_file_tmp |"
                                : "/usr/local/bin/zcat $posts_file_tmp |";
	$temp_str = `$posts_file_tmp_str wc -l` ;
	$temp_str =~ /(\d+)/;
	$post_accounts = $1;
}

###############################################
# PRINT NUMBER OF UNIQUE ACCOUNTS IN EACH FILE
###############################################

select(STDOUT);
 
print "==========================================================================\n";
print "  Following data is for 6-digit BINs only\n";

if ( $opt_a) { $~ = NUMAUTHS; write; }
if ( $opt_c) { $~ = NUMCARDS; write; }
if ( $opt_f) { $~ = NUMFRAUDS; write; }
if ( $opt_n) { $~ = NUMNONMONS; write; }
if ( $opt_p) { $~ = NUMPAYMNTS; write; }
if ( $opt_t) { $~ = NUMPOSTS; write; }

print "==========================================================================\n";

#####################
# FIND INTERSECTIONS
#####################

################
# FRAUDS IN ...
################

# FRAUDS IN AUTHS
if ( $opt_f && $opt_a ) {
	if ( $opt_v ) { print "Fishing in auths pond with frauds as bait...\n"; }
	$temp_str = `fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $auths_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$frauds_auths_intersect  = $1;
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($frauds_auths_intersect / $fraud_accounts) * 100;
		$~ = NUMFINA;
		write;
	} else { print "Number of original fraud accounts ZERO!\n"; }
}

# FRAUDS IN CARDS
if ( $opt_f && $opt_c ) {
	if ( $opt_v ) { print "Fishing in cards pond with frauds as bait...\n"; }
	$temp_str = `fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $cards_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$frauds_cards_intersect = $1;
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($frauds_cards_intersect / $fraud_accounts) * 100;
                $~ = NUMFINC;
                write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 
}

#FRAUDS IN NONMONS
if ( $opt_f && $opt_n ) {
	if ( $opt_v ) { print "Fishing in nonmons pond with frauds as bait...\n"; }
	$temp_str = `fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $nonmons_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$frauds_nonmons_intersect = $1;
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($frauds_nonmons_intersect / $fraud_accounts) * 100;
                $~ = NUMFINN;
                write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 
}

# FRAUDS IN PAYMENTS
if ( $opt_f && $opt_p ) {
	if ( $opt_v ) { print "Fishing in paymnts pond with frauds as bait...\n"; }
	$temp_str = `fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $paymnts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$frauds_paymnts_intersect = $1;
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($frauds_paymnts_intersect / $fraud_accounts) * 100;
                $~ = NUMFINP;
                write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 
}

# FRAUDS IN POSTS
if ( $opt_f && $opt_t ) {
	if ( $opt_v ) { print "Fishing in postings pond with frauds as bait...\n"; }
	$temp_str = `fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$frauds_posts_intersect = $1;
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($frauds_posts_intersect / $fraud_accounts) * 100;
                $~ = NUMFINPOST;
                write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 
}

if ( $opt_f ) { print "--------------------------------------------------------------------------\n"; }

###########################
# AUTHS IN ...
###########################

# AUTHS IN CARDS
if ( $opt_a && $opt_c ) {
	if ( $opt_v ) { print "Fishing in cards pond with auths as bait...\n"; }
	$temp_str  = `fish.pl -b $auths_file_tmp -c 1-6 -s 1 $compress_flag $cards_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$auths_cards_intersect  = $1;
	if ($auth_accounts != 0) { 
		$percent_tmp = ($auths_cards_intersect / $auth_accounts) * 100;
                $~ = NUMAINC;
                write;
	} else { print "Number of original auths accounts ZERO!\n"; }
}

# AUTHS IN FRAUDS
	if ($auth_accounts != 0) { 
		$percent_tmp = ($frauds_auths_intersect / $auth_accounts) * 100;
                $~ = NUMAINF;
                write;
	} else { print "Number of original auths accounts ZERO!\n"; }

# AUTHS IN NONMONS
if ( $opt_a && $opt_n ) {
	if ( $opt_v ) { print "Fishing in nonmons pond with auths as bait...\n"; }
	$temp_str  = `fish.pl -b $auths_file_tmp -c 1-6 -s 1 $compress_flag $nonmons_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$auths_nonmons_intersect  = $1;
	if ($auth_accounts != 0) { 
		$percent_tmp = ($auths_nonmons_intersect / $auth_accounts) * 100;
                $~ = NUMAINN;
                write;
	} else { print "Number of original auths accounts ZERO!\n"; }
}

# AUTHS IN PAYMENTS
if ( $opt_a && $opt_p ) {
	if ( $opt_v ) { print "Fishing in payments pond with auths as bait...\n"; }
	$temp_str  = `fish.pl -b $auths_file_tmp -c 1-6 -s 1 $compress_flag $paymnts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$auths_paymnts_intersect  = $1;
	if ($auth_accounts != 0) { 
		$percent_tmp = ($auths_paymnts_intersect / $auth_accounts) * 100;
                $~ = NUMAINP;
                write;
	} else { print "Number of original auths accounts ZERO!\n"; }
}

# AUTHS IN POSTS
if ( $opt_a && $opt_t ) {
	if ( $opt_v ) { print "Fishing in postings pond with auths as bait...\n"; }
	$temp_str  = `fish.pl -b $auths_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$auths_posts_intersect  = $1;
	if ($auth_accounts != 0) { 
		$percent_tmp = ($auths_posts_intersect / $auth_accounts) * 100;
                $~ = NUMAINPOST;
                write;
	} else { print "Number of original auths accounts ZERO!\n"; }
}

###########################
# CARDS IN ...
###########################

if ( $opt_c ) { 

  print "--------------------------------------------------------------------------\n";

# CARDS IN AUTHS
	if ($card_accounts != 0) { 
		$percent_tmp = ($auths_cards_intersect / $card_accounts) * 100;
                $~ = NUMCINA;
                write;
	} else { print "Number of original cards accounts ZERO!\n"; }

# CARDS IN FRAUDS
	if ($card_accounts != 0) { 
		$percent_tmp = ($frauds_cards_intersect / $card_accounts) * 100;
                $~ = NUMCINF;
                write;
	} else { print "Number of original cards accounts ZERO!\n"; }

# CARDS IN NONMONS
  if ( $opt_n ) {
	if ( $opt_v ) { print "Fishing in nonmons pond with cards as bait...\n"; }
	$temp_str  = `fish.pl -b $cards_file_tmp -c 1-6 -s 1 $compress_flag $nonmons_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$cards_nonmons_intersect  = $1;
	if ($card_accounts != 0) { 
		$percent_tmp = ($cards_nonmons_intersect / $card_accounts) * 100;
                $~ = NUMCINN;
                write;
	} else { print "Number of original cards accounts ZERO!\n"; }
  }

# CARDS IN PAYMENTS
  if ( $opt_p ) {
	if ( $opt_v ) { print "Fishing in payments pond with cards as bait...\n"; }
	$temp_str  = `fish.pl -b $cards_file_tmp -c 1-6 -s 1 $compress_flag $paymnts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$cards_paymnts_intersect  = $1;
	if ($card_accounts != 0) { 
		$percent_tmp = ($cards_paymnts_intersect / $card_accounts) * 100;
                $~ = NUMCINP;
                write;
	} else { print "Number of original cards accounts ZERO!\n"; }
  }

# CARDS IN POSTS
  if ( $opt_t ) {
	if ( $opt_v ) { print "Fishing in postings pond with cards as bait...\n"; }
	$temp_str  = `fish.pl -b $cards_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$cards_posts_intersect  = $1;
	if ($card_accounts != 0) { 
		$percent_tmp = ($cards_posts_intersect / $card_accounts) * 100;
                $~ = NUMCINPOST;
                write;
	} else { print "Number of original cards accounts ZERO!\n"; }
  }

}

#################
# NONMONS IN ...
#################

if ( $opt_n ) { 

  print "--------------------------------------------------------------------------\n";

# NONMONS IN AUTHS
	if ($nonmon_accounts != 0) { 
		$percent_tmp = ($auths_nonmons_intersect / $nonmon_accounts) * 100;
                $~ = NUMNINA;
                write;
	} else { print "Number of original nonmons accounts ZERO!\n"; } 

# NONMONS IN CARDS
	if ($nonmon_accounts != 0) { 
		$percent_tmp = ($cards_nonmons_intersect / $nonmon_accounts) * 100;
                $~ = NUMNINC;
                write;
	} else { print "Number of original nonmons accounts ZERO!\n"; } 

# NONMONS IN FRAUDS
	if ($nonmon_accounts != 0) { 
		$percent_tmp = ($frauds_nonmons_intersect / $nonmon_accounts) * 100;
                $~ = NUMNINF;
                write;
	} else { print "Number of original nonmons accounts ZERO!\n"; } 

# NONMONS IN PAYMENTS
  if ( $opt_p ) {
	if ( $opt_v ) { print "Fishing in payments pond with nonmons as bait...\n"; }
	$temp_str = `fish.pl -b $nonmons_file_tmp -c 1-6 -s 1 $compress_flag $paymnts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$nonmons_paymnts_intersect = $1;
	if ($nonmon_accounts != 0) { 
		$percent_tmp = ($nonmons_paymnts_intersect / $nonmon_accounts) * 100;
                $~ = NUMNINP;
                write;
	} else { print "Number of original nonmons accounts ZERO!\n"; } 
  }

# NONMONS IN POSTS
  if ( $opt_t ) {
	if ( $opt_v ) { print "Fishing in posts pond with nonmons as bait...\n"; }
	$temp_str = `fish.pl -b $nonmons_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$nonmons_posts_intersect = $1;
	if ($nonmon_accounts != 0) { 
		$percent_tmp = ($nonmons_posts_intersect / $nonmon_accounts) * 100;
                $~ = NUMNINPOST;
                write;
	} else { print "Number of original nonmons accounts ZERO!\n"; } 
  }

}

##################
# PAYMENTS IN ...
##################

if ( $opt_p ) { 

  print "--------------------------------------------------------------------------\n";

# PAYMNTS IN AUTHS
	if ($paymnt_accounts != 0) { 
		$percent_tmp = ($auths_paymnts_intersect / $paymnt_accounts) * 100;
                $~ = NUMPINA;
                write;
	} else { print "Number of original paymnts accounts ZERO!\n"; } 

# PAYMNTS IN CARDS
	if ($paymnt_accounts != 0) { 
		$percent_tmp = ($cards_paymnts_intersect / $paymnt_accounts) * 100;
                $~ = NUMPINC;
                write;
	} else { print "Number of original paymnts accounts ZERO!\n"; } 

# PAYMNTS IN FRAUDS
	if ($paymnt_accounts != 0) { 
		$percent_tmp = ($frauds_paymnts_intersect / $paymnt_accounts) * 100;
                $~ = NUMPINF;
                write;
	} else { print "Number of original paymnts accounts ZERO!\n"; } 

# PAYMNTS IN NONMONS
	if ($paymnt_accounts != 0) { 
		$percent_tmp = ($nonmons_paymnts_intersect / $paymnt_accounts) * 100;
                $~ = NUMPINN;
                write;
	} else { print "Number of original paymnts accounts ZERO!\n"; } 

# PAYMNTS IN POSTS
   if ( $opt_t ) 
   {
	if ( $opt_v ) { print "Fishing in posts pond with paymnts as bait...\n"; }
	$temp_str = `fish.pl -b $paymnts_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$paymnts_posts_intersect = $1;
	if ($paymnt_accounts != 0) { 
		$percent_tmp = ($paymnts_posts_intersect / $paymnt_accounts) * 100;
                $~ = NUMPINPOST;
                write;
	} else { print "Number of original paymnts accounts ZERO!\n"; } 
   }

}

###########################
# POSTS IN ...
###########################

if ( $opt_t ) { 

  print "--------------------------------------------------------------------------\n";

# POSTS IN AUTHS
	if ($post_accounts != 0) { 
		$percent_tmp = ($auths_posts_intersect / $post_accounts) * 100;
                $~ = NUMPOSTINA;
                write;
	} else { print "Number of original posts accounts ZERO!\n"; }

# POSTS IN CARDS
	if ($post_accounts != 0) { 
		$percent_tmp = ($cards_posts_intersect / $post_accounts) * 100;
                $~ = NUMPOSTINC;
                write;
	} else { print "Number of original posts accounts ZERO!\n"; }

# POSTS IN FRAUDS
	if ($post_accounts != 0) { 
		$percent_tmp = ($frauds_posts_intersect / $post_accounts) * 100;
                $~ = NUMPOSTINF;
                write;
	} else { print "Number of original posts accounts ZERO!\n"; }

# POSTS IN NONMONS
	if ($post_accounts != 0) { 
		$percent_tmp = ($nonmons_posts_intersect / $post_accounts) * 100;
                $~ = NUMPOSTINN;
                write;
	} else { print "Number of original posts accounts ZERO!\n"; }

# POSTS IN PAYMENTS
	if ($post_accounts != 0) { 
		$percent_tmp = ($paymnts_posts_intersect / $post_accounts) * 100;
                $~ = NUMPOSTINP;
                write;
	} else { print "Number of original posts accounts ZERO!\n"; }

}

#########################################
# INTERSECTION OF CARDS , AUTHS , FRAUDS
#########################################

if ( $opt_c && $opt_a && $opt_f ) {
        print "--------------------------------------------------------------------------\n";
	if ( $opt_v ) { print "Computing the intersection of auths, cards, and frauds...\n"; }

        if ( $dont_uncompress ) {
                system ( "fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $auths_file_tmp > $temp_file" );  
        } else {
                system ( "fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $auths_file_tmp | gzip > $temp_file" );
        }

	$temp_str  = `fish.pl -b $temp_file -c 1-6 -s 1 $compress_flag $cards_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$auths_cards_frauds_intersect  = $1;
	if ($auth_accounts != 0) { 
		$percent_tmp = ($auths_cards_frauds_intersect / $auth_accounts) * 100;
		$~ = NUMAINCF;
		write;
	} else { print "Number of original auths accounts ZERO!\n"; } 
	if ($card_accounts != 0) { 
		$percent_tmp = ($auths_cards_frauds_intersect / $card_accounts) * 100;
		$~ = NUMCINAF;
		write;
	} else { print "Number of original cards accounts ZERO!\n"; } 
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($auths_cards_frauds_intersect / $fraud_accounts) * 100;
		$~ = NUMFINAC;
		write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 

}

#########################################
# INTERSECTION OF CARDS , POSTS , FRAUDS
#########################################

if ( $opt_c && $opt_t && $opt_f ) {
        print "--------------------------------------------------------------------------\n";
	if ( $opt_v ) { print "Computing the intersection of posts, cards, and frauds...\n"; }

        if ( $dont_uncompress ) {
                system ( "fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp > $temp_file" );  
        } else {
                system ( "fish.pl -b $frauds_file_tmp -c 1-6 -s 1 $compress_flag $posts_file_tmp | gzip > $temp_file" );
        }

	$temp_str  = `fish.pl -b $temp_file -c 1-6 -s 1 $compress_flag $cards_file_tmp | wc -l`;  
	$temp_str =~ /(\d+)/;
	$posts_cards_frauds_intersect  = $1;
	if ($post_accounts != 0) { 
		$percent_tmp = ($posts_cards_frauds_intersect / $post_accounts) * 100;
		$~ = NUMPINCF;
		write;
	} else { print "Number of original posts accounts ZERO!\n"; } 
	if ($card_accounts != 0) { 
		$percent_tmp = ($posts_cards_frauds_intersect / $card_accounts) * 100;
		$~ = NUMCINPF;
		write;
	} else { print "Number of original cards accounts ZERO!\n"; } 
	if ($fraud_accounts != 0) { 
		$percent_tmp = ($posts_cards_frauds_intersect / $fraud_accounts) * 100;
		$~ = NUMFINPC;
		write;
	} else { print "Number of original frauds accounts ZERO!\n"; } 

}

print "==========================================================================\n";

########################
# CLEANUP ALL TMP FILES 
########################

if ( $opt_a ) { system ( "rm $auths_file_tmp" ); }
if ( $opt_c ) { system ( "rm $cards_file_tmp" ); }
if ( $opt_f ) { system ( "rm $frauds_file_tmp" ); }
if ( $opt_n ) { system ( "rm $nonmons_file_tmp" ); }
if ( $opt_p ) { system ( "rm $paymnts_file_tmp" ); }
if ( $opt_t ) { system ( "rm $posts_file_tmp" ); }
if ( ($opt_a && $opt_c && $opt_f) || ($opt_a && $opt_t && $opt_f) ) { system ( "rm $temp_file" ); }


###############
# SET END TIME 
###############

$elapsed_end_time = time;
$user_end_time = (times)[0];
$system_end_time = (times)[1];
$cuser_end_time = (times)[2];
$csystem_end_time = (times)[3];

printf "\n\n";
printf "FINISHED: "; system ( "date" ); printf "\n";
printf "Elapsed time            : %12.5f     seconds\n",  $elapsed_end_time - $elapsed_start_time;
printf "Execution time (user)   : %12.5f CPU seconds\n",  $user_end_time - $user_start_time;
printf "Execution time (system) : %12.5f CPU seconds\n",  $system_end_time - $system_start_time;
printf "Execution time (cuser)  : %12.5f CPU seconds\n",  $cuser_end_time - $cuser_start_time;
printf "Execution time (csystem): %12.5f CPU seconds\n\n",$csystem_end_time - $csystem_start_time;

#################
# DEFINE FORMATS
#################

format NUMAUTHS =
Number of unique auths accounts:  @#########
                                   $auth_accounts
.

format NUMCARDS =
Number of unique cards accounts:  @#########
                                   $card_accounts
.

format NUMFRAUDS =
Number of unique fraud accounts:  @#########
                                   $fraud_accounts
.

format NUMNONMONS =
Number of unique nonmon accounts: @#########
                                   $nonmon_accounts
.

format NUMPAYMNTS =
Number of unique payment accounts:@#########
                                   $paymnt_accounts
.

format NUMPOSTS =
Number of unique posts accounts:  @#########
                                   $post_accounts
.

format NUMFINA =
Number of frauds in auths ....... -> Total:@######### @###.#### %
                                             $frauds_auths_intersect,$percent_tmp
.

format NUMFINC =
Number of frauds in cards ....... -> Total:@######### @###.#### %
                                             $frauds_cards_intersect,$percent_tmp
.

format NUMFINN =
Number of frauds in nonmons ..... -> Total:@######### @###.#### %
                                             $frauds_nonmons_intersect,$percent_tmp
.

format NUMFINP =
Number of frauds in paymnts ..... -> Total:@######### @###.#### %
                                             $frauds_paymnts_intersect,$percent_tmp
.

format NUMFINPOST =
Number of frauds in posts ....... -> Total:@######### @###.#### %
                                             $frauds_posts_intersect,$percent_tmp
.

format NUMAINC =
Number of auths in cards ........ -> Total:@######### @###.#### %
                                             $auths_cards_intersect,$percent_tmp
.

format NUMAINF =
Number of auths in frauds ....... -> Total:@######### @###.#### %
                                             $frauds_auths_intersect,$percent_tmp
.

format NUMAINN =
Number of auths in nonmons ...... -> Total:@######### @###.#### %
                                             $auths_nonmons_intersect,$percent_tmp
.

format NUMAINP =
Number of auths in payments ..... -> Total:@######### @###.#### %
                                             $auths_paymnts_intersect,$percent_tmp
.

format NUMAINPOST =
Number of auths in posts ........ -> Total:@######### @###.#### %
                                             $auths_posts_intersect,$percent_tmp
.

format NUMCINA =
Number of cards in auths ........ -> Total:@######### @###.#### %
                                             $auths_cards_intersect,$percent_tmp
.

format NUMCINF =
Number of cards in frauds ....... -> Total:@######### @###.#### %
                                             $frauds_cards_intersect,$percent_tmp
.

format NUMCINN =
Number of cards in nonmons ...... -> Total:@######### @###.#### %
                                             $cards_nonmons_intersect,$percent_tmp
.

format NUMCINP =
Number of cards in payments ..... -> Total:@######### @###.#### %
                                             $cards_paymnts_intersect,$percent_tmp
.

format NUMCINPOST =
Number of cards in posts ........ -> Total:@######### @###.#### %
                                             $cards_posts_intersect,$percent_tmp
.

format NUMNINA =
Number of nonmons in auths ...... -> Total:@######### @###.#### %
                                             $auths_nonmons_intersect,$percent_tmp
.

format NUMNINC =
Number of nonmons in cards ...... -> Total:@######### @###.#### %
                                             $cards_nonmons_intersect,$percent_tmp
.

format NUMNINF =
Number of nonmons in frauds ..... -> Total:@######### @###.#### %
                                             $frauds_nonmons_intersect,$percent_tmp
.

format NUMNINP =
Number of nonmons in payments ... -> Total:@######### @###.#### %
                                             $nonmons_paymnts_intersect,$percent_tmp
.

format NUMNINPOST =
Number of nonmons in posts ...... -> Total:@######### @###.#### %
                                             $nonmons_posts_intersect,$percent_tmp
.

format NUMPINA =
Number of paymnts in auths ...... -> Total:@######### @###.#### %
                                             $auths_paymnts_intersect,$percent_tmp
.

format NUMPINC =
Number of paymnts in cards ...... -> Total:@######### @###.#### %
                                             $cards_paymnts_intersect,$percent_tmp
.

format NUMPINF =
Number of paymnts in frauds ..... -> Total:@######### @###.#### %
                                             $frauds_paymnts_intersect,$percent_tmp
.

format NUMPINN =
Number of paymnts in nonmons..... -> Total:@######### @###.#### %
                                             $nonmons_paymnts_intersect,$percent_tmp
.

format NUMPINPOST =
Number of paymnts in posts ...... -> Total:@######### @###.#### %
                                             $paymnts_posts_intersect,$percent_tmp
.

format NUMPOSTINA =
Number of posts in auths ........ -> Total:@######### @###.#### %
                                             $auths_posts_intersect,$percent_tmp
.

format NUMPOSTINC =
Number of posts in cards ........ -> Total:@######### @###.#### %
                                             $cards_posts_intersect,$percent_tmp
.

format NUMPOSTINF =
Number of posts in frauds ....... -> Total:@######### @###.#### %
                                             $frauds_posts_intersect,$percent_tmp
.

format NUMPOSTINN =
Number of posts in nonmons ...... -> Total:@######### @###.#### %
                                             $nonmons_posts_intersect,$percent_tmp
.

format NUMPOSTINP =
Number of posts in payments ..... -> Total:@######### @###.#### %
                                             $paymnts_posts_intersect,$percent_tmp
.

format NUMAINCF =
Number of auths in cards + frauds -> Total:@######### @###.#### %
                                             $auths_cards_frauds_intersect,$percent_tmp
.

format NUMCINAF =
Number of cards in auths + frauds -> Total:@######### @###.#### %
                                             $auths_cards_frauds_intersect,$percent_tmp
.

format NUMFINAC =
Number of frauds in auths + cards -> Total:@######### @###.#### %
                                             $auths_cards_frauds_intersect,$percent_tmp
.

format NUMPINCF =
Number of posts in cards + frauds -> Total:@######### @###.#### %
                                             $posts_cards_frauds_intersect,$percent_tmp
.

format NUMCINPF =
Number of cards in posts + frauds -> Total:@######### @###.#### %
                                             $posts_cards_frauds_intersect,$percent_tmp
.

format NUMFINPC =
Number of frauds in posts + cards -> Total:@######### @###.#### %
                                             $posts_cards_frauds_intersect,$percent_tmp
.

