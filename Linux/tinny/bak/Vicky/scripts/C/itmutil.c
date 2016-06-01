#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "itmutil.h"


void * ITM_OFFS_2_ABS(char *tabaddr)
{
long int * p1;
long int   Lint;
long int   Temr;

void * tabbegin;
tITM_PAGE    * pITM_PAGE;
tabbegin=(void*)tabaddr;
pITM_PAGE=(tITM_PAGE    *)tabbegin;


for (Lint=0; Lint < pITM_PAGE->curr_cell; Lint++)
    {
    Temr=        pITM_PAGE->dITM_INDEX.offs_cell[Lint];
    p1= (long int*) ((char *)pITM_PAGE + (long int)Temr);
    if(*p1 > pITM_PAGE->nextavail) continue;
    * p1 = (long int)  ((char *) *p1 +(long int)pITM_PAGE);
    }

pITM_PAGE->tablebegin=(char *)pITM_PAGE;
pITM_PAGE->nextavail=(char * )((long int)pITM_PAGE->nextavail+(unsigned long int)pITM_PAGE);
pITM_PAGE->pNet=
    (char *) ((long int)pITM_PAGE->pNet+(long int)pITM_PAGE->tablebegin);

return(pITM_PAGE);

}



void *   ITM_LOGADDR(void * pTrsl)
{
long int * c1;
long int * p1;
long int   Lint;
extern  tITM_PAGE * pGLBL_ITM_PAGE;
tITM_PAGE * pITM_PAGE;
pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;
pITM_PAGE->curr_cell++;

if(pITM_PAGE->curr_cell > INDEX_LIMIT)
  {
  pITM_PAGE->curr_cell--;
  printf("\n");
  printf("ITM_LOGADDR; Severe error; Attempt to log address exceeds index capacity\n");
  printf("limit is %ld \n", pITM_PAGE->curr_cell);
  exit (16);
  }


if (((long int)pTrsl > (long int)pITM_PAGE->nextavail)
                    ||
    ((long int)pTrsl <  (long int)pITM_PAGE->tablebegin))

    {
    pITM_PAGE->curr_cell--;
    printf("\n");
    printf("ITM_LOGADDR; Warnin1; Attempt to log address outside boundaries\n");
    printf("pITM_PAGE->tablebegin = %0lx , pTrsl is %0lx \n",
            pITM_PAGE->tablebegin,  pTrsl);
    return(NULL);
    }

/* gsz 960905 at request of Craig Nies */
c1= (long int *) pTrsl;


if(( * c1 > (long int)pITM_PAGE->nextavail)
                    ||
    ( * c1  <  (long int)pITM_PAGE->tablebegin))
    {
    pITM_PAGE->curr_cell--;

    if(*c1 != 0)
      {  
       fprintf(stderr, "\n");
       fprintf(stderr, "ITM_LOGADDR; Warnin2; Attempt to log address outside boundaries\n");
       fprintf(stderr, "pITM_PAGE->tablebegin = %0lx ,*pTrsl is %0lx \n",
                pITM_PAGE->tablebegin, *c1);

      }
      return(NULL);
    }

/* gsz 960905 at request of Craig Nies */

p1= (long int *)pTrsl;
Lint= (long int) p1;
Lint=Lint-(long int)pITM_PAGE->tablebegin;
pITM_PAGE->dITM_INDEX.offs_cell[pITM_PAGE->curr_cell-1]= (long int)  Lint;
return(NULL);
}



void * ITM_UTILIT(char *  fnc)

{

typedef struct
{
char fnccode[4];
long int addr;
long int lngt;
} tITM_INTERNAL;

tITM_INTERNAL *pITM_INTERNAL;
char buf[80];


void * p1;
long int temp;
extern  tITM_PAGE * pGLBL_ITM_PAGE;
static FILE *  pADDR_FILE=NULL;
static FILE *  pFREE_FILE=NULL;
tITM_PAGE * pITM_PAGE;

pITM_PAGE = (tITM_PAGE *) &dITM_PAGE;

if (memcmp(fnc, INQIR_FUNTION, sizeof(INQIR_FUNTION))==0)
   {
   return ((void *) pITM_PAGE);
   }


if (memcmp(fnc, SYNCP_FUNTION, sizeof(SYNCP_FUNTION))==0)
   {
   p1=ITM_MALLOC(32);
   return (p1);
   }


if (memcmp(fnc, RESET_FUNTION, sizeof(RESET_FUNTION))==0)
   {
   memset (pITM_PAGE, 0x00, sizeof(dITM_PAGE));
   pITM_PAGE->tablebegin = (char *)pITM_PAGE;
   pITM_PAGE->eyecatcher = MARKER_POINTER;
   pITM_PAGE->nextavail  =  (char *) &pITM_PAGE->dLNGARR;
   return ((void *) pITM_PAGE);
   }

#ifdef TMM
memset(buf,0x00,80);


if (memcmp(fnc,  LGADDR_FUNTION, sizeof(LGADDR_FUNTION))==0)
   
   pITM_INTERNAL  =  (tITM_INTERNAL *) fnc;
   if (pADDR_FILE==NULL)
      {
      pADDR_FILE=fopen(ADDR_FILE_NAME,  "wb");
      }
   sscanf(buf,
      "%0lx %0lx ",
      &pITM_INTERNAL->addr,
      &pITM_INTERNAL->lngt);
   fwrite((void *)buf, 1, 40, pADDR_FILE);
   return(NULL);
   }

