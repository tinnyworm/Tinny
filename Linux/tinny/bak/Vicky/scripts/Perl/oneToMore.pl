#!/usr/local/bin/perl 

@list=("ASSOC1", "ASSOC2", "ASSOC3", "ASSOC4", "METRIC");
@clientID=("5414", "5524","5998", "6509", "5994");

for ($i=0; $i<@list; $i++){
    `mkdir $list[$i]`;
    $id = $clientID[$i];
    $pair{$id} = $list[$i];
}

while (defined($line = <STDIN>)) {
    $bin = substr($line,186,4);
    
    $client = $pair{$bin};
    if (!defined($client)) {
#        print STDERR "Error: no client defined for bin  $bin.  Skipping transaction...\n";
        next;
    }
     
    if (!defined($fh{$bin})) {
        $file = "f$bin";
        open $file, "| gzip -c > $client/com.cards.dat.gz";
        $fh{$bin} = $file;
    }

    $fh = "f$bin";
    print $fh $line;
}
