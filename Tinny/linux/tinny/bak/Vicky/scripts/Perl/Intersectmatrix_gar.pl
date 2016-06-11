#!/usr/local/bin/perl
#
# Intersectmatrix.pl
# PURPOSE:
#    Finds intersection between any given two files,
#
# AUTHOR: gar
# CREATION DATE: 10-4-2005
# Last Modified Date: 01-22-2006
# $Source: /home/gar/Intesection/Intesectmatrix.pl
#
use lib ("/home/gar/Perl_Lib");
use Getopt::Std;
use Spreadsheet::WriteExcel;
use Data::Dumper;
$Data::Dumper::Purity=1;
use strict;
my $usage = "\nUsage: Intersectmatrix.pl [ -a <Authfilelist> ] [ -c <Cardfilelist> ] [-f <fraudlist> ] \n \n " 
         . "     [-p <give position (1 to n) of period in filename should be dot limited Ex .yymm.; .yymm-yymm.>] \n \n"
         . "     [ Ex :For file name authsampsort.0404.gz give -p 2 ] \n \n"
         . "     [-m <give the start month like 200401 used only when -p is not defined>] [ -h show this message] \n \n"
	 . "     The files can be unsorted. If -p is not defined all filelist should  contain same number of files\n \n"
         . "     Gives intersection of row in column  for Self Matrix \n \n"
         . "     If  -p option is not defined assumes the files are in ascending month order\n \n"
         . "     If  -m and -p option is not defined puts 1 to n instead of month in the heading\n \n"
         . "     The position -p and the uniq periods should be same for all file types\n \n"
         . "     When position -p is defined it combines  multiple files with same period for each type and compute intersection\n \n"
         . " Ex : Intersectmatrix.pl -a Authnewlist.txt -p 2   \n \n"
         . " Ex : Intersectmatrix.pl -a Authnewlist.txt -m200405   \n \n"
         . " Ex : Intersectmatrix.pl -a Authnewlist.txt  \n \n";

###########################
# Get the user input

##############################

