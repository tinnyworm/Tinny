#! /usr/local/bin/perl 
use Getopt::Std;

$USAGE = "
#############
 This is to translate the nf/f ratio from .csv to each select file. 
 eg:
 $0 <-c file.csv> <-g hi> <-d work_dir>
 or
 cat file.csv | $0 -
#############
"; 

die "$USAGE" if ($#ARGV == -1); 

&getopts('c:g:d:h', \%opts);
die "$USAGE" if (defined $opts{'h'}); 

if (defined $opts{'g'})
{
	$seg = $opts{'g'}; 
}
else 
{
	warn "NOTE: Not specify segment or wrong seg name. Using \"gen\" as default!\n"; 
	$seg = "gen"; 
}

if (defined $opts{'d'})
{
	$seleDir = $opts{'d'}; 
}
else 
{
	warn "NOTE: Not specify select dir. Using ./ as default!\n"; 
	$seleDir = "."; 
}

if (! defined $opts{'c'})
{
	warn "NOTE: Not specify .csv file. Using STDIN as default\n";  
	$input = "-"; 
}
else 
{
	 $input = $opts{'c'}; 
}

open (IN, "< $input") || die "Could not open $input:\n"; 
while ($_=<IN>)
{
	chomp;
	 
	if ($_ =~ m/Dataset\,Mix-in chosen/o)
	{
		$hdr = <IN>; 
		for ($i=0; $i<40; $i++){
		  $in = <IN>; 
		  chop($in);
		  if ($in !~ /res.count/)
		  {
				next; 
		  }
		  else 
		  {
				@temp=split "\"", $in;
				@f_tmp  = split ",", $temp[0];
			   $filenm = $f_tmp[2]; 
				
				@temp2=split "\,", $in; 
				$len=$#temp2;
				print $len . "\n"; 
				if ($temp2[$len] =~ /[0-9]\./)
				{
					$ff = $temp2[$len]*1; 
					$nff  = $temp2[$len - 1]*1;
				}
				else 
				{
					 $ff = $temp2[$len - 1]*1; 
					$nff  = $temp2[$len - 2]*1;
		  		}
	
				
				if ($nff >1 or $ff >1 or $nff <= 0 or $ff <= 0)
				{
					warn "ERROR: The fraction of $filenm is either bigger than 1 or not greater than 0. Check this!\n"; 
				}
			 
				$seleFile = $filenm; 
				$seleFile =~ s/res.count/select.mgen/;
				
				if (-s "$seleDir/mgen/$seleFile.$seg") 
				{
					print STDERR "$seleDir/mgen/$seleFile.$seg is found!\n"; 
					open (SELIN, "< $seleDir/mgen/$seleFile.$seg"); 
					open (SELOUT, "> $seleDir/mgen/$seleFile.$seg.new");
					while (defined($line=<SELIN>))
					{
						 if ($line =~ /^\s*$/)
						 {
						 	next; 
						 }
						 $line =~ s/PCTFRAUD\=[0-9.]*/PCTFRAUD\=$ff/; 
						 $line =~ s/PCTNOFRAUD\=[0-9.]*/PCTNOFRAUD\=$nff/;
						 print SELOUT "$line";  
						 #print $line; 
					}
				
					close(SELIN);
					warn "NOTE: Overwriting the new select file to old file\n"; 
				  `mv $seleDir/mgen/$seleFile.$seg.new  $seleDir/mgen/$seleFile.$seg;`; 	
 
				} 
				else
				{
					warn "ERROR: This select file as $seleDir/mgen/$seleFile.$seg is not found!! Check your .csv file!\n"; 
				}

				
				#print "$filenm : $seleFile : $nff : $ff\n";
				
			}
		}
	}

}
close(IN); 
