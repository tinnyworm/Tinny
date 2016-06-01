/* ******************************************************************

Name:  scaling.c


Description: This file will support falconer program by
             providing support for scaling operations. 


Programing notes:

             The scaling.c internal cache
             read/write for scaling is still active. 

             After review the file_cache I/O routines are being abandoned.

             It looks like the next speed improvement would come from  
             using memory mapped files under UNIX to store/modify the 
             scaling file.  

             

Copyright 2004  Fair Isaac Corporation, all rights reserved
HNC Software.  This is HNC Confidential Software.
Copyright 2000, 2002 HNC Software, all rights reserved

****************************************************************** */

/* Include files */

/* System Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
/* #include <float.h> */

/* defines */

#define MAX_VARIABLE_NAME   256
#define MAX_DATA_LENGTH                 10000*12
#define NODE_LENGTH                     12

typedef struct Snode
{
    char   name[MAX_VARIABLE_NAME];
    double nonZeroCount;
    double min;
    double max;
    double sum;
    double sumSq;
    double mean;
    double stdDev;
    double divisor;
    double range;
    int do_zscale;
    struct Snode  *next;
} Snode;

void readCFG(FILE *cfgfile);
long buildNode(FILE *namelist, FILE *datafile, FILE *cfgfile, int is_bin);
void nodeInitialize(Snode *pnode);
void nodeUpdate(void);
void nodeCalc(long numObservations, int noZeroCalc, int SpecialBinaryVariable);
void printWarnings(void);
void writeCFG(FILE *cfgfile,long numObservations,int noZeroCalc);
void writeCFGEntry(FILE *pfile,
        int    nodeNumber, /* The node or input variable number */
        char   *nodeName,  /* node or input variable name       */
        char   *nodeType,  /* Literal: CONTINOUS or BINARY      */
        char   *nodeSlab,  /* Literal: INPUT or TARGET          */
        int    offset,     /* Offset into                       */
        int    length,     /* Length                            */
        long   recordCount, /* How many records processed       */
        double min,       /* Minimum value                      */
        double max,       /* Maximum value                      */
        double missing,   /* Called missing but its the mean or 0 */
        double sum,       /* Sum of all the values              */
        double mean,      /* Mean                               */
        double standardDeviation, /* Standard deviation         */
        char  *scaleFunction, /* Z or NONE                      */
        double divisor,   /* divisor                            */
        double range);    /* Range of the values                */
long writeSCL(FILE *datafile, FILE *sclfile, int is_bin, int do_capping,
    long *nfraud);

Snode *startNode=NULL;
float *varBuff=NULL;
int numberNodes=0;
float fraudSampleRate=1;
float nonFraudSampleRate=1;

