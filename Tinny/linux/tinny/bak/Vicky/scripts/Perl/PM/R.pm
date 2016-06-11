package R;

use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;
require DynaLoader;
require AutoLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
	
);
$VERSION = '0.01';

bootstrap R $VERSION;

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

R - Perl extension for R, the statistical computing environment

=head1 SYNOPSIS

  use R;


=head1 DESCRIPTION

 This module allows one to call R functions, evaluate R expressions
 and access R objects from within the Perl language. R's statistical,
 numeric and graphical functionality are immediately available.

=head1 Calling S Functions

This is is the primary mechanism in this module by which one can invoke an S function
from Perl. The arguments are 
=over 4
=item[1] the name of the S function or an RFunctionReference object
   obtained from an earlier computation
=item[2] an arbitrary number of arguments which are converted to S values
  and used as arguments to the S function, in the same order they 
  are giving here.
=back  

If the value of this call is assigned in Perl or passed on to another
call, the return value from the S function is converted to a Perl
object. Otherwise, it is ignored.


=head1 Call With Named Arguments

This allows one to call an S function, passing
some of the arguments by name rather than position.
The arguments to this are:
=over 4
=item a)
  an identifier for an S function,
  either a name or an C<RFunctionReference>
  object computed in an earlier call.
=item b)
  a hash of name-value pairs.
  Currently, you must name all the arguments.
  In the near future, this will be extended to allow
  an array of names-value pairs with some values allowed to be
  C<''>.
=back

The return value of the S function is converted 
to a Perl object (if the result is assigned).


=head1 R initialization

Initialize the R engine. This must be done 
 first when running R within Java,
 before making any calls to R functions or accessing
 R objects. The arguments should be character strings
 as if they were command line arguments to the R 
 process. 

=head1 Evaluating S expressions

This routine provides a mechanism for evaluating 
S expressions given as strings in the S language.
Because one must first construct the string,
and since it cannot refer to any Perl or anonymous S 
objects, it is less rich that than the 
C<call> and C<callWithNames> routines. However, it is
convenient for evaluating self-contained S expressions
such as interactive queries of the state of the S session,
defining functions and methods, etc.

  @x = R::eval("search");
  R::eval("myFun <<- function(n) { rnorm(n) + 100 }");


=head1  Register a user-level converter.

This function allows one to register a user-level converter that are
used when converting a non-primitive S object to a Perl value. When
the conversion is attempted, we loop over all registered converters
and ask the predicate/matching function in the converter whether the
converter can process the S object. If it can (returning TRUE), then
we call the actual converter function in this converter.
One can use closures, etc. to parameterize the converter and matching
routines.

See converter.pl in the examples/ directory.

=head1 Discard R Reference Value

This routine allows us to remove an element in the S reference
manager that maintains a list of the different S objects that have
been exported as references to Perl.  This helps us free resources
being used by S objects for which we no longer need the reference in
Perl. This is rarely used explicitly by the user, but implicitly
during the destruction of an C<RReference> object.



=head1 setDebug()

This allows one to dynamically toggle the debug status
(i.e. the variable C<RPerlDebug>)
for this module. If C<RPerlDebug> is C<TRUE>,
then a very large number of messages are printed to the console
from this module explaining detailing different steps in its activities.


=head1 Loading an S library

This is a convenience mechanism for loading an R library and
processing the return value easily.  This could be deprecated now that
we discard return values that are not assigned in Perl.

  R::library("mva");
  $ok = R::call("library", "mva");



=head1 AUTHOR

Duncan Temple Lang duncan@research.bell-labs.com
Bug reports to omega-bugs@omegahat.org.

=head1 SEE ALSO

perl(1).

http://www.omegahat.org/RSPerl

=cut
