package Utils;

use Exporter;

@ISA = ('Exporter'); 

@EXPORT = qw(&trim &round &max_list &min_list &max_hash &min_hash &timediff &str2num);

## use this functin on a scalar like:
##    $string = trim($string);
## on an array like:
##    @many   = trim(@many);

sub trim {
    my @out = @_;
    for (@out) {
        s/^\s+//;
        s/\s+$//;
    }
    return wantarray ? @out : $out[0];
}



#rounds integers

sub round_to_int {
  my($number) = shift;
  return int($number + .5);
}


# my @list = qw(3 5 8 20 3 129 4 4 2 34374 -48747 33 244 2237); 

# my %hs = (a =>3,  b => 5,  c => 8,  d => 20, e => 3, f => 129,  g => 4,   h => 4,  j => 2,  k => 34374,  l => -48747,  m => 33,  n => 244,  o => 2237);


# print "max of list  is ", max_list(@list), "\n";
# print "min of list is ", min_list(@list), "\n";


# my $max_h = max_hash(\%hs);
# foreach (keys %$max_h) {print "max  of hash  is $_ => ", $max_h->{$_}, "\n";}

# my $min_h = min_hash(\%hs);
# foreach (keys %$min_h) {print "min  of hash  is $_ => ", $min_h->{$_}, "\n";}




sub max_list {
  @list  = @_;
  
  $max = -9999999999999999999999999999999;
  
  foreach (@list) {
    if ($_ > $max) {
      $max = $_;
    }
  }
  return $max;
}

sub min_list {
  @list  = @_;
  
  $min = 9999999999999999999999999999999;
  
  foreach (@list) {
    if ($_ < $min) {
      $min = $_;
    }
  }
  return $min;
}


sub max_hash {
  
  $hr = shift @_;

  %h = %$hr;

  $max = -9999999999999999999999999999999;
  $max_key;  

  foreach (keys %h) {
    if ($h{$_} > $max) {
      $max = $h{$_};
      $max_key = $_;
    }
  }

  return {$max_key => $max};
}


sub min_hash {
  
  $hr = shift @_;

  %h = %$hr;

  $min = 9999999999999999999999999999999;
  $min_key;  

  foreach (keys %h) {
    if ($h{$_} < $min) {
      $min = $h{$_};
      $min_key = $_;
    }
  }

  return {$min_key => $min};
}



sub timediff {

  $t1 = shift;
  $t2 = shift;

  ($h1, $m1, $s1) = unpack("a2 a2 a2", $t1);
  ($h2, $m2, $s2) = unpack("a2 a2 a2", $t2); 

  $diff =  ($h1 - $h2)*3600 + ($m1- $m2)*60 + ($s1 - $s2);
  

  return $diff;
}


sub sinh {
  $val = shift;
  $t = exp($val);
  $sinh = ($t - 1/$t)/2;
  return $sinh;
}

sub cosh {
  $val = shift;
  $t = exp($val);
  $cosh = ($t + 1/$t)/2;
  return $cosh;
}

sub tanh {
  $val = shift;
  $t = exp($val);
  $tanh = ($t - 1/$t)/($t + 1/$t);
  return $tanh;
}


sub coth {
  $val = shift;
  $t = exp($val);
  $coth = ($t + 1/$t)/($t - 1/$t);
  return $coth;
}

sub str2num {

    use POSIX qw(strtod);

    my $str = shift;

    $str =~ s/^\s+//;
    $str =~ s/\s+$//;

    $! = 0;

    my($num, $unparsed) = strtod($str);

    if (($str eq '') || ($unparsed != 0) || $!) {
        return;
    } else {
        return $num;
    } 
} 



1;
