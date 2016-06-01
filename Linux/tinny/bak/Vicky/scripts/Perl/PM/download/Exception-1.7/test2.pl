#!/usr/local/bin/perl

use Exception;
package AmountExceededException; # User-defined exception
@ISA = ('Exception');

package OverdraftException;      # User-defined exception
@ISA = ('Exception');

package BankAccount;
sub withdraw_money {
    my $amount = shift;
    if ($amount > 300) {
        throw new AmountExceededException;
    }
    if ($amount > $balance) {
         throw new OverdraftException;
    }
}

try {
    print "How much do you need?"; chomp($amount = <STDIN>);
    withdraw_money ($amount);
}
catch AmountExceededException => 
               sub {print 'Cannot withdraw more than $300'},
      OverdraftException      => 
               sub {print $_[0]->message},
      Default =>
               sub {print "Internal error. Try later"};
