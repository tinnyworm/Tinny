#!/usr/local/bin/perl 

$usage = 
"Usage: calib_k.pl [-a|-t] [-p|-k|-s] [-fx] [-gn] <reference research file> <new research file> 
     Multi-purpose score conversion table generator. 
     The conversion table will be spit to standard out.

     <Options>
     -a  : Account level calibration (default).
     -t  : Transaction level calibration. 

    [-p|-k|-s] Use these options to specify the curves to match:
     -p  : FPR (False Positive Rate) vs Score
     -k  : K (case multiplier) vs Score (default)
     -s  : Case (account level) or Score (transaction level) Distribution

     -fx : Fine adjustment relative to the reference curve in x percentage (negative or positive) (x=0 by default).
     -gn : Granularity of the conversion table in n (n=10 by default)
\n";

die $usage if ($#ARGV < 1);

#default configurations:
$fTol=0;$trans=0;$kase=1;$fpr=0;$dist=0;

for ($ac==0 ; $ac<$#ARGV; $ac++){
    $av = $ARGV[$ac];
    if ($av =~ /^-f([+-]*\d+)/){	$fTol = $1/100;}
    elsif ($av =~ /^-g(\d+\.*\d*)/){	$gran = $1;} #Allow non-integers
    elsif ($av eq "-t"){	$trans=1;}
    elsif ($av eq "-a"){	$trans=0;}
    elsif ($av eq "-k"){	$kase=1;$fpr=0;$dist=0;}
    elsif ($av eq "-p"){	$kase=0;$fpr=1;$dist=0;}
    elsif ($av eq "-s"){	$kase=0;$fpr=0;$dist=1;}
    else{	last;    }
}

die "Can't open file $ARGV[$ac]\n$usage" unless open(REF, $ARGV[$ac]);
die "Can't open file $ARGV[$ac+1]\n$usage" unless open(NEW, $ARGV[$ac+1]);

%ref_tbl = buildTable(REF);
%new_tbl = buildTable(NEW);

@new_keys = sortKey(\%new_tbl);
@ref_vals = sortKeyByValue(\%ref_tbl,1);

close REF;
close NEW;

if (defined $gran){
    $max = sprintf("%d", 1000/$gran);
    @keylist = map{$gran*$_}(0..$max);
}else{
    @keylist = sort{$a <=> $b}(keys %new_tbl);
}

foreach $score (@keylist){
    $ref_val = lookupTbl($score, \%new_tbl, \@new_keys);
    $ref_val *= 1-$fTol;
    $new_scr = sprintf("%d",lookupRvs($ref_val, \%ref_tbl, \@ref_vals)+0.5);
    $new_scr = 1 if ($new_scr==0 && $score !=0);
    $new_scr = 999 if ($new_scr==1000);
    print "$score,$new_scr\n";
}

#Builds Score-Value table. What value to put in is determined by the 
#global variables $trans, $fpr, $kase, and $dist
#(I wish I wrote them as function parameters... )
sub buildTable{
    my $fp = shift;
    my %tbl, $atscr, $atfr, $atdr, $atfpr, $atk;

    while($line = <$fp>){
	last if ($trans && ($line =~ /TRANSACTION-BASED INFORMATION/));
	last if ($line =~ /ACCOUNT BASED INFORMATION/);
    }
    while($line = <$fp>){
	last if ($line =~ /Threshold/);
    }
    $line = <$fp>;
    while($line = <$fp>){
	@dataline = split(' ', $line);
	$atscr = $dataline[0];
	if ($trans){
	    $atfr = $dataline[3] if ($dataline[0]==0);
	    $atfpr = $dataline[3];
	    $atdr = sprintf("%f",$dataline[2])+0;
	    last if ($#dataline != 3);
	}else{
	    $atfr = $dataline[1] if ($dataline[0]==0);
	    $atfpr = $dataline[1];
	    $atdr = $dataline[2];
	    last if ($#dataline != 7);
	}
	$atk = ($atfpr+1) * $atdr;
	if ($kase==1){	$tbl{$atscr} = $atk;}
	elsif ($fpr==1){	$tbl{$atscr} = $atfpr;}
	else{$tbl{$atscr} = $atk/$atfr;}
    }
    return %tbl;
}

#Return list of keys in a hashtable sorted by numerical order
#First parameter is reference to hash
#Set optional second parameter to '1' to sort by descending order
sub sortKey{
    my ($tbl, $order) = @_;
    my (@keylist);
    if ($order != 1){
	@keylist = sort{$a <=> $b}(keys %$tbl);
    }else{
	@keylist = sort{$b <=> $a}(keys %$tbl);
    }
    return @keylist;
}

#Return list of keys in a hashtable sorted by numerical order of the 
# *associated value*
#First parameter is reference to hash
#Set optional second parameter to '1' to sort by descending order
sub sortKeyByValue{
    my ($tbl, $order) = @_;
    my (@keylist);
    if ($order !=1){
	@keylist = sort{$$tbl{$a} <=> $$tbl{$b}}(keys %$tbl);
    }else{
	@keylist = sort{$$tbl{$b} <=> $$tbl{$a}}(keys %$tbl);
    }
    return @keylist;
}

#Lookup interpolated value for key 
#First parameter is key, second is reference to hash, optional third is the
#reference to sorted list of keys specifying the order to go through the hash
sub lookupTbl{
#    my ($key, $tbl, @keylist) = @_;
    my ($key, $tbl, $keylist) = @_;
    my ($score, $val, $scr, $ret_val, $prv_val, $prv_scr);
    if (exists $$tbl{$key}){
	return $$tbl{$key};
    }else{
	# keylist is sorted in ascending order of the key itself
	if (!defined $keylist){
	    @$keylist = sort{$a <=> $b}(keys %$tbl);
	}
	foreach $score (@$keylist){
	    $val = $$tbl{$score};
	    $scr = $score;
	    last unless ($score < $key);
	    $prv_val = $val;
	    $prv_scr = $scr;
	}
	$ret_val = $val + ($prv_val - $val) * ($key - $scr) / ($prv_scr - $scr);
	return $ret_val;
    }
}
    
#Reverse lookup, finds the interpolated index with the specified value
#First parameter is key, second is reference to hash, optional third is the
#reference to sorted list of keys specifying the order to go through the hash
sub lookupRvs{
    my ($key, $tbl, $keylist) = @_;
    my ($scr,$ret_scr,$val,$prv_val,$prv_scr,$score);
    # keylist is sorted in descending order of the value in table
    if (!defined $keylist){
	@$keylist = sort{$$tbl{$b} <=> $$tbl{$a}}(keys %$tbl);
    }
    if ($key > $$tbl{$$keylist[0]}){
	$ret_scr = $$keylist[0];
    }elsif ($key < $$tbl{$$keylist[$#$keylist]}){
	$ret_scr = $$keylist[$#$keylist];
    }else{
	foreach $score (@$keylist){
	    $val = $$tbl{$score};
	    $scr = $score;
	    last unless ($val > $key);
	    $prv_val = $val;
	    $prv_scr = $score;
	}
	#Linear interpolation
	$ret_scr = $scr + ($prv_scr - $scr) * ($key - $val) / ($prv_val - $val);
    }
    return $ret_scr;
}
