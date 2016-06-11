package ParseFmt;
require 5.000;
require Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(parseFmtFileForFields parseLine);

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
    my (%data);
    my ($i);

    @data{@{ $fmt{'name'} }}=unpack $fmt{'unpack'}, $line;
#   @data=unpack $fmt{'unpack'}, $line;

    return (\%data);
}

# vi:cindent:
