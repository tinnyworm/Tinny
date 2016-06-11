#ifndef HNC_ISAMDATA_H
#define HNC_ISAMDATA_H

#ifndef BATCH
#include "hncCtISAM.h"
#else
#define HNCISAMPREFIXLEN   20
typedef int pISAMConfig;
#endif

typedef struct sISAMData {
   char          name[HNCISAMPREFIXLEN+1];
   int           *retrieved;
   int           lock;
   int           modelOnly;
   pISAMConfig   config;
   char          *dataBuffer;

   struct sISAMData *next;
} tISAMData;


long GetISAMData(tISAMData *ISAM);
long SaveISAMData(tISAMData *ISAM);

#endif
