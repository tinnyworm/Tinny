## -*- Mode: awk -*-
## Title:  extract.by.account_num3
##
## Doc: /home/fxc/project/NTL/programs/Awk/extract.by.account_num3.awk
## Original Author:     Fei Chen  <fxc@production1.fairisaac.com>
## Created:             Fri Dec 19 2003
##
## $Header$
##
## $Log$
##
## $Id$
########################################################


BEGIN{
# generated by
# cfg=/home/fxc/project/NTL/ux58//field,length
# cat $cfg | awk "BEGIN{printf(\"FIELDWIDTHS = \\\"\")}{printf(\" %d \", \$2)}END{printf(\"\\\"\")}"

# to extract the account number
# zcat NTL_FM_DATA/NTLConsortium2003_032_2217.gz  | awk "BEGIN{
# $(cat $cfg | awk "BEGIN{printf(\"FIELDWIDTHS = \\\"\")}{printf(\" %d \", \$2)}END{printf(\"\\\"\")}")
# }
# { print \$3; 
# }" 

 if(prefix == "")
   exit;
  set_arrays();
  FIELDWIDTHS = " 1  4  24  24  8  6  16  2  1  5  5  5  5  1  1  1  24  24  32  24  3  1  16  3  3  5  5  5  8  6  10  8  6  1  1  1  1  1  4  1  24  24 ";

 nfraud = 1;
 notfraud = 1;
 unconfirmed = 1;

 print "initialization done";

}
{
  acct = $3;
  # certain acct numbers begin with a prefix in the fraud files
  if(acct in map)
    acct = map[acct];

  if(acct in fraud)
    {
      v = fraud[acct];

      if(v == "Fraud")
	nfraud ++;
      else if(v == "NotFraud")
	notfraud ++;
      else if(v == "UnConfirmed")
	unconfirmed ++;

      fn = prefix "_" fraud[acct];
      print $0 > fn;
    }

  at = 500000;

  if(NR % at == 0)
    printf("%d:%-8d: unconfirmed=%-5d notfraud=%-5d fraud=%-5d\n", filenum, NR , unconfirmed-1, notfraud-1, nfraud-1);

#  print_freqently_at(fraud, NR, at, "Fraud");
#  print_freqently_at(notfraud, NR, at, "NotFraud");
#  print_freqently_at(unconfirmed, NR, at, "UnConfirmed");

}
END{
  if(prefix == "")
    {
      print "-v prefix=XX for output file name is required";
      exit;
    }
  printf("%d:%-8d: unconfirmed=%-5d notfraud=%-5d fraud=%-5d\n", filenum, NR , unconfirmed-1, notfraud-1, nfraud-1);
}


function print_freqently_at(x, counter, at, label)
{
  # print x and label at a rate of "at"
  if(counter % at == 0)
    printf("%s: %15s %d\n", counter , label, x);
}
