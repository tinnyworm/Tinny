#! /usr/local/bin/tcsh -f

# Usage: create_dmwwts.sh <BASENAME> <No. of hidden nodes>

cat /work/mse4/pmax/nnet/template.mod | /work/mse4/pmax/nnet/replace.tcl template $1 > $1.mod
/work/mse4/pmax/nnet/model $1 -modeb
/work/mse4/pmax/nnet/nnw2wts.sh $1 $2
