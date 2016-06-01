/*
** $Id: optinet.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: optinet.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.5  1997/02/07  19:38:42  can
 * Changed fileName from hard 256 to MODELPATHLEN
 *
 * Revision 1.4  1997/01/21  19:16:49  can
 * Made sure CONTINUOUS target variables are not squashed and converted from 1-999
 *
 * Revision 1.3  1997/01/18  03:28:39  can
 * Added support for GROUP input variables
 *
 * Revision 1.2  1997/01/17  19:49:27  gak
 * changed #include "safeopen.h" to #include "hncSafeFopen.h"
 *
** Revision 1.1  1997/01/17 19:17:40  can
** Initial revision
**
 *
**
*/


/* System includes */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "mymall.h"
#include "optinet.h"
#include "optierr.h"
#include "safopen.h"


/* Global Variable for number of profile variables */
int   iGlobalNumSpvFields = 0;

/* KMH ADDED 7/24/1997 */
/* DIRTY GLOBAL VAR FOR CAPTURING cfgRec.recCnt ACROSS ALL ACTIVE VARS IN *.cfg FILE */
int   iGlobalMaxRecCnt = -1;

/* add for testing */
extern float fGlobalFalseZeroThreshold;
/* KMH END */

/* Global Variable for types of risk tables input */
long  lGlobalRiskTabFound = 0;


long ReadCfgNode(FILE *cfgFp, tCfgNode *cfgNode, char *name, char *symbols);
long ReadCfgFile(char *baseName, tOptiNet *net);
long ReadCtaFile(char *baseName, tOptiNet *net);
long ReadWtaFile(char *baseName, tOptiNet *net);
long GetNumInputs(char *baseName, short *numInputs);


typedef struct sTokenEnum {
   char  *token;
   short value;
} tTokenEnum;


static tTokenEnum typeVals[] = {
{ "CONTINUOUS", VAR_CONTINUOUS },
{ "EXCLUDE", VAR_EXCLUDE },
{ "BINARY", VAR_BINARY },
{ "GROUP", VAR_GROUP },
{ "ID", VAR_ID },
{ "", -1 }
};

static tTokenEnum slabVals[] = {
{ "TARGET", SLAB_TARGET },
{ "INPUT", SLAB_INPUT },
{ "NONE", SLAB_NONE },
{ "", -1 }
};

static tTokenEnum scaleVals[] = {
{ "NONE", SCALE_NONE },
{ "LOG", SCALE_LOG },
{ "LIN", SCALE_LIN },
{ "Z", SCALE_Z },
{ "", -1 }
};

long GetTokenEnum(char *token, tTokenEnum *tbl, short *value)
{
   int i=0;

   while(tbl[i].value != -1)
   {  if(!strcmp(token, tbl[i].token))
      {  *value = tbl[i].value;
         return(1);
      }

      i++;
  }
   
   return(0);
}


