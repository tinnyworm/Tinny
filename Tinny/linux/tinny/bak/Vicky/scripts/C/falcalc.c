#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "falcalc.h"
#include "falmfnc.h"
#include "except.h"
#include "hncrtyp.h"
#include "config.h"
#include "dates.h"
#include "mgmodel.h"
#include "dyntabl.h"



#define MILES    6.9
float CalcDistance(short lat1, short long1, short lat2, short long2)
{
   double   distance, degrees, normLat, normLong;

   /************************************/
   /* Sanity check the lat/long values */
   /************************************/
   if ( (lat1==0 && long1==0) || 
        (lat2==0 && long2==0) ||
        (lat1 > 1800 || lat2 > 1800)  ||
        (lat1 < -1800 || lat2 < -1800) ||
        (long1 > 1800 || long2 > 1800)  ||
        (long1 < -1800 || long2 < -1800) )
      return(0.0);

   /******************************************/
   /* Set the normalized lat and long values */
   /******************************************/
   normLat = (double)(lat1 - lat2);
   normLong = (double)(long1 - long2);
   normLong *= cos(fabs((double)(lat1 + lat2)) * 3.14/3600);

   /***********************************/
   /* Determine the degree difference */
   /***********************************/
   distance = normLat*normLat + normLong*normLong;
   degrees = sqrt(distance);

   /****************************************************/
   /* If the degrees wrap around 180, go the other way */
   /****************************************************/
   if (degrees > 1800)
     degrees = 3600 - degrees;

   /***********************************************/
   /* Calculate and return the distance, in miles */
   /***********************************************/
   return((long)(degrees * MILES));
}




#define NUMVBINS        43  /* number of virtual time bins */
#define BACKBINS         4
#define MAXTIME     604800L

typedef enum {
   WEEK_WORK=0,
   WEEK_NWRK,
   WEEK_NIGHT,
   WEEK_SLEEP,
   WKND_NIGHT,
   WKND_SLEEP,
   SATD_MORN,
   SUND_MORN,
   WKND_AFTR,

   NUM_TIMES
} weekTimes;

typedef struct {
  int    bin1;
  int    bin2;
  float  frac1;
  float  frac2;
} tBinInfo; 

static float twidth[NUM_TIMES] =
{ 22.5, 32.5, 30, 35, 16, 12, 4, 4, 12 };

typedef struct {
   long      llim;
   long      ulim;
   weekTimes binID;
} tTimeBin;

