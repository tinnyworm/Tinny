#! /usr/local/bin/perl -w
## 
##  merge the saved perl Hashes by the common keys and tabulate them
## yxc 02/2004


$usage = "\nUsage: tableHash.pl [ -f filename:<filename:...> ]"
         . "                         [ -h ] [ -p ] \n"
         . " where:\n"
         . "    -h    :   show this message\n\n"
         . "    -p    :   show the plot of the first file frequency \n\n"
	 . " Note:   1) Please do not leave any blank within the option content. \n\n"
	 . "example:    tableHash.pl -f hash.auths.dat.gz.save:hash.cards.dat.gz.save \n\n";

require "getopts.pl";

&Getopts('f:hp');
$file=$opt_f;

die "$usage" unless $opt_f ||  $opt_p ;

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
  printf ("%8s ", "Field_$j");
}  
for ($i=0; $i<@splitfile; $i ++) {
$file[$i]=substr($field[$i][0],5,10);
printf ("%11s ","$file[$i]");
}
print "\n";

#### catch max

$max=0;
foreach $key (keys %{ $fieldHash[0]}){
       if ($max < $fieldHash[0]{$key}) {
         $max = $fieldHash[0]{$key};
       }
}
#print "max=$max\n";

### print the hash
#### for option as p for plot

if ($opt_p){
foreach $field_1 (dedup(@{ $list[0]})){
   printf ("%8s", "$field_1\n");
     for ($i=0; $i<@splitfile; $i ++) {
      $total[$i]=0;
     }
   foreach $field_2 (dedup(@{ $list[1]})){
      printf ("%16s", $field_2);
      for ($i=0; $i<@splitfile; $i ++) {
        $com_key=$field_1 . ":" . $field_2 . ":";
        if ($i eq 0){
           if (exists ($fieldHash[$i]{$com_key})) {
              $total[$i] += $fieldHash[$i]{$com_key};
              $chart_line="*"x(int(60*($fieldHash[$i]{$com_key}/$max)));
              printf ("| %-60s  %12s" , $chart_line, commify($fieldHash[$i]{$com_key}));
                 
           } else {
              $chart_line="*"x(int(60*(0/$max)));
              printf ("| %-60s  %12s" , $chart_line,  "0");
               
           }                                                                                                         
	 } else {
            if (exists ($fieldHash[$i]{$com_key})) {
              $total[$i] += $fieldHash[$i]{$com_key};
              printf ("%12s" ,commify($fieldHash[$i]{$com_key}));
            } else {
              printf ("%12s" ,"0");
            }                                                       
        }
        print "\n";
	}
    }
        printf ("%16s","BIN=$field_1    ");
	printf ("%60s  "," ");
        for ($i=0; $i<@splitfile; $i ++) {
             printf ("%12s", commify($total[$i]));
        } 
        print "\n\n"; 
    } 

} else {
foreach $field_1 (dedup(@{ $list[0]})){
     printf ("%8s", "$field_1\n");
     
      for ($i=0; $i<@splitfile; $i ++) {
	    $total[$i]=0;
	    }
	    
       foreach $field_2 (dedup(@{ $list[1]})){
           printf ("%16s", $field_2);
#          printf ("%16s", "$field_2\n");
#	    foreach $field_3 (dedup(@{ $list[2]})){
#            printf ("%24s", $field_3);
                  for ($i=0; $i<@splitfile; $i ++) {
		  $com_key=$field_1 . ":" . $field_2 . ":";
#	          $com_key=$field_1 . ":" . $field_2 . ":" . $field_3 . ":";
	          if (exists ($fieldHash[$i]{$com_key})) {
                      printf ("%12s" , commify($fieldHash[$i]{$com_key}));
		       $total[$i] += $fieldHash[$i]{$com_key};
		  }
		  else {
		      printf ("%12s" ,"0");
	   	  }
		}
#              print "\n";
#	    }
	     print "\n";
        }  
	printf ("%16s","BIN=$field_1      ");
	for ($i=0; $i<@splitfile; $i ++) {
	printf ("%12s", commify($total[$i]));
        } print "\n\n";
      
    }
#}
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
