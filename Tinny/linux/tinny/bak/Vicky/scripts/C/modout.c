/*========================================================================*/
/*                                                                        */
/*                              output.c                                  */
/*                                                                        */
/*  DESCRIPTION:  Functions to output dat file records, scale file
 *                records, or net input records for validation.  
 *                The specific output function that is used depends
 *                upon which driver compile option was used.
 *
 *  NOTES: FOR HNC INTERNAL USE ONLY!!
 *                                                                        */
/*========================================================================*/
/*                                                                        */
/*
$Source: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/modout.c,v $
 *
$Log: modout.c,v $
Revision 1.2  1999/08/26 00:21:52  xxs
Add options to exclude zeros in computing mean, stdev.
stats for FRAUDQ will not be collected.
IS_xxx, RECENT_xxx variables will not be scaled.

Revision 1.1  1998/06/08 22:55:28  jcb
Initial revision

 * Revision 1.2  1998/03/13  18:49:54  jcb
 * changed dat file format to e from f
 *
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.3  1997/01/27  23:43:18  ojo
 * Added counts for fraud/nonFr scale file statistics
 * Corrected fraudType output for nonFraud scale records - ojo
 *
 * Revision 1.2  1996/08/05  20:04:27  kmh
 * *** empty log message ***
 *
 * Revision 1.2  1995/04/24  17:42:22  can
 * Added variable variance=0 to .gen file
 *
 * Revision 1.1  1995/03/22  19:53:15  can
 * Initial revision
 *
 *
 *                                                                        */
/*========================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <values.h>
#include <unistd.h>

#include "maxstrc.h"
#include "model.h"
#include "hnctype.h"
#include "hncrtyp.h"


extern BOOL  isFraud;
extern BOOL  isOutput;
extern BOOL  doScale;
extern BOOL  silent;
extern BOOL  doModel;
extern long  unsclBufSize;

/* for tag file */
extern int extHashVal;
extern int extTestThreshold;

/* add for testing */
extern float fGlobalFalseZeroThreshold;




typedef struct sStatStruct {
  double min;
  double max;
  double sum;
  double sumSq;
  double mean;
  double stdDev;
  double divisor;
  double range;
  double num_nonzero;
} tStatStruct;


typedef struct sOutModel {
   int          first;
   tModel       *model;
   int          numInput;
   tStatStruct  *statBuf;
   double       num; 
   FILE         *sclFp;
   FILE         *datFp;
   FILE         *datBadFp;
   FILE         *tagFp;

   /* datamiester */
   FILE         *dataFile;
   float        *dataBlock;
   int          readFile; 
   int          recSize;
   int          numRecs;
   int          index;
   int          numStored;
   int          total;
   char         tFileName[300];
   /* datamiester */
   /* other stuff */
   int          countFraud;
   int          countNonFraud;

   /* tag file related vars */
   int tag;
   int numTest;
   int numTestFraud;
   int numTestNonFraud;
   int numTrain;
   int numTrainFraud;
   int numTrainNonFraud;
   int testOutMark;
   int trainOutMark;
} tOutModel;

#define MAXNBROFMODELS 20

/* array of outmod struct to handle mutiple models */
tOutModel pOutMod[MAXNBROFMODELS]; 



double CalcSdev(tOutModel *outMod, tStatStruct *stat)
{ 
   double x, divisor;

//   if(outMod->num <= 0) return(0.0);

    //	
    // XXS: optionally, exclude zeros from calculating the stdev
    // (is there any better way to handle this skewed distribution?)
    //
   if (fGlobalFalseZeroThreshold != 0.0) {
       x = (stat->sumSq - (stat->sum*stat->sum)/stat->num_nonzero);
       divisor = stat->num_nonzero - 1;
   }
   else {
       x = (stat->sumSq - (stat->sum*stat->sum)/outMod->num);
       divisor = outMod->num - 1;
   }

   if(x<0.0) x=0.0;
   if(divisor<=0.0) divisor=1.0;

   return(sqrt(x/divisor));
}


