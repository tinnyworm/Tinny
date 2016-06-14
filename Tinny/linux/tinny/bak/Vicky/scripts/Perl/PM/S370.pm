package S370;


require Exporter;

@ISA = ("Exporter");
@EXPORT_OK=qw(ebcdic2ascii ascii2ebcdic I2S370 unpackS370);

$dd_ascii = 
'\000\001\002\003\234\011\206\177\227\215\216\013\014\015\016\017'.
'\020\021\022\023\235\205\010\207\030\031\222\217\034\035\036\037'.
'\200\201\202\203\204\012\027\033\210\211\212\213\214\005\006\007'.
'\220\221\026\223\224\225\226\004\230\231\232\233\024\025\236\032'.
'\040\240\241\242\243\244\245\246\247\250\325\056\074\050\053\174'.
'\046\251\252\253\254\255\256\257\260\261\041\044\052\051\073\176'.
'\055\057\262\263\264\265\266\267\270\271\313\054\045\137\076\077'.
'\272\273\274\275\276\277\300\301\302\140\072\043\100\047\075\042'.
'\303\141\142\143\144\145\146\147\150\151\304\305\306\307\310\311'.
'\312\152\153\154\155\156\157\160\161\162\136\314\315\316\317\320'.
'\321\345\163\164\165\166\167\170\171\172\322\323\324\133\326\327'.
'\330\331\332\333\334\335\336\337\340\341\342\343\344\135\346\347'.
'\173\101\102\103\104\105\106\107\110\111\350\351\352\353\354\355'.
'\175\112\113\114\115\116\117\120\121\122\356\357\360\361\362\363'.
'\134\237\123\124\125\126\127\130\131\132\364\365\366\367\370\371'.
'\060\061\062\063\064\065\066\067\070\071\372\373\374\375\376\377';


$dd_asciib = 
'\000\001\002\003\234\011\206\177\227\215\216\013\014\015\016\017'.
'\020\021\022\023\235\205\010\207\030\031\222\217\034\035\036\037'.
'\200\201\202\203\204\012\027\033\210\211\212\213\214\005\006\007'.
'\220\221\026\223\224\225\226\004\230\231\232\233\024\025\236\032'.
'\040\240\241\242\243\244\245\246\247\250\133\056\074\050\053\041'.
'\046\251\252\253\254\255\256\257\260\261\135\044\052\051\073\136'.
'\055\057\262\263\264\265\266\267\270\271\174\054\045\137\076\077'.
'\272\273\274\275\276\277\300\301\302\140\072\043\100\047\075\042'.
'\303\141\142\143\144\145\146\147\150\151\304\305\306\307\310\311'.
'\312\152\153\154\155\156\157\160\161\162\313\314\315\316\317\320'.
'\321\176\163\164\165\166\167\170\171\172\322\323\324\325\326\327'.
'\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347'.
'\173\101\102\103\104\105\106\107\110\111\350\351\352\353\354\355'.
'\175\112\113\114\115\116\117\120\121\122\356\357\360\361\362\363'.
'\134\237\123\124\125\126\127\130\131\132\364\365\366\367\370\371'.
'\060\061\062\063\064\065\066\067\070\071\372\373\374\375\376\377';


$pipeAsSnakebite =
'\000\001\002\003\234\011\206\177\227\215\216\013\014\015\016\017'.
'\020\021\022\023\235\205\010\207\030\031\222\217\034\035\036\037'.
'\200\201\202\203\204\012\027\033\210\211\212\213\214\005\006\007'.
'\220\221\026\223\224\225\226\004\230\231\232\233\024\025\236\032'.
'\040\240\241\242\243\244\245\246\247\250\325\056\074\050\053\345'.
'\046\251\252\253\254\255\256\257\260\261\041\044\052\051\073\136'.
'\055\057\262\263\264\265\266\267\270\271\174\054\045\137\076\077'.
'\272\273\274\275\276\277\300\301\302\140\072\043\100\047\075\042'.
'\303\141\142\143\144\145\146\147\150\151\304\305\306\307\310\311'.
'\312\152\153\154\155\156\157\160\161\162\313\314\315\316\317\320'.
'\321\176\163\164\165\166\167\170\171\172\322\323\324\133\326\327'.
'\330\331\332\333\334\335\336\337\340\341\342\343\344\135\346\347'.
'\173\101\102\103\104\105\106\107\110\111\350\351\352\353\354\355'.
'\175\112\113\114\115\116\117\120\121\122\356\357\360\361\362\363'.
'\134\237\123\124\125\126\127\130\131\132\364\365\366\367\370\371'.
'\060\061\062\063\064\065\066\067\070\071\372\373\374\375\376\377';

my $default = $dd_ascii;

# Preloaded methods go here.


sub ebcdic2ascii {
    my $e = shift;

    eval '$e =~ tr/\000-\377/' . $default . '/';

    return $e;
}

sub ascii2ebcdic {
    my $a = shift;

    eval '$a =~ tr/' . $default . '/\000-\377/';

    return $a;
}


sub I2S370 ($){
  my $input=shift;
  my ($sign,$xponent,$mantissa,$result);

  if (($input & 0x80000000) != 0){
    $sign = -1.0;
  } else { 
    $sign = 1.0;
  }

  $exponent = ($input & 0x7F000000) >>24;

  $mantissa = $input & 0x00FFFFFF;

  $result = $sign * $mantissa * 16 ** ($exponent - 0x40 - 6);
  return $result;
}

sub PD{
  my ($str,$len)=@_;

  $len*=2;
  my ($rt,$sign)=unpack("a" . ($len - 1) . " a", unpack("H$len",$str));
  $rt= -1*$rt if (lc($sign) eq 'b' || lc($sign) eq 'd');
  return $rt;
}

sub unpackS370 {

#  my ($tmplt,$rec)=@_;
  my $tmplt=shift;
  my $rec=shift;
  my $method=shift || 'dd_ascii';

  if ($method eq 'dd_ascii' || $method eq 'default'){
    $default=$dd_ascii;
  }elsif ($method eq 'dd_asciib'){
    $default=$dd_asciib;
  }elsif ($method eq 'pipeAsSnakebite'){
    $default=$pipeAsSnakebite;
  }else{
    print STDERR "badbad\n";
    die "bad parameter!\n";
  }

  my ($ttmp,@rlist,$i,$pt);

  $ttmp=$tmplt;
  $ttmp=~s/f/N/g;
  $ttmp=~s/D/a/g;

  @rlist=unpack($ttmp,$rec);

  $i=0;
  foreach $pt (split " ",$tmplt){
    next if ($pt =~ /[xX\@]/);
    die "Bad tamplet : $tmplt \n" if ($pt!~/[a-zA-Z]\d*/);
    if ($pt eq 'f'){
      $rlist[$i]=I2S370($rlist[$i]);
    }
    if ($pt =~ /[aA]\d*/){
      $rlist[$i]= ebcdic2ascii($rlist[$i]);
    }
    if ($pt =~ /D(\d*)/){
      $len=($1)?$1:1;
      $rlist[$i]=PD($rlist[$i],$len);
    }
    $i++;
  }

  return (wantarray) ? @rlist : $rlist[0];
}

1;