long ReadCfgNode(FILE *cfgFp, tCfgNode *cfgNode, char *name, char *symbols)
{
   char *ptr, buf[128], token[50];
   int  temp, len, i, s;

   /* Skip blank */
   fgets(buf, 128, cfgFp);

   /* Read node number */
   fgets(buf, 128, cfgFp);
   if(!strcmp(buf, "-1\n"))
      return(-1);

   /* Name */
   fgets(buf, 128, cfgFp);
   sscanf(buf, "Name= %s", name);   

   /* Type */ 
   fgets(buf, 128, cfgFp);
   sscanf(buf, "Type= %s", token);
   if(!GetTokenEnum(token, typeVals, &cfgNode->varType))
      return(BAD_CFG_TYPE);

   /* Slab */
   fgets(buf, 128, cfgFp);
   sscanf(buf, "Slab= %s", token);
   if(!GetTokenEnum(token, slabVals, &cfgNode->slabType))
      return(BAD_CFG_SLAB);

   /* Size */
   fgets(buf, 128, cfgFp);

   /* Start / Length */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "Start= %d  Length= %d", &temp, &len)!=2)
      return(BAD_CFG_LENGTH);
   cfgNode->length = (short)len;

   /* RecCnt */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "RecCnt= %d", &cfgNode->recCnt)!=1)
      return(BAD_CFG_RECCNT);

   /* Min / Max */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "Min= %g  Max= %g", &cfgNode->minVal, &cfgNode->maxVal)!=2)
      return(BAD_CFG_MINMAX);

   /* Missing Value */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "MissingValue= %g", &cfgNode->missingVal)!=1)
      return(BAD_CFG_MISSVAL);

   /* Sum */
   fgets(buf, 128, cfgFp);

   /* Mean */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "Mean= %g", &cfgNode->mean)!=1)
      return(BAD_CFG_MEAN);

   /* StdDev */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "StdDev= %g", &cfgNode->stddev)!=1)
      return(BAD_CFG_STDDEV);

   /* Derivative */
   fgets(buf, 128, cfgFp);

   /* Timeslice */
   fgets(buf, 128, cfgFp);

   /* NbrOfSymbols */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "NbrOfSymbols= %d", &cfgNode->numSymbols)!=1)
      return(BAD_CFG_NUMSYM);

   /* Symbolic */
   fgets(buf, 128, cfgFp);

   /* Read all symbols, if any */
   if(cfgNode->varType == VAR_GROUP || cfgNode->varType == VAR_BINARY)
   {  char *ptr = symbols;
      for(i=0; i<cfgNode->numSymbols; i++)
      {  /* Read the symbol */
         fgets(buf, 128, cfgFp);
         if(sscanf(buf, "%d: %s", &s, token)!=2)
            return(BAD_SYM_READ);

         /* Make sure symbol index = loop index */
         if(s!=i)
            return(BAD_SYM_INDEX);

         /* Check for {MISSING} */
         if(!strcmp(token, "{MISSING}"))
            memset((char*)ptr, 0, cfgNode->length);
         else if(strlen(token) != cfgNode->length)
            return(BAD_SYM_LENGTH);
         else
            strcpy(ptr, token);
      
         ptr += cfgNode->length;
      }
   }

   /* ScaleMode / Fn */
   fgets(buf, 128, cfgFp);
   if((ptr = strstr(buf, "ScaleFn"))==NULL)
      return(BAD_CFG_SCALEFN);
   if(sscanf(ptr, "ScaleFn= %s", token)!=1)
      return(BAD_CFG_SCALEFN);
   if(!GetTokenEnum(token, scaleVals, &cfgNode->scaleType))
      return(BAD_CFG_SCALEFN);

   /* Divisor / Range */
   fgets(buf, 128, cfgFp);
   if(sscanf(buf, "Divisor= %g  Range= %g", &cfgNode->divisor, &cfgNode->range)!=2)
      return(BAD_CFG_DIVRANGE);

   return(0);
}


long ReadCfgFile(char *baseName, tOptiNet *net)
{
   FILE     *cfgFp;
   long     status=0, node=0, i;
   char     buf[128], fileName[MODELPATHLEN+1], nodeName[50], symbols[4096], *ptr;
   tCfgNode *cfgNode;

   /* Open the cfg file */
   sprintf(fileName, "%s.cfg", baseName);
   if((cfgFp=SafeFopen(fileName))==NULL)
      return(CANT_OPEN_CFG);

   /* Skip the record length */
   fgets(buf, 128, cfgFp);

   /* Loop through reading all the cfg nodes */
   while(node < net->numCfgNodes)
   {  /* Read the next node from the file */ 
      if((status = ReadCfgNode(cfgFp, net->aCfgNodes+node, nodeName, symbols)))
         break;

      /* Toss any excluded nodes */
      cfgNode = net->aCfgNodes + node; 
      if(cfgNode->varType == VAR_EXCLUDE || cfgNode->slabType == SLAB_NONE)
         continue;

      /* If target node, determine if output is to be squashed */
      if(cfgNode->slabType == SLAB_TARGET)
      {  if(cfgNode->varType == VAR_CONTINUOUS)
            net->squashOutput = 0;
         else
            net->squashOutput = 1;
         continue;
      }

      /* Copy the name */
      if((cfgNode->name = (char*)MyMalloc(strlen(nodeName)+1))==NULL)
         return(NO_NET_MEMORY);
      strcpy(cfgNode->name, nodeName);

      /* Copy the symbols, if any */
      if(cfgNode->numSymbols)
      {  if((cfgNode->aSymbols = (char**)MyMalloc(cfgNode->numSymbols * sizeof(char*)))==NULL)
            return(NO_NET_MEMORY);

         ptr = symbols;
         for(i=0; i<cfgNode->numSymbols; i++)
         {  /* Allocate the symbol string */
            if((cfgNode->aSymbols[i] = (char*)MyMalloc(cfgNode->length+1))==NULL)
               return(NO_NET_MEMORY);

            /* Copy the symbol that was read in */
            memcpy(cfgNode->aSymbols[i], ptr, cfgNode->length);
            cfgNode->aSymbols[i][cfgNode->length] = '\0';
            ptr += cfgNode->length;
         }
      }

      /* KMH ADDED 7/24/1997: COMPUTE MAX cfgRec.recCnt FOR THE ACTIVE VARIABLES */
      if ( cfgNode->recCnt > iGlobalMaxRecCnt ) {
	iGlobalMaxRecCnt = cfgNode->recCnt;
      } 
      /* KMH END */
  
      node++; 
   }

   /* If status is not -1 (end of cfg file), return it now */ 
   if(status != -1)
      return(status);

   /* Make sure number of nodes read is the same as numCfgNodes */
   if(node!=net->numCfgNodes)
      return(CFG_NODE_MISMATCH);

   fclose(cfgFp);
   return(OK);
}


