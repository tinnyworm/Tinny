#!/usr/local/bin/perl -w

open(BAITAD, "cat SYSPRIN/advanta.list | ") || die "Couldn't read advanta.list\n";
while(<BAITAD>) {
    $keyad = substr($_, 0, 8);
    $adkeys{$keyad} = 1;
}
close(BAITAD);

open(BAITNA, "cat SYSPRIN/nastycity.list | ") || die "Couldn't read nastycity.list\n";
while(<BAITNA>) {
    $keyna = substr($_, 0, 8);
    $nakeys{$keyna} = 1;
}
close(BAITNA);

open(BAITSU, "cat SYSPRIN/suntrust.list |") || die "Couldn't read suntrust.list\n";
while(<BAITSU>) {
    $keysu = substr($_, 0, 8);
    $sukeys{$keysu} = 1;
}
close(BAITSU);

open(BAITWA, "cat SYSPRIN/wachovia.list | ") || die "Couldn't read wachovia.list\n";
while(<BAITWA>) {
    $keywa = substr($_, 0, 8);
    $wakeys{$keywa} = 1;
}
close(BAITWA);

open(BAITWF, "cat SYSPRIN/wfb.list | ") || die "Couldn't read wfb.list\n";
while(<BAITWF>) {
    $keywf = substr($_, 0, 8);
    $wfkeys{$keywf} = 1;
}
close(BAITWF);

$pond_file = shift(@ARGV); 

$WFB1 = 5518;
$advanta1 = 5962;
$natcity1 = 5170;
$suntrust = 5207;
$wacho = 5095;
$pscu = 9006;
$metris = 5994;

open (FFMAD, " | gzip -c > ffm/ADVANTA/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FPAD, " | gzip -c > fp/ADVANTA/$pond_file") || die "Couldn't write fp $pond_file\n";
open (FFMNA, " | gzip -c > ffm/NATCITY/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FPNA, " | gzip -c > fp/NATCITY/$pond_file") || die "Couldn't write fp $pond_file\n";
open (FFMSU, " | gzip -c > ffm/SUNTRUST/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FPSU, " | gzip -c > fp/SUNTRUST/$pond_file") || die "Couldn't write fp $pond_file\n";
open (FFMWA, " | gzip -c > ffm/WACHOVIA/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FPWA, " | gzip -c > fp/WACHOVIA/$pond_file") || die "Couldn't write fp $pond_file\n";
open (FFMWF, " | gzip -c > ffm/WFB/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FPWF, " | gzip -c > fp/WFB/$pond_file") || die "Couldn't write fp $pond_file\n";
open (FFMPS, " | gzip -c > ffm/PSCU/$pond_file") || die "Couldn't write ffm $pond_file\n";
open (FFMME, " | gzip -c > ffm/METRIS/$pond_file") || die "Couldn't write ffm $pond_file\n";

    while ( <STDIN> ) {
         $fish_key = substr($_, 168, 8);
         $clientID = substr($_, 164, 4);

         if ($clientID eq $WFB1) {
            $countwf ++;
            print FFMWF if (!defined ($wfkeys{$fish_key}));
            print FPWF if (defined ($wfkeys{$fish_key}));

         } elsif ($clientID eq $advanta1){
            $countad ++;
            print FFMAD if (!defined ($adkeys{$fish_key}));
            print FPAD if (defined ($adkeys{$fish_key}));
         
         }  elsif ($clientID eq $natcity1 ){
            $countna ++;
            print FFMNA if (!defined ($nakeys{$fish_key}));
            print FPNA if (defined ($nakeys{$fish_key}));         
           
         }  elsif ($clientID eq $suntrust) {
            $countsu ++;
            print FFMSU if (!defined ($sukeys{$fish_key}));
            print FPSU if (defined ($sukeys{$fish_key}));         
            
         }  elsif ($clientID eq $wacho) {
            $countwa ++;
            print FFMWA if (!defined ($wakeys{$fish_key}));
            print FPWA if (defined ($wakeys{$fish_key}));         
            
         }  elsif ($clientID eq $pscu) {
            $countps ++;
            print FFMPS ;
            
         }  elsif ($clientID eq $metris) {
            $countme ++;
            print FFMME ;
         }
         
    }

    close(FFMAD);
    close(FPAD);
    close(FFMNA);
    close(FPNA);
    close(FFMSU);
    close(FPSU);
    close(FFMWA);
    close(FPWA);
    close(FFMWF);
    close(FPWF);
    close(FFMPS);
    close(FFMME);
    
    $pond_file = "";
 print "ADVANTA : $countad\n";
 print "NACITY  : $countna\n";
 print "SUNTRUST: $countsu\n";
 print "WACHOVIA: $countwa\n";
 print "WFB     : $countwf\n";
 print "PSCU    : $countps\n";
 print "METRIS  : $countme\n";
