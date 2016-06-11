package ReadFilePerFmt;
require 5.000;
require Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(parseFmtFileForFields parseLine);

################################################################################
#
#  Example code
#
################################################################################
# #!/usr/local/bin/perl -w
# 
# use lib '.';
# use ReadFilePerFmt;
# 
# 
# if (scalar @ARGV != 1 || $ARGV[0] =~ /^---*h/) {
#     print STDERR "Usage: $0 <SASfmtFile>\n";
#     print STDERR "<SASfmtFile> has very specific restrictions at this time\n";
#     exit 1;
# }
# 
# %c102 = &ReadFilePerFmt::parseFmtFile ($ARGV[0]);
# 
# # print "$c102{'unpack'}\n";
# # for  ($i = 0; $i < $c102{"nFields"}; $i++) {
# #     print "$i: $c102{'name'}[$i]\n";
# # }
# 
# while (defined ($line = <STDIN>)) {
#     %data = &ReadFilePerFmt::parseLine ($line, %c102);
#     print "|$data{'ACCT_NBR'}|  |$data{'TRANS_DATE'}|\n";
# }
# 
################################################################################

sub parseFmtFile
{
    my ($fileName) = @_;
    return( &ReadFilePerFmt::parseFmtFileForFields ($fileName, ()));

    # use strict;
    # my ($fileName) = @_;
    # my (%fileFormat);
    # my ($line, $name, $startColumn, $length);
    # my ($nFields);
    # my ($unpack);

    # open FMT, "< $fileName" or die "Can't open $fileName";
    # $nFields = 0;
    # $unpack = "";
    # while (defined ($line = <FMT>)) 
    #     chomp $line;
    #     if ($line =~ /^ *@(\d+)\s+(\S+)\s+\$CHAR(\d+)\.\s*$/) {
    #         $startColumn = $1;
    #         $name = $2;
    #         $length = $3;
    #         # print "$name $startColumn $length\n";

    #         $fileFormat{"name"}[$nFields] = $name;
    #         $fileFormat{"start"}[$nFields] = $startColumn;
    #         $fileFormat{"length"}[$nFields] = $length;
    #         $nFields++;
    #         $unpack .= " \@" . ($startColumn - 1) . "a$length";
    #     } else {
    #         print STDERR "Sorry, cannot parse |$line|\n";
    #     }
    # }
    # $fileFormat{"nFields"} = $nFields;
    # $fileFormat{"unpack"} = $unpack;
    # close (FMT);
    # return (%fileFormat);
}


sub parseFmtFileForFields
{
    use strict;
    my ($fileName, @fields) = @_;
    my (%fileFormat);
    my ($line, $name, $startColumn, $length);
    my ($nFields);
    my ($unpack);
    my ($i, $parseLine);

    # print STDERR "\@fields = @fields\n";
    open FMT, "< $fileName" or die "Can't open $fileName";
    $nFields = 0;
    $unpack = "";
    while (defined ($line = <FMT>)) {
        chomp $line;
        if ($line =~ /^ *@(\d+)\s+(\S+)\s+\$CHAR(\d+)\./) {
                        #Changed by BXZ for allowing comments in fmt file
            $startColumn = $1;
            $name = $2;
            $length = $3;
            # print "$name $startColumn $length\n";

            $parseLine = 0;
            for ($i = 0; $i < scalar @fields; $i++) {
                if ($fields[$i] eq $name) {
                    $parseLine = 1;
                    last;
                }
            }
            if (scalar @fields == 0 || $parseLine == 1)  {
                $fileFormat{"name"}[$nFields] = $name;
                $fileFormat{"start"}[$nFields] = $startColumn;
                $fileFormat{"length"}[$nFields] = $length;
                $nFields++;
                $unpack .= " \@" . ($startColumn - 1) . "a$length";
            }
        } else {
            print STDERR "Sorry, cannot parse |$line|\n";
        }
    }
    $fileFormat{"nFields"} = $nFields;
    $fileFormat{"unpack"} = $unpack;
    close (FMT);
    return (%fileFormat);
}


sub parseLine
{
    use strict;
    my ($line, %fmt) = @_;
    my (%returnHash);
    my ($i);

    @returnHash{@{ $fmt{'name'} }}=unpack $fmt{'unpack'}, $line;

#   @data = unpack $fmt{'unpack'}, $line;
#   for  ($i = 0; $i < $fmt{"nFields"}; $i++) {
#       $returnHash{$fmt{'name'}[$i]} = $data[$i];
#   }

    return (%returnHash);
}

# vi:cindent:
