#! /usr/local/bin/perl -w


open (FILELIST, "ls CODE* | ") || die "couldn't list the file";

print "FILENAME  Count Sum Mean Std  Var  Min  Q1  Median  Q3  Max  Range  NumberBlank\n";
while (<FILELIST>){
   chomp;
   $file=$_;
   print $file;
   open (FILE, "cat $file |") || die "couldn't open $file";
   while (<FILE>) {
     chomp;
     @list=split("=",$_);
     $content=$list[1];
     print $content;
   }
   print "\n";
}
