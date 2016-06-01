#! /usr/local/bin/perl -w 

##############################################################
#  This script to get auth/card (F/NF) monthly distribution  #
#  Put them into hashes and print it out as hbar.png.        #
#  Also, get each hash for estimated active account.         #
#                                                            #
#  yxc: created on 12/2004                                   #
#                                                            #
##############################################################

#use gdchart0.94b;
$usage = 
  "$0 client startDate:endDate \n\n".
  "Generate png plot for AUTH (Fraud/NonFraud) and CARD (Fraud/NonFraud) \n".
  "Example: $0 FIRSTUN 200301:200312\n\n ";

### >>>>>>>> Change here if the directory is different<<<<<<<<<<<<<####
$indir="/work/aisle03/FDR_sample/FUSA/0307_0409_1pct/merge";
#######################################################################

open (FRAUD, "/usr/local/bin/zcat $indir/frauds.dat.gz|") || die "Couldn't open  frauds.dat.gz\n";
open (FRAU, "| gzip -c >  $indir/auths.frauds.dat.gz ") || die "Couldn't write auth.frauds.dat.gz\n";

open (NFAU, "| gzip -c > $indir/auths.nonfrauds.dat.gz") || die "Couldn't write auths.nonfrauds.dat.gz\n";


while (<FRAUD>){
    $key = substr($_, 0, 19); 
    $keys{$key} = 1; 
}
close(FRAUD);

######### Seperate Auths
open $file , "/usr/local/bin/zcat $indir/auths.dat.gz |"  || die "Couldn't open auths.dat.gz \n";
  while ($line=<$file>){
     $authAcct = substr($line, 0, 19);
        if (defined($keys{$authAcct})){
           print FRAU $line; 
        } else {
           print NFAU $line;
        }
    
   }
  close($file);

