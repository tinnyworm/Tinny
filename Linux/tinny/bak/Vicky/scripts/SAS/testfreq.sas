/*%macro fileinput(file1,file2, file3, file4);
%let file1=/work/nobackup1_tmp1/freq/&file1;
%let file2=/work/nobackup1_tmp1/freq/&file2;
%let east=/work/nobackup1_tmp1/freq/&file3;
%let west=/work/nobackup1_tmp1/freq/&file4;*/

/*filename fileone pipe "gunzip -c 4664.gz";
filename filetwo pipe "gunzip -c 2838.gz";
filename east pipe "gunzip -c 4412.gz";
filename west pipe "gunzip -c 5010.gz";*/

data temp1 (keep=bins name1);
input namefile $;
infile dummy filevar=namefile end=lastrec;
do until(lastrec=1);
 input bins; 
 name1=namefile;
 output;
end;
cards;
clit2838
clit4664
;
run;
proc sort data=temp1;
by bins;
run;

data temp2 (keep=bins name2);
input namefile $;
infile dummy filevar=namefile end=lastrec;
do until(lastrec=1);
 input bins;
 name2=namefile;
 output;
end;
cards;
clit4412
clit5010
;
run;
proc sort data=temp2;
by bins;
run;

data temp;
merge temp1 temp2;
by bins;
run;

/*ods rtf file='/home/yxc/testsum.rtf' style=statdoc;*/
proc freq data=temp;
tables bins*(name1 name2)/norow nocol nopercent;
run;
/*%mend fileinput;

%fileinput(4664.gz, 2838.gz, 4412.gz, 5010.gz);*/
