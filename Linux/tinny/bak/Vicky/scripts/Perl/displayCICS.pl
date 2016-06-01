#!/usr/local/bin/perl -w
# 
# Read in a mainframe CICS file and display the hex (and optionally the
# ASCII) values.  This does not translate packed decimal (comp-3) or
# binary (comp), but does show the hex dump along with the ascii
# translation, so the user can usually devine the meaning.
#
# Basically, this is intended to aid in debugging mainframe files before
# writing translation code.
#
#
# Author: Paul C. Dulany
# Date: 2002-02-26
#
#
# Major inspiration and help from Sri "Buzz" Soundararajan.  Thanks!
#
#
# Revisions:
#
# 2002-04-02:  Added ability to read variable-blocked files!
#

############################
# 
# For Debugging:
#
# select STDERR; $| = 1;
# select STDOUT; $| = 1;

############################
# 
$usage = 
    "\n" .
    "Usage: $0 [-na] [-vb] [-ascii] recLen\n" .
    "\n" .
    "       -vb    =>  Variable blocked file!  Determine the relevent\n" .
    "                  information from the file.  The <recLen> is the\n" .
    "                  length of the ruler used (usually the max line\n" .
    "                  length...).\n" .
    "\n" .
    "       -na    =>  DON'T write ASCII translation above hex dump.\n" .
    "                  (I recommend that you never use this.)\n" .
    "\n" .
    "\n" .         
    "       -ascii =>  Interpret the bytes as ASCII, not EBCDIC.\n" .
    "\n" .
    "Read in a mainframe CICS file and display the hex (and optionally the\n" .
    "ASCII) values.  This does not translate packed decimal (comp-3) or\n" .
    "binary (comp), but does show the hex dump along with the ascii\n" .
    "translation, so the user can usually devine the meaning.\n" .
    "\n" .
    "Basically, this is intended to aid in debugging mainframe files before\n" .
    "writing translation code.\n" .
    "\n" .
    "Preferred usage:\n" .
    "   gzip -dc Z05599.1.00463.rawdat.gz | displayCICS.pl 463 | less -S\n" .
    "\n" .
    " Author: Paul C. Dulany\n" .
    " Date: 2002-02-26\n" .
    "\n" .
    " Major inspiration and help from Sri Soundararajan.  Thanks!\n" .
    "\n" .
    "\n";


$origInAscii = 0;
$printAscii = 1;
$variableBlocked = 0;
while (scalar @ARGV) {
   $arg = shift (@ARGV);
   if ($arg eq "-a") {
      $printAscii = 1;
   } elsif ($arg eq "-vb") {
       $variableBlocked = 1;
   } elsif ($arg eq "-na") {
      $printAscii = 0;
   } elsif ($arg eq "-ascii") {
      $origInAscii = 1;
   } elsif ($arg =~ /[0-9]+/o) {
      $reclen = $arg;
   } else {
      print STDERR $usage;
      exit 1;
   }
}

################################################################################
#
# Get printable EBCDIC to ASCII table
# Set-up the ruler
#

@ASCII_from_EBCDIC = &printableEBCDICToASCIITable;
$ruler = &makeRuler();


################################################################################
# 
# Set up variables for the line reader
#
$blockLength = 0;
$recordLength = 0;
$blockPosition = 0;
$recordNumber = 0;
$blockNumber = 0;
$record = "";

$recordLength = $reclen if ($variableBlocked == 0);



################################################################################
#
# The main work-horse
#
################################################################################

$nLines = 0;
# print STDERR "Getting records: recordLength = $recordLength\n";
while (($numBytes = &getNextRecord()) > 0) {

    if ($nLines * (4 + $printAscii) % 20 == 0) {
        print "$ruler\n";
    }
    $nLines++;

    @val = unpack (("C"x$numBytes), $::record);

    ########################
    #
    # print the ascii line
    #
    if ($printAscii) {
       # printf "%8.8d: ", $nLines;
       printf "          ";
       for ($i = 0; $i < $numBytes; $i++) {
           if ($origInAscii == 0) {
               if ($ASCII_from_EBCDIC[$val[$i]] ne (-1)) {
                   printf STDOUT "%1.1s", $ASCII_from_EBCDIC[$val[$i]];
               } else {
                   print STDOUT ".";
               }
           } elsif ($origInAscii == 1) {
               my ($tmp) = chr($val[$i]);
               if ($tmp ge ' ' && $tmp le '~') {
                   printf STDOUT "%1.1s", $tmp;
               } else {
                   print STDOUT ".";
               }
           }
       }
       print "\n";

    }

    ########################
    #
    # print the hex lines
    #
    $hexLine1 = "";
    $hexLine2 = "";
    for ($i = 0; $i < $numBytes; $i++) {
        ($n1, $n2) = &getNibbles ($val[$i]);
        $hexLine1 .= $n1;
        $hexLine2 .= $n2;
    }
    printf "%8.8d: $hexLine1\n", $nLines;
    printf "%8.8d: $hexLine2\n", $nLines;

    ########################
    #
    # print a blank line
    #
    print "\n";
}

