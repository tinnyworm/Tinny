#! /usr/local/bin/perl -w
## 
##  merge the saved perl Hashes by the common keys and tabulate them
## yxc 02/2004


$usage = "\nUsage: $0  -f filename:<filename:...> -s startDate:endDate\n"
         . "                         [ -h ] [ -p ] \n"
         . " where:\n"
         . "    -h    :   show this message\n\n"
         . "    -p    :   show the plot of the first file frequency \n\n"
	 . " Note:   1) Please do not leave any blank within the option content. \n\n"
	 . "example:    tableHash.pl -f hash.auths.dat.gz.save:hash.cards.dat.gz.save -s200301:200403 \n\n";

require "getopts.pl";

&Getopts('f:s:hp');
$file=$opt_f;
$dates=$opt_s;

die "$usage" unless $opt_f ||  $opt_p || $opt_s ;

#### split the file array
@splitfile=split(/:/, $file);
for ($i = 0; $i < @splitfile; $i++) {
    $field[$i][0]= $splitfile[$i];
    }

@dtList=split(/:/, $dates);
$start=$dtList[0]*1;
$end=$dtList[1]*1;

if ($end < $start) {
	print "Warning: End date is before start date!! Please check it...\n";
	
} else {
	$i=$start-1;
	while ($i < $end) {
		$i++; 
		if (($i % 100) > 12 ){
			$i += 88;
		}
		push (@monthList, $i);
	}
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

### print the hash
#### for option as p for plot

if ($opt_p){
foreach $field_1 (dedup(@{ $list[0]})){
   printf ("%8s", "$field_1\n");
     for ($i=0; $i<@splitfile; $i ++) {
      $total[$i]=0;
     }
   foreach $field_2 (@monthList){
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
	    
       foreach $field_2 (@monthList){
           printf ("%16s", $field_2);
             for ($i=0; $i<@splitfile; $i ++) {
		  				$com_key=$field_1 . ":" . $field_2 . ":";
	          	if (exists ($fieldHash[$i]{$com_key})) {
                      printf ("%12s" , commify($fieldHash[$i]{$com_key}));
					       $total[$i] += $fieldHash[$i]{$com_key};
		  			}
					  else {
					      printf ("%12s" ,"0");
	   	  		}
				 }
	     print "\n";
        }  
	printf ("%16s","BIN=$field_1      ");
	for ($i=0; $i<@splitfile; $i ++) {
	printf ("%12s", commify($total[$i]));
        } print "\n\n";
      
    }
}

#### subrountines for dedup the duplicates elements
sub dedup {
	%seen =();
	@uniqu = grep {! $seen{$_}++} @_;
	@sortuniqu = sort {$a cmp $b} @uniqu;	
	return @sortuniqu;
	}

sub commify {
     my $text = reverse $_[0];
     $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
     return scalar reverse $text;
}