int AccumStats(tOutModel *outMod)
{
   int       i;
   float     value;
   tNetInput *input;

   if(outMod->statBuf==NULL)
   {  outMod->statBuf = (tStatStruct*)malloc((outMod->numInput+1)*sizeof(tStatStruct));
      for(i=0; i<outMod->numInput+1; i++)
      {  outMod->statBuf[i].min = MAXDOUBLE;
         outMod->statBuf[i].max = MINDOUBLE;
         outMod->statBuf[i].sum = 0.0;
         outMod->statBuf[i].sumSq = 0.0;
         outMod->statBuf[i].mean = 0.0;
         outMod->statBuf[i].stdDev = 0.0;
         outMod->statBuf[i].divisor = 0.0;
         outMod->statBuf[i].range = 0.0;
	 // XXS
	 outMod->statBuf[i].num_nonzero = 0.0;
      }
   }
   outMod->num += 1.0;


   /* FRAUDQ hack */
   /* XXS: there's no need to collect statistics for FRAUDQ */
   /*
   if(isFraud) value = 1.0;
   else value = 0.0;

   if(value < outMod->statBuf[0].min) outMod->statBuf[0].min = (double)value;
   if(value > outMod->statBuf[0].max) outMod->statBuf[0].max = (double)value;

   outMod->statBuf[0].sum += (double)value;
   outMod->statBuf[0].sumSq += ((double)value * (double)value);
   */
   // XXS: however, we still need the counts
   if (isFraud) outMod->statBuf[0].num_nonzero += 1.0; 
   /* End FRAUDQ hack */

   for(i=1, input=outMod->model->inputList; input; input=input->next, i++)
   {  
      value = *((float*)input->data);

      if(value < outMod->statBuf[i].min) outMod->statBuf[i].min = (double)value;
      if(value > outMod->statBuf[i].max) outMod->statBuf[i].max = (double)value;

/******************************************************************/
/*   Test on using only non-zero values for means, std dev, etc   */
/*           jcb   2/24/97                                        */
/*    If fGlobalFalseZeroThreshold = 0., then include all values  */
/*         in means, otherwise only include non-zeroes            */
/*    If fGlobalFalseZeroThreshold < 0., will not put false       */
/*         zeroes to the mean                                     */
/******************************************************************/
      if ( value != 0. && fGlobalFalseZeroThreshold != 0.0)
	     outMod->statBuf[i].num_nonzero += 1.0; 

      outMod->statBuf[i].sum += (double)value;
      outMod->statBuf[i].sumSq += ((double)value * (double)value);
   }

   return(0);
}


