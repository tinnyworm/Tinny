#!/bin/sh
# merge sampling data

col_merge -c1-33 /work/nobackup1_tmp1/jaccs/0204_0302/datas/auth.* | gzip \
> /aisle16/yxc/jaccs/0204_0302/merge/auth.gz
sleep 15

col_merge -c1-19,91-98 /work/nobackup1_tmp1/jaccs/0204_0302/datas/card.* | \
gzip > /aisle16/yxc/jaccs/0204_0302/merge/card.gz
sleep 15

col_merge -c1-19,49-56 /work/nobackup1_tmp1/jaccs/0204_0302/datas/post.* | \
gzip > /aisle16/yxc/jaccs/0204_0302/merge/post.gz
sleep 15
