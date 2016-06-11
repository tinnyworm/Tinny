#! /bin/csh -f
#

sed "s/</a/g" | sed "s/?/b/g" | sed "s/&/d/g" | sed "s/!/e/g" | sed "s/*/f/g"|\
sed "s/>/g/g" | sed "s/%/h/g" | sed "s/#/i/g" | sed "s/@/j/g" | sed 's/\$/c/g'