/*****************************************************************/
/* The data goes into buffer sized at the users request (default */
/* is 70 Megs.  When it is full, it gets dumped to file.         */
/*****************************************************************/
long DataMeister(int dumpFlag, tOutModel *outMod)
{
   int         recOut=0, r, i;
   int         numFields = outMod->numInput + 1;  /* Add 1 for FRAUDQ */
   tNetInput   *input;
   int    iMaxRecCnt = 0;

   iMaxRecCnt = outMod->countFraud + outMod->countNonFraud;

   /*******************************************************************/
   /* If values==NULL, that indicates to write data to the scale file */
   /*******************************************************************/
   if(dumpFlag)
   {  float  *dataVector;
      float  sclVal;

      dataVector = (float*)malloc(outMod->recSize);

      if(!silent) fprintf(stderr, "Creating scale file...\n");

      /*************************************************/
      /* Output number of fields per record at the top */
      /*************************************************/
      fwrite((char*)&numFields, sizeof(int), 1, outMod->sclFp);

      /************************************/
      /* Read from the file first, if any */
      /************************************/
      if(outMod->readFile)
      {  rewind(outMod->dataFile);

         while(fread((char*)dataVector, 1, outMod->recSize, outMod->dataFile))
         {  for(i=0; i<numFields; i++)
            {  
               if(i == 0)
               {  if(dataVector[i] == 0.0) sclVal = -1.0;
                  else sclVal = 1.0;
               } else {
		 if(outMod->statBuf[i].stdDev == 0.0) {
		   sclVal = 0.0;
		 }
		 /* KMH ADDED 7/24/1997: TO MAP FALSE ZEROS TO MEAN */
		 else if ( fabs((float)(outMod->statBuf[i].num_nonzero) / (float) iMaxRecCnt ) 
                                  < fGlobalFalseZeroThreshold ) {
		   sclVal = 0.0; 
                 } else {
		   sclVal = (float)(((double)dataVector[i]
                          - outMod->statBuf[i].mean)/outMod->statBuf[i].stdDev);
		 }
               }

               fwrite((char*)&sclVal, sizeof(float), 1, outMod->sclFp);
            }

            if(!silent && (recOut%500)==0)
               fprintf(stderr, "%d: %d\r", outMod->total, recOut);
            recOut++;
         }
      }


      /*****************************/
      /* Read from the memory area */
      /*****************************/
      for(r=0; r<outMod->numStored; r++)
      {  for(i=0; i<numFields; i++)
         {  if(i == 0)
            {  if(outMod->dataBlock[(r*numFields) + i] == 0.0) sclVal = -1.0;
               else sclVal = 1.0;
            } else {
              if(outMod->statBuf[i].stdDev == 0.0) {
		sclVal = 0.0;
              /* KMH ADDED 7/24/1997: TO MAP FALSE ZEROS TO MEAN */
              } else if ( fabs((float)(outMod->statBuf[i].num_nonzero) / (float) iMaxRecCnt) 
                                  < fGlobalFalseZeroThreshold ) {
		sclVal = 0.0; 
              } else {
		sclVal = (float)(((double)outMod->dataBlock[(r*numFields) + i]
                       - outMod->statBuf[i].mean)/outMod->statBuf[i].stdDev);
	      }
            }

            fwrite((char*)&sclVal, sizeof(float), 1, outMod->sclFp);
         }

         if(!silent && (recOut%500)==0)
            fprintf(stderr, "%d: %d\r", outMod->total, recOut);
         recOut++;
      }


      if(!silent) fprintf(stderr, "Total of %d records written\n\n", recOut);

      unlink(outMod->tFileName);
      free((char*)outMod->dataBlock);
      return(0);
   }


   /***********************************************/
   /* If starting to read data, malloc the buffer */
   /***********************************************/
   if(outMod->dataBlock == NULL)
   {  outMod->recSize = numFields*sizeof(float);  
      outMod->numRecs = unsclBufSize / outMod->recSize;

      /* Allocate the data buffer */
      if((outMod->dataBlock = 
            (float*)malloc((outMod->numRecs) * (outMod->recSize)))==NULL)
      {  fprintf(stderr, "Can't allocate memory for scaled data.  Failing hard\n\n");
         exit(1);
      }

      /* Create temporary file now */
      sprintf(outMod->tFileName, "%sTEMP%s", 
                     outMod->model->modelPath, outMod->model->modelName);
      if((outMod->dataFile = fopen(outMod->tFileName, "w+"))==NULL)
      {  free((char*)outMod->dataBlock);
         fprintf(stderr, "Can't create temp file for scaled data.  Failing hard\n\n");
         exit(2);
      }
   }


   /****************************************************/
   /* If buffer is full, dump it to the temporary file */
   /****************************************************/
   if(outMod->index == numFields*outMod->numRecs)
   {  fprintf(stderr, "Dumping Buffer...\n");

      for(r=0; r<outMod->numRecs; r++)
      {  for(i=0; i<numFields; i++)
            fwrite((char*)&(outMod->dataBlock[i+(r*numFields)]), 
                                       sizeof(float), 1, outMod->dataFile);

         if(!silent && (recOut%1000)==0)
            fprintf(stderr, "%d: %d\n", outMod->total, recOut);
         recOut++;
      }
      outMod->readFile = 1;

      /*************************************************/
      /* Start back at the beginning of the data block */
      /*************************************************/
      outMod->index = 0;
      outMod->numStored = 0;
   }
   
   /******************************************/
   /* Accumulate the statistics for the data */
   /******************************************/
   AccumStats(outMod);

   /*****************************************/
   /* Dump the data into the memory buffer. */
   /*****************************************/
   if(isFraud)
      outMod->dataBlock[outMod->index++] = 1.0;
   else
      outMod->dataBlock[outMod->index++] = 0.0;

   for(input=outMod->model->inputList; input; input=input->next)
   {  switch(input->type)
      {  case  HNC_STRING:
            fprintf(stderr, "Cannot handle string type\n");
            exit(1);
            break;
         default :
            outMod->dataBlock[outMod->index++] = *((float*)input->data);
            break;
      }
   }

   outMod->numStored++;
   outMod->total++;
 
   return(0);
}


