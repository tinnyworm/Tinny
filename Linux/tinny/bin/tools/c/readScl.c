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
  char *cvalue=NULL;
  int c,i;
  int nVars;
  float rate,rNum;
  float *buffer;

  /*
  while ((c = getopt (argc, argv, "n:r:")) != -1)
    switch (c)
      {
      case 'n':
        nVars = atoi(optarg);
        break;
      case 'r':
        rate = atof(optarg);
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
  */
  nVars=176;
  buffer = (float *)malloc((nVars+1)*sizeof(float));
  if(!buffer) {
    fprintf(stderr, "Can not allocate memeory!\n");
    exit(1);
  }
  /*
  c=rand();
  rNum=(float)rand()/RAND_MAX;
  fprintf(stderr, "%d, %d, %f\n",RAND_MAX,c,rNum);
  */
  while(fread(buffer,sizeof(float),(size_t)(nVars+1),stdin)) {
    for(i=0;i<177;i++) {
    fprintf(stdout,"%5.2f   ",buffer[i]);
    }
    fprintf(stdout,"\n");
  }
}

