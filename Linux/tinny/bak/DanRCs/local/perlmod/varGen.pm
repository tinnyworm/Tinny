# varGen package for rule engining variable generation

# $Log: varGen.pm,v $
# Revision 1.3  2007/04/10 19:08:04  dxl
# consolids DailyRate and DailyNum to one, change flag to the full name
#
# Revision 1.2  2007/04/09 22:44:41  dxl
# fixed est_percentile.
#
# Revision 1.1  2007/03/14 22:50:33  dxl
# Initial revision
#

package  varGen;
require  Exporter;

our @ISA       = qw(Exporter);
our @EXPORT    = qw(DailyRateCoef 
                    DailyRate
                    EventAvgCoef
                    EventAvg
                    FalConsec
                    Normalize
                    MaxShift
                    MaxDecay
                    Percentile
                    Risk);

# Computers approximate values of the 1st and 2nd coefficients for time
# decay and decay rate for time-decayed value
sub DailyRateCoef {
    my ($time,$age,$delta,$grp,$namePre) = @_;
    print("VAR $namePre\_DR_COEF1_$time : FLOAT\n".
          "GRP = $grp\n".
          "CALC =\n".
          "FAL_DLY_COEF1(\_$time, $namePre\_$age);\n\n".
          "VAR $namePre\_DR_COEF2_$time : FLOAT\n".
          "GRP = $grp\n".
          "CALC =\n".
          "RATE_DECAY($namePre\_$delta, \_$time, $namePre\_$age);\n\n".
          "VAR $namePre\_DECAY_COEF_$time : FLOAT\n".
          "GRP = $grp\n".
          "CALC =\n".
          "RATE_DECAY($namePre\_$delta, \_$time);\n\n");
}

sub DailyRate {
    my ($baseVar, $grp, $profPre, $namePre, $flag, $time, $comment) = @_;
# deal with base var name
    my $baseName=$baseVar;
    if($baseName=~/^IS_(\S+)/i) {$baseName=$1;};
    if($baseName=~/^CON_1$/i) {$baseName="NUM";};
# generate core name
    my $coreName = $baseName."_".$flag."_".$time;
# deal with prefix and comment
    if($namePre=~/\S+/) {
        $namePre .= "_";
    }
    if($comment=~/\S+/) {
        $comment=" \/\/".$comment;
    }
#    if ($baseVar =~ /^num$/i) {$baseVar="CON_1";}
    print("VAR $namePre"."DAILY_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre\.DAILY_$coreName\n".
          "RCOD = 0\n".
          "CALC =\n".
          "DAILY_RATE($namePre"."DAILY\_$coreName, IS\_$flag, $baseVar, ".
          "$namePre"."DR_COEF1_$time, $namePre"."DR_COEF2_$time);\n\n");
}

sub FalConsec {
    my ($event,$grp,$profPre,$namePre,$flag,$decay,$comment) = @_;
    $baseVar=$event;
    if($baseVar=~/^is_(\S+)/i) {$baseVar=$1;}
    my $coreName;
    if($flag=~/\S+/) {
        $coreName = $baseVar."_".$flag."_".$decay;
    } else {
        $coreName = $baseVar."_".$decay;
    }
    if($namePre=~/\S+/) {
        $namePre .= "_";
    }
    if($comment=~/\S+/) {
        $comment=" \/\/".$comment;
    }
    print("VAR $namePre"."CONS_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre.CONS_$coreName\n".
          "RCOD = 0\n".
          "CALC =\n");
    if($flag =~ /\w+/) {
        print(
          "FAL_CONSEC($namePre"."CONS_$coreName, IS_$event, ".
          "$namePre"."DECAY_COEF_$decay, IS_$flag);\n\n");
    } else {
        print(
          "FAL_CONSEC($namePre"."CONS_$coreName, $event, ".
          "$namePre"."DECAY_COEF_$decay);\n\n");
    }
}

#sub DailyNum {
#    my ($grp, $profPre, $namePre, $flag, $time) = @_;
#    my $coreName = "NUM_".$flag."_".$time;
#    print("VAR $namePre\_DAILY_$coreName : FLOAT\n".
#          "GRP = $grp\n".
#          "MAP = $profPre\.DAILY_$coreName\n".
#          "RCOD = 0\n".
#          "CALC =\n".
#          "DAILY_RATE($namePre\_DAILY\_$coreName, CON_1, IS\_$flag, ".
#          "$namePre\_DR_COEF1_$time, $namePre\_DR_COEF2_$time);\n\n");
#}