void InitOutModelStruct(void)
{
    int i = 0;

    for(i=0; i<MAXNBROFMODELS; i++)
    {  memset(&pOutMod[i],0,sizeof(tOutModel));
       pOutMod[i].first = 1;
    }
}


tOutModel* 
FindCurrOutMod(tModel *model)
{
    int       i;
    tNetInput *input;
    tOutModel *outMod=NULL;

    for(i=0; i<MAXNBROFMODELS; i++)
    {
        if(pOutMod[i].model == NULL)
        {
           pOutMod[i].model = model;
           outMod = pOutMod+i;

           outMod->numInput = 0;
           for(input=model->inputList; input; input=input->next)
              outMod->numInput++;

           return(outMod);
        }
        else if(pOutMod[i].model == model)
           return(pOutMod+i);
    }

    fprintf(stderr,"Too many models; Max maodel this program can handle is %d",MAXNBROFMODELS); 
    exit(1);
}


//
// XXS: cName[] is needed in WriteNode() to determine binary variables,
// XXS: which will not be scaled.
//
void
WriteNode(int target, char* nodeName, char* cName,
          int nodeNum, int offset, int length, FILE* cfgFp, tOutModel *outMod)
{
   tStatStruct *stat;

   /*******************************************/
   /* Output the data into the given cfg file */
   /*******************************************/
   fprintf(cfgFp,"\n");
   fprintf(cfgFp,"%d\n", nodeNum);
   // XXS: add cName in output
   fprintf(cfgFp,"Name= %s  .  %s\n", nodeName,cName);
   
   if(target) fprintf(cfgFp,"Type= BINARY\n");
   else fprintf(cfgFp,"Type= CONTINUOUS\n");
 
   if(!target) fprintf(cfgFp,"Slab= INPUT\n");
   else fprintf(cfgFp,"Slab= TARGET\n");

    // XXS
   stat = outMod->statBuf+nodeNum;

   if(outMod->num==0 || (fGlobalFalseZeroThreshold!=0.0 && stat->num_nonzero==0))
   {  fprintf(cfgFp, "Size= 1\n");
      fprintf(cfgFp, "Start= %d  Length= %d\n", offset, length);
      fprintf(cfgFp, "RecCnt= 0\n");
      fprintf(cfgFp, "Min= 0  Max= 0\n");
      fprintf(cfgFp, "MissingValue= 0\n");
      fprintf(cfgFp, "Sum= 0\n");
      fprintf(cfgFp, "Mean= 0\n");
      fprintf(cfgFp, "StdDev= 0\n");
      fprintf(cfgFp, "Derivative= 0\n");
      fprintf(cfgFp, "TimeSlice= 0\n");
      fprintf(cfgFp, "NbrOfSymbols= 0\n");
      fprintf(cfgFp, "Symbolic= NUMERIC\n");
      if(!target) fprintf(cfgFp, "ScaleMode= AUTO  ScaleFn= Z\n");
      else fprintf(cfgFp, "ScaleMode= AUTO  ScaleFn= NONE\n");
      fprintf(cfgFp,"Divisor= 0  Range= 1\n");
   }
   else
   {
      if(stat->max == MINDOUBLE) stat->max = 0;

	// XXS: binary variables will not be scaled
	//  (by forcing mean and std-dev to be 0 and 1)
      if (strncasecmp(cName,"IS",2)==0 || strncasecmp(cName,"RECENT",6)==0) {
         stat->mean = 0.0;
         stat->stdDev = 1.0;
      }  
      else {
	    //
	    // XXS: optionally, exclude zeros when calculating the mean
	    //
         if (fGlobalFalseZeroThreshold != 0.0)
            stat->mean = stat->sum / stat->num_nonzero;
	 else
            stat->mean = stat->sum / outMod->num;

	 stat->stdDev = CalcSdev(outMod, stat);
      }  

      stat->divisor = ((stat->min <= 0.0) ?
                 (double)0.0 : log(stat->max / stat->min));
      stat->range   = ((stat->max <= stat->min) ?
                    (double)0.0 : (stat->max - stat->min));

      fprintf(cfgFp, "Size= 1\n");
      fprintf(cfgFp, "Start= %d  Length= %d\n", offset, length);
      // XXS
      if (fGlobalFalseZeroThreshold != 0.0)
	  fprintf(cfgFp, "RecCnt= %d\n", (int)stat->num_nonzero);
      else
	  fprintf(cfgFp, "RecCnt= %d\n", (int)outMod->num);
      
      if(!target) fprintf(cfgFp, "Min= %.7g  Max= %.7g\n", stat->min, stat->max);
      else fprintf(cfgFp, "Min= 0  Max= 1\n");

      if(!target) fprintf(cfgFp, "MissingValue= %.15g\n", stat->mean);
      else fprintf(cfgFp, "MissingValue= 0\n");

      fprintf(cfgFp, "Sum= %.15g\n", stat->sum);
      fprintf(cfgFp, "Mean= %.15g\n", stat->mean);
      fprintf(cfgFp, "StdDev= %.15g\n", stat->stdDev);
      fprintf(cfgFp, "Derivative= 0\n");
      fprintf(cfgFp, "TimeSlice= 0\n");
      fprintf(cfgFp, "NbrOfSymbols= 0\n");
      fprintf(cfgFp, "Symbolic= NUMERIC\n");

      if(!target) fprintf(cfgFp, "ScaleMode= AUTO  ScaleFn= Z\n");
      else fprintf(cfgFp, "ScaleMode= AUTO  ScaleFn= NONE\n");

      if(!target) fprintf(cfgFp,"Divisor= %.15g  Range= %.15g\n", 
                               stat->divisor, stat->range);
      else fprintf(cfgFp,"Divisor= 0  Range= 1\n");
   }
} 


