# by Seyhan Ersoy

package Xml;
require Exporter;
@ISA = (Exporter);
@EXPORT = qw(array);

use strict;

my $obj_id = 0;
my $tree_level = 0;
my %seen = ();
my $S2 = "  ";
my $S3 = "   ";

# this hash holds names

sub new {
    my $P = shift;
    my $O = {};
    bless $O,ref $P || $P;
    $O->{ID} = $obj_id++; # each object has its uniq id
    $O->{name}  = shift;
    $O->{K}  = []; # contains key names "something"
    $O->{V}  = []; # contains values for keys
    $O->{KV} = (); # contains key value  pairs
    $O->{ARG}= (); # contains argument (unit) <temperature unit=F>98</temperature>
    $O->{HASH}= (); # contains key=unit value=number which is incrimented
    return $O;
}

=pod
sub array() {
#   my $O = shift;
    my $temp = shift;
    my @arr = ();
    if(ref($temp) eq "ARRAY") {
       @arr = @$temp;
    } else {
       $arr[0] = $temp;
    }
    return @arr;
}
=cut
# AUTOLOAD is used to set values or to get values
# if there is no argument it is to get values
# if there is argument it is to set values
sub AUTOLOAD {
    my $O = shift;
    my $func = $Xml::AUTOLOAD;
    $func =~ s/.*:://;
    return if $func eq 'DESTROY';

    if( @_) {
       # AUTOLOAD is used in set mode  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
       my $argm = shift; 
       if(ref($argm) eq "ARRAY") { # used in get mode with array ref <<<<<
          my $mmm = $argm->[0];
          my $id = $O->{ID};
          my $indx = 0;
          if(@_) {
             $indx = shift;
          }
          $id .= $indx;
          my $keyy = $func.$mmm.$id;
          my $str = $O->{ATTR0}->{$keyy};
          return $str;
       }
       if( @_ ) { # there is attributes in the argument 
          my $args = shift;
          my @arrr = ( %$args );
          my $ssss = join(" ",@arrr);
          $ssss =~ s/=/ /g;
          $args = { split(" ",$ssss) };
          my $key;
          my $temp = "";
          my $indx;
          my $keyy;
          my $id;
          foreach $key ( keys %$args ) {
             $id = $O->{ID};
             $temp .= " $key = ".$args->{$key};
             $keyy = $func.$key; 
             $indx = $O->{HASH}->{$keyy}; 
             if($indx) {
                 $indx++;
             } else {
                 $indx = 1;
             }
             $indx--;
             $id .= $indx;
             $indx++;
             $O->{HASH}->{$keyy} = $indx;
             my $keyyy = $keyy.$id;
             $O->{ATTR0}->{$keyyy} = $args->{$key};
          }
          $O->{ATTR}->{$func.$id} = $temp;
       }
       push(@{$O->{K}},$func);
       push(@{$O->{V}},$argm);
       push(@{$O->{KV}->{$func}},$argm);
       return;
    }
    # AUTOLOAD is used in get mode  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if(!$O->{KV}->{$func}) {
       return ""; # value is not defined return emty string
    }
    my @values = @{$O->{KV}->{$func}};

    if($#values == 0) {
       return $values[0];
    } else {
#       return \@values;
       return @values;
    }
}

sub count {                            
  
  my $O = shift;               
  my $name = @_[0]; 
 
  my @ark = @{$O->{K}};             
  my @arv = @{$O->{V}};            
  my $key;  
  my $count = 0;                 
 
  my $i = 0;  
  foreach $key ( @ark ) {       
    my $temp = $arv[$i];  
                        
    if ($name eq $temp->{name} ) 
    {                                     
      $count++; 
    }                                                
    $i++; 
  }                         
  return $count; 
   
}     
   


sub print {
    my $O = shift;
    print $O->string;
}

