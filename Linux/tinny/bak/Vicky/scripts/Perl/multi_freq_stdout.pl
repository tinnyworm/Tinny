#! /usr/local/bin/perl -w

$usage = "\nUsage: multi_freq.pl [ -f filename,field_1 start-end,field_2 start-end,...:<filename,field_1 start-end,field_2 start-end,...> ]"
         . "                         [ -h ] [-s]\n"
         . " where:\n"
			. "    -h    :   show this message\n\n"
			. "    -s    :   want to use STDOUT iff only ONE file is inputed\n\n "
	 . " Note:   1) Please do not leave any blank within the option content. \n\n"
	 . "example:    multi_freq.pl -f  auths.dat.gz,1-6,20-25:cards.dat.gz,1-6,91-96 \n\n";

require "getopts.pl";

&Getopts('f:hs');
$file=$opt_f;

die "$usage" unless $opt_f ;
use Data::Dumper;
$Data::Dumper::Purity=1;
#### split the file array
@splitfile=split(/:/, $file);

for ($i = 0; $i < @splitfile; $i ++) {
    @tmp=split(/,/, $splitfile[$i]);
    $field[$i][0]=$tmp[0];
    for ($j = 1; $j < @tmp; $j ++){
        if ( $tmp[$j] =~ /(\d+)-(\d+)/ ) {
	   $field[$i][$j+$j-1]= $1 -1;
	   $field[$i][$j+$j] = $2 - $1 + 1;
           die "Bad column specification\n" if $field[$i][$j+$j] < 0 || $field[$i][$j+$j-1]< 0;
          } else {
  	   die "Bad column specification\n";
          }
     }
}


for ($i=0; $i<@splitfile; $i ++) {

	 if ( $field[$i][0] =~ m/\.gz$/ || $field[$i][0] =~ m/\.Z$/){
		  $OP = "/usr/local/bin/zcat $field[$i][0] |";
	 } else {
	 	  $OP = "< $field[$i][0]"; 
    }
   open (TEMP, "$OP") || die ("Couldn't open $field[$i][0]\n");
      while (<TEMP>){
      $hashkey="";
          for ($j = 1; $j < @tmp; $j ++){
              $tmp1=substr($_,$field[$i][$j+$j-1],$field[$i][$j+$j]);
              $hashkey=$tmp1 . ":" . $hashkey  ;
           }
	   $hashkey=join(":", reverse split(":", $hashkey)) . ":";
	   
#	  $hashkey= $field[$i][0] . $hashkey ;
#         ($fieldHash{$hashkey} ++) or ($fieldHash{$hashkey}=1);
         ($fieldHash[$i]{$hashkey} ++) or ($fieldHash[$i]{$hashkey}=1);
       } 
     close(TEMP);
}

for ($i=0; $i<@splitfile; $i ++) {

	 if ($opt_s) {
	 		print Data::Dumper->Dump([\%{ $fieldHash[$i]}], ['*{ $fieldHash[$i]}']);
	 } else {
 #   		open (SAVE, "> ./hash.$field[$i][0].save") || die "Couldn't open hash.$field[$i][0].save:$!";
   		print Data::Dumper->Dump([\%{ $fieldHash[$i]}], ['*{ $fieldHash[$i]}']);
#			close SAVE   || die "Couldn't close hash.$field[$i][0].save:$!";
	 }
}