static tTimeBin vTimeBins[NUMVBINS] = {
/*  1 Day(1) Hour(07) Min(00) */  {      1L,  25200L, WEEK_SLEEP },
/*  2 Day(1) Hour(09) Min(00) */  {  25201L,  32400L, WEEK_NWRK  },
/*  3 Day(1) Hour(11) Min(00) */  {  32401L,  39600L, WEEK_WORK  },
/*  4 Day(1) Hour(14) Min(00) */  {  39601L,  50400L, WEEK_NWRK  },
/*  5 Day(1) Hour(16) Min(30) */  {  50401L,  59400L, WEEK_WORK  },
/*  6 Day(1) Hour(18) Min(00) */  {  59401L,  64800L, WEEK_NWRK  },
/*  7 Day(1) Hour(24) Min(00) */  {  64801L,  86400L, WEEK_NIGHT },
/*  8 Day(2) Hour(07) Min(00) */  {  86401L, 111600L, WEEK_SLEEP },
/*  9 Day(2) Hour(09) Min(00) */  { 111601L, 118800L, WEEK_NWRK  },
/* 10 Day(2) Hour(11) Min(00) */  { 118801L, 126000L, WEEK_WORK  },
/* 11 Day(2) Hour(14) Min(00) */  { 126001L, 136800L, WEEK_NWRK  },
/* 12 Day(2) Hour(16) Min(30) */  { 136801L, 145800L, WEEK_WORK  },
/* 13 Day(2) Hour(18) Min(00) */  { 145801L, 151200L, WEEK_NWRK  },
/* 14 Day(2) Hour(24) Min(00) */  { 151201L, 172800L, WEEK_NIGHT },
/* 15 Day(3) Hour(07) Min(00) */  { 172801L, 198000L, WEEK_SLEEP },
/* 16 Day(3) Hour(09) Min(00) */  { 198001L, 205200L, WEEK_NWRK  },
/* 17 Day(3) Hour(11) Min(00) */  { 205201L, 212400L, WEEK_WORK  },
/* 18 Day(3) Hour(14) Min(00) */  { 212401L, 223200L, WEEK_NWRK  },
/* 19 Day(3) Hour(16) Min(30) */  { 223201L, 232200L, WEEK_WORK  },
/* 20 Day(3) Hour(18) Min(00) */  { 232201L, 237600L, WEEK_NWRK  },
/* 21 Day(3) Hour(24) Min(00) */  { 237601L, 259200L, WEEK_NIGHT },
/* 22 Day(4) Hour(07) Min(00) */  { 259201L, 284400L, WEEK_SLEEP },
/* 23 Day(4) Hour(09) Min(00) */  { 284401L, 291600L, WEEK_NWRK  },
/* 24 Day(4) Hour(11) Min(00) */  { 291601L, 298800L, WEEK_WORK  },
/* 25 Day(4) Hour(14) Min(00) */  { 298801L, 309600L, WEEK_NWRK  },
/* 26 Day(4) Hour(16) Min(30) */  { 309601L, 318600L, WEEK_WORK  },
/* 27 Day(4) Hour(18) Min(00) */  { 318601L, 324000L, WEEK_NWRK  },
/* 28 Day(4) Hour(24) Min(00) */  { 324001L, 345600L, WEEK_NIGHT },
/* 29 Day(5) Hour(07) Min(00) */  { 345601L, 370800L, WEEK_SLEEP },
/* 30 Day(5) Hour(09) Min(00) */  { 370801L, 378000L, WEEK_NWRK  },
/* 31 Day(5) Hour(11) Min(00) */  { 378001L, 385200L, WEEK_WORK  },
/* 32 Day(5) Hour(14) Min(00) */  { 385201L, 396000L, WEEK_NWRK  },
/* 33 Day(5) Hour(16) Min(30) */  { 396001L, 405000L, WEEK_WORK  },
/* 34 Day(5) Hour(18) Min(00) */  { 405001L, 410400L, WEEK_NWRK  },
/* 35 Day(6) Hour(02) Min(00) */  { 410401L, 439200L, WKND_NIGHT },
/* 36 Day(6) Hour(08) Min(00) */  { 439201L, 460800L, WKND_SLEEP },
/* 37 Day(6) Hour(12) Min(00) */  { 460801L, 475200L, SATD_MORN  },
/* 38 Day(6) Hour(18) Min(00) */  { 475201L, 496800L, WKND_AFTR  },
/* 39 Day(7) Hour(02) Min(00) */  { 496801L, 525600L, WKND_NIGHT },
/* 40 Day(7) Hour(08) Min(00) */  { 525601L, 547200L, WKND_SLEEP },
/* 41 Day(7) Hour(12) Min(00) */  { 547201L, 561600L, SUND_MORN  },
/* 42 Day(7) Hour(18) Min(00) */  { 561601L, 583200L, WKND_AFTR  },
/* 43 Day(7) Hour(24) Min(00) */  { 583201L, 604800L, WEEK_NIGHT },
};

void FalNormTime (tEngVar *calcVar, tModelVar *modelVar)
{
   float     timeoweek, *result;
   float     startBin[9];
   tCalcVar  *args;
   float     denom;
   int       i, vbin=-1;
   long      ulim, llim, prevTow; 
   tBinInfo  binVector[BACKBINS+1];

   result = (float *)calcVar->data;

   timeoweek = *((float  *)modelVar->args->var->data);
   args = modelVar->args->next; 
   i=0;
   /* 8/18/1999 pcd change limit of while from i<7 to i<9 */
   while(i < 9)
   {  startBin[i] = *((float *)args->var->data);
      i++;
      args = args->next;
   }

   /* Find the bin for the time of the week */
   for(i=0; i<NUMVBINS; i++)
   {  if( (timeoweek >= (*(vTimeBins+i)).llim) && 
          (timeoweek <= (*(vTimeBins+i)).ulim))
      {  vbin = i;
         break;
      }
   }
   if(vbin<0 || vbin>NUMVBINS-1) 
   {  *result = 1;
      return;
   }


   /* Determine the bins spanned in the last BACKBINS hours */
   ulim = vTimeBins[vbin].ulim;
   llim = vTimeBins[vbin].llim;
   for(i=0; i<BACKBINS; i++)
   {  prevTow = timeoweek;

      if(timeoweek > _1HOURSECS) timeoweek -= _1HOURSECS;
      else timeoweek += MAXTIME-_1HOURSECS;

      if(timeoweek >= llim && timeoweek <= ulim)
      {  binVector[i].bin1 = vTimeBins[vbin].binID;
         binVector[i].frac1 = 1.0;
         binVector[i].bin2 = binVector[i].bin1;
         binVector[i].frac2 = 0;
      }
      else
      {  binVector[i].bin1 = vTimeBins[vbin].binID;
         binVector[i].frac1 = (float)(prevTow-llim) / _1HOURSECS;
         binVector[i].frac2 = 1 - binVector[i].frac1;
    
         if (vbin>0) --vbin;
         else vbin = NUMVBINS-1;

         ulim = vTimeBins[vbin].ulim;
         llim = vTimeBins[vbin].llim;

         binVector[i].bin2 = vTimeBins[vbin].binID;
      }
   }


   /* Determine the norm time denominator */
   for(denom=0.0, i=0; i<BACKBINS; i++)
   {  denom += 
             ((168*binVector[i].frac1)/twidth[binVector[i].bin1]) * 
                                            *(startBin+binVector[i].bin1);
      denom += 
             ((168*binVector[i].frac2)/twidth[binVector[i].bin2]) * 
                                            *(startBin+binVector[i].bin2);
   }
 
   *result = denom/4;
}