long CreateModelCfg(tOutModel *outMod)
{
   int          i, recordLength;
   int          length=12;
   int          offset=0, numNodes=0;
   tNetInput    *input;
   tModel       *model = outMod->model;
   char         fileName[256];
   FILE         *cfgFp;

   /* Create the file */
   sprintf(fileName, "%s%s.cfg", model->modelPath, model->modelName);
   if((cfgFp = fopen(fileName, "w"))==NULL)
   {  fprintf(stderr, "Couldn't create '%s'\n", fileName);
      exit(1);
   }

   /***************************************************/
   /* Determine the overall record length, assuming a */
   /* constant length for each variable.              */
   /***************************************************/
   for(recordLength=offset, input=model->inputList; input; input=input->next, numNodes++)
      recordLength += length;
   fprintf(cfgFp, "Record Length= %d\n", recordLength);

   /********************************************/
   /* Loop throught the inputs and output each */
   /********************************************/
   //
   // XXS: cName[] is needed in WriteNode() to determine binary variables,
   // XXS: which will not be scaled.
   //
   WriteNode(TRUE, "FRAUDQ", "FRAUDQ", 0, offset, length, cfgFp, outMod);
   for(i=1, input=model->inputList; input; input=input->next, i++) { 
   
      WriteNode(FALSE, input->modelVar->cfgName, input->modelVar->cName,
                i, offset, length, cfgFp, outMod);

      if(doScale)
      {  if(outMod->statBuf[i].stdDev == 0.0)
            fprintf(stderr, "Variable %s has 0 standard deviation\n", 
                                                   input->modelVar->cfgName);
      }

      offset+=length;
   }
 
   fprintf(cfgFp,"\n-1\n");


   /***************************/
   /* Output scaled data file */
   /***************************/
   if(doScale) DataMeister(1, outMod);

   /* close appropriate files */
   if(doScale)
      fclose(outMod->sclFp);
   else
   {  fclose(outMod->datFp);
      if(outMod->datBadFp) 
         fclose(outMod->datBadFp);
   }

   /* Output count info in tag file */
   fseek(outMod->tagFp, outMod->testOutMark, 0);
   fprintf(outMod->tagFp, "%d", outMod->numTest);

   fseek(outMod->tagFp, outMod->trainOutMark, 0);
   fprintf(outMod->tagFp, "%d", outMod->numTrain);
   fclose(outMod->tagFp);

   /* Tag realated display */
   fprintf(stderr,"\n\n");
   fprintf(stderr,"--------------------------------------------------------------------------\n");
   fprintf(stderr,"  STATISTICS FOR SCALE FILE FOR MODEL %s\n", model->modelName);
   fprintf(stderr,"--------------------------------------------------------------------------\n");
   fprintf(stderr,"                                    Total    Train     Test   %% Test\n");
   fprintf(stderr,"    Number of records           = %7i  %7i  %7i    %5.2f\n",outMod->countFraud+outMod->countNonFraud,outMod->numTrain,outMod->numTest,100.*(float)(outMod->numTest)/(float)(outMod->countFraud+outMod->countNonFraud));
   fprintf(stderr,"    Number of Non-Fraud records = %7i  %7i  %7i    %5.2f\n",outMod->countNonFraud,outMod->numTrainNonFraud,outMod->numTestNonFraud,100.*(float)(outMod->numTestNonFraud)/(float)(outMod->countNonFraud));
   fprintf(stderr,"    Number of Fraud records     = %7i  %7i  %7i    %5.2f\n",outMod->countFraud,outMod->numTrainFraud,outMod->numTestFraud,100.*(float)(outMod->numTestFraud)/(float)(outMod->countFraud));
   if(outMod->countFraud > 0)
      fprintf(stderr,"    Ratio of NonFrauds/Frauds   = %7.4f  %7.4f  %7.4f\n",(float)(outMod->countNonFraud)/(float)(outMod->countFraud),(float)(outMod->numTrainNonFraud)/(float)(outMod->numTrainFraud),(float)(outMod->numTestNonFraud)/(float)(outMod->numTestFraud));

   fprintf(stderr,"--------------------------------------------------------------------------\n");

   return(0);
}


