#! /bin/rm
#  fit the new length to the old length for Japanese clients

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
sas=/export/home/Applications/sas/sas

# for auths file with past data
echo "auths is transfering..."
$zz auth*0111*  | $sas  /home/yxc/test/filter_auth.sas | \
     $zort -c1-33 | gzip >auths.200201.00195.dat.gz
echo " auths is done."
echo " "     

# for cards file with past data
echo "cards is transfering..."
$zz card*0111* | $sas /home/yxc/test/filter_card.sas | \
     $zort -c1-19,91-98 | gzip >cards.200201.00202.dat.gz
echo "cards is done."
echo " "     

# for posts file with past data
echo "posts is transfering..."
$zz posts*0111* | $sas /home/yxc/test/filter_post.sas | \
     $zort -c1-19,49-56 | gzip >posts.200201.00179.dat.gz
echo "posts is done."
     