my $xlsfile='Intersectmatrix';
my (@head,@rsltkey,@Matrix)=();
my ($format,$elapsed_start_time,$interpct1,$interpct2,$ff,$elapsed_end_time);
my ($workbook,$path,$m,$file,$hshname,$cc,$key,$format2,$arrayhead_ref,$file1file2interpct,$file2file1interpct,$k,$i,$j,$file,$hsh_2,$hsh_1);
my %opts=();
my ($worksheet,$acc,$rec,$p,$diag,$g);
my (%Period_auth,%Period_card,%Period_fraud,%Resulthash,$key,$arraymat_ref,%Accthash,%format,%format2,%fraud_hash,%card_hash,%auth_hash,%hash_1,%hash_2,)=();
if ($#ARGV==-1)
	{ 
 	die "\n $usage \n";
}
&getopts('a:c:f:p:m:d:h',\%opts);
die "\n $usage Give atleast Auth Filelist or Card Filelist " unless ( defined($opts{'a'}) || defined($opts{'c'}) );
die "\n $usage "  if (defined($opts{'h'}));


$elapsed_start_time = `date`;

print ("\n Started at $elapsed_start_time\n");
 
######################### Auth and Auth Intersection matrix
 my @aa=(); 
 my @cc=();
 my @ff=();
 my (@kk,@keys,@srtkey,@temp,@Fraudcard,@Fraudauth)=();
 if (defined $opts{'a'}) {
  open (AUT,"$opts{'a'}") || die "Cant open Auth filelist $opts{'a'}"; print "\n Input Auth Filelist $opts{'a'} \n ";
  %Period_auth=();
  my $cnt=0;
  my $mnth;
  while(my $path=<AUT>)
  {
     chomp($path);
     my $mnth="$cnt";
     if (defined $opts{'p'}){
     my @tmp=split(/\//,$path);
     my @temp=split(/\./,$tmp[$#tmp]);
     $mnth="$temp[$opts{'p'}-1]";
        if (defined ($Period_auth{$mnth})) {
         $Period_auth{$mnth}.=", ".$path;
         }
         else {
         $Period_auth{$mnth}=$path; 
          }
        }
     else { 
     $Period_auth{$mnth}=$path;
     }
   $cnt++;
   }
  @aa= keys %Period_auth;@kk=@aa;
 @keys=keys %Period_auth; 
 }

 if (defined $opts{'c'}) {
  open (CRD,"$opts{'c'}") || die "Cant open Card filelist $opts{'c'}"; print "\n Input Card Filelist $opts{'c'} \n ";
   %Period_card=();
  my $cnt=0;
  my $mnth;
  while($path=<CRD>)
  {
     chomp($path);
     $mnth="$cnt";
     if (defined $opts{'p'}){
     my @tmp=split(/\//,$path);
     my @temp=split(/\./,$tmp[$#tmp]);
     $mnth="$temp[$opts{'p'}-1]";
     $Period_card{$mnth}=$path;
     }
     else { 
     $Period_card{$mnth}=$path;
     }
   $cnt++;
   }
  @cc=keys %Period_card ;@kk=@cc;
 @keys=keys %Period_card;
 }
 if (defined $opts{'f'}) {
  open (FRD,"$opts{'f'}") || die "Cant open Fraud filelist $opts{'f'}";print "\n Input Fraud Filelist $opts{'f'} \n ";
   %Period_fraud=();
  my $cnt=0;
  my $mnth;
  while($path=<FRD>)
  {
     chomp($path);
     $mnth="$cnt";
     if (defined $opts{'p'}){
     my @tmp=split(/\//,$path);
     my @temp=split(/\./,$tmp[$#tmp]);
     $mnth="$temp[$opts{'p'}-1]";
     $Period_fraud{$mnth}=$path;
     }
     else { 
     $Period_fraud{$mnth}=$path;
     }
   $cnt++;
   }
  @ff=keys %Period_fraud ;@kk=@ff;
 @keys=keys %Period_fraud;
 }
 
############sort keys

if (defined $opts{'p'}) {
@srtkey= sort {$a cmp $b} @keys; print  ("\n ***Intersect month order @srtkey\n*** ");
@aa = sort {$a cmp $b} @aa; print "\n auth keys @aa  \n";
@cc = sort {$a cmp $b} @cc; print "\n card keys @cc\n";
@ff = sort {$a cmp $b} @ff; print "\n fraud keys @ff\n";
}
else {
@srtkey= sort sortnumeric @keys; print  ("\n ***Intersect month order @srtkey\n*** ");
@aa= sort sortnumeric @aa; print "\n auth keys @aa\n";
@cc= sort sortnumeric @cc; print "\n card keys @cc\n";
@ff= sort sortnumeric @ff; print "\n fraud keys @ff\n";
}


   if (  (!($#aa==-1)) && (!($#cc==-1)) && (!($#aa==$#cc)) && (!(defined $opts{'p'})) )
        { die "No of files in Auth list and Card list  should be same when option p is not defined";}
   elsif ( (!(&ArrayCompare(\@aa,\@cc))) && (!($#aa==-1)) && (!($#cc==-1)) ) 
	 {  die "\n The period for Auth and Cards are different"; }

   if (  (!($#aa==-1)) && (!($#ff==-1)) && (!($#aa==$#ff))&& (!(defined $opts{'p'})) )
       { die "No of files in Auth list and Fraud list  should be same when option p is not defined";}
   elsif ( ($#aa!=-1) && ($#ff!=-1) && (!(&ArrayCompare(\@aa,\@ff))) )
	 {  die "\n The period for Auth and Frauds are different "; } 

  if (  (!($#cc==-1)) && (!($#ff==-1)) && (!($#cc==$#ff)) && (!(defined $opts{'p'})))
       { die "No of files in Card list and Fraud list should be same when option p is not defined";}
  elsif ( ($#cc!=-1) && ($#ff!=-1) && (!(&ArrayCompare(\@ff,\@cc))) )
	 {  die "\n The period for Frauds and Cards are different"; }
 
print("\n no: months $#kk per type \n");





###################Get unique accounts hash 
 ########## Auth hash
if  (!($#aa==-1)) { 
    for ($m=0;$m<$#aa+1;$m++){
      $file=$Period_auth{$srtkey[$m]};print  ("\n$file \n");
         %auth_hash= &Acct_hash($file); # array of hash
         my @temp = split (/\//,$file);
         $hshname="$temp[$#temp].hsh.tmp";   
         open (HSH, "> $hshname");
         print HSH Data::Dumper->Dump([\%auth_hash], ['*auth_hash']);
         close HSH  || die "\n Couldn't close  $hshname \n"; 
          }    
 } 
 ########## Card hash
if (!($cc==-1) ){
        for ($m=0;$m<$#cc+1;$m++){
          $file=$Period_card{$srtkey[$m]};print  ("\n$file\n");
           %card_hash= &Acct_hash($file); # array of hash
          my @temp = split (/\//,$file);
         $hshname="$temp[$#temp].hsh.tmp";
         open (HSH, "> $hshname");
         print HSH Data::Dumper->Dump([\%card_hash], ['*card_hash']);
         close HSH  || die "\n Couldn't close  $hshname \n";        }    
 }


########## Fraud hash
if (!($ff==-1)) {
        for ($m=0;$m<$#ff+1;$m++){
           $file=$Period_fraud{$srtkey[$m]};print  ("\n$file\n");
          my @temp = split (/\//,$file);
         $hshname="$temp[$#temp].hsh.tmp";
         %fraud_hash= &Acct_hash($file); # array of hash
         open (HSH, "> $hshname");
         print HSH Data::Dumper->Dump([\%fraud_hash], ['*fraud_hash']);
         close HSH  || die "\n Couldn't close  $hshname \n";      
       }    
 }
###################################################### End of crreating hash

print  (" Extracting unique accounts Hash over ");

 ###########################Find the intersection authself
 my @Authself=();
 if (!($#aa==-1)) {
   for ($j=0;$j<$#aa+1;$j++)
      {        
       %auth_hash=();%hash_1=();%hash_2=();
         $file=$Period_auth{$srtkey[$j]};
         my @temp = split (/\//,$file);
         $hsh_1="$temp[$#temp].hsh.tmp";
         {
        open (THSH,"$hsh_1")||die"Cant open the $hsh_1";
        local  $/;  # read in file at once
        eval <THSH>;
        die "can't recreate the hashes from  : $@" if $@;
        close THSH or die "can't close SAVE: $!"; 
         }
         %hash_1=%auth_hash; 
         if ($j==0){$diag=1;}
           for ($k=$diag;$k<$#aa+1;$k++){
           $file=$Period_auth{$srtkey[$k]};
            my @temp = split (/\//,$file);
              %auth_hash=();

           $hsh_2="$temp[$#temp].hsh.tmp";
               {
                open (THSH,"$hsh_2")||die"Cant open the $hsh_1";
                local  $/;  # read in file at once
                eval <THSH>;
                die "can't recreate the hashes from  : $@" if $@;
                close THSH or die "can't close SAVE: $!"; 
               }
               %hash_2=%auth_hash; 
               ($interpct1,$interpct2) = &intersect(\%hash_2,\%hash_1); #%of hash1 in hash2
               push(@Authself,$interpct1);
         }
        $diag++;
        }
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Auths in Auths*************************\n");
 my @Matdum= &Matrixdummy($#aa+1);
 &Insert_matrixself(\@Matdum,\@Authself,$#aa+1);
 @{$Resulthash{"Auth Self"}}=@Matdum;
 &print_matrix(\@Matdum,$#aa+1);

}

	######### Find the intersection card self

my @Cardself=();
if (!($#cc==-1)) {
    for ($j=0;$j<$#cc+1;$j++)
      {        
       %card_hash=();%hash_1=();%hash_2=();

        $file=$Period_card{$srtkey[$j]};
         my @temp=split(/\//,$file);
         $hsh_1="$temp[$#temp].hsh.tmp";
         {
        open (THSH,"$hsh_1")||die"Cant open the $hsh_1";
        local  $/;  # read in file at once
        eval <THSH>;
        die "can't recreate the hashes from  : $@" if $@;
        close THSH or die "can't close SAVE: $!"; 
         }
         %hash_1=%card_hash; 
         if ($j==0){$diag=1;}
         for ($k=$diag;$k<$#cc+1;$k++){
         $file=$Period_card{$srtkey[$k]};
         @temp=split(/\//,$file);
         $hsh_2="$temp[$#temp].hsh.tmp";
        %card_hash=();
         {
         open (THSH,"$hsh_2")||die"Cant open the $hsh_2";
         local  $/;  # read in file at once
         eval <THSH>;
         die "can't recreate the hashes from  : $@" if $@;
         close THSH or die "can't close SAVE: $!"; 
         }
            %hash_2=%card_hash;  
            ($interpct1,$interpct2) = &intersect(\%hash_2,\%hash_1); 
             push(@Cardself,$interpct1);
         }
        $diag++;
        }
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Cards in Cards*************************\n");
 my @Matdum= &Matrixdummy($#cc+1);
 &Insert_matrixself(\@Matdum,\@Cardself,$#cc+1);
 @{$Resulthash{"Card Self"}}=@Matdum;
&print_matrix(\@Matdum,$#cc+1);
}


####################Find the intersection of Auth in Card and Card in Auth
my @Authcard=();
my @Cardauth=();
if (($#cc==$#aa) && (!($#cc==-1))){
   for ($j=0;$j<$#aa+1;$j++)
       {
   %card_hash=();%auth_hash=();
     $file=$Period_card{$srtkey[$j]};
     @temp=split(/\//,$file);         
     $hsh_1="$temp[$#temp].hsh.tmp";
         {
        open (THSH,"$hsh_1")||die"Cant open the $hsh_1";
        local  $/;  # read in file at once
        eval <THSH>;
        die "can't recreate the hashes from  : $@" if $@;
        close THSH or die "can't close SAVE: $!"; 
         }
         %hash_1=%card_hash;  
        $file=$Period_auth{$srtkey[$j]};
           @temp=split(/\//,$file);         
          $hsh_2="$temp[$#temp].hsh.tmp";
            {
                open (THSH,"$hsh_2")||die"Cant open the $hsh_2";
                local  $/;  # read in file at once
                eval <THSH>;
                die "can't recreate the hashes from  : $@" if $@;
                close THSH or die "can't close SAVE: $!"; 
               }
               %hash_2=%auth_hash; 
            ($interpct1,$interpct2) = &intersect(\%auth_hash,\%card_hash); #%of hash2 in hash1
             push(@Authcard,$interpct1);
             push(@Cardauth,$interpct2);

           }
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Auths in Card*************************\n\n");
 my @Matdum= &Matrixdummy($#aa+1);
 &Insert_matrixcross(\@Matdum,\@Authcard,$#aa+1);
 @{$Resulthash{"Auth in Card"}}=@Matdum;
 &print_matrix(\@Matdum,$#aa+1);
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Cards in Auths *************************\n\n");
 my @Matdum= &Matrixdummy($#aa+1);
 &Insert_matrixcross(\@Matdum,\@Cardauth,$#aa+1);
 @{$Resulthash{"Card in Auth"}}=@Matdum;
 &print_matrix(\@Matdum,$#aa+1);
 }


############Find the intersection Fraud in Auth
@Fraudauth=();
if (($#ff==$#aa) && (!($#ff==-1))){
   for ($j=0;$j<$#ff+1;$j++)
        {
     %fraud_hash=(); %auth_hash=();
        $file=$Period_fraud{$srtkey[$j]};
       @temp=split(/\//,$file);         
      $hsh_1="$temp[$#temp].hsh.tmp";
         {
            open (THSH,"$hsh_1")||die"Cant open the $hsh_1";
            local  $/;  # read in file at once
            eval <THSH>;
            die "can't recreate the hashes from  : $@" if $@;
            close THSH or die "can't close SAVE: $!"; 
         }
           %hash_1=%fraud_hash; 
          $file=$Period_auth{$srtkey[$j]};
           @temp=split(/\//,$file);         
           $hsh_2="$temp[$#temp].hsh.tmp";
           {
                open (THSH,"$hsh_2")||die"Cant open the $hsh_2";
                local  $/;  # read in file at once
                eval <THSH>;
                die "can't recreate the hashes from  : $@" if $@;
                close THSH or die "can't close SAVE: $!"; 
            }
               %hash_2=%auth_hash; 
            ($interpct1,$interpct2) = &intersect(\%auth_hash,\%fraud_hash); #%of hash2 in hash1
             push(@Fraudauth,$interpct2);
           }
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Frauds in Auths *************************\n\n");
 my @Matdum= &Matrixdummy($#ff+1);
 &Insert_matrixcross(\@Matdum,\@Fraudauth,$#aa+1);
 @{$Resulthash{"Fraud in Auth"}}=@Matdum;
 &print_matrix(\@Matdum,$#ff+1);
}


############Find the intersection Fraud in Card
@Fraudcard=();
if (($#ff==$#cc) && (!($#ff==-1))){
  for ($j=0;$j<$#ff+1;$j++) {
    %card_hash=();%fraud_hash=();

        $file=$Period_fraud{$srtkey[$j]};
         @temp=split(/\//,$file);         
         $hsh_1="$temp[$#temp].hsh.tmp"; 
         {
            open (THSH,"$hsh_1")||die"Cant open the $hsh_1";
            local  $/;  # read in file at once
            eval <THSH>;
            die "can't recreate the hashes from  : $@" if $@;
            close THSH or die "can't close SAVE: $!"; 
         }
           %hash_1=%fraud_hash;  
          $file=$Period_card{$srtkey[$j]};
           @temp=split(/\//,$file);         
          $hsh_2="$temp[$#temp].hsh.tmp"; 
         {
         open (THSH,"$hsh_2")||die"Cant open the $hsh_2";
         local  $/;  # read in file at once
         eval <THSH>;
         die "can't recreate the hashes from  : $@" if $@;
         close THSH or die "can't close SAVE: $!"; 
         }
         %hash_2=%card_hash; 
            ($interpct1,$interpct2) = &intersect(\%card_hash,\%fraud_hash); #%of hash2 in hash1
             push(@Fraudcard,$interpct2);
           }
 print (" \n \n ############################################################################### \n \n");
 print  (" \n************************************Frauds in Cards  *************************\n\n");
 my @Matdum= &Matrixdummy($#ff+1);
 &Insert_matrixcross(\@Matdum,\@Fraudcard,$#ff+1);
 @{$Resulthash{"Fraud in Card"}}=@Matdum;
 &print_matrix(\@Matdum,$#ff+1);
  }

  $elapsed_end_time =`date`;
  print ("\n Finished at $elapsed_end_time\n");

####################################################### Create a new   Excel workbook and keep open through out the loop
`rm *hsh.tmp`; print " \n Removed tmp files \n";
@rsltkey= keys %Resulthash;
   
# Add a worksheet
   my $workbook = Spreadsheet::WriteExcel->new("$xlsfile.xls"); print " \n Add Worksheet \n";
#  Add and define a format
    $format = $workbook->add_format(); # Add a format
    $format->set_color('black');
    $format->set_align('center');
    $format->set_num_format('0.00');
    $format2 = $workbook->add_format();
    %format=%format2;
    $format2->set_align('center');
    $format2->set_bold();
    my @head=();
    $head[0]='Month';
  if ( (!(defined($opts{'p'}))) && (defined($opts{'m'})) ) 
    {
      $head[1]=$opts{'m'};  print "\n -m option  used ---used \n";
       for ($g=2;$g<$#kk+2;$g++){
          if ( substr($head[$g-1],-2)==12 ) { $head[$g]= $head[$g-1] + 89 ;} 
          else { $head[$g]= $head[$g-1] + 1; }
        }
     }
 elsif (defined($opts{'p'})) {
    for ($g=1;$g<$#kk+2;$g++){
       $head[$g]='20'.$srtkey[$g-1];  
      } print "\n -p $opts{'p'} ---used \n";      
  } 
 else {
     for ($g=1;$g<$#kk+2;$g++){
       $head[$g]=$g; 
      }   print "\n -m and -p not---used \n";    
  } print (" \n @head ***\n");


    

  foreach $key (@rsltkey){
    my $worksheet = $workbook->add_worksheet("$key");
    # Fill in the field name
    $worksheet->merge_range("C2:D2", 'Vertical and horizontal', $format2);
    $worksheet->write("C2", "$key   ",$format2);
    # Fill in the field values
    $arrayhead_ref  =\@head;
    $worksheet->write_col('C3',$arrayhead_ref ,$format2);
    $worksheet->write_row('C3',$arrayhead_ref ,$format2);
    @Matrix =@{$Resulthash{"$key"}}; 
    $arraymat_ref  = \@Matrix;
    $worksheet->write_col('D4',$arraymat_ref,$format);
 }
    

    
##############################################################


#####################Sub routines

sub intersect {
        my $ref1=$_[0];
        my $ref2=$_[1];
        my $acct1= keys %$ref1; 
        my $acct2= keys %$ref2;
       if ((!(defined ($acct1))) || ( $acct1==0)){ 
       print ("\n No accounts in hash1 " ); 
       $file1file2interpct='Err';
       $file2file1interpct='Err';
       return ($file1file2interpct,$file2file1interpct);
       };
       if ((!(defined ($acct2))) || ( $acct2==0)){ 
       print ("\n No accounts in hash2 " ); 
       $file1file2interpct='Err';
       $file2file1interpct='Err';
       return ($file1file2interpct,$file2file1interpct);
       };
        my $cnt=0;
        @keys=keys %$ref1;
        foreach $k (@keys) { 
	       if (defined($ref2->{$k}) )
                {
                $cnt++;          
                }
            }
	$file1file2interpct = sprintf ("%.2f",($cnt/$acct1) * 100);
        $file2file1interpct = sprintf ("%.2f",($cnt/$acct2) * 100);
        return ($file1file2interpct,$file2file1interpct);
         }



sub Acct_hash {
        my  $files=$_[0];
        my @temp=split(/\,/,$files);
	%Accthash=();
	foreach $file (@temp) {
        open (ACC,"/usr/local/bin/zcat $file |") || print "\n Problem reading $file \n"; 
         while($rec=<ACC>) 
	      {
		$acc=substr($rec,0,19);
		$Accthash{$acc}=1;
		}
           }
	return %Accthash;	
	}

sub Matrixdummy{
   my $sq=$_[0];
   my @Mat=();
    for ($j=0;$j<$sq;$j++)
        {        
          for ($k=0;$k<$sq;$k++){
               $Mat[$j][$k]='-';
         }
        }
  return @Mat;
}

sub Insert_matrixself{
   my $Matout=$_[0];
   my $Inter=$_[1];
   my $sq=$_[2];
   $p=0;
   for ($j=0;$j<$sq;$j++)
      {        
         if ($j==0){$diag=1;}
         for ($k=$diag;$k<$sq;$k++){
               $Matout->[$k][$j]=$Inter->[$p]; 
           $p++;
         }
        $diag++;
      }
 }

sub Insert_matrixcross{
   my $Matout=$_[0];
   my $Inter=$_[1];
   my $sq=$_[2];
   $p=0;
   for ($j=0;$j<$sq;$j++)
      {        
        $Matout->[$j][$j]=$Inter->[$p];
        $p++;
      }
 }

sub print_matrix{
    my $Matout=$_[0];
    my $sq=$_[1]; 
    print("\t XX");
 for ($g=0;$g<$sq;$g++)
  {
    print ("\t $g");
  } print ("\n \n ");
   for ($j=0;$j<$sq;$j++)
      {        
         print("\t $j");
         for ($k=0;$k<$sq;$k++){
               print ("\t $Matout->[$j][$k]");
         }
      print ("\n \n");
      }
}

sub sortnumeric {
 if ($a < $b) { -1; } 
 elsif ($a>$b) { 1 ;}
 else { 0 ; } 
}

sub ArrayCompare {
 my $Arr_a=$_[0];
 my $Arr_b=$_[1];
 my $cnt=0;
 my $flag=1;
 my $ele_a=0;
 foreach  $ele_a (@$Arr_a) {
          if  ($ele_a ne $Arr_b->[$cnt])  {
	      $flag=0;
	      return $flag;
	  }
   $cnt++;
  } # for
 return $flag;
}