void FalAgeDecay(tEngVar *calcVar, tModelVar *modelVar)
{
   float age;

   age = *((float*)modelVar->args->var->data);
   if (age < 0)
     *((float*)calcVar->data) =  0.;
   else
     *((float*)calcVar->data) = 
         (float)SAFE_EXP((double)(-(long)(age /
                                     *((float*)modelVar->args->next->var->data))));
}


void FalDlyCoef1 (tEngVar *calcVar, tModelVar *modelVar)
{
   float    decayRate, acctAge, tmpRate;

   decayRate = *((float  *)modelVar->args->var->data);
   acctAge = *((float  *)modelVar->args->next->var->data);
 
/*******  jcb   5/29/98  minimum account age should be one day *****/
      if ( acctAge < 1.0) acctAge = 1.0;

   tmpRate = (decayRate > acctAge) ? acctAge : decayRate;

   if (tmpRate <= 0)
      *((float*)calcVar->data) = 1.0;
   else
      *((float*)calcVar->data) = 1/tmpRate;
}


void FalEventAvgCoef1 (tEngVar *calcVar, tModelVar *modelVar)
{
   float  numEvents, eventCount;

   numEvents = *((float*)modelVar->args->var->data);
   eventCount = *((float*)modelVar->args->next->var->data);

   *((float*)calcVar->data) = (1 / (numEvents<eventCount?numEvents:eventCount));  
}


void FalEventAvgCoef2 (tEngVar *calcVar, tModelVar *modelVar)
{
   float  numEvents, eventCount, maxVal;

   numEvents = *((float*)modelVar->args->var->data);
   eventCount = *((float*)modelVar->args->next->var->data);
   maxVal = *((float*)modelVar->args->next->next->var->data);

   if(numEvents < eventCount)
      *((float*)calcVar->data) = (float)SAFE_EXP((double)(-1/numEvents));   
   else
      *((float*)calcVar->data) = maxVal;
}


void FalConsec (tEngVar *calcVar, tModelVar *modelVar)
{
   float    coef, newVal, oldVal, doCalc=1;

   oldVal = *((float *)calcVar->data);

   /* Check for optional extra parameter to do decay or not */
   if(modelVar->args->next->next)
      doCalc = *((float  *)modelVar->args->next->next->var->data);

   newVal      = *((float  *)modelVar->args->var->data);
   coef        = *((float  *)modelVar->args->next->var->data);

   if (doCalc)
      newVal *= oldVal * coef + 1;
   else
      newVal = oldVal * coef;

   *((float*)calcVar->data) = newVal;
}


void FalNormDay (tEngVar *calcVar, tModelVar *modelVar)
{
   float    *result, dayofwk, locTime, fracToday, fracYesdy;
   float    startBin[7];
   tCalcVar *args;
   int      i;

   result = (float *)calcVar->data;

   locTime = *((float  *)modelVar->args->var->data);
   dayofwk = *((float  *)modelVar->args->next->var->data);

   args = modelVar->args->next->next;
   i=0;
   while(i < 7)
   {
      startBin[i] = *((float *)args->var->data);
      i++;
      args = args->next;
   }

   fracToday = locTime;
   fracYesdy = 1.0 - fracToday;

   *result = fracToday * startBin[((int)(dayofwk) -1)];
   *result += fracYesdy * startBin[(((int)(dayofwk) -2) + 7) % 7];
   *result *= 7;
}


void FalRiskRate(tEngVar *calcVar, tModelVar *modelVar)
{
   float adjustRatio = *((float*)modelVar->args->var->data);
   float numFrds     = (float)*((long*)modelVar->args->next->var->data);
   float total       = (float)*((long*)modelVar->args->next->next->var->data);
   float popRate     = *((float*)modelVar->args->next->next->next->var->data);

   *((float*)calcVar->data) = (numFrds + (adjustRatio * popRate)) / (total + adjustRatio); 
}