int main(int argc, char** argv){
  char usage[]="\
Usage: %s [-h]  \n\
          [-z nozerocalc,binary]   \n\
          [-l var_name_list_file]\n\
           -b input_data_file (BINARY)\n\
           -d input_data_file (ASCII)\n\
              -b and -d are mutually exclusive\n\
           -c input_cfg_file \n\
           -C output_cfg_file \n\
              -c and -C are mutually exclusive\n\
           -f fraudSampleRate, default 1;\n\
           -n nonFraudSampleRate, default 1;\n\
          [-m]\n\
          [-o output_scl_file]\n";

  char help[]="\n\
%s - Scale file generating tool. \n\n\
\
This program will generate DMW \"scl\" and \"cfg\" files. \n\
It read in a <var_name_list_file>, which contents the variable name list,\n\
and <input_data_file>, which contents the un-scaled variable values.\n\n\
\
<input_data_file> (ASCII) format:\n\
        0,value_of_var1,value_of_var2,......\n\
        1,value_of_var1,value_of_var2,......\n\
        ....................................\n\
where the first field should be the target, 0 is nonfraud, 1 is fraud.\n\n\
\
<input_data_file> (BINARY) format:\n\
        All variable values are coded as (float), and sequentially organized.\n\
        The variable values should be same as the ASCII format file.\n\n\
\
Options:\n\
   -h:  Display this message.\n\
   -z:  Scaling options:\n\
        nozerocalc - do not include zero-valued records in mean, \n\
                     std deviation calculation.\n\
               Default is off.\n\
        binary     - detect and process binary input variables \n\
                     differently by forcing the min and max to be 0,\n\
                     and 1 respectively. The mean will be reset to 0,\n\
                     and std Dev will be reset to 1.\n\
                     Variable detection based on nonzero count equal to\n\
                     the sum and max=1\n\
               Default is off.\n\
   -l:  var_name_list_file.\n\
   -b:  input_data_file (BIN). \n\
   -d:  input_data_file (ASCII). \n\
        The value of -b or -d option could be \"-\". In this case, STDIN will\n\
        be used. \n\
        Please note that STDIN can NOT be used, if both \"cfg\" file and \"scl\" \n\
        file need to be generated in the same run.\n\
   -c:  input \"cfg\" file. Do z-scaling on the data.\n\
   -C:  output \"cfg\" file. Calculate statistics from the inupt_data_file, \n\
        and generate the \"cfg\" file.\n\
        -c and -C are mutually exclusive.\n\
   -m:  use Min and Max in the \"cfg\" for capping.\n\
        Default is no-capping.\n\
   -o:  output \"scl\" file.\n\
        If the value of this option is \"-\", the scale file will output to \n\
        STDOUT. If this option is not specified, no scale file will be output,\n\
        i.e., only output \"cfg\" file.\n\n\
\
Examples:\n\
    All-In-One: read ASCII input data, generate \"cfg\" file, do z-scaling and\n\
    create \"scl\" file:\n\
         %s -l varlist_file -d input_ascii_file -C cfg_file.cfg \\\
            -z nozerocalc,binary -o scl_file.scl\n\n\
\
    Do-it-step-by-step:\n\
    Read ASCII input data and generate \"cfg\" file only:\n\
         cat input_ascii_file | %s -d - -l varlist_file -C cfg_file.cfg\n\
    Do z-scaling only:\n\
         cat input_ascii_file | %s -d - -c exist_cfg_file.cfg -o scl_file.scl\n\
\n";
      

  FILE *namelist=NULL, *datafile=NULL, *cfgfile=NULL, *sclfile=NULL;
  int status=0;
  char lnbuff[MAX_DATA_LENGTH];
  long numObservations=0L, numFraud=0L;
  int scl_nozerocalc=0, scl_binary=0;

  Snode *s;
  long longreclen;
  int i;

  char options[]= "C:c:b:d:f:n:hl:mo:z:";
  int option_returned;
  int cfg_exist=0,is_bin=0,do_capping=0; 
  char *subopts, *value;

  enum JOB_TYPE
  {
    STATISTICS_ONLY,
    ZSCALING_ONLY,
    ALL
  } jobtype;

  enum SCALE_OPTIONS
  {
      SCALE_NOZEROCALC_OPT,
      SCALE_BINARY_OPT,
      SCALE_OPTIONS_LAST
  };
  const char *scale_opts[] =
  {
      [SCALE_NOZEROCALC_OPT] = "nozerocalc",
      [SCALE_BINARY_OPT]     = "binary",
      [SCALE_OPTIONS_LAST]   = NULL
  };

  while ((option_returned = getopt(argc,argv,options)) != -1){
    switch(option_returned)
    {
      case 'h':
        fprintf(stderr,usage,argv[0]);
        fprintf(stderr,help,argv[0],argv[0],argv[0],argv[0]);
        exit(0);
        break;
      case 'l':
        if((namelist=fopen(optarg,"r"))==NULL){
          fprintf(stderr,"ERROR: cann't open file %s\n",optarg);
          fprintf(stderr,usage,argv[0]);
          exit(1);
        }
        break;
      case 'b':
        is_bin=1;;
        if (strcmp(optarg,"-")==0){
          datafile=stdin;
        }else{
          if((datafile=fopen(optarg,"r"))==NULL){
            fprintf(stderr,"ERROR: cann't open file %s\n",optarg);
            fprintf(stderr,usage,argv[0]);
            exit(1);
          }
        }
        break;
      case 'd':
        is_bin=0;
        if (strcmp(optarg,"-")==0){
          datafile=stdin;
        }else{
          if((datafile=fopen(optarg,"r"))==NULL){
            fprintf(stderr,"ERROR: cann't open file %s\n",optarg);
            fprintf(stderr,usage,argv[0]);
            exit(1);
          }
        }
        break;
      case 'c':
        cfg_exist=1;
        if((cfgfile=fopen(optarg,"r"))==NULL){
          fprintf(stderr,"ERROR: cann't open file %s for read.\n",optarg);
          fprintf(stderr,usage,argv[0]);
          exit(1);
        }
        break;
      case 'C':
        cfg_exist=0;
        if((cfgfile=fopen(optarg,"w"))==NULL){
          fprintf(stderr,"ERROR: cann't open file %s for write.\n",optarg);
          fprintf(stderr,usage,argv[0]);
          exit(1);
        }
        break;
      case 'f':
        fraudSampleRate=atof(optarg);
        break;
      case 'n':
        nonFraudSampleRate=atof(optarg);
        break;
      case 'm':
        do_capping=1;
        break;
      case 'o':
        if (strcmp(optarg,"-")==0){
          sclfile=stdout;
        }else{
          if((sclfile=fopen(optarg,"w"))==NULL){
            fprintf(stderr,"ERROR: cann't open file %s\n",optarg);
            fprintf(stderr,usage,argv[0]);
            exit(1);
          }
        }
        break;
      case 'z':
        subopts = optarg;
        while(*subopts != '\0'){
          switch(getsubopt(&subopts, (char *const *)scale_opts, &value))
          {
            case SCALE_NOZEROCALC_OPT:
                scl_nozerocalc=1;
              break;
            case SCALE_BINARY_OPT:
                scl_binary=1;
              break;
            default:
              fprintf(stderr,"ERROR: unknown SCALE option!\n");
              fprintf(stderr,usage,argv[0]);
              exit(1);
              break;
          }
        }
        break;
      case '?':
        fprintf(stderr,"ERROR: unknown option!\n");
        fprintf(stderr,usage,argv[0]);
        exit(1);
        break;
    }
  }

  if (optind != argc){
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }


  if (cfg_exist==1){
    jobtype=ZSCALING_ONLY;
  }else if (sclfile==NULL){
    jobtype=STATISTICS_ONLY;
  }else{ 
    jobtype=ALL;
    if (datafile == stdin){
      fprintf(stderr,"ERROR: can not input from STDIN, if both \"cfg\" and \"scl\" \n");
      fprintf(stderr,"       file need to be generated in the same run.\n");
      fprintf(stderr,usage,argv[0]);
      exit(1);
    }
  }

  if (datafile==NULL || cfgfile==NULL){
    fprintf(stderr,"ERROR: input_data_file and cfg_file must be specified.\n");
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }

  if((jobtype == STATISTICS_ONLY || jobtype == ALL) && namelist==NULL){
    fprintf(stderr,"ERROR: a var_name_list_file need to be specified.\n");
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }

  if((jobtype == ZSCALING_ONLY || jobtype == ALL) && sclfile==NULL){
    fprintf(stderr,"ERROR: an output scl file need to be specified.\n");
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }

  if (jobtype == STATISTICS_ONLY || jobtype == ALL ){
    numObservations = buildNode(namelist,datafile,cfgfile,is_bin);
    nodeCalc(numObservations,scl_nozerocalc,scl_binary);
    writeCFG(cfgfile,numObservations,scl_nozerocalc);
    printWarnings();
  } else {
    readCFG(cfgfile);
  }


  if (jobtype == ALL)
    rewind(datafile);

  if (jobtype == ZSCALING_ONLY || jobtype == ALL ){
    numObservations = writeSCL(datafile,sclfile,is_bin,do_capping,&numFraud);
  }else{
    numFraud = (long)startNode->sum;
  }

  fprintf(stderr,"-------------------------------------------------------------\n");
  fprintf(stderr,"   Scaling results\n");
  fprintf(stderr,"-------------------------------------------------------------\n");
  fprintf(stderr,"   Number of records                  = %7li\n",
      numObservations);
  fprintf(stderr,"   Number of Non-Fraud records        = %7li\n",
      numObservations - numFraud);
  fprintf(stderr,"   Number of Fraud recodes            = %7li\n",
      numFraud);
  fprintf(stderr,"   Ratio of Non-Fraud/Fraud           = %7.4f\n",
      (float)numObservations / (float) numFraud);
  return(0);
}

