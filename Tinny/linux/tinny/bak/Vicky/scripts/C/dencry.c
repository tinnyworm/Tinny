#include <stdlib.h>
#include <stdio.h>



void main(int argc, char**  argv)
{
	
	long i,j,k,ln;
	int l,s;
	char N[10]="9125764803";
	char RN[10]="0123456789";
	long key=101;
	char line[1024],tmp[20];
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
    for (i=0;i<6;i++)
    {
      tmp[i]=line[i];
     // printf("%c",line[i]);
    }
    tmp[6]='\0';
    sscanf(tmp, "%ld",&j);
    k=j%key;
   // k=k%10;
    tmp[1]='\0';
    for (i=6;i<16;i++)
    {
      tmp[0]= line[i];
      sscanf(tmp, "%d",&l);
      if (l==0) s=8;
      else if (l==1) s=1; 
      else if (l==2) s=2; 
      else if (l==3) s=9; 
      else if (l==4) s=6; 
       else if (l==5) s=3; 
      else if (l==6) s=5; 
      else if (l==7) s=4; 
      else if (l==8) s=7;
       else if (l==9) s=0;
      l=(s+1000-k)%10;
      line[i]=RN[l];
    }
    printf("%s\n",line);    
    }
}