#define FABS(x) (x) < 0 ? -(x) : (x)
void FalRiskZ(tEngVar *calcVar, tModelVar *modelVar)
{
   float numer;
   float value       = *((float*)modelVar->args->var->data);
   float stddev      = *((float*)modelVar->args->next->var->data);
   float mean        = *((float*)modelVar->args->next->next->var->data);
   float def_stddev  = *((float*)modelVar->args->next->next->next->var->data);
   float def_mean    = *((float*)modelVar->args->next->next->next->next->var->data);

   if(stddev)
   {  numer = FABS(value - mean);
      *((float*)calcVar->data) = numer / stddev;
   }
   else if(def_stddev)
   {  numer = FABS(value - def_mean);
      *((float*)calcVar->data) = numer / def_stddev;
   }
   else
      *((float*)calcVar->data) = 0;
}


void FalCodeConv(tEngVar *calcVar, tModelVar *modelVar)
{
   tRecData* conv     = (tRecData*)((tRecInstance*)modelVar->args->var->data)->data;
   char*     code     = (char*)modelVar->args->next->var->data;
   long      defCntry = (long)*((float*)modelVar->args->next->next->var->data);

   if(isdigit(code[0]))
   {  long i, value=0;
  
      /* Assume have a three digit code.  Anything else */
      /* will use the default country code.             */
      for(i=0; i<3; i++)
      {  if(isdigit(code[i]))
            value = (value * 10) + (code[i]-'0');
         else if(isspace(code[i]))
            break;
         else
         {  *((long*)calcVar->data) = defCntry;
            return;
         }
      }

      *((long*)calcVar->data) = value;
   }
   else if(!memcmp(code, "   ", 3))
      *((long*)calcVar->data) = defCntry;
   else
   {  tTableTemplate *table = (tTableTemplate*)conv->record->user;

      /* Do lookup.  Table should be a string and a short, which is the code */
      /* Note, this is assumed and it will crash if there is a problem.      */
      if(DoStringTableLookup(table, code, 3, conv))
         *((long*)calcVar->data) = defCntry;
      else
         *((long*)calcVar->data) = (long)*((short*)((tFlatData*)conv->flds[1].data)->data);
   }
}


