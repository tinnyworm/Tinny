#! /usr/local/bin/perl -w

%branchHash=(); 
while (<>){
  chomp;

  ($branch, $num)= split(" ", $_); 
  $branchHash{$branch}+=$num; 

}


foreach $branch (sort keys %branchHash){
    print "$branch", "  :  ",  $branchHash{$branch}, "\n";
}