####################################################
# 
# End main code
#
# Start subroutines
#
####################################################


sub makeRuler () {
    use strict;
    my ($i, $j, $k, $ruler);
    my ($n, @r, $l10);

    $l10 = log(10);


    for ($n = 0; $n < int (log($::reclen) / $l10 + 1); $n++) {
        $r[$n] = "-line#--: ";
        #         1234567890
    }

    $ruler = "";
    for ($n = 0; $n < scalar @r; $n++) {
        $j = 1;
        for ($i = 1 ; $i < $::reclen + 1; $i++) {
            if (0 == $i % (10 ** ($n))) {
                $r[$n] .= sprintf "%1.1d", $j++;
                $j = 0 if ($j > 9);
            } else {
                $r[$n] .= "-";
            }
        }
        $ruler = "$r[$n]\n" . $ruler;
    }
    return $ruler;
}


####################################################


sub printableEBCDICToASCIITable {
   use strict;
   my ($i, @ebcdicToAsciiTable);

   #
   # Initialize to all unknown.
   for ($i=0; $i<256; $i++) {
       $ebcdicToAsciiTable[$i] = (-1);
   }

   # Fill in the known values.  These come from /home/pcd/bin/ebcdic.pl,
   # which uses "dd" to conver ASCII printables to EBCDIC.
   #
   #                  EBCDIC   ASCII
   $ebcdicToAsciiTable[  64 ] =  chr(  32 );
   $ebcdicToAsciiTable[  90 ] =  chr(  33 );
   $ebcdicToAsciiTable[ 127 ] =  chr(  34 );
   $ebcdicToAsciiTable[ 123 ] =  chr(  35 );
   $ebcdicToAsciiTable[  91 ] =  chr(  36 );
   $ebcdicToAsciiTable[ 108 ] =  chr(  37 );
   $ebcdicToAsciiTable[  80 ] =  chr(  38 );
   $ebcdicToAsciiTable[ 125 ] =  chr(  39 );
   $ebcdicToAsciiTable[  77 ] =  chr(  40 );
   $ebcdicToAsciiTable[  93 ] =  chr(  41 );
   $ebcdicToAsciiTable[  92 ] =  chr(  42 );
   $ebcdicToAsciiTable[  78 ] =  chr(  43 );
   $ebcdicToAsciiTable[ 107 ] =  chr(  44 );
   $ebcdicToAsciiTable[  96 ] =  chr(  45 );
   $ebcdicToAsciiTable[  75 ] =  chr(  46 );
   $ebcdicToAsciiTable[  97 ] =  chr(  47 );
   $ebcdicToAsciiTable[ 240 ] =  chr(  48 );
   $ebcdicToAsciiTable[ 241 ] =  chr(  49 );
   $ebcdicToAsciiTable[ 242 ] =  chr(  50 );
   $ebcdicToAsciiTable[ 243 ] =  chr(  51 );
   $ebcdicToAsciiTable[ 244 ] =  chr(  52 );
   $ebcdicToAsciiTable[ 245 ] =  chr(  53 );
   $ebcdicToAsciiTable[ 246 ] =  chr(  54 );
   $ebcdicToAsciiTable[ 247 ] =  chr(  55 );
   $ebcdicToAsciiTable[ 248 ] =  chr(  56 );
   $ebcdicToAsciiTable[ 249 ] =  chr(  57 );
   $ebcdicToAsciiTable[ 122 ] =  chr(  58 );
   $ebcdicToAsciiTable[  94 ] =  chr(  59 );
   $ebcdicToAsciiTable[  76 ] =  chr(  60 );
   $ebcdicToAsciiTable[ 126 ] =  chr(  61 );
   $ebcdicToAsciiTable[ 110 ] =  chr(  62 );
   $ebcdicToAsciiTable[ 111 ] =  chr(  63 );
   $ebcdicToAsciiTable[ 124 ] =  chr(  64 );
   $ebcdicToAsciiTable[ 193 ] =  chr(  65 );
   $ebcdicToAsciiTable[ 194 ] =  chr(  66 );
   $ebcdicToAsciiTable[ 195 ] =  chr(  67 );
   $ebcdicToAsciiTable[ 196 ] =  chr(  68 );
   $ebcdicToAsciiTable[ 197 ] =  chr(  69 );
   $ebcdicToAsciiTable[ 198 ] =  chr(  70 );
   $ebcdicToAsciiTable[ 199 ] =  chr(  71 );
   $ebcdicToAsciiTable[ 200 ] =  chr(  72 );
   $ebcdicToAsciiTable[ 201 ] =  chr(  73 );
   $ebcdicToAsciiTable[ 209 ] =  chr(  74 );
   $ebcdicToAsciiTable[ 210 ] =  chr(  75 );
   $ebcdicToAsciiTable[ 211 ] =  chr(  76 );
   $ebcdicToAsciiTable[ 212 ] =  chr(  77 );
   $ebcdicToAsciiTable[ 213 ] =  chr(  78 );
   $ebcdicToAsciiTable[ 214 ] =  chr(  79 );
   $ebcdicToAsciiTable[ 215 ] =  chr(  80 );
   $ebcdicToAsciiTable[ 216 ] =  chr(  81 );
   $ebcdicToAsciiTable[ 217 ] =  chr(  82 );
   $ebcdicToAsciiTable[ 226 ] =  chr(  83 );
   $ebcdicToAsciiTable[ 227 ] =  chr(  84 );
   $ebcdicToAsciiTable[ 228 ] =  chr(  85 );
   $ebcdicToAsciiTable[ 229 ] =  chr(  86 );
   $ebcdicToAsciiTable[ 230 ] =  chr(  87 );
   $ebcdicToAsciiTable[ 231 ] =  chr(  88 );
   $ebcdicToAsciiTable[ 232 ] =  chr(  89 );
   $ebcdicToAsciiTable[ 233 ] =  chr(  90 );
   $ebcdicToAsciiTable[ 173 ] =  chr(  91 );
   $ebcdicToAsciiTable[ 224 ] =  chr(  92 );
   $ebcdicToAsciiTable[ 189 ] =  chr(  93 );
   $ebcdicToAsciiTable[  95 ] =  chr(  94 );
   $ebcdicToAsciiTable[ 109 ] =  chr(  95 );
   $ebcdicToAsciiTable[ 121 ] =  chr(  96 );
   $ebcdicToAsciiTable[ 129 ] =  chr(  97 );
   $ebcdicToAsciiTable[ 130 ] =  chr(  98 );
   $ebcdicToAsciiTable[ 131 ] =  chr(  99 );
   $ebcdicToAsciiTable[ 132 ] =  chr( 100 );
   $ebcdicToAsciiTable[ 133 ] =  chr( 101 );
   $ebcdicToAsciiTable[ 134 ] =  chr( 102 );
   $ebcdicToAsciiTable[ 135 ] =  chr( 103 );
   $ebcdicToAsciiTable[ 136 ] =  chr( 104 );
   $ebcdicToAsciiTable[ 137 ] =  chr( 105 );
   $ebcdicToAsciiTable[ 145 ] =  chr( 106 );
   $ebcdicToAsciiTable[ 146 ] =  chr( 107 );
   $ebcdicToAsciiTable[ 147 ] =  chr( 108 );
   $ebcdicToAsciiTable[ 148 ] =  chr( 109 );
   $ebcdicToAsciiTable[ 149 ] =  chr( 110 );
   $ebcdicToAsciiTable[ 150 ] =  chr( 111 );
   $ebcdicToAsciiTable[ 151 ] =  chr( 112 );
   $ebcdicToAsciiTable[ 152 ] =  chr( 113 );
   $ebcdicToAsciiTable[ 153 ] =  chr( 114 );
   $ebcdicToAsciiTable[ 162 ] =  chr( 115 );
   $ebcdicToAsciiTable[ 163 ] =  chr( 116 );
   $ebcdicToAsciiTable[ 164 ] =  chr( 117 );
   $ebcdicToAsciiTable[ 165 ] =  chr( 118 );
   $ebcdicToAsciiTable[ 166 ] =  chr( 119 );
   $ebcdicToAsciiTable[ 167 ] =  chr( 120 );
   $ebcdicToAsciiTable[ 168 ] =  chr( 121 );
   $ebcdicToAsciiTable[ 169 ] =  chr( 122 );
   $ebcdicToAsciiTable[ 192 ] =  chr( 123 );
   $ebcdicToAsciiTable[  79 ] =  chr( 124 );
   $ebcdicToAsciiTable[ 208 ] =  chr( 125 );

   # for ($i=0; $i<256; $i++) {
   #     print "ebcdic $i => ascii $ebcdicToAsciiTable[$i]\n";
   # }


   return (@ebcdicToAsciiTable);

}


