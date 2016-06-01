#! /bin/bash

mUseFiles=`ls select/select.muse.*`

for file in $mUseFiles; do

    #echo ${file/muse/mcount}
    cat $file | perl -nle '$_=~ s/NUMFRAUDAYS=1000/NUMFRAUDAYS=7/; print $_;' > select.tmp
    mv select.tmp ${file/muse/mcount}

done
