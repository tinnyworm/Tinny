#! /usr/bin/perl -w

#for ( my $i = 0; $i < 100; $i++ ) {

#    print "Yes\n";
#    print STDERR "No\n";

#}


@array = qw(1 2 3 4
            5 6 7 8);

$array_ref = [1, 2, 3, 4,
              5, 6, 7, 8];

$" = "\t";
#print "@array\n";
#print "@{$array_ref}\n";

@array = ();
push @array, ['ssss', 'name'];
push @array, ['ssss', 'name'];
push @array, ['ssss', 'name'];

print "hello\n";
