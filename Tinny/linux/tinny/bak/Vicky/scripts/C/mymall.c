/*
** $Id: mymall.c,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: mymall.c,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.2  1997/02/05  03:31:22  can
 * Added MPAGE options for a simple page-style memory
 * management scheme.  This is mainly for the mainframe, but
 * it can provide a slight boost to the overall performance.
 *
 * Revision 1.1  1997/01/16  22:44:06  can
 * Initial revision
 *
*/


/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HNC Rule Engine includes */
#include "mymall.h"

static long totBytes=0;


#ifdef MPAGE
static void* GetMem(int size, int type);
static void FreeMem(void *ptr);

static long totTemp=0, totMain=0, totAdjWaste;
static long totTPage=0, totMPage=0;
static tMemPage *mainMem=NULL;
static tMemPage *tempMem=NULL;

/* Memory types */
#define TMP_MEMORY   1
#define MAIN_MEMORY  2


void FreeMainMemory()
{
   tMemPage *page, *delPage;

   page = mainMem;
   while(page)
   {  delPage = page;
      page = page->next;

      free((void*)delPage->data);
      free((void*)delPage);
   }

   mainMem = NULL;
}

void FreeTempMemory()
{
   tMemPage *page, *delPage;

   page = tempMem;
   while(page)
   {  delPage = page;
      page = page->next;

      free((void*)delPage->data);
      free((void*)delPage);
   }

   tempMem = NULL;
}

void FreeAllPages()
{
   FreeTempMemory();
   FreeMainMemory();
}


static tMemPage* 
GetMemPage(int size, int type)
{
   tMemPage *newPage, *memPage=NULL;

   /* Allocate new page */
   if((newPage = (tMemPage*)malloc(sizeof(tMemPage)))==NULL)
      return(NULL);
   memset((char*)newPage, 0, sizeof(tMemPage));
 
   /* Allocate data area */
   if((newPage->data = (char*)malloc(size))==NULL)
      return(NULL);
   memset((char*)newPage->data, 0, sizeof(size));
   newPage->size = size;

   /* Link in page to proper list */
   if(type == MAIN_MEMORY)
   {  if(mainMem)
      {  for(memPage=mainMem; memPage->next; memPage=memPage->next);
         memPage->next = newPage;
      }
      else
         mainMem = newPage;
      totMPage++;
      totMain += size;
   }
   else
   {  if(tempMem)
      {  for(memPage=tempMem; memPage->next; memPage=memPage->next);
         memPage->next = newPage;
      }
      else
         tempMem = newPage;
      totTPage++;
      totTemp += size;
   }

   return(newPage);
}

void* GetMem(int size, int type)
{
   tMemPage *memPage;
   char     *dataPtr;
   int      adjust;

   if(type == MAIN_MEMORY) memPage = mainMem;
   else memPage = tempMem;

   /* If requested size if larger that the maximum page size */
   /* then allocate a new page of the requested size         */
   if(size > MEM_PAGE_SIZE)
   {  if((memPage = GetMemPage(size, type))==NULL)
         return(NULL);

      /* Update page parameters and return the data pointer */
      memPage->offset = size;
      memPage->avail = 0;

      dataPtr = memPage->data;
      memset(dataPtr, 0, size);
      return((void*)dataPtr);
   }

   /* If no pages have been allocated, create one */
   if(!memPage)
   {  if((memPage = GetMemPage(MEM_PAGE_SIZE, type))==NULL)
         return(NULL);

      /* Update page parameters and return the data pointer */
      memPage->offset = size;
      memPage->avail = memPage->size - size;

      dataPtr = memPage->data;
      memset(dataPtr, 0, size);
      return((void*)dataPtr);
   }


   /* Loop through the pages looking for available space */
   for(; memPage; memPage=memPage->next)
   {  /* Determine the adjustment for alignment to this page */
      adjust = 0;
      if(size==sizeof(short) && size<sizeof(long) && (memPage->offset%(sizeof(short)))!=0)
         adjust = sizeof(short) - memPage->offset%(sizeof(short));
      else if(size>=sizeof(long) && size<sizeof(double) && (memPage->offset%(sizeof(long)))!=0)
         adjust = sizeof(long) - memPage->offset%(sizeof(long));
      else if(size>=sizeof(double) && (memPage->offset%(sizeof(double)))!=0)
         adjust = sizeof(double) - memPage->offset%(sizeof(double));

      /* Check if space is available */
      if(memPage->avail >= (size + adjust))
      {  /* Update page parameters and return the data pointer */
         dataPtr = memPage->data + memPage->offset + adjust;
         memPage->avail -= (size + adjust);
         memPage->offset += size + adjust;
         totAdjWaste += adjust;

         memset(dataPtr, 0, size);
         return((void*)dataPtr);
      }
   }

   /* If here, then space not found so allocate a new page */
   if((memPage = GetMemPage(MEM_PAGE_SIZE, type))==NULL)
      return(NULL);

   /* Update page parameters and return the data pointer */
   memPage->offset = size;
   memPage->avail = memPage->size - size;

   dataPtr = memPage->data;
   memset(dataPtr, 0, size);
   return((void*)dataPtr);
}

