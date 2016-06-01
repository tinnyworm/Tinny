#!/usr/bin/env perl

# Automatic variable generator for rule engine
# usage :
#  $0 vars.config > varfile
#
# See /fo1/bin/perl/vars.config for an example
#
# Change vars.config for different variable sets. Do not change this file
# unless you want to add new variable group
#
# $Log: varGen.pl,v $
# Revision 1.4  2007/04/10 19:09:00  dxl
# consolids DailyRate and DailyNum to one, change flag to the full name.
#
# Revision 1.3  2007/04/09 22:32:00  dxl
# now, empty line can be used in Bases and Elements fields.
#
# Revision 1.2  2007/04/09 21:28:36  dxl
# allow multiple line in Bases and Elements fields
#
# Revision 1.1  2007/03/14 22:56:54  dxl
# Initial revision
#

use lib '/home/dxl/local/perlmod';
use warnings;
use Getopt::Std;
use varGen;

my $usage = "Usage:\n".
            "\t $0 [-h] -c config\n\n";
            
my %opt=();

die $usage unless ($#ARGV>0);
die unless getopts("c:h",\%opt);
die $usage if($opt{h});
           
my $listFile = $opt{c};
my ($type,$grp);
my (@baseVars, @elements, @flags);
my $newLine;
my @tmpList;

open LIST, "<$listFile";
while(<LIST>) {
    if(/^Type\s+=\s+(\w+)/) {
        $type = $1;
        $newLine = <LIST>;
        chomp($newLine);
        $newLine =~ s/\s+//g;
        @tmpList = split /=/, $newLine;
        if(exists($tmpList[1])) {
            @baseVars = split /,/, $tmpList[1];
        }
        $newLine = <LIST>;
        while(!($newLine =~ /=/)) {
            chomp($newLine);
            $newLine =~ s/\s+//g;
            @tmpList = split /,/, $newLine;
            push @baseVars, @tmpList;
            $newLine = <LIST>;
        }
        chomp($newLine);
        $newLine =~ s/\s+//g;
        @tmpList = split /=/, $newLine;
        if(exists($tmpList[1])) {
            @elements= split /,/, $tmpList[1];
        }
        $newLine = <LIST>;
        while(!($newLine =~ /=/)) {
            chomp($newLine);
            $newLine =~ s/\s+//g;
            @tmpList = split /,/, $newLine;
            push @elements, @tmpList;
            $newLine = <LIST>;
        }
        chomp($newLine);
        @tmpList = split /=/, $newLine;
        $grp = $tmpList[1];
        $grp =~ s/\s+//g;
        $newLine = <LIST>;
        chomp($newLine);
        @tmpList = split /=/, $newLine;
        $map = $tmpList[1];
        $map =~ s/\s+//g;
        
        $newLine = <LIST>;
        chomp($newLine);
        @tmpList = split /=/, $newLine;
        if(defined($tmpList[1])) {
         @flags = split /,/, $tmpList[1];
        } else {
            @flags=();
        }

        $newLine = <LIST>;
        chomp($newLine);
        @tmpList = split /=/, $newLine;
        if(defined($tmpList[1])) {
            $comment = $tmpList[1];
            $comment =~ s/\s+//g;
        } else {
            $comment ="";
        }

        WriteVars();
    }
}
close LIST;
        
       
sub WriteVars {
    my ($flag, $var, $time);
    my ($age, $delta);
    if($type =~ /^DailyRate$/i) {
        foreach my $flag (@flags) {
            $flag =~ s/\s+//g; 
            foreach my $var (@baseVars) {
                $var =~ s/\s+//g;
                foreach my $time (@elements) {
                    $time =~ s/\s+//g;
                   DailyRate($var,$grp,"PROF".$map,$map,$flag,$time);
                }
            }
        }
    } elsif($type =~ /^DailyRateCoef$/i) {
        foreach $time (@elements) {
            $time =~ s/\s+//g;
            $age = $baseVars[0];
            $age =~ s/\s+//g;
            $delta = $baseVars[1];
            $delta =~ s/\s+//g;
            DailyRateCoef($time,$age,$delta,$grp,$map);
        }
    } elsif($type =~ /^FalConsec$/i) {
        foreach my $event (@baseVars) {
            $event =~ s/\s+//g;
            foreach my $decay (@elements) {
                $decay =~ s/\s+//g;
                if(defined($flags[0])) {
                    foreach my $flag (@flags) {
                     $flag=~s/\s+//g;
                     FalConsec($event,$grp,"PROF".$map,$map,$flag,$decay,$comment);
                     } 
                } else {
                     FalConsec($event,$grp,"PROF".$map,$map,"",$decay,$comment);
                }
            }
        }
    } elsif($type =~ /^EventAvgCoef$/i) {
        foreach my $nEvent (@elements) {
            $nEvent =~ s/\s+//g;
            my $counter = $baseVars[0];
            $counter =~ s/\s+//g;
            EventAvgCoef($counter,$nEvent,$grp,$map);
        }
    } elsif($type =~/^EventAvg$/i) {
        foreach my $flag (@flags) {
            $flag =~ s/\s+//g; 
            foreach my $var (@baseVars) {
                $var =~ s/\s+//g;
                foreach my $time (@elements) {
                    $time =~ s/\s+//g;
                   EventAvg($var,$grp,"PROF".$map,$map,$flag,$time,$comment);
                }
            }
        }
    } elsif($type =~/^Normalize$/i) {
        foreach my $flag (@flags) {
            $flag =~ s/\s+//g; 
            foreach my $var (@baseVars) {
                $var =~ s/\s+//g;
                foreach my $time (@elements) {
                    $time =~ s/\s+//g;
                   Normalize($var,$grp,"PROF".$map,$map,$flag,$time,$comment);
                }
            }
        }
    } elsif($type =~/^MaxShift$/i) {
        foreach my $flag (@flags) {
            $flag =~ s/\s+//g; 
            foreach my $var (@baseVars) {
                $var =~ s/\s+//g;
                foreach my $time (@elements) {
                    $time =~ s/\s+//g;
                    MaxShift($var,$grp,"PROF".$map,$map,$flag,$time,$comment);
                }
            }
        }
    } elsif($type =~/^Risk$/i) {
        my $def=$flags[0];
        $def =~ s/\s+//g;
        my $ratio=$elements[0];
        $ratio =~ s/\s+//g;
        foreach my $var (@baseVars) {
            $var =~ s/\s+//g;
            Risk($var,$grp,"PROF".$map,$map,$def,$ratio,$comment);
        }
    } elsif($type =~/^MaxDecay$/i) {
        my $decay = $baseVars[0];
        $decay =~ s/\s+//g;
        my $flag = $flags[0];
        $flag =~ s/\s+//g;
        foreach my $baseVar (@elements) {
            $baseVar =~ s/\s+//g;
            MaxDecay($baseVar, $decay, $map, $grp, $flag);
        }
    } elsif($type =~/^Percentile$/i) {
        my $flag = $flags[0];
        $flag =~ s/\s+//g;
        my $epoch_size = $flags[1];
        $epoch_size =~ s/\s+//g;
        foreach my $baseVar (@baseVars) {
            $baseVar =~ s/\s+//g;
            foreach my $percent (@elements) {
                $percent =~ s/\s+//g;
                Percentile($baseVar, $percent, $map, $grp, $flag,
                $epoch_size);
            }
        }
    }
}
