#! /bin/rm
#  fit the new length to the old length for Japanese clients

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
sas=/export/home/Applications/sas/sas

# for auths file with the new length of 324
echo "auths is transfering..."
$zz auth*00324*.dat.gz  | $sas  /home/yxc/test/fixlength_auth.sas | \
     $zort -c1-33 | gzip >auths.0211_0304.00195.dat.gz
echo " auths is done."
echo " "     

# for cards file with the new length of 291
echo "cards is transfering..."
$zz card*00291*dat.gz | $sas /home/yxc/test/fixlength_card.sas | \
     $zort -c1-19,91-98 | gzip >cards.0211_0304.00202.dat.gz
echo "cards is done."
echo " "     

# for posts file with the new length of 318
echo "posts is transfering..."
$zz posts*00318*.dat.gz | $sas /home/yxc/test/fixlength_post.sas | \
     $zort -c1-19,49-56 | gzip >posts.0211_0304.00179.dat.gz
echo "posts is done."
echo " "
     
# for nonmons file with the new length of 63
#echo "nonms is transfering..."
#$zz *00063*.gz | $sas /home/yxc/test/fixlength_nonms.sas | \
#      $zort -c1-19,21-28 | gzip > nonms.months.00047.dat.gz
#echo "nonms is done."
     