####################################################


sub getNibbles () {
    use strict;
    my ($val) = @_;
    my ($tmp, $n1, $n2);

    ###########
    #
    # Yes, this is not efficient.  But it's for displaying to humans, so
    # we don't need speed...
    #
    $tmp = sprintf "%2.2x", $val;
    ($n1, $n2) = unpack 'aa', $tmp;

    return ($n1, $n2);
}


####################################################
# 
# Read in the next record.
#
# For fixed-record-size files, this is simple.  Get that many bytes and
# return.
#
# For variable-blocked files, this is a little more complicated...
#
# The first four bytes of a block are the Block-Descriptor-Word (BDW).
#    - The first two bytes (big endian) are the number of bytes in the
#      block, including the BDW.
#    - The second two bytes are always zero.  (For future expansion?)
#
# Then follow records within a block.  The first four bytes of a record
# are the Record-Descriptor-Word (RDW).
#    - The first two bytes (big endian) are the number of bytes in the
#      record plus the length of the RDW ("record-length-plus-4").
#    - The second two bytes are always zero.  (For future expansion?)
#
# Then follows the record in (record_length) bytes.
#
# Note that the number in the RDW is 4 bytes more than the record length
# (the number of bytes of data in the record), and the block size is 4
# more bytes than the sum of the (record-length-plus-4) sizes within it.
#
# Here goes my attempt to follow this prescription.
#


