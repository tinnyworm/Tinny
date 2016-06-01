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

int main(int argc, char** argv) {
  int c,i;
  int nVars=0;
  float *buffer;
 
  char usage[]="\
Usage: zcat prescale | %s [-h] \n\
                           -l number of vars, including tag\n\n\
\
Convert model manager generated prescale file (using fwriterec) to\n\
linreg and logreg scale file format (tab delimited)\n\n";
  
  while ((c = getopt (argc, argv, "l:h")) != -1)
    switch (c)
      {
      case 'h':
        fprintf(stderr,usage,argv[0]);
        exit(0);
        break;
      case 'l':
        nVars = atoi(optarg);
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

  if(!nVars) {
    fprintf(stderr, "ERROR, number of vars must be specified!\n");
    exit(1);
  }
    
  buffer = (float *)malloc((nVars)*sizeof(float));
  if(!buffer) {
    fprintf(stderr, "Can not allocate memeory!\n");
    exit(1);
  }

  while(fread(buffer,sizeof(float),(size_t)nVars,stdin)) {
    printf("%2.1f",buffer[0]);
    for(i=1;i<nVars;i++) {
      printf("\t%f",buffer[i]);
    }
    printf("\n");
  }
    
  free(buffer);

  return(0);
}


