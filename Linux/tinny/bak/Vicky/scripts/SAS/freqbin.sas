option nocenter nodate ps=max;
%macro freqbin(ifile);
%let ifile=/work/nobackup1_tmp1/freq/&ifile;
%let ofile=%substr(&ifile,1, 4);
filename ifile pipe "gunzip -c &ifile";

data temp;
infile ifile;
input bins @@;

run;

ods html body="/home/yxc/&ofile..html";
proc freq data=temp;
run;
%mend freqbin;

%freqbin(2838.gz)
/*%freqbin(4236.gz, 4236.html);*/
%freqbin(4412.gz);
/*%freqbin(4664.gz, 4664.html);
%freqbin(5010.gz, 5010.html);
%freqbin(5730.gz, 5730.html);
%freqbin(6100.gz, 6100.html);
%freqbin(2320.gz, 2320.html);
%freqbin(2330.gz, 2330.html);*/
