#! /usr/local/bin/perl -w

while (defined($line = <>)){
   if ($line =~ /\b[0-9]\b/){
        $total ++;
        $key = substr($line, 30,20);
         if ($key =~ /^[ ]+$/) {
           $blank ++;
         }
   }
}

print "Total profile variables occupied\t: ", $total-1,"\n";
print "Total blank profile variables   \t: ", $blank-1, "\n";
print "Total net profile variables     \t: ", $total-$blank, "\n"; 