long CreateCfgFiles()
{
   int i;

   /* Loop through all models to create .cfg files */
   for(i=0; i<MAXNBROFMODELS; i++)
   {  if(pOutMod[i].model == NULL)
         break;

      CreateModelCfg(pOutMod+i);
   }

   return(0);
}


int CheckDatRec(tNetInput *netInput)
{
   char      outBuff[64];
   tNetInput *input;

   /**************************************/
   /* Check the data values for bad data */
   /**************************************/
   for(input=netInput; input; input=input->next)
   {  switch(input->type)
      {  case  HNC_STRING:
            fprintf(stderr, "Cannot handle string type\n");
            exit(1);
            break;
         default :
            sprintf(outBuff, "%12.6e", *((float*)input->data));
            if(strlen(outBuff)>12) 
               return(1);
            if(strstr(outBuff,"Inf")!=NULL)
               return(1);
            break;
      }
   }

   return(0);
}



/***********************************************************/
/* OutModelData is run when generating a dat or a scl file */
/***********************************************************/
int OutModelData(tModel *model)
{ 
   static int veryFirst = 1;

   char       modelOutFileName[256];
   tNetInput  *input;
   tOutModel  *outMod;
   FILE       *datOutFile;

 
   /*****************************************************/
   /* Only output data if isOutput and doModel are TRUE */
   /*****************************************************/
   if(!(isOutput && doModel))
      return(0);

   /***************************/
   /* init outMod struct  */
   /***************************/
   if(veryFirst)
   {
      InitOutModelStruct();
      veryFirst = 0;
   }
   outMod = FindCurrOutMod(model);

   /***************************************************/
   /* open proper files if first time for this model  */
   /***************************************************/
   if(outMod->first)
   {
      sprintf(modelOutFileName, "%s%s.tag", model->modelPath, model->modelName);
      outMod->tagFp = fopen(modelOutFileName, "w");
      if (outMod == NULL)
      {  fprintf(stderr, "\nError opening <%s> for writing\n", modelOutFileName);
         exit(1);
      }

      /* Output header for tag File */
      fprintf(outMod->tagFp, "test:        \n");
      outMod->testOutMark = ftell(outMod->tagFp) - 7L;

      fprintf(outMod->tagFp, "train:        \n");
      outMod->trainOutMark = ftell(outMod->tagFp) - 7L;
    
      if(doScale)
      {
         sprintf(modelOutFileName, "%s%s.scl", model->modelPath, model->modelName);
         outMod->sclFp = fopen(modelOutFileName, "w");
         if (outMod->sclFp == NULL)
         {  fprintf(stderr, "\nError opening <%s> for writing\n", modelOutFileName);
            exit(1);
         }
      }
      else
      {
         sprintf(modelOutFileName, "%s%s.dat", model->modelPath, model->modelName);
         outMod->datFp = fopen(modelOutFileName, "w");
         if (outMod->datFp == NULL)
         {  fprintf(stderr, "\nError opening <%s> for writing\n", modelOutFileName);
            exit(1);
         }
      }
      outMod->first = 0;
   }

   /********************************************/
   /* write tag file                           */
   /********************************************/
   if (outMod->tagFp)
   {  /* Include in tag File */
      if(extHashVal < extTestThreshold)
      {  outMod->numTest++;
         if (isFraud)         /* added for scale file stats per jcb - ojo 1/24 */
            outMod->numTestFraud++;
         else
            outMod->numTestNonFraud++;
         fprintf(outMod->tagFp, " %d test\n", outMod->tag);
      }
      else
      {  outMod->numTrain++;
         if (isFraud)         /* added for scale file stats per jcb - ojo 1/24 */
            outMod->numTrainFraud++;
         else
            outMod->numTrainNonFraud++;
         fprintf(outMod->tagFp, " %d train\n", outMod->tag);
      }
      outMod->tag++;
   }

        
   /********************************************/
   /* Check if going to .scl file or .dat file */
   /********************************************/
   if(doScale)
   {  /*****************************/
      /* Set the fraudq for output */
      /*****************************/
      if(isFraud)
         outMod->countFraud++;          /* for stats on scale file - ojo 1/24  */
      else
         outMod->countNonFraud++;       /* for stats on scale file - ojo 1/24  */
      
      DataMeister(0, outMod);
      return(0);
   }
   else
   {  
      /*****************************/
      /* Set the fraudq for output */
      /*****************************/
      if(isFraud)
         outMod->countFraud++;          /* for stats on scale file - ojo 1/24  */
      else
         outMod->countNonFraud++;       /* for stats on scale file - ojo 1/24  */

      /************************************************************************/
      /* Switch output value of file ptr depending on good or bad transaction */
      /************************************************************************/
      if(CheckDatRec(model->inputList)==0)
         datOutFile = outMod->datFp;
      else 
      {  if(!outMod->datBadFp) 
         {  sprintf(modelOutFileName, "%s%s.datBad", model->modelPath, model->modelName);
            outMod->datBadFp = fopen(modelOutFileName, "w");
            if (outMod->datBadFp == NULL)
            {  fprintf(stderr, "\nError opening <%s> for writing\n", modelOutFileName);
               exit(1);
            }
         }

         datOutFile = outMod->datBadFp;
      }


      
      /************************************************************/
      /* Output fraud value assuming the very first one is FRAUDQ */
      /************************************************************/
      if(isFraud)
        fprintf(datOutFile, "1.0 ");
      else
        fprintf(datOutFile, "0.0 ");
   
      for(input=model->inputList; input; input=input->next)
      {  switch(input->type)
         {  case  HNC_STRING:
               fprintf(stderr, "Cannot handle string type\n");
               exit(1);
               break;
            default :
               fprintf(datOutFile, "%12.6e ", *((float*)input->data));
               break;
         }
      }

      fprintf(datOutFile, "\n");
   }

   return(0);
}
