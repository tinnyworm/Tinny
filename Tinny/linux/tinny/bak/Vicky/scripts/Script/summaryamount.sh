#! /bin/sh 
#  Japan dqr summary for auth and post

zz=/usr/local/bin/zcat
sas=/export/home/Applications/sas/sas

###############################
####### CUT AUTH  #############
###############################
echo "cutting auths..."
$zz auths.195.gz | cut -c34-49,98-100 | gzip >auth.AM_CR_MC.gz 
echo "auth.AM_CR_MC.gz is ready..."
echo " "

###############################
######## CUT POST  ############
###############################
echo "cutting posts..."
$zz posts.179.gz | cut -c20-35,106-108 | gzip >post.AM_CR_MC.gz 
echo "post.AM_CR_MC.gz is ready..."
echo " "

###############################
### Generate the tables #######
###############################

echo "SAS is starting to work..."
$sas /home/yxc/test/summaryamount.sas
echo "______________"
echo ">>>> DONE <<<<"