void readCFG(FILE *cfgfile)
{
  Snode *s, *new;
  char buff[MAX_VARIABLE_NAME];
  char scalefn[20];

  numberNodes=0;

  while(fgets(buff,MAX_VARIABLE_NAME,cfgfile))
  {
    if(numberNodes == 0 && memcmp(buff,"Sum=",4)==0){
      if ((startNode = (Snode*)malloc(sizeof(Snode) ))==NULL)
      {
        fprintf(stderr, "--> Fatal Memory problems (1)\n");
        exit(1);
      }
      nodeInitialize(startNode);
      if ( (sscanf(buff, "Sum= %lf", &(startNode->sum))) != 1){
        fprintf(stderr, "Bad value of Sum!\n");
        exit(1);
      }
      fgets(buff,MAX_VARIABLE_NAME,cfgfile);
      if ( (sscanf(buff, "Mean= %lf", &(startNode->mean))) != 1){
        fprintf(stderr, "Bad value of mean!\n");
        exit(1);
      }
      fgets(buff,MAX_VARIABLE_NAME,cfgfile);
      if ((sscanf(buff, "StdDev= %lf",&(startNode->stdDev))) != 1){
        fprintf(stderr, "Bad value of stdDev!\n");
        exit(1);
      }
      s=startNode;
      numberNodes++;
    }

    if(numberNodes > 0 && memcmp(buff,"Min=",4)==0){
      if ((s->next = (Snode*)malloc(sizeof(Snode) ))==NULL)
      {
        fprintf(stderr, "--> Fatal Memory problems (1)\n");
        exit(1);
      }
      nodeInitialize(s->next);
      s=s->next;

      if ( (sscanf(buff, "Min= %lf Max= %lf", &(s->min), &(s->max))) != 2){
        fprintf(stderr, "Bad value of min or max!\n");
        exit(1);
      }
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*MissingValue*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*Sum*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*Mean*/
      if ( (sscanf(buff, "Mean= %lf", &(s->mean))) != 1){
        fprintf(stderr, "Bad value of mean!\n");
        exit(1);
      }
      fgets(buff,MAX_VARIABLE_NAME,cfgfile);
      if ((sscanf(buff, "StdDev= %lf",&(s->stdDev))) != 1){
        fprintf(stderr, "Bad value of stdDev!\n");
        exit(1);
      }
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*Derivative*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*TimeSlice*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*NbrOfSymbols*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*Symbolic*/
      fgets(buff,MAX_VARIABLE_NAME,cfgfile); /*ScaleMode= AUTO  ScaleFn= */
      if ((sscanf(buff, "ScaleMode= AUTO  ScaleFn= %s",scalefn)) != 1) {
        fprintf(stderr, "Bad value of ScaleFn!\n");
        exit(1);
      }
      if (memcmp(scalefn,"Z",1)==0){
        s->do_zscale=1;
      }else if (memcmp(scalefn,"NONE",4)==0){
        s->do_zscale=0;
      }else{
        fprintf(stderr, "Bad value of ScaleFn!\n");
        fprintf(stderr, "Can only be \"Z\" or \"NONE\"!\n");
        exit(1);
      }
      numberNodes++;
    }
  }

  varBuff = (float *)malloc(numberNodes*sizeof(float));
  if (varBuff == NULL)
  {
    fprintf(stderr, "--> Fatal Memory problems (3)\n");
    exit(1);
  }
  else  /* initialize the memory */
  {
    memset((void*)varBuff, 0, numberNodes*sizeof(float));
  }
}

