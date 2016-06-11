###############################################################################
##                                                                           ##
##    Copyright (c) 2000  by HNC Software                                    ##
##    All rights reserved.                                                   ##
##                                                                           ##
##    This package reads the Consortium data format.                         ##
##                                                                           ##
##    Author: Armand Sarkissian (axs@hnc.com)                                ##
##                                                                           ##
###############################################################################
#
# This packages provides seven procedure for reading Consortium files:
#
#      	unpack_cards
#       unpack_auths
#      	unpack_nonmons
#      	unpack_posts
#      	unpack_payments
#      	unpack_frauds
#	unpack_api
#       unpack_scr
#      	pack_inqs
#       pack_cards
#       pack_auths
#       pack_nonmons
#       pack_posts
#       pack_payments
#       pack_frauds
#       pack_inqs
#
# Each function returns a "structure" accessible using the exact naming
# convention of the consortium specification.
#
# Example of use: 
# ---------------
# #!/usr/local/bin/perl
# use lib ("/work/price/falcon/local/perlmod/");
#
# use Consortium qw (unpack_nonmons);
#
# while (<>)
# {
#     $nm= unpack_nonmons($_);
#     print "$nm->{'acct-id'} , $nm->{'tran-type'}, " . 
#           "$nm->{'tran-date'}, $nm->{'postal-code'}, $nm->{'cred-line'} \n";
#            
# }
#
#
package Consortium;
require Exporter;

@ISA= qw(Exporter);
@EXPORT= qw();

@EXPORT_OK = qw(
		unpack_cards
		unpack_auths
		unpack_nonmons
		unpack_posts
		unpack_payments
		unpack_frauds
		unpack_inqs
		unpack_api
		unpack_scr
		pack_cards
		pack_auths
		pack_nonmons
		pack_posts
		pack_payments
		pack_frauds
		pack_inqs
		);

%EXPORT_TAGS = (all => [@EXPORT_OK]);

use vars qw ( $Version );

$Consortium::Version = "0.03";