# Calculate the exact event average coefficients
sub EventAvgCoef {
    my ($counter,$nEvent,$grp,$namePre) = @_;
    print("VAR $namePre\_AVG_COEF1_$nEvent"."EVENT: FLOAT\n".
          "GRP = $grp\n".
          "CALC =\n".
          "EVENT_COEF1_CALC(EXP_$nEvent"."EVENT, _$nEvent"."EVENT, ".
          "$namePre\_$counter);\n\n".
          "VAR $namePre\_AVG_COEF2_$nEvent"."EVENT: FLOAT\n".
          "GRP = $grp\n".
          "CALC =\n".
          "FLOAT_SUB(CON_1, $namePre\_AVG_COEF1_$nEvent"."EVENT);\n\n");
}

sub EventAvg{
    my ($baseVar, $grp, $profPre, $namePre, $flag, $time, $comment) = @_;
    my $coreName = $baseVar."_".$flag."_".$time."E";
    if($namePre=~/\S+/) {
        $namePre .= "_";
    }
    if($comment=~/\S+/) {
        $comment=" \/\/".$comment;
    }
    print("VAR $namePre"."AVG_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre\.AVG_$coreName\n".
          "RCOD = 0\n".
          "CALC =\n".
          "EVENT_AVG($namePre"."AVG\_$coreName, IS\_$flag, $baseVar, ".
          "$namePre"."AVG_COEF1_$time"."EVENT, ".
          "$namePre"."AVG_COEF2_$time"."EVENT);\n\n");
}

sub Normalize {
    my ($baseVar, $grp, $profPre, $namePre, $flag, $time, $comment) = @_;
    my $coreName = $baseVar;
    if($namePre=~/\S+/) {
        $namePre .= "_";
    }
    if($comment=~/\S+/) {
        $comment=" \/\/".$comment;
    }
    my ($decay1,$decay2)=split /x/, $time;
    my $n = $namePre.$coreName."_".$decay1;
    if($flag eq "NMN") {
        my $N = $namePre.$coreName."_".$decay2;
        print("VAR $namePre"."N_$coreName"."_$time"."_NMN : FLOAT$comment\n".
              "GRP = $grp\n".
              "RCOD = 0\n".
              "CALC =\n".
              "NORMALIZE($n, $N, MAX_RATIO, CON_0);\n\n");
    } elsif($flag eq "NMX") {
        my $N = $namePre."MAXP_".$coreName."_".$decay1."_".$decay2;
        print("VAR $namePre"."N_$coreName"."_$time"."_NMX : FLOAT$comment\n".
              "GRP = $grp\n".
              "RCOD = 0\n".
              "CALC =\n".
              "NORMALIZE($n, $N, MAX_RATIO, CON_0);\n\n");
    } elsif($flag eq "MP") {
        $n = $namePre."MAXM_".$coreName;
        my $N = $namePre."MAXP_".$coreName;
        print("VAR $namePre"."N_$coreName"."_MP : FLOAT$comment\n".
              "GRP = $grp\n".
              "RCOD = 0\n".
              "CALC =\n".
              "NORMALIZE($n, $N, MAX_RATIO, CON_0);\n\n");
    } elsif($flag eq "DIST") {
        $n = $namePre.$coreName."_NONFRAUD";
        my $N = $namePre.$coreName."_FRAUD";
        print("VAR $namePre"."N_$coreName"."_DIST : FLOAT$comment\n".
              "GRP = $grp\n".
              "RCOD = 0\n".
              "CALC =\n".
              "NORMALIZE($n, $N, MAX_RATIO, CON_0);\n\n");
    } else {
        warn "Unrecognized flag $flag in NORMALIZE";
    }
}