long ReadCtaFile(char *baseName, tOptiNet *net)
{
   FILE   *ctaFp;
   char   buf[128], fileName[256];
   int    i;

   /* Open the cta file */
   sprintf(fileName, "%s.cta", baseName);
   if((ctaFp=SafeFopen(fileName))==NULL)
      return(CANT_OPEN_CTA);

   /* Read the header line -- should start with "cts" */
   fgets(buf, 128, ctaFp);
   if(memcmp(buf, "cts", 3))
      return(BAD_CTA_FORMAT);

   /* Skip next line */
   fgets(buf, 128, ctaFp);
   
   /* Next line is the number of weighted inputs */
   fgets(buf, 128, ctaFp);
   sscanf(buf, "%d", &net->numScaled);

   /* Skip next two lines */
   fgets(buf, 128, ctaFp);
   fgets(buf, 128, ctaFp);

   /* Next line is the number of hidden nodes */
   fgets(buf, 128, ctaFp);
   sscanf(buf, "%d", &net->numHidden);

   /* Allocate the structure for the hidden nodes */
   if((net->aHidden = (tHidden*)MyMalloc(net->numHidden * sizeof(tHidden)))==NULL)
      return(NO_NET_MEMORY);
   memset((char*)net->aHidden, 0, net->numHidden*sizeof(tHidden)); 

   /* Allocate the array of weights for each hidden node */
   for(i=0; i<net->numHidden; i++) 
   {  if((net->aHidden[i].aWeights = 
              (float*)MyMalloc(net->numScaled * sizeof(float)))==NULL)
         return(NO_NET_MEMORY);
      memset((char*)net->aHidden[i].aWeights, 0, net->numScaled*sizeof(float)); 
   }

   /* Allocate the scaled data array */
   if((net->aScaled = (float*)MyMalloc(net->numScaled * sizeof(float)))==NULL)
      return(NO_NET_MEMORY);
   memset((char*)net->aScaled, 0, net->numScaled * sizeof(float)); 


   fclose(ctaFp);
   return(OK);
}


long ReadWtaFile(char *baseName, tOptiNet *net)
{
   FILE   *wtaFp;
   char   buf[128], fileName[256];
   int    numWeights;
   int    hid, node;

   /* Open the wta file */
   sprintf(fileName, "%s.wta", baseName);
   if((wtaFp=SafeFopen(fileName))==NULL)
      return(CANT_OPEN_WTA);

   /* Read the header line -- should start with "wta" */
   fgets(buf, 128, wtaFp);
   if(memcmp(buf, "wts", 3))
      return(BAD_WTA_FORMAT);

   /* Skip next 4 lines */
   fgets(buf, 128, wtaFp);
   fgets(buf, 128, wtaFp);
   fgets(buf, 128, wtaFp);
   fgets(buf, 128, wtaFp);
   
   /* Next line is the number of weights to follow.  This should */
   /* be the same as (numScaled+1) * numHidden                   */
   fgets(buf, 128, wtaFp);
   sscanf(buf, "%d", &numWeights);
   if(numWeights != ((net->numScaled + 1) * net->numHidden))
      return(NUM_WEIGHT_MISMATCH);

   /* Loop through the file reading the weights */
   for(hid=0; hid<net->numHidden; hid++)
   {  /* Read the initial weight for the node */
      if(fscanf(wtaFp, "%g ", &net->aHidden[hid].initWeight)!=1)
         return(IN_WEIGHT_SCAN_ERROR);

      /* Read the input weights */
      for(node=0; node<net->numScaled; node++)
         if(fscanf(wtaFp, "%g ", &net->aHidden[hid].aWeights[node])!=1)
            return(IN_WEIGHT_SCAN_ERROR);
   }
    
   /* Read any blank lines */
   while(1)
   {  fgets(buf, 128, wtaFp);
      if(isdigit(buf[0])) break;
   }

   /* Skip the next line */
   fgets(buf, 128, wtaFp);

   /* Next line the number of weights to follow, which should be */
   /* the same as the number of hidden nodes + 1.                */
   fgets(buf, 128, wtaFp);
   sscanf(buf, "%d", &numWeights);
   if(numWeights != net->numHidden + 1)
      return(NUM_HIDDEN_MISMATCH);

   /* Read the output node weights */
   if(fscanf(wtaFp, "%g ", &net->outputWeight)!=1)
      return(OUT_WEIGHT_SCAN_ERROR);
   for(hid=0; hid<net->numHidden; hid++)
   {  /* Read the output weight fornode */
      if(fscanf(wtaFp, "%g ", &net->aHidden[hid].outputWeight)!=1)
         return(OUT_WEIGHT_SCAN_ERROR);
   }

   fclose(wtaFp);
   return(OK);
}