void FreeMem(void *ptr)
{
   return;
}
#endif



void*
MyTmpCalloc(int numElems, int size)
{
#ifdef MPAGE 
   totBytes += size*numElems;
   return(GetMem(numElems*size, TMP_MEMORY));
#else
   totBytes += size*numElems;
   return(calloc(numElems, size));
#endif
}

void*
MyTmpRealloc(void *ptr, int size)
{
#ifdef MPAGE 
   void *newMem;

   newMem = GetMem(size, TMP_MEMORY);
   totBytes += size;

   memcpy(newMem, ptr, size);
   return(newMem);
#else
   totBytes += size;
   return(realloc(ptr, size));
#endif
}

void*
MyTmpMalloc(int size)
{
#ifdef MPAGE 
   totBytes += size;
   return(GetMem(size, TMP_MEMORY));
#else
   totBytes += size;
   return(malloc(size));
#endif
}



void*
MyCalloc(int numElems, int size)
{
#ifdef ITM
   int  i;
   void *start=NULL;

   totBytes += size*numElems;
   for(i=0; i<numElems; i++)
   {  if(i==0) start = ITM_MALLOC(size);
      else ITM_MALLOC(size);
   }
   return(start);
#else
#ifdef MPAGE 
   totBytes += size*numElems;
   return(GetMem(numElems*size, MAIN_MEMORY));
#else
   totBytes += size*numElems;
   return(calloc(numElems, size));
#endif
#endif
}

void*
MyRealloc(void *ptr, int size)
{
#ifdef ITM
   void *newMem;

   newMem = ITM_MALLOC(size);
   totBytes += size;

   memcpy(newMem, ptr, size);
   return(newMem);
#else
#ifdef MPAGE 
   void *newMem;

   newMem = GetMem(size, MAIN_MEMORY);
   totBytes += size;

   memcpy(newMem, ptr, size);
   return(newMem);
#else
   totBytes += size;
   return(realloc(ptr, size));
#endif
#endif
}

void*
MyMalloc(int size)
{
#ifdef ITM
   totBytes +=  size;
   return(ITM_MALLOC(size));
#else

#ifdef MPAGE 
   totBytes += size;
   return(GetMem(size, MAIN_MEMORY));
#else
   totBytes += size;
   return(malloc(size));
#endif
#endif
}




void MyFree(void *ptr)
{
#ifdef ITM
   ITM_FREE(ptr);
#else
#ifdef MPAGE
   FreeMem(ptr);
#else
   free(ptr);
#endif
#endif
}



static totLogCt=0, logCt=0;
void PrintLogCt()
{
   fprintf(stderr, "Number of unique pointers logged is %d\n", logCt);
   fprintf(stderr, "Number of requested pointers logged is %d\n", totLogCt);
}




#ifdef ITM

static char *addrLog=NULL;
static void *startAddr;
static long tabSize=sizeof(tLNGARR);

long ClearLog()
{
   if(addrLog) 
      memset(addrLog, 0, tabSize);
   return(0);
}

long LogAddress(void *ptr)
{
   long offset;

   if(!addrLog)
   {  addrLog = malloc(tabSize);
      memset(addrLog, 0, tabSize);
      startAddr = ITM_GETSTART();
   }

   offset = (long)ptr - (long)startAddr;
   if(offset > tabSize)
   {  fprintf(stderr, "Offset %ld is greater than log table size of %ld\n", offset, tabSize);
      exit(1);
   }

   if(!addrLog[offset])
   {  addrLog[offset] = 1;
      ITM_LOGADDR(ptr);
      logCt++;
   }
   
   totLogCt++;
   return(0);
}
#endif


void prntmall()
{
   fprintf(stderr, "\n\n"
                   "           Total bytes requested: %ld\n", totBytes);

#ifdef MPAGE
   fprintf(stderr, "Total bytes wasted for alignment: %ld\n", totAdjWaste);
   fprintf(stderr, "         Total main memory bytes: %ld\n", totMain);
   fprintf(stderr, "         Total main memory pages: %ld\n", totMPage);
   fprintf(stderr, "         Total temp memory bytes: %ld\n", totTemp);
   fprintf(stderr, "         Total temp memory pages: %ld\n", totTPage);
   fprintf(stderr, "                    Total memory: %ld\n", totMain+totTemp);
   fprintf(stderr, "             Total memory wasted: %ld\n", ((totMain+totTemp) - totBytes) + totAdjWaste);
#endif

#ifdef ITM
   PrintLogCt();
#endif
}