long buildNode(FILE *namelist, FILE *datafile, FILE *cfgfile, int is_bin)
{
  Snode *s;
  char lnbuff[MAX_DATA_LENGTH];
  long reclen;
  long numObservations=0L,badObservations=0L;
  char buff[MAX_VARIABLE_NAME];
  int nr=0;
  
  /* initialize first node which is fraudq and is treated specially */
  startNode = (Snode*)malloc(sizeof(Snode) );
  if (startNode == NULL)
  {
    fprintf(stderr, "--> Fatal Memory problems (1)\n");
    exit(1);
  }
  nodeInitialize(startNode);
  strcpy(startNode->name, "FRAUDQ");

  numberNodes=1;
  s=startNode;
  while(fscanf(namelist,"%s",buff)==1)
  {
    s->next = (Snode*)malloc(sizeof(Snode) );
    if (s->next == NULL)
    {
      fprintf(stderr, "--> Fatal Memory problems (2)\n");
      exit(1);
    }
    s = s->next;
    nodeInitialize(s);
    strcpy(s->name, buff);
    numberNodes++;
  }

  varBuff = (float *)malloc(numberNodes*sizeof(float));
  if (varBuff == NULL)
  {
    fprintf(stderr, "--> Fatal Memory problems (3)\n");
    exit(1);
  }
  else  /* initialize the memory */
  {
    memset((void*)varBuff, 0, numberNodes*sizeof(float));
  }

  while(nr = (is_bin)?fread(varBuff,sizeof(float),(size_t)numberNodes,datafile):
                      readAsciiRec(datafile))
  {
    if( nr != numberNodes ){
      fprintf(stderr,"ERROR: Record length mismatch: Expect length=%i, readin length=%i\n",numberNodes,nr);
      exit(1);
    }else if(varBuff[0] != (double) 0.0 && varBuff[0] != (double)1.0){
      fprintf(stderr,"ERROR: The values of target variable are not 0 or 1: %f\n",varBuff[0]);
      exit(1);
    }else{
      nodeUpdate();
    }
    numObservations++;
  }

  if (startNode->sum != startNode->nonZeroCount 
      || startNode->min != (double)0.0
      || startNode->max != (double)1.0
      || startNode->sum != startNode->sumSq){
    fprintf(stderr,"ERROR: The values of target variable are not 0 or 1!\n");
    exit(1);
  }

  if (badObservations > 0){
    fprintf(stderr,"Warning: Bad input records with not (0 or 1) target values.\n");
    fprintf(stderr,"         Count:   %d \n",(int)badObservations);
    fprintf(stderr,"         Percent: %5.3f \n",(float)badObservations/numObservations);
  }

  return numObservations;
}