if (memcmp(fnc,  FRADDR_FUNTION, sizeof(FRADDR_FUNTION))==0)
   {
   pITM_INTERNAL  =  (tITM_INTERNAL *) fnc;
   if (pFREE_FILE==NULL)
      {
      pFREE_FILE=fopen(FREE_FILE_NAME,  "wb");
      }
   sscanf(buf,
      "%0lx %0lx ",
      &pITM_INTERNAL->addr,
      &pITM_INTERNAL->lngt);
   fwrite((void *)buf, 1, 40, pFREE_FILE);
   return(NULL);
   }

if (memcmp(fnc,  CLOSFL_FUNTION, sizeof(CLOSFL_FUNTION))==0)
   {
   pITM_INTERNAL  =  (tITM_INTERNAL *) fnc;
   fclose(pFREE_FILE);
   fclose(pADDR_FILE);
   return(NULL);
   }


#endif

return (NULL);
}




#define SZSHORT  sizeof(short)
#define SZLONG  sizeof(long)
#define SZDBL  sizeof(double)

void * ITM_MALLOC(size_t  size)
{
char *t;
char ITM_UTIL_COMM[sizeof(LGADDR_FUNTION)+sizeof(long int) +sizeof(long int)];

static char *p1=NULL;
void  * p0;
tITM_PAGE    * pITM_PAGE;
pITM_PAGE  = (tITM_PAGE *)   &dITM_PAGE;

if (size==0)
     {
     return(NULL);
     }

if (p1==NULL)
  {
  memset (pITM_PAGE, 0x00, sizeof(dITM_PAGE));
  pITM_PAGE->tablebegin = (char *)          pITM_PAGE;
  pITM_PAGE->eyecatcher = MARKER_POINTER;
  pITM_PAGE->nextavail  =  (char *) &pITM_PAGE->dLNGARR;
  pGLBL_ITM_PAGE =   (tITM_PAGE *) pITM_PAGE;
  }


/* Make sure nextavail is properly aligned given the size */
if(size == SZSHORT && size < SZLONG && (((long)pITM_PAGE->nextavail)%SZSHORT)!=0)
   pITM_PAGE->nextavail += SZSHORT - ((long)pITM_PAGE->nextavail)%SZSHORT;
else if(size >= SZLONG && size < SZDBL && (((long)pITM_PAGE->nextavail)%SZLONG)!=0)
   pITM_PAGE->nextavail += SZLONG - ((long)pITM_PAGE->nextavail)%SZLONG;
else if(size >= SZDBL && (((long)pITM_PAGE->nextavail)%SZDBL)!=0)
   pITM_PAGE->nextavail += SZDBL - ((long)pITM_PAGE->nextavail)%SZDBL;

p1 = pITM_PAGE->nextavail;
pITM_PAGE->nextavail = pITM_PAGE->nextavail + size;

if (pITM_PAGE->nextavail >
    (char *) (pITM_PAGE->tablebegin + sizeof(dITM_PAGE)))
   {
   return(NULL);
   }

#ifdef LEV01
printf("returning memory at %0lx \n", p1);
#endif

memset(p1, 0x00, size);

/*
strcpy(ITM_UTIL_COMM, LGADDR_FUNTION);

*   (ITM_UTIL_COMM + sizeof(LGADDR_FUNTION)) = (long int)p1;
*   (ITM_UTIL_COMM + sizeof(LGADDR_FUNTION) + sizeof(LGADDR_FUNTION))
        = (long int)size;
p0=ITM_UTILIT(ITM_UTIL_COMM);
*/

return((void *)p1);


}








long int ITM_ABS_2_OFFS()



