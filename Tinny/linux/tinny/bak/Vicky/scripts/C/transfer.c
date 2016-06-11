#include "stdio.h"
#include "stdlib.h"

main(int argc, char **argv)
{
 char line[1024], Filename[100];
 FILE *f1;
 long i,j;
 strcpy(Filename, argv[1]);
 j=atol(argv[2]);
 f1=fopen(Filename, "r");
 


while(j>0)
{
  fgets(line,1024,f1);
 i=0;
 while (line[i]!='\0')
 { 
	if((line[i]!=',') &&(line[i]!='"'))
       printf("%c", line[i]); i++;
 }
//printf("\n");
j--;
}
fclose(f1);
}