void printWarnings()
{
  Snode *s;
  int i;

  for (i = 0,s = startNode; i < numberNodes; i++, s = s->next){
    if (s->sum == s->nonZeroCount &&
        s->min == (double) 0.0 &&
        s->max == (double) 1.0 )
      fprintf(stderr,"- Note: Possible binary variable: %s\n",s->name);
    if (s->stdDev == (double) 0.0) 
      fprintf(stderr,"- Note: Variable %s has 0 standard deviation\n",s->name);
  }
}

long writeSCL(FILE *datafile, FILE *sclfile, int is_bin, int do_capping,
    long *nfref)
{
  Snode *s;
  int32_t longreclen=(int32_t)numberNodes;
  long reclen;
  long numObservations=0L,badObservations=0L;
  int nr=0,i;
  float rNum;
  int outFlag;
  long nFraudRecord=0, nNonfraudRecord=0;

  fwrite(&longreclen,sizeof(int32_t),1,sclfile);

  while(nr = (is_bin)?fread(varBuff,sizeof(float),(size_t)numberNodes,datafile):
                      readAsciiRec(datafile))
  {
    numObservations++;
    if( nr != numberNodes
      || (varBuff[0] != (double)0.0 && varBuff[0] != (double)1.0))
    {
      badObservations++;
      continue;
    }
 
    rNum=(float)rand()/RAND_MAX;
    if(varBuff[0] > 0.5){
      if(rNum<fraudSampleRate) {
        varBuff[0]=1.0;
        outFlag=1;
        nFraudRecord++;
      } else {
        outFlag=0;
      }
      (*nfref)++;
    }else{
      if(rNum<nonFraudSampleRate) {
        varBuff[0]=-1.0;
        outFlag=1;
        nNonfraudRecord++;
      } else {
        outFlag=0;
      }
    }

    if(outFlag) {
      for (i = 1,s = startNode->next; i < numberNodes; i++, s = s->next)
      { 
        if (s->do_zscale){
          /* 
              calculate the zscale of the value base on:
              1. Std Dev is 0  OR
              2. Value, and zerolimit options OR
              3. direct calculation
              
          */
          if (s->stdDev == (double)0.0 )   /* Check for Zero */
          {
              varBuff[i] = 0.0;
          }
          else
          {
              /* zscale = (value - mean) / standard_deviation */
              if(do_capping){
                varBuff[i] = ((double)varBuff[i] > s->min)?varBuff[i]:(float)(s->min);
                varBuff[i] = ((double)varBuff[i] < s->max)?varBuff[i]:(float)(s->max);
              }
              varBuff[i] = (float)(((double)varBuff[i] - s->mean) / s->stdDev);
          }
        }
      }  
      fwrite(varBuff,sizeof(float),numberNodes,sclfile);
    }
  }

  fprintf(stderr,"Transaction sampling:\n");
  fprintf(stderr,"nonFraud: %d, %f\n",
      nNonfraudRecord,(float)nNonfraudRecord/(numObservations-(*nfref)));
  fprintf(stderr,"Fraud: %d, %f\n",
      nFraudRecord,(float)nFraudRecord/(*nfref));
  if (badObservations > 0){ 
    fprintf(stderr,"Warning: Bad input records with not (0 or 1) target values.\n");
    fprintf(stderr,"         Count:   %d \n",(int)badObservations);
    fprintf(stderr,"         Percent: %5.3f \n",(float)badObservations/numObservations);
  }
  return numObservations;
}

