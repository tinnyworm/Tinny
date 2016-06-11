#! /usr/local/bin/perl -w

$usage = "\nUsage: bin_x_mth_freq2.pl [ -f filename,BIN column start,Month column start:<filename,BIN column start,Month column start> ]"
         . "                         [ -h ] \n"
         . " where:\n"
         . "    -h    :   show this message\n\n"
	 . " Note : 1) Please do not leave any blank within the option content. \n\n"
	 . "example: bin_x_mth_freq2.pl  -f auths.dat.gz,1,20:cards.dat.gz,1,91 \n\n";

require "getopts.pl";

&Getopts('f:h');
$file=$opt_f;

die "$usage" unless $opt_f ;

#### split the file array
#$file =~ tr/ //;
@splitfile=split(/:/, $file);

for ($i = 0; $i < @splitfile; $i ++) {
 @tmp=split(/,/, $splitfile[$i]);
 $filename[$i]=$tmp[0];
 $binstart[$i]=$tmp[1]-1;
 $monthstart[$i]=$tmp[2]-1;
# %monthHash[$i] = qw(); 
}

## get bin list
#
#open (BIN, "/usr/local/bin/zcat $binList|" ) || die ("Coundn't open $binList\n");
#while (<BIN>){
# chomp;
# push(@BIN,$_);
#}
#close(BIN);

open (OUT, " |/usr/local/bin/gzip > invalid.gz") || die ("Couldn't write to invalid.gz \n");
for ($i=0; $i<@splitfile; $i ++) {
   open (TEMP, "/usr/local/bin/zcat $filename[$i] |") || die ("Couldn't open $filename[$i]\n");
      while (<TEMP>){
       $bin_num=substr($_,$binstart[$i],6);
       $month=substr($_,$monthstart[$i],6);
   if ($month =~ /^\d+$/ && $month < 200603 && $month > 199000) { 
        $binmonth=$bin_num . $month;
       ($monthHash[$i]{$binmonth} ++) or ($monthHash[$i]{$binmonth}=1);
     } else {
            print OUT "$_\n";} 
	    }
     close(TEMP);
}
close(OUT);

for ($i=0; $i<@splitfile; $i ++) {
   foreach (keys %{ $monthHash[$i]}) {
      push (@bin, substr($_,0,6));
      pust (@month, substr($_,6,6));
   }   
}     


### print the 2-way table ###########

print " \t Bin by month analysis \n";
printf ("%8s   %8s | ","BIN", "Month");
for ($i=1; $i<=@splitfile; $i ++) {
printf ("%10s ","File_$i");
}
print "\n";

@bin_sort= sort {$a <=> $b} @BIN;

#### obtain the files hashes


foreach $bin_tmp (@bin_sort){

### construct a 2-D array
$i=0;   ### i stand for each line 
### store all files into 2-D array
for ($t=0; $t<@splitfile; $t ++) { ### t stand for each column
   foreach $key (keys %{ $monthHash[$t] }){
    $bin_file=substr($key,0,6);
    $month_file=substr($key,6,6);
         
    if ($bin_file eq $bin_tmp)
    {
       $j=0;  ### insert the information into jth line, $t+1 column
       $found=0; # not found yet
       while ($found==0 && $j<$i )
       {
        if ($month_file == $order_array[$j][0])  ### find the month the previous input
	  {
              $order_array[$j][$t+1]= $monthHash[$t]{$key};
	      $found=1; # found
	  }
	         $j++; 
      }
      if ($found==0){  # if the month appears in cards file, add the auth freq; otherwise, add a new line
       $array[0]=$month_file;
       for ($column=0; $column<@splitfile; $column++){
      $array[$column+1]=0;
     }
       $array[$t+1]=$monthHash[$t]{$key};
       $order_array[$i]=[@array];
       $i=$i+1;
     }
   
    }
  }
}

if ($i>0){

### print order the freq and plot
@order_array= sort {$a->[0] <=> $b->[0]} @order_array;
#order the array according to month
$firstmonth=$order_array[0][0];
$lastmonth=$order_array[$i-1][0];
$k=$firstmonth;  # $k is month from each input file 
#print "@order_array\n";
for ($j=0;$j<$i; $j++){
       if ($k % 100 >12) {$k+=88; }
      if ($order_array[$j][0]==$k) #find the freq of the month
                       {
                   
           if ($j==0) {
                printf("%8s   %8s |", $bin_tmp, $order_array[$j][0]);
           } else {
                printf("%8s   %8s |", "",      $order_array[$j][0]);
           }
	   for ($t=0; $t<@splitfile; $t ++) 
	    { 
	    printf("%10d ", $order_array[$j][$t+1]);}
	    printf("\n");
           $k++;
     } else # not find the related month then print zero
     {
           if ($j==0) {
                printf("%8s   %8s |", $bin_tmp, $k);
           } else {
                printf("%8s   %8s |", "",      $k);
           }
	   for ($t=0; $t<@splitfile; $t ++) 
	    { printf("%10d ", 0);}
	    printf("\n");
	    if ($k<$lastmonth)
	    {
	  $j--;
         $k++;
	 }
    }
 }
print "\n";
} 
}
############ subrountines for dedup and adding the comma into the numbers ######## 
sub dedup {
	%seen =();
	@uniqu = grep {! $seen{$_}++} @_;
	@sortuniqu = sort {$a cmp $b} @uniqu;	
	return @sortuniqu;
	}
	#:","$fieldHash[$i]{$key}\n"); 

sub commify {
     my $text = reverse $_[0];
     $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
     return scalar reverse $text;
}
