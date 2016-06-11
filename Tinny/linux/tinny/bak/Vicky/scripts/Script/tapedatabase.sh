#! /bin/sh
	dbfile=/work/applshar/jcl/dbexport/export_view_all.gz
	sas=/export/home/Applications/sas/sas
	zz=/usr/local/bin/zcat

#	dd if=$dbfile | grep -v nternal | sed "s:| |:|:g" | $sas tapedatabase 
	$zz $dbfile | dos2unix | grep -v nternal | $sas tapedatabase 