long GetNumInputs(char *baseName, short *numInputs)
{
   FILE   *cfgFp;
   short  count=0;
   char   buf[128], fileName[256];

   /* Open the cfg file */
   sprintf(fileName, "%s.cfg", baseName);
   if((cfgFp=SafeFopen(fileName))==NULL)
      return(CANT_OPEN_CFG);

   /* Count the number of INPUT nodes in the cfg file */
   while(fgets(buf, 128, cfgFp))
   {  if(!memcmp(buf, "Slab", 4) && strstr(buf, "INPUT"))
         count++;
   }
   
   *numInputs = count;

   fclose(cfgFp);
   return(OK);
}


long InitOptiNet(char *baseName, tOptiNet **pNet)
{
   tOptiNet  *net;
   long      status;

   /* Allocate a tOptiNet structure */
   if((net = (tOptiNet*)MyMalloc(sizeof(tOptiNet)))==NULL) 
      return(FATAL_NET|NET_INIT|NO_NET_MEMORY);
   memset((char*)net, 0, sizeof(tOptiNet));

   /* Get the number of inputs in the network */
   if((status= GetNumInputs(baseName, &net->numCfgNodes)))
      return(FATAL_NET|NET_INIT|status);

   /* Allocate the tCfgNode array and tNodeData array */
   if((net->aCfgNodes = (tCfgNode*)MyMalloc(net->numCfgNodes * sizeof(tCfgNode)))==NULL)
      return(FATAL_NET|NET_INIT|NO_NET_MEMORY);
   memset((char*)net->aCfgNodes, 0, net->numCfgNodes * sizeof(tCfgNode));

   if((net->aInputData = (tNodeData*)MyMalloc(net->numCfgNodes * sizeof(tNodeData)))==NULL)
      return(FATAL_NET|NET_INIT|NO_NET_MEMORY);
   memset((char*)net->aInputData, 0,net->numCfgNodes * sizeof(tNodeData));

   /* Read the .cfg file */
   if((status = ReadCfgFile(baseName, net)))
      return(FATAL_NET|NET_INIT|status);

   /* Get the number of hidden nodes and the */
   /* number of weights from the .cta file  */
   if((status = ReadCtaFile(baseName, net)))
      return(FATAL_NET|NET_INIT|status);
    
   /* Read the .wta file, filling the tHidden structure */
   if((status = ReadWtaFile(baseName, net)))
      return(FATAL_NET|NET_INIT|status);

   /* Set the return pointer */
   *pNet = net;

   return(OK);
}


#define EXPCAP  10 
#define LO_VAL  -1 
#define HI_VAL  1 

#define SIGMOID(x) (x < -EXPCAP ? LO_VAL : x > EXPCAP ? HI_VAL : \
(LO_VAL + (HI_VAL - LO_VAL) / (1.0 + exp(-1.0 * ((double)x)))) )

