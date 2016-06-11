#include <stdlib.h>
#include <stdio.h>



void main(int argc, char**  argv)
{
	
	long i,j,k,ln;
	int l;
	char N[10]="9125764803";
	long key=101;
	char line[1024],tmp[20];
	int ok,found;
	for( i = 1; i < argc; i++)
    {
     if( strncmp(argv[i], "-k", strlen("-k")) == 0 )
     {
      key=atol(argv[++i]);
     }    
    /* if( strncmp(argv[i], "-l", strlen("-l")) == 0 )
     {
      ln=atol(argv[++i]);
     }   */
    }
   
    while (gets(line)!=NULL)
    {
    found=0;
    for(i=0;i<16;i++)
    {
      ok=1;
      for(j=0;j<10;j++)
         if (line[i]==N[j]) ok=0;
      if (ok==1) //line[i] is not a number
         found=1;
    }
    if (found==0)
    {
    for (i=0;i<6;i++)
    {
      tmp[i]=line[i];
     // printf("%c",line[i]);
    }
    tmp[6]='\0';
    sscanf(tmp, "%ld",&j);
    k=j%key;
    k=k%10;
    tmp[1]='\0';
    for (i=6;i<16;i++)
    {
      tmp[0]= line[i];
      sscanf(tmp, "%d",&l);
      l=(l+k)%10;
      line[i]=N[l];
    }
    printf("%s\n",line);  
    }
    else {printf("Wrong Account number %s\n",line);}  
    }
}