#define USED_POST_CODE     1
#define USED_CNTRY_CODE    2
#define USED_CURR_CODE     3
void FalDistHome(tEngVar *calcVar, tModelVar *modelVar)
{
   long       cUsed = 0;
   float      cTime=0;
   short      cLat=0, cLong=0, mLat=0, mLong=0;

   long       defCntry   = (long)*((float*)modelVar->args->var->data);
   char*      cardPost   =     (char*)modelVar->args->next->var->data;
   long*      cardCtry   =     (long*)modelVar->args->next->next->var->data;
   char*      mrchPost   =     (char*)modelVar->args->next->next->next->var->data;
   long*      mrchCtry   =     (long*)modelVar->args->next->next->next->next->var->data;
   long*      mrchCurr   =     (long*)modelVar->args->next->next->next->next->next->var->data;
   tCalcVar   *args      =            modelVar->args->next->next->next->next->next->next;

   tRecData*  postLL     = 
           (tRecData*)((tRecInstance*)args->var->data)->data;
   tRecData*  cntryLL    = 
           (tRecData*)((tRecInstance*)args->next->var->data)->data;
   char*      logic      =     (char*)args->next->next->var->data;
   long       atHome     =   *((long*)args->next->next->next->var->data);
   long*      mUsed      =     (long*)args->next->next->next->next->var->data;
   float*     mTime      =    (float*)args->next->next->next->next->next->var->data;


   /***************************/
   /* Get cardholder location */
   /***************************/
   /* First try postal codes, if available */
   if((logic[0] == '1') && (*cardCtry == defCntry) && cardPost[0] != ' ')
   {  tTableTemplate *table = (tTableTemplate*)postLL->record->user; 
      if(!DoStringTableLookup(table, cardPost, table->keyLength, postLL))
      {  
         /* Assume layout of lat/long table to be code, lat, long, time */ 
         cLat = *((short*)((tFlatData*)postLL->flds[1].data)->data); 
         cLong = *((short*)((tFlatData*)postLL->flds[2].data)->data); 
         cTime = (float)*((short*)((tFlatData*)postLL->flds[3].data)->data) / 10.0; 
         cUsed = USED_POST_CODE; 
      }
   }
   
   /* Next try the country code */
   if(!cUsed && (logic[1] == '1'))
   {  tTableTemplate *table = (tTableTemplate*)cntryLL->record->user; 
      if(!DoLongTableLookup(table, *cardCtry, cntryLL))
      {  
         /* Assume layout of lat/long table to be code, lat, long, time */ 
         cLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
         cLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
         cTime = (float)*((short*)((tFlatData*)cntryLL->flds[3].data)->data) / 10.0; 
         cUsed = USED_CNTRY_CODE; 
      }
   }

   /* Finally, get the location for the default country */
   if(!cUsed)
   {  tTableTemplate *table = (tTableTemplate*)cntryLL->record->user; 
      DoLongTableLookup(table, defCntry, cntryLL);

      /* Assume layout of lat/long table to be code, lat, long, time */ 
      cLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
      cLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
      cTime = (float)*((short*)((tFlatData*)cntryLL->flds[3].data)->data) / 10.0; 
      cUsed = USED_CNTRY_CODE; 
   }


   /***************************************************************/
   /* If atHome is set, the SIC must be one like mail order where */
   /* the cardholder makes the purchase from home.                */
   /***************************************************************/

   if(atHome)
   {  *mrchCtry = *cardCtry;
      *mrchCurr = *cardCtry;
      *mUsed = cUsed;
      *mTime =  cTime;
      memcpy(mrchPost, cardPost, 9);

      /* Distance from home is 0 */
      *((float*)calcVar->data) = 0.0;
      return;
   }
   else
   {  /*****************************/
      /* Get the merchant location */  
      /*****************************/
      /* First try postal codes, if available */
      *mUsed = 0;
      if((logic[0] == '1') && (*mrchCtry == defCntry) && mrchPost[0] != ' ')
      {  tTableTemplate *table = (tTableTemplate*)postLL->record->user; 
         if(!DoStringTableLookup(table, mrchPost, table->keyLength, postLL))
         {  
            /* Assume layout of lat/long table to be code, lat, long, time */ 
            mLat = *((short*)((tFlatData*)postLL->flds[1].data)->data); 
            mLong = *((short*)((tFlatData*)postLL->flds[2].data)->data); 
            *mTime = (float)*((short*)((tFlatData*)postLL->flds[3].data)->data) / 10.0; 
            *mUsed = USED_POST_CODE; 
         }
      }
   
      /* Next try the country code */
      if(!*mUsed && (logic[1] == '1'))
      {  tTableTemplate *table = (tTableTemplate*)cntryLL->record->user; 
         if(!DoLongTableLookup(table, *mrchCtry, cntryLL))
         {  
            /* Assume layout of lat/long table to be code, lat, long, time */ 
            mLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
            mLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
            *mTime = (float)*((short*)((tFlatData*)cntryLL->flds[3].data)->data) / 10.0; 
            *mUsed = USED_CNTRY_CODE; 
         }
      }

      /* Next try the currency code */
      if(!*mUsed && (logic[2] == '1'))
      {  tTableTemplate *table = (tTableTemplate*)cntryLL->record->user; 
         if(!DoLongTableLookup(table, *mrchCurr, cntryLL))
         {  
            /* Assume layout of lat/long table to be code, lat, long, time */ 
            mLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
            mLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
            *mTime = (float)*((short*)((tFlatData*)cntryLL->flds[3].data)->data) / 10.0; 
            *mrchCtry = defCntry;
            *mUsed = USED_CURR_CODE; 
         }
      }

      /* Finally, get the location for the default country */
      if(!*mUsed)
      {  tTableTemplate *table = (tTableTemplate*)cntryLL->record->user; 
         DoLongTableLookup(table, defCntry, cntryLL);
   
         /* Assume layout of lat/long table to be code, lat, long, time */ 
         mLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
         mLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
         *mTime = (float)*((short*)((tFlatData*)cntryLL->flds[3].data)->data) / 10.0; 
         *mrchCtry = defCntry;
         *mUsed = USED_CNTRY_CODE; 
      }

      /*******************************************************************/
      /* Make sure both cardholder and merchant use either country codes */
      /* or postal codes when in the default country.                    */
      /*******************************************************************/
      if(cUsed == USED_POST_CODE)
      {  if((*mUsed == USED_CNTRY_CODE && *mrchCtry == defCntry) ||
            (*mUsed == USED_CURR_CODE && *mrchCurr == defCntry))
         {  *mTime = cTime;
            *mUsed = USED_POST_CODE;
            memcpy(mrchPost, cardPost, 9);

            /* Dist from home is 0 */
            *((float*)calcVar->data) = 0;
            return;
         }
      }
      else if(*mUsed == USED_POST_CODE)
      {  if((cUsed == USED_CNTRY_CODE) && (*cardCtry == defCntry))
         {  *mTime = cTime;
            *mUsed = cUsed;

            /* Dist from home is 0 */
            *((float*)calcVar->data) = 0;
            return;
         }
      }

      /* Calculate distance from home */
      *((float*)calcVar->data) = CalcDistance(cLat, cLong, mLat, mLong);
   }
}


