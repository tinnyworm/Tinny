#package Module_name; # puts vars and subs in module's namespace
package SimpleVarTrans;

use strict;
use Carp;							      # identifies error locations from user's POV

our (@ISA, @EXPORT, @EXPORT_OK);

# List any variables or subs to be exported by module here
# our ( module_vars, module_subs );

require Exporter; @ISA=( 'Exporter' );				      # import/export services

# Classify module's exports as automatic or on-request
@EXPORT    = qw( cutOffBin patternMap );			      # automatic
@EXPORT_OK = qw( directMap constMap );				      # on-request

# The code that implements the module's functionality follows

# Direct Mapping
sub directMap {

    my $strVal;

    $strVal = shift @_;
    return $strVal;

}

# Constant Mapping
sub constMap {

    my $rVal;

    $rVal = shift @_;
    return $rVal;

}

# Customized Cut-Off Binning for numeric variable
sub cutOffBin {

    my ( $strVal, $patterns, $cutOffs );

    $strVal  = shift @_;
    $patterns = shift @_;
    $cutOffs = shift @_;

    my $idx = 0;
    foreach my $val ( @{$patterns} ) {

	if ( $strVal =~ /$val/ ) {
	    return $idx;
	}
	else {
	    ++$idx;
	}

    }

    foreach my $val ( @{$cutOffs} ) {
	
	if ( $strVal < $val ) {
	    return $idx;
	}
	else {
	    ++$idx;
	}

    }

    return $idx;

}

# Regular Expressions Mapping
sub patternMap {

    my ( $strVal, $patterns );

    $strVal   = shift @_;
    $patterns = shift @_;

    my $idx = 0;
    foreach my $val ( @{$patterns} ) {

	if ( $strVal =~ /$val/ ) {
	    return $idx;
	}
	else {
	    ++$idx;
	}

    }

    return $idx;

}

1;								      # Indicate module's end, with True value