int readAsciiRec(FILE *datafile){
  int i=0;
  char lnbuff[MAX_DATA_LENGTH];
  char *pt;

  if (fgets(lnbuff,MAX_DATA_LENGTH,datafile)==NULL)
    return 0;

  pt=lnbuff;
  sscanf(pt,"%f",&(varBuff[0]));
  while(pt=strchr(pt,',')){
    i++;
    pt++;
    sscanf(pt,"%f",&(varBuff[i]));
  }
  return i+1;
}

/* ******************************************************************
    nodeInitialize - Initialize the node structure.
    args:
        pnode - pointer to Snode to be initialized
    returns:
        N/A                     
****************************************************************** */
void nodeInitialize(Snode *pnode)
{
    /* Initialize values */
    pnode->nonZeroCount = (double)0.0;
    pnode->next    = NULL;        /* Make sure end is marked */
    pnode->max     = -DBL_MAX;
    pnode->min     =  DBL_MAX;
    pnode->sum     = (double) 0.0;
    pnode->sumSq   = (double) 0.0;
    pnode->mean    = (double) 0.0;
    pnode->stdDev  = (double) 0.0;
    pnode->divisor = (double) 0.0;
    pnode->range   = (double) 0.0;
    pnode->do_zscale = 1;
    /* set name to End of String initially */
    memset(pnode->name, '\0', sizeof(pnode->name) );

}  /* -- end of nodeInitialize */

/* ******************************************************************
    nodeUpdate - Does statistical calculations that can be done
        a particular record.
    args:
        pnode - pointer to Snode block                         
    returns:
        N/A
****************************************************************** */
void nodeUpdate()
{
  Snode *pnode;
  int i;

  for (i=0,pnode=startNode;i<numberNodes;i++,pnode=pnode->next)
  {
    if (varBuff[i] != 0.0)              /* non zero count */
        pnode->nonZeroCount = pnode->nonZeroCount + 1.0;
        
    if ((double)(varBuff[i]) > pnode->max)         /* max */
        pnode->max = (double)(varBuff[i]);
        
    if ((double)(varBuff[i]) < pnode->min)         /* min */
        pnode->min = (double)(varBuff[i]);
        
    pnode->sum   = pnode->sum + (double)(varBuff[i]);  /* sum and sum of squares */
    pnode->sumSq = pnode->sumSq + (double)(varBuff[i]) * (double)(varBuff[i]);
  }

} /* -- end of nodeUpdate */

