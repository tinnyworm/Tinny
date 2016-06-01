#! /usr/local/bin/bash

par=$1; 

cat plot*.ps > tt.ps
ps2ps tt.ps tt1.ps
psnup -l -pletter -Pletter -$par tt1.ps tt.ps &>/dev/null
lpr -Psdoc0301 tt.ps
ps2pdf tt.ps


