#!/usr/local/bin/perl -w

my $usage = "getGodelHashValue.pl - \n\t\tgenerates hash values of samples generated using GodelHashSample.\n\t\tSamples are fed in through STDIN. Output to STDOUT. Options:\t\n\
               -h Prints this message.\n\t
               -l Length of account number. Defaults to 19.\n\t
               -s Start byte of account number. Defaults to 0.\n";


use strict;
use Getopt::Std;

my $acct_len = 19;
my $start_byte = 1;

our $opts = 'l:s:h';
our %opt = (); our ($opt_l,$opt_s);
getopts($opts,\%opt) or die "$!"; 

$acct_len = $opt{l} if defined $opt{l};
$start_byte = $opt{s} if defined $opt{s};

die "$usage" if (defined $opt{h});

my @genHashWeights = ();

$genHashWeights[0] = 2.833213344;
$genHashWeights[1] = 2.944438979;
$genHashWeights[2] = 3.135494216;
$genHashWeights[3] = 3.218875825;
$genHashWeights[4] = 3.367295830;
$genHashWeights[5] = 3.433987204;
$genHashWeights[6] = 3.610917913;
$genHashWeights[7] = 3.713572067;
$genHashWeights[8] = 3.761200116;
$genHashWeights[9] = 3.850147602;
$genHashWeights[10] = 3.891820298;
$genHashWeights[11] = 3.970291914;
$genHashWeights[12] = 4.077537444;
$genHashWeights[13] = 4.110873864;
$genHashWeights[14] = 4.204692619;
$genHashWeights[15] = 4.262679877;
$genHashWeights[16] = 4.290459441;
$genHashWeights[17] = 4.369447852;
$genHashWeights[18] = 4.418840608;
$genHashWeights[19] = 4.488636370;
$genHashWeights[20] = 4.574710979;
$genHashWeights[21] = 4.615120517;
$genHashWeights[22] = 4.634728988;
$genHashWeights[23] = 4.672828834;
$genHashWeights[24] = 4.691347882;
$genHashWeights[25] = 4.727387819;
$genHashWeights[26] = 4.795790546;
$genHashWeights[27] = 4.844187086;
$genHashWeights[28] = 4.875197323;
$genHashWeights[29] = 4.919980926;
$genHashWeights[30] = 4.934473933;
$genHashWeights[31] = 5.003946306;
$genHashWeights[32] = 5.017279837;
$genHashWeights[33] = 5.056245805;
$genHashWeights[34] = 5.093750201;
$genHashWeights[35] = 5.117993812;
$genHashWeights[36] = 5.129898715;
$genHashWeights[37] = 5.153291594;
$genHashWeights[38] = 5.187385806;
$genHashWeights[39] = 5.198497031;
$genHashWeights[40] = 5.252273428;
$genHashWeights[41] = 5.262690189;
$genHashWeights[42] = 5.283203729;
$genHashWeights[43] = 5.293304825;
$genHashWeights[44] = 5.351858133;
$genHashWeights[45] = 5.407171771;
$genHashWeights[46] = 5.424950017;
$genHashWeights[47] = 5.433722004;
$genHashWeights[48] = 5.451038454;
$genHashWeights[49] = 5.476463552;
$genHashWeights[50] = 5.484796933;
$genHashWeights[51] = 5.525452939;
$genHashWeights[52] = 5.549076085;
$genHashWeights[53] = 5.572154032;
$genHashWeights[54] = 5.594711380;
$genHashWeights[55] = 5.602118821;
$genHashWeights[56] = 5.624017506;
$genHashWeights[57] = 5.638354669;
$genHashWeights[58] = 5.645446898;
$genHashWeights[59] = 5.666426688;
$genHashWeights[60] = 5.680172609;
$genHashWeights[61] = 5.726847748;
$genHashWeights[62] = 5.739792912;
$genHashWeights[63] = 5.746203191;
$genHashWeights[64] = 5.758901774;
$genHashWeights[65] = 5.802118375;
$genHashWeights[66] = 5.820082930;
$genHashWeights[67] = 5.849324780;
$genHashWeights[68] = 5.855071922;
$genHashWeights[69] = 5.866468057;
$genHashWeights[70] = 5.883322388;
$genHashWeights[71] = 5.888877958;
$genHashWeights[72] = 5.905361848;
$genHashWeights[73] = 5.921578420;
$genHashWeights[74] = 5.937536205;
$genHashWeights[75] = 5.948034989;
$genHashWeights[76] = 5.963579344;
$genHashWeights[77] = 5.983936281;
$genHashWeights[78] = 5.993961427;
$genHashWeights[79] = 6.013715156;
$genHashWeights[80] = 6.037870920;
$genHashWeights[81] = 6.042632834;
$genHashWeights[82] = 6.066108090;
$genHashWeights[83] = 6.070737728;
$genHashWeights[84] = 6.084499413;
$genHashWeights[85] = 6.093569770;
$genHashWeights[86] = 6.107022888;
$genHashWeights[87] = 6.124683391;
$genHashWeights[88] = 6.133398043;
$genHashWeights[89] = 6.137727054;
$genHashWeights[90] = 6.146329258;
$genHashWeights[91] = 6.171700597;
$genHashWeights[92] = 6.188264123;
$genHashWeights[93] = 6.196444128;
$genHashWeights[94] = 6.212606096;
$genHashWeights[95] = 6.220590170;
$genHashWeights[96] = 6.232448017;
$genHashWeights[97] = 6.255750042;
$genHashWeights[98] = 6.259581464;
$genHashWeights[99] = 6.270988432;
$genHashWeights[100] = 6.293419279;
$genHashWeights[101] = 6.304448802;
$genHashWeights[102] = 6.322565240;
$genHashWeights[103] = 6.333279628;
$genHashWeights[104] = 6.343880434;
$genHashWeights[105] = 6.347389210;
$genHashWeights[106] = 6.357842267;
$genHashWeights[107] = 6.375024820;
$genHashWeights[108] = 6.385194399;
$genHashWeights[109] = 6.395261598;
$genHashWeights[110] = 6.398594935;
$genHashWeights[111] = 6.408528791;
$genHashWeights[112] = 6.418364936;
$genHashWeights[113] = 6.424869024;
$genHashWeights[114] = 6.428105273;
$genHashWeights[115] = 6.447305863;
$genHashWeights[116] = 6.463029457;
$genHashWeights[117] = 6.466144724;
$genHashWeights[118] = 6.472346295;
$genHashWeights[119] = 6.481577129;
$genHashWeights[120] = 6.490723535;
$genHashWeights[121] = 6.493753840;
$genHashWeights[122] = 6.511745330;
$genHashWeights[123] = 6.517671273;
$genHashWeights[124] = 6.526494860;
$genHashWeights[125] = 6.538139824;
$genHashWeights[126] = 6.552507887;

while(<>) {
    chomp;
    my $account = substr($_,$start_byte-1,$acct_len); #Keep it simple for the moment

    my $hash = GenerateHashValue($account);
    print "$hash\n";

}

sub GenerateHashValue {

    my @acct = split //,$_[0];
    my $hashVal = 0;

    for (my $i = 0; $i <= $#acct; $i++) {

	if ($acct[$i] eq " ") {
	    $acct[$i] = '\0';
	}

	my $charVal = ord($acct[$i]) - ord('0');
	$hashVal += $charVal*$genHashWeights[$i];

    }

    $hashVal -= int($hashVal);
    $hashVal = int($hashVal * 1000);
    return $hashVal;
}