/* ******************************************************************
    nodeCalc - Does the final statistical calculations for a node.
    args:
        pfalCtrl - pointer to Scontrol block    
        pnode - pointer to Snode block
        numObservations - Total number of observations being
                processed.

    Note: nonZeroCount is the count of non zero calcuated values
          for a node.  Depending on options this can affect the
          results.                 
                
    returns:
        N/A
****************************************************************** */
void nodeCalc(long numObservations, int noZeroCalc, int SpecialBinaryVariable)
{
  double varianceTop, StdDeviationDivisor;
  Snode *pnode;
  int i;
  for (i=0,pnode=startNode;i<numberNodes;i++,pnode=pnode->next)
  {
    /* 
        IF there were NO observations OR
        If no Zero Calc (no zero values are to be included in the
        calculations) and this node has all zero calcuations, we 
        need to zero out the node.
    */
    if (numObservations == 0 ||
        (noZeroCalc &&
         pnode->nonZeroCount == (double)0.0) 
       )
    {
        pnode->min   = (double)0.0;
        pnode->max   = (double)0.0;
        pnode->sum   = (double)0.0;
        pnode->sumSq = (double)0.0;
        pnode->mean  = (double)0.0;
        pnode->stdDev = (double)0.0;
        pnode->divisor = (double)0.0;
        pnode->range = (double)1.0;
        continue;
    }

    /* Normalize the Max if needed, as done in drvfalcon30u */
    if (pnode->max < (double)0.0)
        pnode->max = (double)0.0;

    /* 
        Calculate the mean 
        For no zero calc case use non zero count
        default use the total number of observations
    */
    if (noZeroCalc)
    {
        pnode->mean = pnode->sum/pnode->nonZeroCount;
    }
    else
    {
        pnode->mean = pnode->sum/(double)numObservations;
    }
    
    /* 
        Calculate the standard deviation 
    */

    /* first calculate the variance */
    /* variance = sumSq - ((sum*sum)/numberOfObservations) ) */
    if (noZeroCalc) /* Use nonZeroCount */
    {
        varianceTop = pnode->sumSq - ((pnode->sum*pnode->sum)/pnode->nonZeroCount);
        StdDeviationDivisor = pnode->nonZeroCount - (double)1.0;
    }
    else
    {
        varianceTop = pnode->sumSq - ((pnode->sum*pnode->sum)/(double)numObservations);
        StdDeviationDivisor = (double)(numObservations - (long)1);
    }

    if (varianceTop < (double) 0.0)   /* Normalize the result */
        varianceTop = (double) 0.0;
        
    if (StdDeviationDivisor <= (double) 0.0)
        StdDeviationDivisor = (double) 1.0;

    /* Now take sqrt of variance to get standard deviation */
    pnode->stdDev = sqrt(varianceTop/StdDeviationDivisor);


    /* 
        Now check for Binary variable and reset mean/std dev as needed
        Doing special binary variable processing AND
        non zero count equals the sum  AND
        min is zero, max 1.0.  (changed to this (min zero) 07/22/2002
        
    */
    if (SpecialBinaryVariable &&
        pnode->nonZeroCount == pnode->sum &&
        pnode->min == (double) 0.0       &&
        pnode->max == (double) 1.0)
    {
        pnode->mean   = (double) 0.0;
        pnode->stdDev = (double) 1.0;
    }

    
    /* compute divisor */
    /*
    if (pnode->min <= (double) 0.0)
        pnode->divisor = (double) 0.0;
    else
        pnode->divisor = log(pnode->max / pnode->min);
        */
    
    /* compute range */
    if (pnode->max <= pnode->min)
        pnode->range = (double) 0.0;
    else
        pnode->range = pnode->max - pnode->min;
  }
    
} /* -- end of nodeCalc */


/* ******************************************************************
    writeCFG - Write all the nodes information to ZSCALE CFG file
    args:
        pfalCtrl - pointer to Snode block                         
    returns:
        N/A
****************************************************************** */
void writeCFG(FILE *cfgfile,long numObservations,int noZeroCalc)
{
int nodeNumber = 0;
int offset     = 0;
long tempCount = 0;

Snode *s;

    /* Write the start of the CFG file */    
    fprintf(cfgfile, 
            "Record Length= %d\n", 
            numberNodes * NODE_LENGTH);
    
    /* 
        Write the fraud entry (FRAUDQ)
        FRAUDQ is the first entry in the scale node list.
    */
      if (noZeroCalc)
          tempCount = (long)(startNode->nonZeroCount);
      else
          tempCount = numObservations;
      writeCFGEntry(cfgfile,
                    0,                                /* node number */
                    startNode->name,
                    "BINARY",
                    "TARGET",
                    0,                                 /* offset */
                    NODE_LENGTH,
                    tempCount,
                    startNode->min,
                    startNode->max,
                    (double)0,                       /* missing value */
                    startNode->sum,
                    startNode->mean,
                    startNode->stdDev,
                    "NONE",                         /* scale func */
                    (double)0.0,                              /* divisor */
                    (double)1.0);                             /* range   */             
  
    /* 
        Loop on all the input nodes and write them out to
        the .cfg file
        Note: The offset for the nodes also starts at 0
    */
    for (s = startNode->next, offset = 0, nodeNumber = 1; 
         s;
         s = s->next, nodeNumber++, offset = offset + NODE_LENGTH)
    {
        /* 
            Write the node entry
        */
            /* Option No zero calc adjustment to record count */
            if (noZeroCalc)
                tempCount = (long)(s->nonZeroCount);
            else
                tempCount = numObservations;
            writeCFGEntry(cfgfile,
                          nodeNumber,
                          s->name,
                          "CONTINUOUS",
                          "INPUT",
                          offset,
                          NODE_LENGTH,
                          tempCount,       /* count of records to consider */
                          s->min,
                          s->max,
                          s->mean,  /* Falconer 3.0 used mean for missing value */
                          s->sum,
                          s->mean,
                          s->stdDev,
                          "Z",           /* zscale func */
                          s->divisor,
                          s->range);
    } /* end for s loop */

    /* Write CFG end of entry */
    fprintf(cfgfile, "\n-1\n");
            
} /* -- end of writeCFG */


