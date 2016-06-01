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
  $up2    = @allFile -1;
}
for ($i=1; $i<$up2; $i++) {
  $returnFile .= "/" . $allFile[$i];
}
print "$returnFile\n";

