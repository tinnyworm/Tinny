#! /usr/local/bin/perl -w
## 
##  merge the saved perl Hashes by the common keys and tabulate them with three keys
## yxc 02/2004


$usage = "\nUsage: tableHash_3way.pl [ -f filename:<filename:...> ]\n"
         . "                         [ -h ]  \n"
         . " where:\n"
         . "    -h    :   show this message\n\n"
         . " Note:   1) Please do not leave any blank within the option content. \n\n"
	 . "example:    tableHash_3way.pl -f hash.auths.dat.gz.save:hash.cards.dat.gz.save \n\n";

require "getopts.pl";

&Getopts('f:h');
$file=$opt_f;

die "$usage" unless $opt_f ;

#### split the file array
@splitfile=split(/:/, $file);
for ($i = 0; $i < @splitfile; $i ++) {
    $field[$i][0]= $splitfile[$i];
    }

#### read the hashes structure into program

for ($i=0; $i<@splitfile; $i ++) {
    open (SAVE, "< $field[$i][0]") || die "Couldn't open $field[$i][0]:$!";
    undef $/;  # read in file at once
    eval <SAVE>;
    die "can't recreate the hashes from hash.$field[$i][0].save: $@" if $@;
    close SAVE;
    foreach (keys %{ $fieldHash[$i]}) {
       @fields=split(/:/, $_);
       $num_field=@fields;
       for ($j=0; $j<$num_field; $j ++){
         push (@{ $list[$j]}, $fields[$j]);
              ## extract each field key
      }   
#   print "$_\n";
  }
}
#print "@{ $list[0]} \n";	
###  construct the tables
#### print the title
#print " \t Bin by month analysis \n";
for ($j=0; $j<$num_field; $j ++){
  printf ("%8s  ", "Field_$j");
}  
for ($i=0; $i<@splitfile; $i ++) {
$file[$i]=substr($field[$i][0],5,5);
printf ("%11s ","$file[$i]");
}
print "\n";


### print the hash
#### for option as p for plot

foreach $field_1 (dedup(@{ $list[0]})){
 #    printf ("%8s", "$field_1\n");
     for ($i=0; $i<@splitfile; $i ++) {
	  $total[$i]=0;
     }
	    
     foreach $field_2 (dedup(@{ $list[1]})){
 #        printf ("%16s\n", $field_2);
	 foreach $field_3 (dedup(@{ $list[2]})){
  #           printf ("%24s", $field_3);
                 for ($i=0; $i<@splitfile; $i ++) {
	          $com_key=$field_1 . ":" . $field_2 . ":" . $field_3 . ":";
	            if (exists ($fieldHash[$i]{$com_key})) {
                      printf ("%8s  %8s  %8s %12s\n" ,$field_1, $field_2, $field_3,commify($fieldHash[$i]{$com_key}));
#                      printf ("%12s" , commify($fieldHash[$i]{$com_key}));
		       $total[$i] += $fieldHash[$i]{$com_key};
		    }
#		    else {
#		      printf ("%8s  %8s  %8s %12s" ,$field_1, $field_2, $field_3, "0");
#	   	    }
		  }
#             print "\n";
	 }
#	  print "\n";
    }  
     print "\n";
     printf ("%16s","BIN=$field_1      ");
     for ($i=0; $i<@splitfile; $i ++) {
	printf ("%12s", commify($total[$i]));
     } print "\n\n";
}


#### subrountines for dedup the duplicates elements
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