sub MaxShift {
    my ($baseVar,$grp,$profPre,$namePre,$flag,$decay,$comment) = @_;
# generate core name
    my $coreName = $baseVar."_".$decay;
# deal with prefix and comment
    if($namePre=~/\S+/) {
        $namePre .= "_";
    }
    if($comment=~/\S+/) {
        $comment=" \/\/".$comment;
    }
    print("VAR $namePre"."MAXB_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre.MAXB_$coreName\n".
          "RCOD = 0;\n".
          "\n".
          "VAR $namePre"."MAXP_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre.MAXP_$coreName\n".
          "RCOD = 0;\n".
          "\n".
          "VAR $namePre"."IS_FIRST_MAXSHIFT_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "CALC =\n".
          "BEGIN\n".
          "   IF($namePre"."MAXB_$coreName = 0) THEN\n".
          "      RETURN(1);\n".
          "   ELSE\n".
          "      RETURN(0);\n".
          "ENDVAR;\n".
          "\n".
          "VAR $namePre"."MAXM_$coreName : FLOAT$comment\n".
          "GRP = $grp\n".
          "MAP = $profPre.MAXM_$coreName\n".
          "RCOD = 0\n".
          "CALC =\n".
          "MAX_SHIFT($namePre"."MAXM_$coreName, IS_$flag, ".
          "$namePre"."IS_FIRST_MAXSHIFT_$coreName, $namePre"."DO_SHIFT, ".
          "$namePre"."MAXP_$coreName, $namePre"."MAXB_$coreName, ".
          "$namePre"."$baseVar, $namePre"."DECAY_COEF_$decay);\n\n");
}
          
sub MaxDecay {
    my ($baseVar,$decay,$map,$grp,$flag) = @_;
    print("VAR $map\_MAX_$baseVar\_$decay : FLOAT\n".
          "GRP = $grp\n".
          "MAP = PROF$map.MAX_$baseVar\_$decay\n".
          "RCOD = 0\n".
          "CALC =\n".
          "MAX_DECAY($map\_MAX_$baseVar\_$decay, $flag, ".
          "$map\_$baseVar, $map\_DECAY_COEF_$decay);\n\n");
}

sub Percentile {
    my ($baseVar,$percent,$map,$grp,$flag,$epoch_size) = @_;
    print("VAR $map\_P$percent\_$baseVar : FLOAT\n".
          "GRP = $grp\n".
          "MAP = PROF$map.P$percent\_$baseVar\n".
          "CALC =\n".
          "BEGIN\n".
          "   RETURN(\@estimate_percentile($baseVar, ".
          "$map\_P$percent\_$baseVar, $flag, ".
          "PROF$map.P$percent\_$baseVar\_INFO, $percent, ".
          "$epoch_size));\n".
          "ENDVAR;\n\n");
}

# calling by reference
sub WriteVars {
    my ($type,$baseVars,$elements,$grp,$map,$flags) = @_;

    my ($flag, $var, $time);
    my ($age, $delta);
    if($$type =~ /^DailyRate$/i) {
        foreach my $flag (@$flags) {
            print("$flag\n");
            $flag =~ s/ //g; 
            foreach my $var (@$baseVars) {
                $var =~ s/ //g;
                foreach my $time (@$elements) {
                    $time =~ s/ //g;
                   DailyRate($var,$$grp,"PROF".$$map,$$map,$flag,$time);
                }
            }
        }
    } elsif($$type =~ /^DailyRateCoef$/i) {
        foreach $time (@$elements) {
            $time =~ s/ //g;
            $age = $$baseVars[0];
            $age =~ s/ //g;
            $delta = $$baseVars[1];
            $delta =~ s/ //g;
            DailyRateCoef($time,$age,$delta,$$grp,$$map);
        }
    }
}

sub Risk {
    my ($tableName, $grp, $profPre, $namePre, $def, $ratio, $comment) = @_;
    my @tmp=split/_/,$tableName;
    my $riskName=$tmp[0]."_".$tmp[1];
    my $curName="CURRENT_".$tmp[0];
    my $riskRate=$riskName."_RATE";
    print("VAR $riskName : $tableName\n".
          "GRP = $grp\n".
          "CALC =\n".
          "VAR\n".
          "   tmp_risk : $tableName;\n".
          "BEGIN\n".
          "   Table_Lookup($curName,tmp_risk);\n".
          "   return(tmp_risk);\n".
          "ENDVAR;\n\n");
    print("VAR $riskRate : FLOAT\n".
          "GRP = $grp\n".
          "RCOD = 0\n".
          "CALC =\n".
          "FAL_RISK_RATE($ratio, $riskName.NUMFRDS, \n".
          "$riskName.TOTAL, $def);\n\n")
}

1;
      