sub string {
    my $O = shift;
    my $str = "";
    if( !@_ ) {
       $str .= "Content-type: text/plain\n\n";
#      $str .= "<?xml version=\"1.0\"?>\n";
    }
    my $space = shift;
    my $space1 = $space."   ";
    my @ark = @{$O->{K}};
    my @arv = @{$O->{V}};
    my $key;
    $str .= $space."<".$O->{name}.">\n";
    my $i = 0;
    my $j = 0;
    my $id = $O->{ID};
    my %hash = ();
    foreach $key ( @ark ) {
       if($hash{$key}) {
          $hash{$key}++;
       } else {
          $hash{$key} = 1; 
       }
       $j = $hash{$key} - 1;
       my $temp = $arv[$i];
       if(ref($temp) eq "Xml") {
          $str .= $temp->string($space1)."\n";
       } elsif(ref($temp) eq "ARRAY") {
          my $ttt;
          foreach $ttt ( @{$temp}) { 
             $str .= "$space1<".$key."$O->{ATTR}->{$key.$id.$j}>".$ttt."</".$key.">"."\n";
          }
       } else {
          $str .= "$space1<".$key."$O->{ATTR}->{$key.$id.$j}>".$temp."</".$key.">"."\n";
       }
       $i++;
    }
    $str .= "$space</".$O->{name}.">";
    return $str;
}

sub parse {
    my $P   = shift;
    my $str = shift;
    _parse($str);
}
    
sub read {
    my $P    = shift;
    my $file = shift;
    open(FILE, $file) or die @!;
    my $str; 
    while(<FILE>) {
       $str .= $_;
    }
    _parse($str);
}

