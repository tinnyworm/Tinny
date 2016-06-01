#! /bin/sh


for file in `cat filelist`
do
name=`echo $file | sed "s:\.Z.gz: :"`
echo $name
gunzip -c $file | gunzip -c > $name
sleep 30
tar xvf $name > $name.log
sleep 30
echo "$name is done..."
done

banner done
