#include "stdio.h"
#include "stdlib.h"
main(int argc, char **argv)
{
 char line[1024], Filename[100],Date[10]; 
 FILE *f1; 
 long i,j,n,k;
strcpy(Filename, argv[1]);
j=atol(argv[2]); 
f1=fopen(Filename, "r");
while(j>0)
{
fgets(line,1024,f1); 
i=0;n=0; 
while (line[i]!='\0') 
{
  if (line[i]=='"')
  {i++;}
  else  if (line[i]==',')
   {
      n++;
      if (n==1) {i++;printf("   ");}
        else
        {
         if ((n==2)||(n==5))
         {
          k=0;
          while (line[i]!=' ')
          {
            i++;
            Date[k]=line[i];
            k++;
          }
          printf("2002");
          k=0;
          if (Date[1]=='/') 
          {k=1; printf("0%c",Date[0]);} 
          else {k=2;printf("%c%c",Date[0],Date[1]);}
          if (Date[k+2]=='/') 
          { printf("0%c",Date[k+1]);} 
          else {printf("%c%c",Date[k+1],Date[k+2]);}          
         }
         i++;
        }        
   }
  else
   if ((line[i]=='0')&&(line[i+1]==':'))
   {while(line[i]!=',') i++;   }
  else
   {
     printf("%c", line[i]); i++;     
   }
}
j--;
}
}

