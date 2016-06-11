#ifndef MY_MALL_H
#define MY_MALL_H

/*
** $Id: mymall.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
**
*/

#ifdef ITM
#include "itmutil.h"
#endif

#ifdef MPAGE
#define MEM_PAGE_SIZE  20 * 1024

typedef struct sMemPage {
   double dummy;
   char   *data;
   
   long   offset;
   long   avail;
   long   size;

   struct sMemPage *next;
} tMemPage;

void FreeAllPages();
void FreeTempMemory();
void FreeMainMemory();
#endif

void* MyCalloc(int numElems, int size);
void* MyMalloc(int size);
void* MyRealloc(void *ptr, int size);
void  MyFree(void *ptr);

void* MyTmpCalloc(int numElems, int size);
void* MyTmpMalloc(int size);
void* MyTmpRealloc(void *ptr, int size);

#ifdef ITM
long  ClearLog(void);
long  LogAddress(void *ptr);
void  PrintLogCt(void);
#endif

void  prntmall(void);

#endif