sub getNextRecord () {
    use strict;
    my ($val) = @_;
    my ($raw, $tmp);

    if ($::variableBlocked == 1) {
        if ($::blockLength == 0 || $::blockPosition == $::blockLength) {
            # Starting a new Block

            # Get block length (BDW):
            $::blockPosition = 0;
            if (read(STDIN, $raw, 2) != 2) {
                # End of file!
                return 0;
            }
            $::blockLength = unpack "n", $raw;
            $::blockPosition += 2;

            # Skip over two zero bytes before the record length
            if (read(STDIN, $raw, 2) != 2) {
                die "Ran out of bytes while examining block header for block" . $::blockNumber;
            }
            $::blockNumber ++;
            $::blockPosition += 2;

            print "Block number $::blockNumber of length $::blockLength\n";
        }

        # Get a record

        # Get record length from the (RDW).
        # Note that the length of the RDW needs to be subtracted to get
        # the recordLength.
        if (read(STDIN, $raw, 2) != 2) {
            die "Ran out of bytes in block $::blockNumber reading record $::recordNumber";
        }
        $::recordLength = (unpack "n", $raw) - 4;
        $::blockPosition += 2;

        # Skip over two zero bytes before the record data
        if (read(STDIN, $raw, 2) != 2) {
            die "Ran out of bytes in block $::blockNumber reading record $::recordNumber";
        }
        $::recordNumber ++;
        $::blockPosition += 2;

        if (read(STDIN, $::record, $::recordLength) != $::recordLength) {
            die "Ran out of bytes in block $::blockNumber reading record $::recordNumber";
        }
        $::blockPosition += $::recordLength;

        print "Record number $::recordNumber of length $::recordLength\n";

        return $::recordLength;

    } else {

        if (($tmp = read(STDIN, $::record, $::recordLength)) != $::recordLength) {
            return 0 if ($tmp == 0);
            die "Ran out of bytes after record $::recordNumber";
        }
        $::recordNumber ++;

        return $::recordLength;

    }
}

# vim:cindent:formatoptions=2tcqrol:
