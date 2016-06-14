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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LINE_MAX 1000

int main(int argc, char** argv) {
  char *listName=NULL;
  char line[LINE_MAX];
  FILE *varList=NULL;
  int c,i;
  int nline=0;
  int nVars=0;
  float *buffer;
  double *sum, mu;
  long nRecord=0;
 
  char usage[]="\
Usage: zcat prescale | %s [-h] \n\
         [-n number of profile vars]\n\
         [-l profile varlist file]\n";
  
  while ((c = getopt (argc, argv, "n:l:h")) != -1)
    switch (c)
      {
      case 'h':
        fprintf(stderr,usage,argv[0]);
        exit(0);
        break;
      case 'n':
        nVars = atoi(optarg);
        break;
      case 'l':
        listName= optarg;
        break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  if(listName) {
    if(!(varList=fopen(listName,"r"))) {
      fprintf(stderr, "ERROR, can't open varList file %s!\n",listName);
      exit(1);
    }
  }

  if(!nVars) {
    fprintf(stderr, "ERROR, number of vars must be specified!\n");
    exit(1);
  }
    
  buffer = (float *)malloc((nVars)*sizeof(float));
  if(!buffer) {
    fprintf(stderr, "Can not allocate memeory for buffer!\n");
    exit(1);
  }

  sum = (double *)calloc(nVars,sizeof(double));
  if(!buffer) {
    fprintf(stderr, "Can not allocate memeory for sum!\n");
    exit(1);
  }

  while(fread(buffer,sizeof(float),(size_t)nVars,stdin)) {
    nRecord++;
    for(i=0;i<nVars;i++) {
      sum[i] += buffer[i];
    }
  }

  nline=0;
  while(fgets(line, LINE_MAX, varList)!=NULL) {
    for(i=0;i<LINE_MAX;i++) {
      if(line[i]=='\n') {
        line[i]='\0';
        break;
      }
    }
    if(nline>=nVars) {
      fprintf(stderr, "More vars in list than data!\n");
      exit(1);
    }
    mu = sum[nline]/(double)nRecord;
    fprintf(stdout, "   PROF.%s := %f;\n",line,mu);
    nline++;
  }

  if(nline<nVars) {
    fprintf(stderr, "Less vars in list than data!\n");
    exit(1);
  }

  fclose(varList);
  free(sum);
  free(buffer);

  return(0);
}