void FalDistPrev(tEngVar *calcVar, tModelVar *modelVar)
{
   short          curLat=0, curLong=0, prevLat=0, prevLong=0;
   tTableTemplate *table;

   long       atHome    = *((long*)modelVar->args->var->data);
   long       curCtry   = *((long*)modelVar->args->next->var->data);
   char*      curPost   =   (char*)modelVar->args->next->next->var->data;
   long       prevCtry  = *((long*)modelVar->args->next->next->next->var->data);
   char*      prevPost  =   (char*)modelVar->args->next->next->next->next->var->data;
   tCalcVar   *args     =          modelVar->args->next->next->next->next->next;

   tRecData*  postLL    = (tRecData*)((tRecInstance*)args->var->data)->data;
   tRecData*  cntryLL   = (tRecData*)((tRecInstance*)args->next->var->data)->data;


   /* If at home, distance is always 0 */
   if(atHome)
   {  *((float*)calcVar->data) = 0.0;
      return;
   }

   /* If number of characters in the postll key match, */
   /* and not blank, in same country, distance is 0    */
   table = (tTableTemplate*)postLL->record->user; 
   if(!memcmp(curPost, prevPost, table->keyLength) && 
       curPost[0]!=' ' && curCtry==prevCtry)
   {  *((float*)calcVar->data) = 0.0;
      return;
   }
   
   /* Get the current lat/long */
   if(!DoStringTableLookup(table, curPost, table->keyLength, postLL))
   {  /* Assume layout of lat/long table to be code, lat, long, time */ 
      curLat = *((short*)((tFlatData*)postLL->flds[1].data)->data); 
      curLong = *((short*)((tFlatData*)postLL->flds[2].data)->data); 
   }
   else
   {  /* If the current country is the same as the previous country */
      /* then call it 0 distance.                                   */ 
      if(curCtry == prevCtry)
      {  *((float*)calcVar->data) = 0.0;
         return;
      }

      /* Get the location of the current country */
      table = (tTableTemplate*)cntryLL->record->user; 
      if(!DoLongTableLookup(table, curCtry, cntryLL))
      {  /* Assume layout of lat/long table to be code, lat, long, time */ 
         curLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
         curLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
      }
      else
      {  /* All has failed.  Set distance to 0 and leave */
         {  *((float*)calcVar->data) = 0.0;
            return;
         }
      }
   }
   

   /* Get the previous lat/long */
   table = (tTableTemplate*)postLL->record->user; 
   if(!DoStringTableLookup(table, prevPost, table->keyLength, postLL))
   {  /* Assume layout of lat/long table to be code, lat, long, time */ 
      prevLat = *((short*)((tFlatData*)postLL->flds[1].data)->data); 
      prevLong = *((short*)((tFlatData*)postLL->flds[2].data)->data); 
   }
   else
   {  /* If the current country is the same as the previous country */
      /* then call it 0 distance.                                   */ 
      if(curCtry == prevCtry)
      {  *((float*)calcVar->data) = 0.0;
         return;
      }

      /* Get the location of the previous country */
      table = (tTableTemplate*)cntryLL->record->user; 
      if(!DoLongTableLookup(table, prevCtry, cntryLL))
      {  /* Assume layout of lat/long table to be code, lat, long, time */ 
         prevLat = *((short*)((tFlatData*)cntryLL->flds[1].data)->data); 
         prevLong = *((short*)((tFlatData*)cntryLL->flds[2].data)->data); 
      }
      else
      {  /* All has failed.  Set distance to 0 and leave */
         {  *((float*)calcVar->data) = 0.0;
            return;
         }
      }
   }
   
   
   /* Calculate distance from previous */
   *((float*)calcVar->data) = CalcDistance(curLat, curLong, prevLat, prevLong);
}


