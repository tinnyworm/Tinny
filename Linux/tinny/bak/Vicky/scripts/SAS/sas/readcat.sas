libname mylib '/work/gold/projects/bin/CCDS/4.10/MACROS/2.2/yxc';
libname library '/work/gold/projects/bin/CCDS/4.10/formats/';

options mstored sasmstore=mylib;

proc catalog catalog=mylib.sasmacr;
  contents ;
 run;

proc catalog catalog=library.formats;
  contents ;
 run;
