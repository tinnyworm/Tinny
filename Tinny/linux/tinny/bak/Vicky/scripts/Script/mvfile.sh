#! /bin/sh
#

for file in `ls *034897*`
do
echo $file
key=`echo $file | cut -c6-7`
case "$key" in 
  AP)     filename=`echo "0306.AUTH.00204.$file"`  ; 
          mv $file ../0306/$filename ;;
  CP)     filename=`echo "0306.CARD.00186.$file"`  ;
          mv $file ../0306/$filename ;;
  FP)     filename=`echo "0306.FRAUD.00049.$file"` ;
          mv $file ../0306/$filename ;;
  IP)     filename=`echo "0306.INQ.00034.$file"`   ;
          mv $file ../0306/$filename ;;
  NP)     filename=`echo "0306.NONM.00047.$file"`  ;
          mv $file ../0306/$filename ;;
  PP)     filename=`echo "0306.POST.00201.$file"`  ;
          mv $file ../0306/$filename ;;
  YP)     filename=`echo "0306.PAY.00108.$file"`   ;
          mv $file ../0306/$filename ;;
esac
echo $filename
echo " "

done