void FalGeoRisk(tEngVar *calcVar, tModelVar *modelVar)
{
   tTableTemplate *table, *first;
   long           numFrds, total;
   float          popRate;
   char           kNkChar;

   char       tranType   =  *((char*)modelVar->args->var->data);
   long       authKeyed  =  *((long*)modelVar->args->next->var->data);
   long       used       =  *((long*)modelVar->args->next->next->var->data);
   char*      postCode   =    (char*)modelVar->args->next->next->next->var->data;
   long       ctryCode   =  *((long*)modelVar->args->next->next->next->next->var->data);
   long       currCode   =  *((long*)modelVar->args->next->next->next->next->next->var->data);
   float      adjRatio   = *((float*)modelVar->args->next->next->next->next->next->next->var->data);
   tCalcVar   *args      =           modelVar->args->next->next->next->next->next->next->next;

   tRecData*  geoRisk    =  (tRecData*)((tRecInstance*)args->var->data)->data;
   tRecData*  defRisk    =  (tRecData*)((tRecInstance*)args->next->var->data)->data;
   tRecData*  riskData   =  (tRecData*)((tRecInstance*)args->next->next->var->data)->data;

   if (authKeyed)  kNkChar = 'K';
    else           kNkChar = 'N';

   /* Get risk information according to used flag */
   if(used == USED_POST_CODE)
   {  first = table = (tTableTemplate*)riskData->record->user;

      /* Look for a POST risk table for type */
      if (tranType == 'A') {
        while(table) {
	  if(tranType == table->tableName[0] &&
	      !memcmp(table->tableName+1, "_POST", 5) &&
              kNkChar == table->tableName[7] )
            break;
          table = table->next;
	}
      } else {
        while(table) {
	  if(tranType == table->tableName[0] && !memcmp(table->tableName+1, "_POST", 5))
            break;
          table = table->next;
	}
      }

      if(table)
      {  memcpy(*defRisk->data, table->defaultData, table->dataLength);
         if(DoStringTableLookup(table, postCode, table->keyLength, geoRisk))
           memcpy(*geoRisk->data, table->defaultData, table->dataLength);
      }
      else
      {  memcpy(*defRisk->data, first->defaultData, first->dataLength);
         memcpy(*geoRisk->data, first->defaultData, first->dataLength);
      }
   }
   else if(used == USED_CURR_CODE)
   {  first = table = (tTableTemplate*)riskData->record->user;

      /* Look for a CURR risk table for type */
      if (tranType == 'A') {
        while(table) {
	  if(tranType == table->tableName[0] &&
	      !memcmp(table->tableName+1, "_CURR", 5) &&
              kNkChar == table->tableName[7] )
            break;
          table = table->next;
	}
      } else {
        while(table)
        {  if(tranType == table->tableName[0] && !memcmp(table->tableName+1, "_CURR", 5))
             break;
           table = table->next;
        }
      }

      if(table)
      {  memcpy(*defRisk->data, table->defaultData, table->dataLength);
         if(DoLongTableLookup(table, currCode, geoRisk))
            memcpy(*geoRisk->data, table->defaultData, table->dataLength);
      }
      else
      {  memcpy(*defRisk->data, first->defaultData, first->dataLength);
         memcpy(*geoRisk->data, first->defaultData, first->dataLength);
      }
   }
   else  
   {  first = table = (tTableTemplate*)riskData->record->user;

      /* Look for a CNTRY risk table for type */
      if (tranType == 'A') {
        while(table) {
	  if(tranType == table->tableName[0] &&
	      !memcmp(table->tableName+1, "_CNTRY", 6) &&
              kNkChar == table->tableName[8] )
            break;
          table = table->next;
	}
      } else {
        while(table)
        {  if(tranType == table->tableName[0] && !memcmp(table->tableName+1, "_CNTRY", 6))
             break;
           table = table->next;
        }
      }

      if(table)
      {  memcpy(*defRisk->data, table->defaultData, table->dataLength);
         if(DoLongTableLookup(table, ctryCode, geoRisk))
            memcpy(*geoRisk->data, table->defaultData, table->dataLength);
      }
      else
      {  memcpy(*defRisk->data, first->defaultData, first->dataLength);
         memcpy(*geoRisk->data, first->defaultData, first->dataLength);
      }
   }

   /* Set the risk rate */
   total = *((long*)((tFlatData*)defRisk->flds[1].data)->data); 
   numFrds = *((long*)((tFlatData*)defRisk->flds[2].data)->data); 
   popRate = (float)numFrds / (float)total;

   total = *((long*)((tFlatData*)geoRisk->flds[1].data)->data); 
   numFrds = *((long*)((tFlatData*)geoRisk->flds[2].data)->data); 

   *((float*)calcVar->data) = 
         ((float)numFrds + (adjRatio * popRate)) / ((float)total + adjRatio); 
}


