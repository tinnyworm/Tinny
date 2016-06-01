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
  char *cvalue=NULL,*configName=NULL,*costName=NULL,*outName=NULL;
  char line[LINE_MAX];
  FILE *config=NULL,*cost=NULL,*costOut=NULL;
  int c;
  int nVars=0;
  float FSRate=1.0,NFSRate=1.0,rNum;
  float *buffer;
  long nFraud=0,nNonFraud=0,sampleFraud=0,sampleNonFraud=0;
  int cOnly=0;
 
  char usage[]="\
Usage: zcat prescale | %s [-h] \n\
         [-l number of vars, including tag]\n\
         [-n non_fraud sampling rate, default 1.0]\n\
         [-f fraud sampling rate, default 1.0]\n\
         [-C counting only, no sampling]\n\
         [-c config file, format:\n\
             <non_fraud sampling rate> <fraud sampling rate>\n\
             it will overwrite -n -f]\n\
         [-p output cost file name (end with .cst)]\n\
         [-x cost file]\n\n";
  
  while ((c = getopt (argc, argv, "l:n:f:c:x:p:Ch")) != -1)
    switch (c)
      {
      case 'h':
        fprintf(stderr,usage,argv[0]);
        exit(0);
        break;
      case 'l':
        nVars = atoi(optarg);
        break;
      case 'n':
        NFSRate = atof(optarg);
        break;
      case 'f':
        FSRate = atof(optarg);
        break;
      case 'C':
        cOnly=1;
        break;
      case 'c':
        configName= optarg;
        break;
      case 'p':
        outName = optarg;
        break;
      case 'x':
        costName= optarg;
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

  if(configName) {
    if(!(config=fopen(configName,"r"))) {
      fprintf(stderr, "ERROR, can't open config file %s!\n",configName);
      exit(1);
    }
    if(fscanf(config,"%f %f",&NFSRate,&FSRate)!=2) {
      fprintf(stderr, "ERROR, wrong format of config file %s!\n", configName);
    }
    fclose(config);
  }

  if(costName) {
    if(!(cost=fopen(costName,"r"))) {
      fprintf(stderr, "ERROR, can't open cost file %s!\n",costName);
      exit(1);
    }
    if(outName==NULL) {
      fprintf(stderr, "output name must be specified when cost is present!\n");
      exit(1);
    }
    if(!(costOut=fopen(outName,"w"))) {
      fprintf(stderr, "ERROR, can't open cost output file %s!\n",outName);
      exit(1);
    }
    
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

  if(cOnly) {//counting only
    while(fread(buffer,sizeof(float),(size_t)nVars,stdin)) {
      if(buffer[0]<0.5) {
        nNonFraud++;
      } else {
        nFraud++;
      }
    }

    fprintf(stderr,"-------------------------------------------------------------\n");
    fprintf(stderr,"   Counting results\n");
    fprintf(stderr,"-------------------------------------------------------------\n");
    fprintf(stderr,"   Number of records                          = %7li\n\n",
        nNonFraud+nFraud);
    fprintf(stderr,"   Number of Non-Fraud records                = %7li\n",
        nNonFraud);
    fprintf(stderr,"   Number of Fraud recodes                    = %7li\n",
        nFraud);
    fprintf(stderr,"   Original Ratio of Non-Fraud/Fraud          = %7.4f\n",
        (float)nNonFraud/ (float) nFraud);
  }
  else {//sampling
    while(fread(buffer,sizeof(float),(size_t)nVars,stdin)) {
      rNum=(float)rand()/RAND_MAX;
      if(cost) {
        if(fgets(line,LINE_MAX,cost)==NULL) {
          fprintf(stderr,"record number mismatch between data and cost!\n");
          exit(1);
        }
      }
      if(buffer[0]<0.5) {
        if(rNum<NFSRate) {
          fwrite(buffer,sizeof(float),(size_t)nVars,stdout);
          sampleNonFraud++;
          if(cost) {
            fprintf(costOut,"%s",line);
          }
        }
        nNonFraud++;
      } else {
        if(rNum<FSRate) {
          fwrite(buffer,sizeof(float),(size_t)nVars,stdout);
          sampleFraud++;
          if(cost) {
            fprintf(costOut,"%s",line);
          }
        }
        nFraud++;
      }
    }

    fprintf(stderr,"-------------------------------------------------------------\n");
    fprintf(stderr,"   Sampling results\n");
    fprintf(stderr,"-------------------------------------------------------------\n");
    fprintf(stderr,"   Number of records                          = %7li\n\n",
        nNonFraud+nFraud);
    fprintf(stderr,"   Number of Non-Fraud records                = %7li\n",
        nNonFraud);
    fprintf(stderr,"   Number of Sampled Non-Fraud records        = %7li\n",
        sampleNonFraud);
    fprintf(stderr,"   Non-Fraud Sampling rate                    = %7.4f\n\n",
        (float)sampleNonFraud/ (float)nNonFraud);
    fprintf(stderr,"   Number of Fraud recodes                    = %7li\n",
        nFraud);
    fprintf(stderr,"   Number of Sampled Fraud recodes            = %7li\n",
        sampleFraud);
    fprintf(stderr,"   Fraud Sampling rate                        = %7.4f\n\n",
        (float)sampleFraud/ (float)nFraud);
    fprintf(stderr,"   Original Ratio of Non-Fraud/Fraud          = %7.4f\n",
        (float)nNonFraud/ (float) nFraud);
    fprintf(stderr,"   Sampled Ratio of Non-Fraud/Fraud           = %7.4f\n",
        (float)sampleNonFraud/ (float) sampleFraud);
  }

  free(buffer);
  if(cost) { fclose(cost); fclose(costOut); }

  return(0);
}