#
# Format templates for unpack
my $frauds_template= "A19 A1 A8 A1 A6 A8 A6"; 
my $auths_template= "A19 A8 A6 A13 A3 A13 A1 A1 A10 A10 A4 A9 A3 A1 A1 A1 A8 A6 A5 A1 A16 A6 A1 A13 A4 A1 A40"; 
my $cards_template= "A19 A9 A3 A8 A8 A1 A8 A10 A1 A8 A10 A1 A1 A3 A8 A26 A26 A5 A1 A1 A19 A2 A8";
my $payments_template= "A19 A13 A13 A13 A13 A10 A8 A8 A8 A1 A1 A1";
my $nonmons_template= "A19 A1 A8 A9 A10";
my $inqs_template= "A19 A1 A8 A6";
my $posts_template= "A19 A13 A3 A13 A8 A8 A6 A10 A10 A1 A1 A4 A9 A3 A6 A6 A16 A6 A1 A13 A4 A1 A40";
my $api_template= "A19 A1 A9 A3 A8 A8 A1 A8 A8 A10 A10 A1 A8 A3 A10 A1 A1 A8 A6 A13 A3 A13 A1 A1 A4 A9 A3 A1 A1 A1 A8 A1 A35 A4 A2 A2 A2";
#my $api_template= "A19 A1 A9 A3 A8 A8 A1 A8 A8 A10 A10 A1 A8 A3 A10 A1 A1 A8 A6 A13 A3 A13 A1 A1 A4 A9 A3 A1 A1 A1 A8 A1 A9 A10 A13 A13 A13 A8 A8 A1 A16 A4 A2 A2 A2";
my $scr_template= "A19 A14 A1 A8 A10 A6 A1 A1 A1 A8 A1 A8 A1 A1 A1 A1 A1 A4 A1 A1 A1 A1 A1 A1 A1 A4 A1 A7 A1 A7 A2 A9 A2 A9 A2 A3 A1 A16";
#-----------------------------------------------------------------------
sub new_card {
#-----------------------------------------------------------------------
#
# We use a reference to an anonymous hash to store the complex structure.
# Note the {} braces instead of the ().
    my $card = {
	"acct-id"        =>   $_[0],  
	"postal-code"    =>   $_[1],  
	"card-cntry"     =>   $_[2],  
	"open-date"      =>   $_[3],  
	"issue-date"     =>   $_[4],  
	"issue-type"     =>   $_[5],  
	"expire-date"    =>   $_[6],  
	"cred-line"      =>   $_[7],  
	"gender"         =>   $_[8],  
	"birthdate"      =>   $_[9],  
	"income"         =>   $_[10], 
	"type"           =>   $_[11], 
	"use"            =>   $_[12], 
	"num-cards"      =>   $_[13], 
	"rec-date"       =>   $_[14], 
	"addr-line-1"    =>   $_[15], 
	"addr-line-2"    =>   $_[16], 
	"ssn-5"          =>   $_[17], 
	"assoc"          =>   $_[18], 
	"incentive"      =>   $_[19], 
	"xref-prev-acct" =>   $_[20], 
	"status"         =>   $_[21], 
	"status-date"    =>   $_[22],  
	};		      
    return $card;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub unpack_cards {
#-----------------------------------------------------------------------
    my (
	$account,       # account id 
	$postcode,      # postal code 
	$cardcnty,      # country 
	$opendate,      # open date 
	$issudate,      # issue date 
	$issutype,      # issue type 
	$exprdate,      # expiration date 
	$credline,      # credit line 
	$cardgndr,      # gender 
	$brthdate,      # birth date 
	$income,        # income 
	$cardtype,      # type 
	$carduse,       # use 
	$numcard,       # number of cards 
	$recdate,       # recording date 
	$addr1,         # address 1  
	$addr2,         # address 2 
	$ssn5,          # ssn-5 
	$assoc,         # association 
	$incentiv,      # incentive 
	$prev_acc,      # encrypted previous account id 
	$status,        # card status 
	$statdate       # card status date
	)= unpack($cards_template, $_[0]);
#
# We use a reference to an anonymous hash to store the complex structure.
# Note the {} braces instead of the ().

    my $card = new_card($account,   
			$postcode,  
			$cardcnty,  
			$opendate,  
			$issudate,  
			$issutype,  
			$exprdate,  
			$credline,  
			$cardgndr,  
			$brthdate,  
			$income,    
			$cardtype,  
			$carduse,   
			$numcard,   
			$recdate,   
			$addr1,     
			$addr2,     
			$ssn5,      
			$assoc,     
			$incentiv,  
			$prev_acc,  
			$status,
			$statdate
			);
    return $card;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub new_auth {
#-----------------------------------------------------------------------
    my $auth= {
	"acct-id"         =>   $_[0],  
	"date"            =>   $_[1],  
	"time"            =>   $_[2],  
	"amount"          =>   $_[3],  
	"curr-code"       =>   $_[4],  
	"curr-rate"       =>   $_[5],  
	"decision"        =>   $_[6],  
	"tran-type"       =>   $_[7],  
	"avail-cred"      =>   $_[8],  
	"cred-line"       =>   $_[9],  
	"sic"             =>   $_[10], 
	"postal-code"     =>   $_[11], 
	"mrch-cntry"      =>   $_[12], 
	"pin-ver"         =>   $_[13], 
	"auth-cvv"        =>   $_[14], 
	"key-swipe"       =>   $_[15], 
	"msg-expire-date" =>   $_[16], 
	"auth-id"         =>   $_[17], 
	"reason-code"     =>   $_[18], 
	"advice"          =>   $_[19], 
	"mrch-id"         =>   $_[20], 
	"term-id"         =>   $_[21], 
	"which-card"      =>   $_[22], 
	"cashback-amt"    =>   $_[23],
	"falcon-score"    =>   $_[24],
	"filler"          =>   $_[25],
	"merchant-name"   =>   $_[26] 
	};
    return $auth;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub unpack_auths {
#-----------------------------------------------------------------------
    my (
	$account,      # account id 
	$authdate,     # date 
	$authtime,     # time 
	$amount,       # amount 
	$currcode,     # currency code 
	$currrate,     # currency rate 
	$authdeci,     # decision 
	$trantype,     # transaction type 
	$availbal,     # available credit 
	$totalbal,     # credit line 
	$siccode,      # sic code 
	$postcode,     # postal code 
	$mrchcnty,     # merchant country 
	$pinver,       # pin verification 
	$authcvv,      # cvv 
	$keyswipe,     # key swipe 
	$expdate,      # expiration date 
	$authid,       # authorization id 
	$reason,       # reason code 
	$advice,       # assoc advice 
	$mrchid,       # merchant id 
	$termid,       # terminal id 
	$whichcrd,     # which card 
	$cshbkamt,     # cash back amount 
	$falscore,     # falcon score 
	$filler,       # filler
	$mrchname      # merchant name
	)= unpack($auths_template, $_[0]);
    

    my $auth= new_auth($account,  
		       $authdate, 
		       $authtime, 
		       $amount,   
		       $currcode, 
		       $currrate, 
		       $authdeci, 
		       $trantype, 
		       $availbal, 
		       $totalbal, 
		       $siccode,  
		       $postcode, 
		       $mrchcnty, 
		       $pinver,   
		       $authcvv,  
		       $keyswipe, 
		       $expdate,  
		       $authid,   
		       $reason,   
		       $advice,   
		       $mrchid,   
		       $termid,   
		       $whichcrd, 
		       $cshbkamt, 
		       $falscore, 
		       $filler,   
		       $mrchname       
		       );
    return $auth;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub new_nonmon {
#-----------------------------------------------------------------------
    my  $nonmon = {
	"acct-id"     =>   $_[0],
	"tran-type"   =>   $_[1],
	"tran-date"   =>   $_[2],
	"postal-code" =>   $_[3],
	"cred-line"   =>   $_[4],
    };
    return $nonmon;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub unpack_nonmons {
#-----------------------------------------------------------------------
    my (
	$account,         # account id       
	$trantype,        # transaction type 
	$trandate,        # transaction date
	$postcode,        # postal code 
	$credline         # credit line 
	)= unpack($nonmons_template, $_[0]);
    

    my  $nonmon = new_nonmon($account,
			     $trantype,
			     $trandate,
			     $postcode,
			     $credline,
			     );
    
    return $nonmon;
}
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub new_post {
#-----------------------------------------------------------------------

    my $post= {
	"acct-id"        =>   $_[0], 
	"amount"         =>   $_[1], 
	"curr-code"      =>   $_[2], 
	"curr-rate"      =>   $_[3], 
	"post-date"      =>   $_[4], 
	"tran-date"      =>   $_[5], 
	"tran-time"      =>   $_[6], 
	"avail-cred"     =>   $_[7], 
	"cred-line"      =>   $_[8], 
	"tran-type"      =>   $_[9], 
	"authed"         =>   $_[10],
	"sic"            =>   $_[11],
	"postal-code"    =>   $_[12],
	"mrch-cntry"     =>   $_[13],
	"auth-id"        =>   $_[14],
	"post-id"        =>   $_[15],
	"mrch-id"        =>   $_[16],
	"term-id"        =>   $_[17],
	"which-card"     =>   $_[18],
	"cashback-amt"   =>   $_[19],
	"falcon-score"   =>   $_[20],
	"filler"         =>   $_[21],
	"merchant-name"  =>   $_[22]
	};
    return $post;
}
#----------------------------- ------------------------------------------

#-----------------------------------------------------------------------
sub unpack_posts {
#-----------------------------------------------------------------------
    my (
	$account,             # account id 
	$amount,              # amount 
	$currcode,            # currency code 
	$currrate,            # currency rate 
	$postdate,            # posting date 
	$trandate,            # transaction date 
	$trantime,            # trans time 
	$availcrd,            # available credit 
	$credline,            # credit line 
	$trantype,            # transaction type 
	$postauth,            # posting authorized 
	$postsic,             # sic code 
	$postcode,            # merchant postal code 
	$mrchcnty,            # merchant country
	$authid,              # authorization id 
	$postid,              # posting id 
	$mrchid,              # merchant id 
	$termid,              # terminal id 
	$whichcrd,            # which card 
	$cashback,            # cash back amount 
	$falscore,            # falcon score 
	$filler,              # filler
	$mrchname             # merchant name
	)= unpack($posts_template, $_[0]);

    my $post= new_post($account,   
		       $amount,    
		       $currcode,  
		       $currrate,  
		       $postdate,  
		       $trandate,  
		       $trantime,  
		       $availcrd,  
		       $credline,  
		       $trantype,  
		       $postauth,  
		       $postsic,   
		       $postcode,  
		       $mrchcnty,  
		       $authid,    
		       $postid,    
		       $mrchid,    
		       $termid,    
		       $whichcrd,  
		       $cashback,  
		       $falscore,  
		       $filler,    
		       $mrchname
		       );
    return $post;
}
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub new_payment {
#-----------------------------------------------------------------------
    my $pay = {
	"acct-id"       =>    $_[0], 
	"amount"        =>    $_[1], 
	"min-due"       =>    $_[2], 
	"filler"        =>    $_[3], 
	"balance"       =>    $_[4], 
	"cred-line"     =>    $_[5], 
	"date"          =>    $_[6], 
	"due-date"      =>    $_[7], 
	"state-date"    =>    $_[8], 
	"reversal"      =>    $_[9], 
	"payment-meth"  =>    $_[10],
	"payment-type"  =>    $_[11]
	};
    return $pay
    }
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub unpack_payments {
#-----------------------------------------------------------------------
    my (
	$account,         # account id       
	$pmtamt,          # payment amount   
	$mindue,          # minimum due      
	$filler,          # filler
	$balance,         # balance          
	$credline,        # credit line      
	$pmtdate,         # payment date     
	$duedate,         # due date         
	$statdate,        # statement date   
	$reversal,        # payment reversal 
	$pmtmeth,         # payment method   
	$pmttype          # payment type   
	)= unpack($payments_template, $_[0]);

    my $pay = new_payment($account,
			  $pmtamt,     
			  $mindue,     
			  $filler,     
			  $balance,    
			  $credline,   
			  $pmtdate,    
			  $duedate,    
			  $statdate,   
			  $reversal,
			  $pmtmeth,
			  $pmttype     
			  );    
    return $pay
    }
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub new_fraud {
#-----------------------------------------------------------------------
    my $fraud= {
	"acct-id"           =>   $_[0],
	"type"              =>   $_[1],
	"date-first-fraud"  =>   $_[2],
	"find-method"       =>   $_[3],
	"time-first-fraud"  =>   $_[4],
	"detected-date"     =>   $_[5],
	"detected-time"     =>   $_[6]   
	};

    return $fraud;
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub unpack_frauds {
#-----------------------------------------------------------------------
    my (
	$account,     # account id 
	$fraudtyp,    # type 
	$frddate,     # fraud date 
	$ffind,       # fraud find method 
	$frdtime,     # fraud time 
	$detdate,     # fraud detection date 
	$dettime      # FRAUD DETECION TIME 
	)= unpack($frauds_template, $_[0]);

    my $fraud= new_fraud($account,   
			 $fraudtyp,  
			 $frddate,   
			 $ffind,     
			 $frdtime,   
			 $detdate,   
			 $dettime    
			 );
    return $fraud;
}
#-----------------------------------------------------------------------



#-----------------------------------------------------------------------
sub new_inq {
#-----------------------------------------------------------------------
    my $inq= {
	"acct-id"  =>    $_[0],
	"type"     =>    $_[1],
	"date"     =>    $_[2],
	"time"     =>    $_[3]
	};
    return $inq;
    
}

#-----------------------------------------------------------------------
sub unpack_inqs {
#-----------------------------------------------------------------------
    my (
	$account,     # account id       
	$type,        # inquiry type 
	$date,        # transaction date
	$time         # transaction time 
	)= unpack($inqs_template, $_[0]);

    my $inq= new_inq($account, 
		     $type, 
		     $date, 
		     $time);

    return $inq;
}


#-----------------------------------------------------------------------
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub pack_cards {
#-----------------------------------------------------------------------
    
    my ($card)= @_;
    
    return pack($cards_template,
		$card->{'acct-id'},
		$card->{'postal-code'},
		$card->{'card-cntry'},
		$card->{'open-date'},
		$card->{'issue-date'},
		$card->{'issue-type'},
		$card->{'expire-date'},
		$card->{'cred-line'},
		$card->{'gender'},
		$card->{'birthdate'},
		$card->{'income'},
		$card->{'type'},
		$card->{'use'},
		$card->{'num-cards'},
		$card->{'rec-date'},
		$card->{'addr-line-1'},
		$card->{'addr-line-2'},
		$card->{'ssn-5'},
		$card->{'assoc'},
		$card->{'incentive'},
		$card->{'xref-prev-acct'},
		$card->{'status'},
		$card->{'status-date'}
		)
}                                         
#-----------------------------------------------------------------------
                                         

#-----------------------------------------------------------------------
sub pack_auths {
#-----------------------------------------------------------------------
    
    my ($auth)= @_;
    
    return pack($auths_template,
		$auth->{'acct-id'},
		$auth->{'date'},
		$auth->{'time'},
		$auth->{'amount'},
		$auth->{'curr-code'},
		$auth->{'curr-rate'},
		$auth->{'decision'},
		$auth->{'tran-type'},
		$auth->{'avail-cred'},
		$auth->{'cred-line'},
		$auth->{'sic'},
		$auth->{'postal-code'},
		$auth->{'mrch-cntry'},
		$auth->{'pin-ver'},
		$auth->{'auth-cvv'},
		$auth->{'key-swipe'},
		$auth->{'msg-expire-date'},
		$auth->{'auth-id'},
		$auth->{'reason-code'},
		$auth->{'advice'},
		$auth->{'mrch-id'},
		$auth->{'term-id'},
		$auth->{'which-card'},
		$auth->{'cashback-amt'},
		$auth->{'falcon-score'},
		$auth->{'filler'},
		$auth->{'merchant-name'}
		);
}                                         
#-----------------------------------------------------------------------
                                         

#-----------------------------------------------------------------------
sub pack_inqs {
#-----------------------------------------------------------------------
    
    my ($nonmon)= @_;
    
    return pack($inqs_template,
		$nonmon->{'acct-id'},
		$nonmon->{'tran-type'},
		$nonmon->{'tran-date'},
		$nonmon->{'postal-code'},
		$nonmon->{'cred-line'}
		);
}
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
sub pack_posts {
#-----------------------------------------------------------------------
    
    my ($post)= @_;
    
    return pack($posts_template,
		$post->{'acct-id'},
		$post->{'amount'},
		$post->{'curr-code'},
		$post->{'curr-rate'},
		$post->{'post-date'},
		$post->{'tran-date'},
		$post->{'tran-time'},
		$post->{'avail-cred'},
		$post->{'cred-line'},
		$post->{'tran-type'},
		$post->{'authed'},
		$post->{'sic'},
		$post->{'postal-code'},
		$post->{'mrch-cntry'},
		$post->{'auth-id'},
		$post->{'post-id'},
		$post->{'mrch-id'},
		$post->{'term-id'},
		$post->{'which-card'},
		$post->{'cashback-amt'},
		$post->{'falcon-score'},
		$post->{'filler'},
		$post->{'merchant-name'}
		);
}
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub pack_payments {
#-----------------------------------------------------------------------
    
    my ($pay)= @_;
    
    return pack($payments_template,
		$pay->{'acct-id'},
		$pay->{'amount'},
		$pay->{'min-due'},
		$pay->{'filler'},
		$pay->{'balance'},
		$pay->{'cred-line'},
		$pay->{'date'},
		$pay->{'due-date'},
		$pay->{'state-date'},
		$pay->{'reversal'},
		$pay->{'payment-meth'},
		$pay->{'payment-type'}
		);
}                       
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
sub pack_frauds {
#-----------------------------------------------------------------------
    
    my ($fraud)= @_;
    
    return pack($frauds_template,
		$fraud->{'acct-id'},
		$fraud->{'type'},
		$fraud->{'date-first-fraud'},
		$fraud->{'find-method'},
		$fraud->{'time-first-fraud'},
		$fraud->{'detected-date'},
		$fraud->{'detected-time'},
		);
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------



#-----------------------------------------------------------------------
sub new_api {
#-----------------------------------------------------------------------
    my $api= {
	"account_number"           =>   $_[0], 
	"auth_post_flag"	   =>   $_[1], 
	"postcode"		   =>   $_[2], 
	"card_cntry_code"	   =>   $_[3], 
	"account_open_date"	   =>   $_[4], 
	"new_plastic_date"	   =>   $_[5], 
	"plastic_issue_type"	   =>   $_[6], 
	"account_expire_date"	   =>   $_[7], 
	"card_expire_date"	   =>   $_[8], 
	"avail_credit"		   =>   $_[9], 
	"credit_line"		   =>   $_[10],
	"gender"		   =>   $_[11],
	"birthdate"		   =>   $_[12],
	"num_cards"		   =>   $_[13],
	"income"		   =>   $_[14],
	"card_type"		   =>   $_[15],
	"card_use"		   =>   $_[16],
	"tran_date"		   =>   $_[17],
	"tran_time"		   =>   $_[18],
	"tran_amount"		   =>   $_[19],
	"tran_curr_code"	   =>   $_[20],
	"tran_curr_conv"	   =>   $_[21],
	"auth_result"		   =>   $_[22],
	"tran_type"		   =>   $_[23],
	"merch_cat"		   =>   $_[24],
	"merch_post_code"	   =>   $_[25],
	"merch_cntry_code"	   =>   $_[26],
        "auth_pin_verify"          =>   $_[27], 
        "auth_cvv_verify"          =>   $_[28], 
        "auth_keyed_swiped"	   =>   $_[29],
	"post_date"		   =>   $_[30],
	"post_authed"		   =>   $_[31],
#	"new_postcode"		   =>   $_[32],
#	"new_credit_line"	   =>   $_[33],
#	"minimum_due"		   =>   $_[34],
#	"total_due"		   =>   $_[35],
#	"balance"                  =>   $_[36],
#	"payment_due_date"	   =>   $_[37],
#	"statement_date"	   =>   $_[38],
#	"deposit_method"	   =>   $_[39],
#	"merch_id"	           =>   $_[40],
	"score"		           =>   $_[32],
	"reason_1"	           =>   $_[33],
	"reason_2"	           =>   $_[34],
	"reason_3"	           =>   $_[35]
	};
    return $api;
    
}

#-----------------------------------------------------------------------
sub unpack_api {
#-----------------------------------------------------------------------
    my (
	$account_number,
	$auth_post_flag,
	$postcode,
	$card_cntry_code,
	$account_open_date,
	$new_plastic_date,
	$plastic_issue_type,
	$account_expire_date,
	$card_expire_date,
	$avail_credit,
	$credit_line,
	$gender,
	$birthdate,
	$num_cards,
	$income,
	$card_type,
	$card_use,
	$tran_date,
	$tran_time,
	$tran_amount,
	$tran_curr_code,
	$tran_curr_conv,
	$auth_result,
	$tran_type,
	$merch_cat,
	$merch_post_code,
	$merch_cntry_code,
	$auth_pin_verify,
	$auth_cvv_verify,
	$auth_keyed_swiped,
	$post_date,
	$post_authed,
#	$new_postcode,
#	$new_credit_line,
#	$minimum_due,
#	$total_due,
#	$balance,
#	$payment_due_date,
#	$statement_date,
#	$deposit_method,
#	$merch_id,
	$filler,
	$score,
	$reason_1,
	$reason_2,
	$reason_3
	)= unpack($api_template, $_[0]);

    my $api= new_api(
		     $account_number,
		     $auth_post_flag,
		     $postcode,
		     $card_cntry_code,
		     $account_open_date,
		     $new_plastic_date,
		     $plastic_issue_type,
		     $account_expire_date,
		     $card_expire_date,
		     $avail_credit,
		     $credit_line,
		     $gender,
		     $birthdate,
		     $num_cards,
		     $income,
		     $card_type,
		     $card_use,
		     $tran_date,
		     $tran_time,
		     $tran_amount,
		     $tran_curr_code,
		     $tran_curr_conv,
		     $auth_result,
		     $tran_type,
		     $merch_cat,
		     $merch_post_code,
		     $merch_cntry_code,
		     $auth_pin_verify,
		     $auth_cvv_verify,
		     $auth_keyed_swiped,
		     $post_date,
		     $post_authed,
#		     $new_postcode,
#		     $new_credit_line,
#		     $minimum_due,
#		     $total_due,
#		     $balance,
#		     $payment_due_date,
#		     $statement_date,
#		     $deposit_method,
#		     $merch_id,
		     $score,
		     $reason_1,
		     $reason_2,
		     $reason_3);
    return $api;
}

##

#-----------------------------------------------------------------------
sub new_scr {
#-----------------------------------------------------------------------
    my $scr= {
	"acct-id"                  =>   $_[0], 
	"amount"	           =>   $_[1], 
	"date"		           =>   $_[2], 
	"time"	                   =>   $_[3], 
	"is-fraud"	           =>   $_[4], 
	"date-first-fraud"	   =>   $_[5], 
	"detected-date"	           =>   $_[6], 
	"fraud-type"           	   =>   $_[7], 
	"type"          	   =>   $_[8], 
	"score" 		   =>   $_[9], 
	"decision"		   =>   $_[10],
	"tran-type"	    	   =>   $_[11],
	"key-swipe"	           =>   $_[12],
	"SIC"         		   =>   $_[13],
	"avail-cred"		   =>   $_[14],
	"cred-line"		   =>   $_[15],
	"card-postal-code"	   =>   $_[16],
	"postal-code"		   =>   $_[17],
	"mrch-cntry"		   =>   $_[18],
	"mrch-id"		   =>   $_[19]
	};
    return $scr;
    
}

#-----------------------------------------------------------------------
sub unpack_scr {
#-----------------------------------------------------------------------
    my (
	$acct_id,
	$amount,
        $filler,
	$date,
        $filler,
	$time,
        $filler,
	$is_fraud,
        $filler,
	$date_first_fraud,
        $filler,
	$detected_date,
        $filler,
	$fraud_type,
        $filler,
	$type,
        $filler,
	$score,
        $filler,
	$decision,
        $filler,
	$tran_type,
        $filler,
	$key_swipe,
        $filler,
	$SIC,
        $filler,
	$avail_cred,
        $filler,
	$cred_line,
        $filler,
	$card_postal_code,
        $filler,
	$postal_code,
        $filler,
	$mrch_cntry,
        $filler,
	$mrch_id
	)= unpack($scr_template, $_[0]);

    my $scr= new_scr(
		     $acct_id,
		     $amount,
		     $date,
		     $time,
		     $is_fraud,
		     $date_first_fraud,
		     $detected_date,
		     $fraud_type,
		     $type,
		     $score,
		     $decision,
		     $tran_type,
		     $key_swipe,
		     $SIC,
		     $avail_cred,
		     $cred_line,
		     $card_postal_code,
		     $postal_code,
		     $mrch_cntry,
		     $mrch_id);
    return $scr;
}