long ValidateFalconFunction(tModelVar *modelVar, short funcCode, char *fileName, char *errMsg)
{
   tCalcVar *arg;
   int      argCt=1, badArg=0, errCode=0, expectArgs=0;

   /*****************************************************/
   /* Set the function pointer, given the function code */
   /* Also, for each function, make sure that the right */
   /* number and type of variables in the arg list      */
   /*****************************************************/
   errMsg[0] = '\0';
   modelVar->funcCode = funcCode;

   switch(funcCode)
   {  case FAL_EVENT_AVG_COEF1:
         modelVar->func = FalEventAvgCoef1;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;
   
      case FAL_EVENT_AVG_COEF2:
         modelVar->func = FalEventAvgCoef2;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_AGE_DECAY:
         modelVar->func = FalAgeDecay;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_DLY_COEF1:
         modelVar->func = FalDlyCoef1;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_CONSEC:
         modelVar->func = FalConsec;

         expectArgs=2;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_NORM_TIME:
         modelVar->func = FalNormTime;

         expectArgs=10;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
               case 5:
               case 6:
               case 7:
               case 8:
               case 9:
               case 10:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_NORM_DAY:
         modelVar->func = FalNormDay;

         expectArgs=9;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
               case 5:
               case 6:
               case 7:
               case 8:
               case 9:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_RISK_RATE:
         modelVar->func = FalRiskRate;

         expectArgs=4;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 4:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               case 2:
               case 3:
                  if(arg->var->type != HNC_INT32)
                     errCode = ARG_NOT_LONG;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_RISK_Z:
         modelVar->func = FalRiskZ;

         expectArgs=5;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 3:
               case 4:
               case 5:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_CODE_CONV:
         modelVar->func = FalCodeConv;

         expectArgs=3;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
                  if(arg->var->type != RECORD_TYPE)
                     errCode = ARG_NOT_RECORD;
                  break;
               case 2:
                  if(arg->var->type != HNC_STRING)
                     errCode = ARG_NOT_STRING;
                  break;
               case 3:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_DIST_HOME:
         modelVar->func = FalDistHome;

         expectArgs=12;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 2:
               case 4:
               case 9:
                  if(arg->var->type != HNC_STRING)
                     errCode = ARG_NOT_STRING;
                  break;
               case 3:
               case 5:
               case 6:
               case 10:
               case 11:
                  if(arg->var->type != HNC_INT32)
                     errCode = ARG_NOT_LONG;
                  break;
               case 7:
               case 8:
                  if(arg->var->type != RECORD_TYPE)
                     errCode = ARG_NOT_RECORD;
                  break;
               case 1:
               case 12:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_GEO_RISK:
         modelVar->func = FalGeoRisk;

         /* 8/18/1999 pcd 
          *    Take into account `new' argument "authKeyed": arg 2, long.
          *    Incremented value of expectArgs, add case 10,
          *    incrementing cases above 2.
          */
         expectArgs=9;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 4:
                  if(arg->var->type != HNC_STRING)
                     errCode = ARG_NOT_STRING;
                  break;
               case 2:
               case 3:
               case 5:
               case 6:
                  if(arg->var->type != HNC_INT32)
                     errCode = ARG_NOT_LONG;
                  break;
               case 7:
                  if(arg->var->type != HNC_FLOAT)
                     errCode = ARG_NOT_FLOAT;
                  break;
               case 8:
               case 9:
               case 10:
                  if(arg->var->type != RECORD_TYPE)
                     errCode = ARG_NOT_RECORD;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      case FAL_DIST_PREV:
         modelVar->func = FalDistPrev;

         expectArgs=7;
         for(arg=modelVar->args; arg; arg=arg->next, argCt++)
         {  switch(argCt)
            {  case 1:
               case 2:
               case 4:
                  if(arg->var->type != HNC_INT32)
                     errCode = ARG_NOT_LONG;
                  break;
               case 3:
               case 5:
                  if(arg->var->type != HNC_STRING)
                     errCode = ARG_NOT_STRING;
                  break;
               case 6:
               case 7:
                  if(arg->var->type != RECORD_TYPE)
                     errCode = ARG_NOT_RECORD;
                  break;
               default:
                  errCode = TOO_MANY_ARGS;
            }

            if(errCode)
            {  badArg = argCt;
               break;
            }
         }

         if(argCt-1 < expectArgs && !errCode)
            errCode = TOO_FEW_ARGS;

         break;

      default:
         sprintf(errMsg, "Invalid function code '%d' used "
                         "for variable '%s' in '%s'", 
                         funcCode, modelVar->cfgName, fileName);
         break;
   }

   return(0);
}
