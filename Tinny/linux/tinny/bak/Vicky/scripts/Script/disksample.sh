#! /bin/sh
zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`

for file in `ls totape`
do
	filetype=`echo $file | sed "s:\.:\ :g" | awk '{print $1}'`
	case "$filetype" in
	auths) 
		key=`echo "1-33"`
		flag=a
		l=160
		;;
	posts)
		key=`echo "1-19,49-56"`
		flag=p
		l=160
		;;
	cards)	
		key=`echo "1-19,91-98"`
		flag=c
		l=200
		;;
	payments)
		key=`echo "1-19,82-89"`
		flag=P
		l=120
		;;
	nonmons)
		key=`echo "1-19,21-28"`
		flag=N
		l=60
		;;
	*) echo "$file" ; exit 13 ;;
	esac

	$zz totape/$file | sampstats -s10 -$flag -ostats/$file |  $zort -c$key | gzip > datas/$file
echo "$? $file `date`"
done

exit 0

	case "$filetype" in
	cards) 
	$zz links/$file | perl -pe 'chomp' | $fix -r$l -C | sampstats -s10 \
	-$flag -ostats/$file |  $sas $modcard | $zort -c$key | gzip > datas/$file
	;;
	*)
	$zz links/$file | perl -pe 'chomp' | $fix -r$l -C | sampstats -s10 \
	-$flag -ostats/$file |  $zort -c$key | gzip > datas/$file
	;;
	esac

echo "$? $file `date`"
mv rejects.gz rejects/$file
done
		
