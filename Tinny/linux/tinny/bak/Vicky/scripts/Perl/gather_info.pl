#! /usr/local/bin/perl

# File Types
@list = ("auth", "card", "fraud", "nonm", "post");

foreach $element (@list) {
  $label = substr($element, 0, 1);

  # Use "case" instead of "fraud" because a fraud file was misspelled as "fradu".
  if ($element eq "fraud") {
    $element2 = "case";
  } else {
    $element2 = $element;
  }

  $crtfile = `ls -l | grep -i $element2 | grep -v batch > $label.list.txt`;

  open ($element, "<$label.list.txt")
    or die ("Cannot open $label.list.txt");

  while ($line = <$element>) {
    chop $line;

    # Capture the Inventory Number.
    $linep = $line;
    $linep =~ tr/A-Z/L/;
    $linep =~ tr/a-z/L/;
    $linep =~ tr/0-9/#/;
    $invloc = index($linep, '######L');
    $inv = substr($line,  $invloc, 7);

    # Capture the Size (in bytes) and the File Name.
    @array = split(/ +/, $line);
    $size = $array[4];
    $file = $array[8];

    # Determine the Number of Records in the File.
    $numrecs = `zcat $file | wc -l`;
    chop $numrecs;

    print "File Name : File Type : Inventory Number : Number of Records : Size (bytes)\n";
    print $file . " : " . $element . " : " . $inv . " : " . $numrecs . " : " . $size . "\n";

    # Generate Histogram of Dates.
    if ($element eq "auth") {
      $cut = "20-27";
    } elsif ($element eq "card") {
      $cut = "91-98";
    } elsif ($element eq "fraud") {
      $cut = "36-43";
    } elsif ($element eq "nonm") {
      $cut = "21-28";
    } elsif ($element eq "post") {
      $cut = "49-56";
    }
    $hist = `zcat $file | ufalcut -c$cut | perl /home/atm/perl/histogram.pl`;
    print $hist . "\n";
  }

  close ($element);
}

