#!/bin/csh -f
# specify as many conversion tables as you please...

gawk 'BEGIN{print "\"(reference)\"";for (i=0;i<=1000;i++){print i,i}}(FNR==1){print "\n\"" FILENAME "\""}{sub(","," ")}1' $* | ~dxl/bin/xgraph