sub _parse {
    my $O;
    my $str = shift;
    my @obj = ();
    # remove all spaces and new lines between > and </
    $str =~ s/>[ \n]*</></g;
    # remove all spaces and new lines at the end of string
    $str =~ s/>[ \n]*$/>/g;
    # first remove all comments from string
    $str =~ s/<!--[^<>]*-->//g;
    # remove xml decleration line, if there is need we can process it later
    $str =~ s/<\?.*\?>//g;
    # remove DOCTYPE decleration, if there is need we can process it later
    $str =~ s/<! *DOCTYPE .>//g;
    # remove <!ELEMENT ...> lines 
    $str =~ s/<!ELEMENT .>//g;
    # @arr contains arrays in each cell. The contained arrays 
    # are called @sarr ( small array ) and contains four values
    # assume that we want to parse following statement
    # <something a='A' b='B'>value</something> 
    # $sarr[0] contains something
    # $sarr[1] contains either 1 or 2 
    #          if it is 1 that means we matched <something a='A' b='B'>
    #          if it is 2 that means we matched </something>
    # $sarr[2] contains value if $sarr[1] == 1 or else it is empty
    # $sarr[3] contains match value which is
    # <something a='A' b='B'> if $sarr[1] == 1 or
    # </something> if $sarr[1] == 2 
    my @arr = ();
    while($str ne "") {
        $_ = $str;
        m/<([a-zA-z0-9:_]+)[-=a-zA-Z0-9_:.'" *]*>|<\/([a-zA-z0-9:_]+)>/;
        my @sarr = ();
        if($1 ne "") {
           push(@sarr,$1);
           push(@sarr,1);
        } else {
           push(@sarr,$2);
           push(@sarr,2);
        }
        push(@sarr,$`);
        push(@sarr,$&);
#       print "DOLAR1 = $sarr[0] \n";
#       print "DOLAR2 = $sarr[1] \n";
#       print "VALUE  = $sarr[2] \n";
#       print "MATCH  = $sarr[3] \n"; 
        $str = $'; # whatever string left after the match
        # check whether keys contain any arguments
        if($sarr[1] == 1) {
           # process arguments <<<<<<<<<<<<<<<<<<<<<<<<<<<<
           my $arg = $sarr[0];
           my $tp = $&;
           $tp =~ s/$arg//;  # remove key from string
           $tp =~ s/ *< *//; # remove < and spaces around it 
           $tp =~ s/ *> *//; # remove > and spaces around it
           if($tp ne "") {
              my $args = {split(" ",$tp)};
              push(@sarr,$args); # matched text itself
           }
        }
        push(@arr,\@sarr);
    } 
    my $len = $#arr;
    my @ar1 = @{$arr[0]};
    my @ar2 = @{$arr[1]};
    my $root = $ar1[0];
    $O = Xml->new($ar1[0]); 
    push(@obj,$O);
    my $i = 1;
    while($i != $len) {
       @ar1 = @{$arr[$i]};
       my $slen = $#ar1;
       my $key1 = $ar1[0];
       if($root ne $key1) {
          @ar2 = @{$arr[$i+1]};
          my $key2 = $ar2[0];
          if($key1 eq $key2) {
             if($ar1[1] == 1) {
                my $obj = $obj[$#obj];
                # ----------------------------
                my $func = $key1;
                my $arg  = $ar2[2];
                if($slen == 4) {
                   $obj->$func($arg,$ar1[4]);
                } else {
                   $obj->$func($arg);
                }
                # ----------------------------
                $i += 2;
             } else {
                # first pop value from object stack
                $#obj  = $#obj-1; 
                my $slen = $#ar2; # overrite slen
                my $temp = Xml->new($key1);
                my $obj = $obj[$#obj];
                # ----------------------------
                if($slen == 4) {
                   $obj->$key1($temp,$ar2[4]);
                } else {
                   $obj->$key1($temp);
                }
                # ----------------------------
                # then add new obj to stack
                push(@obj,$temp);
                $i += 2;
             }
          } else {
             if($ar1[1] == 1) { # this is beginning of new object
                my $temp = Xml->new($key1); 
                my $obj = $obj[$#obj];
                # ----------------------------
                if($slen == 4) {
                   $obj->$key1($temp,$ar1[4]);
                } else {
                   $obj->$key1($temp);
                }
                # ----------------------------
=pod
                if($slen == 4) {
                   my $key;
                   foreach $key ( keys %{$ar1[4]} ) {
                      my $kk = $key1."_".$key;
                      $temp->{$kk} = $ar1[4]->{$key};
                   }
                }
=cut
                push(@obj,$temp);
             } else { # this is closing for previous object
                $#obj  = $#obj-1; # popup a value from stack
             }
             $i++;
          }
       }
    }
   
    return $O;
}

sub write_sun_java {
    my $O = shift;
    my $str = "";
    my $to_str = "";

    my @ark = @{$O->{K}};
    my @arv = @{$O->{V}};
    my $key;
    my $sps = "";
    my $sp1_2   = "    ";
    if($tree_level > 0) {
       $sps = "   ";
    }
    if(!$seen{$O->{name}}) {
       if($tree_level == 0) {
          print "import java.io.*;\n";
          print "import org.w3c.dom.*;\n";
          print "\n";
       }
       $seen{$O->{name}} = 1;
       print "$sps $sp1_2 public class \u$O->{name}\n";
       print "$sps $sp1_2 {\n";
       $to_str  = "\n\n";
       $to_str .= "$sps $sp1_2 public String toXML() {\n";
       $to_str .= "$sps $sp1_2 String str = new String();\n";
       $to_str .= qq{$sps $sp1_2 str = "<$O->{name}>" + "\\n";\n}; # OPEN01
       my $i = 0;
       my @temp_ark = @ark;
       my %lseen = ();
       foreach $key ( @ark ) {
          my $count = 0;
          my $k;
          foreach $k (@temp_ark) {
             if($k eq $key) {
                $count++;
             }
          }
          my $temp = $arv[$i];
          if(ref($temp) eq "Xml") {
             my $name = $temp->{name};
             if(!$lseen{$name}) {
                $lseen{$name} = 1;
#               $to_str .= qq{$sps $sp1_2 str += "<$name>"}; # OPEN02
#               $to_str .= qq{ + "\\n";\n};
                if($count == 1) {
                   $to_str .= "$sps $sp1_2 str += $name.toXML();\n";
                   print "$sps $sp1_2 public \u$name $key;\n";
                }
                if($count > 1) {
                   my $sp = "      ";
                   $to_str .= "$sps $sp1_2 for(int i = 0; i < $name"."Length; i++) {\n";
                   $to_str .= "$sps $sp1_2    str += $name"."[i].toXML();\n";
                   $to_str .= "$sps $sp1_2 }\n";
#                  $to_str .= qq{$sps $sp1_2 str += "</$name>" + "\\n";\n}; # CLOSE02
                   print "$sps $sp1_2 public int $key"."Length;\n";
                   print "$sps $sp1_2 public \u$name"."[] $key;\n";
                }
             }
          } else {
             if(!$lseen{$key}) {
                $lseen{$key} = 1;
                $to_str .= qq{$sps $sp1_2 str += "<$key>"};
                $to_str .= " + $key ";
                $to_str .= qq{+ "</$key>" + "\\n";\n};
                if($count == 1) {
                   print "$sps $sp1_2 public String $key;\n";
                }
                if($count > 1) {
                   print "$sps $sp1_2 public int $key"."Length;\n";
                   print "$sps $sp1_2 public String[] $key;\n";
                }
             }
          }
          $i++;
       }
       $to_str .= qq{$sps $sp1_2 str += "</$O->{name}>" + "\\n";\n}; # CLOSE01
       $to_str .= "$sps $sp1_2 return str;\n";
       $to_str .= "$sps $sp1_2 }\n";
       print "\n";
# now set lseen to empty hash so that we can go through one more time
       %lseen = ();
       $i = 0;
       if($tree_level == 0) {
          # constractor Type(File file)
          print "$sps $sp1_2 public \u$O->{name}"."(File file) {\n";
          print "$sps $sp1_2 this(XmlParserHelper.getDocFromFile(file));\n";
          print "$sps $sp1_2 }\n\n";
          # constractor Type(String str)
          print "$sps $sp1_2 public \u$O->{name}"."(String str) {\n";
          print "$sps $sp1_2 this(XmlParserHelper.getDocFromStr(str));\n";
          print "$sps $sp1_2 }\n\n";
          # constractor Type()
          print "$sps $sp1_2 public \u$O->{name}"."() {\n";
          $O->_fill_constractor();
          print "$sps $sp1_2 }\n\n";
          print "$sps $sp1_2 public \u$O->{name}"."(Document doc) {  // OPEN_FN_1\n";
          print "$sps $sp1_2 this();\n";
          print "$sps $sp1_2 if(doc != null) {   // OPEN_IF_1\n";
          print "$sps $sp1_2 Node $O->{name}"."_node\n";
          print "$sps $sp1_2 = XmlParserHelper.getNodeByTagName(doc,\"$O->{name}\");\n";
       } else {
          print "$sps $sp1_2 private \u$O->{name}"."() { // OPEN_CON_2\n";
          print $O->_fill_constractor();
          print "$sps $sp1_2 }              // CLOSE_CON_2\n";
          print "$sps $sp1_2 public \u$O->{name}"."(Node $O->{name}"."_node) { // OPEN_CON_1\n";
          print "$sps $sp1_2 this();\n";
          print "$sps $sp1_2 if( $O->{name}"."_node != null) {              // OPEN_FN_2\n";
       }
       foreach $key ( @ark ) {
          my $count = 0;
          my $k;
          foreach $k (@temp_ark) {
             if($k eq $key) {
                $count++;
             }
          }
          my $temp = $arv[$i];
          if(ref($temp) eq "Xml") {
             my $name = $temp->{name};
             if(!$lseen{$name}) {
                $lseen{$name} = 1;
                if($count == 1) {
                   print "$sps $sp1_2 Node $name"."_node\n";
                   print "$sps $sp1_2 = XmlParserHelper.getNodeByTagName($O->{name}";
                   print "_node, \"$key\");\n";
                   print "$sps $sp1_2 $key = new \u$key"."($key"."_node);\n";
                }
                if($count > 1) {
                   print "$sps $sp1_2 NodeList $name"."_nodeList\n";
                   print "$sps $sp1_2 = XmlParserHelper.getNodeListByTagName($O->{name}";
                   print "_node, \"$key\");\n\n";
#                  print "$sps $sp1_2 $key"."Length = $name"."_nodeList.getLength();\n";
                   print "$sps $sp1_2 $key"."Length = $name"."_nodeList.getLength();\n";
                   print "$sps $sp1_2 $key = new \u$name"."[".$key."Length];\n";
                   print "\n"; 
                   print "$sps $sp1_2 for(int i = 0; i < $key"."Length; i++) {\n";
                   print "$sps $sp1_2     Node $key"."_node = $key"."_nodeList.item(i);\n";
                   print "$sps $sp1_2     $key"."[i] = new \u$key"."($key"."_node);\n";
                   print "$sps $sp1_2 }\n";
                }
             }
          } else {
             if(!$lseen{$key}) {
                $lseen{$key} = 1;
                if($count == 1) {
                   print "$sps $sp1_2 Node $key"."_node\n";
                   print "$sps $sp1_2 = XmlParserHelper.getNodeByTagName($O->{name}";
                   print "_node,\"$key\");\n";
                   print "$sps $sp1_2 $key = XmlParserHelper.myGetNodeValue($key"."_node);\n";
                }
                if($count > 1) {
                   print "$sps $sp1_2 NodeList $key"."_nodeList\n";
                   print "$sps $sp1_2 = XmlParserHelper.getNodeListByTagName($O->{name}";
                   print "_node, \"$key\");\n\n";
#                  print "$sps $sp1_2 $key"."Length = $key"."_nodeList.getLength();\n";
                   print "$sps $sp1_2 int $key"."Length = $key"."_nodeList.getLength();\n";
                   print "\n";
                   print "$sps $sp1_2 $key = new String[$key"."Length];";
                   print "$sps $sp1_2 for(int i = 0; i < $key"."Length; i++) {\n";
                   print "$sps $sp1_2     Node $key"."_node = $key"."_nodeList.item(i);\n";
                   print "$sps $sp1_2     $key"."[i] = XmlParserHelper.myGetNodeValue($key";
                   print "_node);\n";
                   print "$sps $sp1_2 }\n";
                }
             }
          }
          $i++;
       }
       if($tree_level > 0) {
          print "$sps $sp1_2 }    // CLOSE_IF_2\n";
          print "$sps $sp1_2 }    // CLOSE_CON_1\n";
          print "$sps $sp1_2 $to_str \n";
          print "$sps $sp1_2 }\n\n";
       } else {
          print "$sps $sp1_2 }    // CLOSE_IF_1\n";
          print "$sps $sp1_2 }    // CLOSE_FN_1\n\n";
          print "$sps $sp1_2 $to_str \n";
       }
    }
    my $i = 0;
    foreach $key ( @ark ) {
       my $temp = $arv[$i];
       if(ref($temp) eq "Xml") {
          $tree_level++;
          $temp->write_sun_java;
          $tree_level--;
       }
       $i++;
    }
    if($tree_level == 0) {
       print "}\n";
    }
}

sub _fill_constractor {
    my $O = shift;
    my $str = "";
    my $to_str = "";

    my @ark = @{$O->{K}};
    my @arv = @{$O->{V}};
    my $key;
    my $sps = "";
    my $sp1_2   = "    ";
    if($tree_level > 0) {
       $sps = "   ";
    }
    my $i = 0;
    my @temp_ark = @ark;
    my %lseen = ();
    foreach $key ( @ark ) {
       my $count = 0;
       my $k;
       foreach $k (@temp_ark) {
          if($k eq $key) {
             $count++;
          }
       }
       my $temp = $arv[$i];
       if(ref($temp) eq "Xml") {
          my $name = $temp->{name};
          if(!$lseen{$name}) {
             $lseen{$name} = 1;
             if($count == 1) {
                print "$sps $sp1_2    this\.$name = new \u$name();\n";
             }
             if($count > 1) {
                print "$sps $sp1_2    this\.$name = new \u$name"."[0];\n";
             }
          }
       } else {
          if(!$lseen{$key}) {
             $lseen{$key} = 1;
             if($count == 1) {
                print "$sps $sp1_2    this\.$key = new String();\n";
             }
             if($count > 1) {
                print "$sps $sp1_2    this\.$key"."[$count]"." = new String[$count];\n";
             }
          }
       }
       $i++;
    }
}
1;  # so the require or use succeeds
