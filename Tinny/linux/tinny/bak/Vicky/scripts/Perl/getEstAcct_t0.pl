#! /usr/local/bin/perl -I/home/yxc/scripts/Perl/PM/test_download/HTML-Table-2.02

#################################################################
#  This program is to restore the estAcct hashes for all clients
#  and print it our to HTML
#
#  yxc: created on 1/2005
#
#################################################################

use Data::Dumper;
$Data::Dumper::Purity=1;

$usage =
  "$0 estActHash1 [estActHash2...]\n\n".
  "Merge the hashes from all FDR clients and print it out to HTML.\n".
  "Example: $0 research_hashes/*est* \n\n";

@clientList=@ARGV;

$fname = shift(@ARGV);
die $usage if(!$fname or $fname eq "-h" or $fname eq "--help");
unshift @ARGV, $fname;

%CLIENT =('ADVANTA' => 'Advanta',
          'ASSOC'   => 'Associates',
	  'CHASE'   => 'Chase',
          'FIRFSD'  => 'First Financial of South Dakota',
          'FIRSTUN' => 'First Union',
          'FLEET'   => 'Fleet',
          'FUSA'    => 'FUSA (F-cycle)',
          'HSBCUSA' => 'HSBC Bank USA',
          'MEMBERCRP'=>'The Members Group',
          'MERRFN'  => 'Merrick Financial',
          'METRIS'  => 'Metris Co.',
          'NATCITY' => 'National City Card Services',
          'PSCU'    => 'PSCU',
          'SEVCORP' => 'Service Corp.',
          'SUNTRUST'=> 'Suntrust',
          'TEXIB'   => 'Texas Independent Bank',
          'TOWNNT'  => 'Town North',
          'USAA'    => 'USAA',
          'WALLSCRT'=> 'WellsFargo Credit',
          'WORLDFB' => 'World Foremost Bank',
          'ZIONBK'  => 'Zions Bank'
          ); 

######## <<<<<<<< Indir can be changed >>>>>>>> ###########
$indir="/work/aisle01/FDR_PerfReviews/2003/credit/research_file";
$model="uscrd03g";
### read in each client's hash

while($fname = shift(@ARGV))
{
### read in the saved hashes one by one

   open (SAVE, "< $fname") || die "Couldn't open $fname:$!";
   undef $/;  # read in file at once
   eval <SAVE>;
   die "can't recreate the hashes from $fname : $@" if $@;
   close SAVE or die "can't close SAVE: $!"; 
### Print the vector of each key into another hash of array
  foreach $key (keys %clientEstAcc){
        push (@clientList, $key);
	if (defined($CLIENT{$key})){
		$outHash{$key}=$clientEstAcc{$key}; 
	}		
  }
}  

foreach $client (keys %CLIENT){
	$file="f$client";
	open ($file, "< $indir/res.$model-$client ") || die "Couldn't open the $client";
	
	while (defined($line=<$file>)){
		$pattern='Basis Points \(10000\*\(Approved Fraud Trx \$\)\/All approved Trx \$\) \=';
		if ($line =~ m/\s+$pattern\s+(\d+)(\.)(\d*)/g) {
			$basis{$client}=$1.$2.$3;
		}
	}
	close($file); 
}


### Print out the hash into HTML

use HTML::Table;
$table1 = new HTML::Table(-rows=>23,
                          -cols=>3,
                          -align=>'CENTER',
#                          -rules=>'GROUPS',
#                          -frame=>'BOX',
                          -border=>1,
                          -bgcolor=>'white',
                          -bordercolor=>'',
                          -width=>'80%',
                          -spacing=>1,
                          -padding=>2,
                          -style=>'color: black',
                          -face=>'Arial, Helvetica, Helv',
                          -size=>3,
                          -class=>'myclass'
                            ); 

$table1->setWidth(700);

### Create the table structure
  $table1->setRowAlign(1,CENTER);
  $table1->setRowBGColor(1,"#D3D3D3");
  $table1->setRowFormat(1,  '<b>', '</b>');

### Fill the table content

$table1->setCell(1,1,"FDR Banks selected");
$table1->setCell(1,2,"Estimated Active Accounts");
$table1->setCell(1,3,"Basis point");

foreach $k (sort {$outHash{$b} <=> $outHash{$a} }keys %outHash) {
  push (@col, $k);
  $outhashF{$k}=commify($outHash{$k});
#  push (@{$outhashF{$k}}, $value );
}

for ($i=0; $i<@col; $i++){
    $row=$i+2;
    $table1->setCell($row,1,"<A HREF=\"HTML/$col[$i].html\">$CLIENT{$col[$i]}</A>");
    $table1->setCellAlign($row,1, LEFT);
    $table1->setCell($row,2,$outhashF{$col[$i]}); 
    $table1->setCell($row,3,$basis{$col[$i]});
    $table1->setCellAlign($row, 2,CENTER);
    $table1->setCellAlign($row, 3,CENTER);
}


$header='<html><body bgcolor="#FFFFFF" text="#000000"><DIV ALIGN="center"><FONT FACE="Arial" COLOR="#0000FF"><B><A HREF="http://isapps.fairisaac.com/hnc/support/ts/ModelingServices/index.cfm">Modeling Services</A></B></FONT> | <FONT FACE="Arial" COLOR="#0000FF"><B><A HREF="http://isapps.fairisaac.com/hnc/support/ts/ModelingServices/Data.cfm">Data Repository</A></B></FONT> | <FONT FACE="Arial" COLOR="#FF0000"><B>2003 FDR Perf. Reviews</B></FONT></DIV><BR><BR><p align="center"><font face="Arial, Helvetica, sans-serif"><b><font color="#660099" size="5">2003 FDR Performance Reviews - Credit</font></b></font></p>';

$trailer='
<pre><font size="2" face="Arial, Helvetica, sans-serif">* Accounts are estimated using a 1% non-fraud sample from Jan. 2003 to Dec. 2003.<BR>** Denotes a bank which did not appear on previous FDR Performance Reviews.<BR> <BR></font></pre><P></body></html>';  
###>>>>>>>>>>>> need also change Credit to Debit if it is for Debit portfolios<<<<<<<<<<<<
print $header; 


$table1->print;

print "\b";

print $trailer;

#### sub rountine to set output number by 3 digits
sub commify {
     my $text = reverse $_[0];
     $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
     return scalar reverse $text;
}
