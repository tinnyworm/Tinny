/**
 * @file
 * @author Danfeng Li <danfengli@fairisaac.com>
 * @date
 *
 * @brief
 *
 * $Log:$
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "unpack_zips.h"

#define LENGTH 10000

int main(int argc, char *argv[]) {
  int c,nVars,i;
  char *cvalue=NULL;
  char buff[LENGTH];
  float *input;
  char **strInput,postal[10];
  long j;

  while((c=getopt(argc,argv,"s:n:")) != -1) 
    switch (c) {
      case 'n':
        cvalue=optarg;
        nVars=atoi(cvalue);
        break;
    }

  if(!(input=(float*)malloc(nVars*sizeof(float)))) {
    fprintf(stderr,"Can NOT allocate memory for input!\n");
    exit(1);
  }
        
  while( fgets(buff,LENGTH,stdin) != NULL ) {
    //fprintf(stdout,"%s",buff);
    strInput = split(" ",buff);
    for(i=0;i<nVars;i++) {
      input[i]=atof(strInput[i]);
      fprintf(stdout,"%f,\t",input[i]);
    }
    //printf("1\n");
    j=labs(input[5]);
    unpackPostalCode(postal,j);
    postal[3]='\0';
    fprintf(stdout,"%s",postal);
    fprintf(stdout,"\n");
    //printf("2\n");
  }

  free(input);
}
        
  

