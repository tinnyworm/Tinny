#!/bin/sh

for i in `seq 0 287`;
do
	grep  "\<VAR$i\>" corr.txt | ../tools/convertCol.pl > corr/var$i.cor
done
