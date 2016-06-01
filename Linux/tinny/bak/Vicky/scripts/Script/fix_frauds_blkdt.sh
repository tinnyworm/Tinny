#! /bin/sh


#runall=/home/yxc/fdr_scripts/run_all 
fdr_scripts=/home/yxc/fdr_scripts
zort=`echo "bmsort -T /tmp -S"`

gunzip -c auths/authsSplit.0.gz | ufalcut -c4-271 | $fdr_scripts/fix_fdr_2000_auths.pl | $fdr_scripts/fix_fdr_merch_cntry.SunOS \
| $fdr_scripts/fix_fdr_turkey.pl| $zort -c1-33| gzip > auths.fdr_fix.dat.gz

sleep 10
echo "starting to generate the blockdate upon auths/authsSplit.0.gz ..."
gunzip -c frauds.dat.gz | $fdr_scripts/blocktime.pl auths.fdr_fix.dat.gz | gzip > frauds.fdr_fix.dat.gz
sleep 10
ls -l frauds.fdr_fix.dat.gz
mv  frauds.fdr_fix.dat.gz frauds.tmp.gz

for file in `ls auths/*gz | grep -v auths/authsSplit.0.gz`
do
echo "----------------------"
gunzip -c $file | ufalcut -c4-271 | $fdr_scripts/fix_fdr_2000_auths.pl | $fdr_scripts/fix_fdr_merch_cntry.SunOS \
| $fdr_scripts/fix_fdr_turkey.pl  | $zort -c1-33|  gzip > auths.fdr_fix.dat.gz
sleep 10
echo "starting to generate the blockdate upon $file..."
gunzip -c frauds.tmp.gz | $fdr_scripts/blocktime.pl auths.fdr_fix.dat.gz | gzip > frauds.fdr_fix.dat.gz
sleep 10
ls -l frauds.fdr_fix.dat.gz
mv  frauds.fdr_fix.dat.gz frauds.tmp.gz
done

banner done
