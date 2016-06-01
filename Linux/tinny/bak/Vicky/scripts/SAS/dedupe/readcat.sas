libname mylib './';


proc catalog catalog=mylib.formats;
  contents ;
 run;

