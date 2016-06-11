use ExtUtils::MakeMaker;

# Set the following to be where your gdchart is.
$GDLOC = "gdchart0.94b";

#----------------------------------------------------------------------
# End of system configuration
#----------------------------------------------------------------------

# These are the files we are linking in from gdchart
@CHART_OBJS = (
        "gdc_pie.o",
        "gdchart.o",
        "price_conv.o",
        "gdc.o",
    );
@CHART_SRC = @CHART_OBJS;

# Make symbolic links from gdchart files if necessary and change the
# CHART_SRC array to have the source files.
foreach my $src (@CHART_SRC)
{
    $src =~ s/\.o$/\.c/;
    if (!(-f "$src"))
    {
        system("ln -s ${GDLOC}/$src .");
        print("ln -s ${GDLOC}/$src .\n");
    }
}

# Now generate the Makefile
WriteMakefile(
          NAME  => 'gdchart',
          VERSION_FROM   => 'gdchart.pm',
          INC   => "-I${GDLOC} -I${GDLOC}/gd1.3",
          LIBS  => "-L${GDLOC}/gd1.3 -lgd",
          OBJECT => "@CHART_OBJS gdchart_wrap.o",
          clean => {FILES => "@CHART_OBJS @CHART_SRC"},
     );