{
void     * p0;
long int * p1;
long int   Lint;
long int   Temp;

extern  tITM_PAGE * pGLBL_ITM_PAGE;
tITM_PAGE * pITM_PAGE;
pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;

#ifdef LEV01
printf("ABS_2_OFS, pGLBL_ITM_PAGE  is %0lx \n",
 pITM_PAGE);
#endif
p0= ITM_UTILIT(SYNCP_FUNTION);


for (Lint=0; Lint < pITM_PAGE->curr_cell; Lint++)
    {
    Temp=        pITM_PAGE->dITM_INDEX.offs_cell[Lint];
    p1= (long int*) ((char *)pITM_PAGE->tablebegin + (long int)Temp);
    if(*p1 < pITM_PAGE->tablebegin) continue;
    * p1 = (long int)  ((char *) *p1 -(long int)pITM_PAGE);
    }



pITM_PAGE->nextavail=(char *)

((unsigned long int) pITM_PAGE->nextavail - (unsigned long int) pITM_PAGE->tablebegin);

pITM_PAGE->pNet= (char *)
    (unsigned long int)  pITM_PAGE->pNet -
    (unsigned long int) pITM_PAGE->tablebegin;
pITM_PAGE->tablebegin =  0;

#ifdef TMM
p0= ITM_UTILIT(CLOSFL_FUNTION);
#endif

return(0);
}





void * ITM_FREE(void *ptr)
/*
 Q1. Purpose of this function?
 A1. To imitate regular "free" call from application;

 Q2. Why do we need it?
 A2. Because really memory is preallocated inside the page, and does not have to be freed.

 Q3. How to use it?
  void * p1;
  p1= ITM_FREE(p1);

 Q6. What is returned from a function?
 A6. There is no return value from this function.
     But during the testing stage; if the request is invalid it will not return to the caller functi
on
     but  exit with condition code 4.
*/
{
void * p1;
void * p0;
char ITM_UTIL_COMM[sizeof(FRADDR_FUNTION)+sizeof(long int) +sizeof(long int)];

#ifdef TMM
   tITM_PAGE    * pITM_PAGE;
   p1= ITM_UTILIT(INQIR_FUNTION);
   pITM_PAGE = (tITM_PAGE    *)   p1;

   if ((char *) ptr == NULL)
      {
      return(NULL);
      }

   strcpy(ITM_UTIL_COMM, FRADDR_FUNTION);

   *   (ITM_UTIL_COMM + sizeof(FRADDR_FUNTION)) = (long int)p1;

   p0=ITM_UTILIT(ITM_UTIL_COMM);

   if (((char *) ptr > (char *)  pITM_PAGE->nextavail)
   ||
       ((char *) ptr <  (char *) pITM_PAGE))
       {
       printf("\n");
       printf
   ("ITM_FREE; Severe error; Attempt to release unallocated storage at %0lx \n", ptr);
      return(NULL);
      }
#endif



return(NULL);
}


long ITM_GET_PNET(void **pNet)
{
tITM_PAGE * pITM_PAGE;
pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;


   *pNet = pITM_PAGE->pNet;
   
   return(0);
}


long ITM_SET_PNET(void *pNet)
{
extern  tITM_PAGE * pGLBL_ITM_PAGE;
tITM_PAGE * pITM_PAGE;
pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;


   pITM_PAGE->pNet = pNet;
   
   return(0);
}


long ITM_COPY_AREA(void *ptr)
{
    extern  tITM_PAGE * pGLBL_ITM_PAGE;
    tITM_PAGE* pITM_PAGE = (tITM_PAGE*)ptr;

    memcpy(pITM_PAGE, pGLBL_ITM_PAGE, sizeof(tITM_PAGE));  

    pGLBL_ITM_PAGE =   (tITM_PAGE *) pITM_PAGE;

   return(0);
}


void* ITM_GETSTART()
{
   extern  tITM_PAGE * pGLBL_ITM_PAGE;
   tITM_PAGE * pITM_PAGE;
   pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;

   return((void*)&pGLBL_ITM_PAGE->dLNGARR);
}


char* ITM_GETSTRTPTR()
{
   extern  tITM_PAGE * pGLBL_ITM_PAGE;
   tITM_PAGE * pITM_PAGE;
   pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;

   return((char*)pGLBL_ITM_PAGE);
}


long ITM_SET_GLBL(void *ptr)
{
   pGLBL_ITM_PAGE = (tITM_PAGE*)ptr;
   return(0);
}


long ITM_GETTOTAL()
{
   extern  tITM_PAGE * pGLBL_ITM_PAGE;
   tITM_PAGE * pITM_PAGE;
   long size;

   pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;

   size = sizeof(tITM_PAGE);
   size -= sizeof(tLNGARR);
   size += (long)pITM_PAGE->nextavail - (long)(&pITM_PAGE->dLNGARR);
   return(size);
}


long ITM_FREEALL()
{
   extern  tITM_PAGE * pGLBL_ITM_PAGE;
   tITM_PAGE * pITM_PAGE;
   long size;

   pITM_PAGE = (tITM_PAGE *) pGLBL_ITM_PAGE;
   free((void*)pITM_PAGE);

   return(0);
}