/* ******************************************************************
    writeCFGEntry - Prints out a formatted CFG entry.
            The caller is responsible for keeping track of all
            the running variables.  This function simply formats
            and outputs the input data.
    NOTE: The input for the 0 node is really the fraud entry
          FRAUDQ.  The FRAUDQ entry typically is hard coded to:
              nodeType - BINARY
              nodeSLab - TARGET
              missing  - 0
              scaleFunc - NONE
              divisor  - 0
              range    - 1
              and the others are as calculated.
          While the actual input variables or nodes is hard coded to:
              nodeType - CONTINOUS
              nodeSLab - INPUT
              scaleFunc - Z       
              and the others are as calculated.

    args:
        pfile - FILE that entry is to be written to.
        For the others - see description below                         
    returns:
        N/A
****************************************************************** */
void writeCFGEntry(FILE *pfile,
        int    nodeNumber, /* The node or input variable number */
        char   *nodeName,  /* node or input variable name       */
        char   *nodeType,  /* Literal: CONTINOUS or BINARY      */
        char   *nodeSlab,  /* Literal: INPUT or TARGET          */
        int    offset,     /* Offset into                       */
        int    length,     /* Length                            */
        long   recordCount, /* How many records processed       */
        double min,       /* Minimum value                      */
        double max,       /* Maximum value                      */
        double missing,   /* Called missing but its the mean or 0 */
        double sum,       /* Sum of all the values              */
        double mean,      /* Mean                               */
        double standardDeviation, /* Standard deviation         */
        char  *scaleFunction, /* Z or NONE                      */
        double divisor,   /* divisor                            */
        double range)     /* Range of the values                */
{
    fprintf(pfile, "\n");
    fprintf(pfile, "%d\n", nodeNumber);
    fprintf(pfile, "Name= %s\n", nodeName);
    fprintf(pfile, "Type= %s\n", nodeType);
    fprintf(pfile, "Slab= %s\n", nodeSlab);

    fprintf(pfile, "Size= 1\n");
    
    fprintf(pfile, "Start= %d  Length= %d\n", offset, length);
    fprintf(pfile, "RecCnt= %ld\n", recordCount);
    fprintf(pfile, "Min= %.7g  Max= %.7g\n", min, max);

    fprintf(pfile, "MissingValue= %.15g\n", missing);

    fprintf(pfile, "Sum= %.15g\n", sum);
    fprintf(pfile, "Mean= %.15g\n", mean);
    fprintf(pfile, "StdDev= %.15g\n", standardDeviation);
    fprintf(pfile, "Derivative= 0\n");
    fprintf(pfile, "TimeSlice= 0\n");
    fprintf(pfile, "NbrOfSymbols= 0\n");
    fprintf(pfile, "Symbolic= NUMERIC\n");

    fprintf(pfile, "ScaleMode= AUTO  ScaleFn= %s\n", scaleFunction);
         
    fprintf(pfile, 
            "Divisor= %.15g  Range= %.15g\n",
            divisor,
            range);
            
} /* -- end of writeCFGEntry */