long IterOptiNet(tOptiNet *net)
{
   long      i, s, hit;
   tHidden   *hidden;
   float     value, *start, *scale, *weight;
   tNodeData *input;
   tCfgNode  *cfgNode;
   

   /* Scale input data */
   scale = net->aScaled;
   cfgNode = net->aCfgNodes;
   input = net->aInputData;
   for(i=0; i<net->numCfgNodes; i++, cfgNode++, input++)
   {  
      if(cfgNode->varType == VAR_CONTINUOUS)
      {  switch(cfgNode->scaleType)
         {  case SCALE_Z:
               if(cfgNode->stddev <= 0.0)
                  *scale++ = 0.0;
               else if ( fabs( (float)cfgNode->recCnt / (float)iGlobalMaxRecCnt )
			 < fGlobalFalseZeroThreshold ) {

		 /* THIS VARIABLE HAS A LOT OF FALSE ZEROS. MAP FALSE ZEROS TO MEAN */
#ifdef KMH_DEBUG
		 printf ( "RecCnt %i, MaxRecCnt %i, FalseZThresh %f \n", 
			  pField->RecCnt, iGlobalMaxRecCnt, fGlobalFalseZeroThreshold );
#endif /* KMH_DEBUG */
		 *scale++ = 0.0;
	       }  
               else
               {  /* Cap the value by the min and max */
                  if(input->data.num < cfgNode->minVal) 
                     value = cfgNode->minVal;
                  else if(input->data.num > cfgNode->maxVal) 
                     value = cfgNode->maxVal;
                  else
                     value = input->data.num;

                  *scale++ = (value - cfgNode->mean) / cfgNode->stddev;
               }
               break;
            case SCALE_NONE:
               *scale++ = input->data.num;
               break;
            case SCALE_LOG:
               if(cfgNode->minVal <= 0.0 || cfgNode->divisor <= 0.0)
                  *scale++ = 0.0;
               else if(input->data.num < 0.0)
                  *scale++ = cfgNode->minVal;
                else
                  *scale++ = (2.0 * log(input->data.num/cfgNode->minVal) /
                                                      cfgNode->divisor - 1.0);
               break;
            case SCALE_LIN:
               if(cfgNode->range <= 0.0)
                  *scale++ = 0.0;
               else
                  *scale++ = (2.0 * ((input->data.num - cfgNode->minVal) / 
                                                       cfgNode->range) - 1.0);
               break;
         }
      }
      else if(cfgNode->varType == VAR_BINARY)
      {  if(cfgNode->numSymbols)
         {  if(!strcmp(cfgNode->aSymbols[0], input->data.str)) 
               *scale++ = -1.0;
            else if(!strcmp(cfgNode->aSymbols[1], input->data.str)) 
               *scale++ = 1.0;
            else
            {  if(cfgNode->missingVal==0)
                  *scale++ = -1.0;
               else
                  *scale++ = 1.0;
            }
         }
         else
         {  if(input->data.num == 0.0)
               *scale++ = -1.0;
            else
               *scale++ = 1.0;
         }
      }
      else if(cfgNode->varType == VAR_GROUP)
      {  start = scale;
         hit = 0;

         /* Find a matching symbol, if any */
         for(s=0; s<cfgNode->numSymbols; s++)
         {  if(!strcmp(cfgNode->aSymbols[s], input->data.str))
            {  *scale++ = 1.0;
               hit = 1;
            }
            else
               *scale++ = 0.0;
         }

         if(!hit)
            start[(int)cfgNode->missingVal] = 1.0;
      }
   }


   /* Get all the hidden slab outputs */ 
   hidden = net->aHidden;
   net->output = net->outputWeight;
   for(i=0; i<net->numHidden; i++, hidden++)
   {  hidden->outputVal = hidden->initWeight;

      scale = net->aScaled;
      weight = hidden->aWeights;
      for(s=0; s<net->numScaled; s++)
         hidden->outputVal += *scale++ * *weight++;
 
      hidden->outputVal = (float)SIGMOID(hidden->outputVal);
      net->output += hidden->outputVal * hidden->outputWeight;
   }
   
   if(net->squashOutput)
   {  net->output = (SIGMOID(net->output) + 1) * 500;

/**** jcb  5/29/98   don't let score be less than 1 or greater than 999 *****/
      if(net->output < 1.0) net->output = 1.0;
      else
        if(net->output > 999.0) net->output = 999.0;

   }
   
   return(OK);
}


long FreeOptiNet(tOptiNet *net)
{
   int i, s; 

   /* Free the cfg data structure -- watch this is sharing */
   for(i=0; i<net->numCfgNodes; i++)
   {  MyFree((void*)net->aCfgNodes[i].name);

      /* Free the symbols */
      for(s=0; s<net->aCfgNodes[i].numSymbols; s++)
         MyFree((void*)net->aCfgNodes[i].aSymbols[s]);
      MyFree((void*)net->aCfgNodes[i].aSymbols);
   }
      
   MyFree((void*)net->aCfgNodes);

   /* Free the input data structure */
   MyFree((void*)net->aInputData);

   /* Free the scaling data structure */
   MyFree((void*)net->aScaled);

   /* Free the hidden structure */
   for(i=0; i<net->numHidden; i++)
      MyFree((void*)net->aHidden[i].aWeights);
   MyFree((void*)net->aHidden);
   
   /* Free the network structure itself */
   MyFree((void*)net);

   return(OK);
}
