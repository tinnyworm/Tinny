#! /usr/local/bin/perl -w
################################################################
#  Purpose:  remove the directory and retain the file name only
#  
#  Argument: none
# 
#  Usage:  ls dir/file | rmdir.pl 
#
#  Revision:
#             yxc  5/4/04  creator
#
################################################################

while (<STDIN>){
  chomp;
  @allFile    = split('/', $_);
  $fileLoc    = @allFile - 1;
  $returnFile = $allFile[$fileLoc];
  print "$returnFile\n";
}
