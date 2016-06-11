################################################################
# Simple Variable Transformation Package
# Wenjie Hu
################################################################

#package Module_name; # puts vars and subs in module's namespace
package VarTrans;

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

    if ( $strVal =~ /^\s*$/ ) {
	return "MISSING"
    }
    else {
	$strVal =~ s/^\s+//; #remove leading spaces
	$strVal =~ s/\s+$//; #remove trailing spaces
	return $strVal;
    }

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

    foreach my $val ( @{$patterns} ) {

	my $pat  = $val->[0];
	my $name = $val->[1];

	if ( $strVal =~ /$pat/ ) {
	    return $name;
	}

    }

    #foreach my $val ( @{$cutOffs} ) {
    for ( my $i = 0; $i < @{$cutOffs}; ++$i ) {

	if ( $strVal <= $cutOffs->[$i] ) {
	    #return $idx;
	    if ( $i == 0 ) {
		return "(-inf" . "-" . $cutOffs->[$i] . "]";
	    }
	    else {
		return "(" . $cutOffs->[$i-1] . "-" . $cutOffs->[$i] . "]";
	    }
	}

    }

    return "(" . $cutOffs->[-1] . "-" . "inf)";

}

# Regular Expressions Mapping
sub patternMap {

    my ( $strVal, $patterns );

    $strVal   = shift @_;
    $patterns = shift @_;

    foreach my $val ( @{$patterns} ) {

	my $pat  = $val->[0];
        my $name = $val->[1];

	if ( $strVal =~ /$pat/ ) {
	    return $name;
	}

    }

    return "OTHERS";

}

1;								      # Indicate module's end, with True value
