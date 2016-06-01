#! /usr/local/bin/perl -w

while (<>){
  chomp;
  $key=substr($_,20,1);
  $key2=substr($_,35,1);
  
  if ($key eq "0" and $key2 eq "0") {
    print substr($_,0,20),"20",substr($_,20,6),substr($_,28,7),"20",substr($_,35,6),substr($_,44,7),"\n";
  } 
    elsif ($key eq "0"){
    print substr($_,0,20),"20",substr($_,20,6),substr($_,28,22), "\n";
  }
    elsif ($key2 eq "0") {
    print substr($_,0,35),"20",substr($_,35,6),substr($_,44,7),"\n";
  } 
    else {
    print "$_\n";
  }

  
}
