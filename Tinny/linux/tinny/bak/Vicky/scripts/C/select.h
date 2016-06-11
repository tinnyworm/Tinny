/*
 * $Id: select.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
 * $Author: jcb $
 * $Date: 1998/06/08 22:55:28 $
 * $Locker:  $
 * $Log: select.h,v $
 * Revision 1.1  1998/06/08 22:55:28  jcb
 * Initial revision
 *
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.4  1997/01/27  23:40:32  ojo
 * Added ZERO_LOSS to fraudTypes - ojo
 *
 * Revision 1.3  1996/10/25  18:46:21  jcb
 * remove del for unknown fraud type
 *
 * Revision 1.2  1996/08/05  20:05:12  kmh
 * *** empty log message ***
 *
 * Revision 1.7  1996/02/15  00:11:02  mjk
 * Added fields reflecting card type, card use, account age, and account
 * number prefix fields for a new, extended select file format.
 *
 */

#ifndef _RANPCT_
#define _RANPCT_

#include "hnctype.h"
#include "std.h"

#define MAX_ACCT_MATCH_STRS 10

#define ranPct() ((double)(rand()&0xFFFFL) / (double)0x7FFFL)

enum {
  APPFRAUD = 0,
  COUNTERFEIT,
  LOST,
  NONRECEIPT,
  OTHER_FRD,
  STOLEN,
  TAKEOVER,
  MAILPHONE,
  CONVCHECK,
  FRIENDLY,
  ZERO_LOSS,
  UNKNOWN_FRD,
  NUM_FRAUD_TYPES
};

/* This list should correspond directly with the above enumeration */
static char fraudCodes[NUM_FRAUD_TYPES] = {
  'A',
  'C',
  'L',
  'N',
  'O',
  'S',
  'T',
  'M',
  'K',
  'F',
  'Z',
  'U'
};

typedef struct {
   char acctPrefix[KEYLENMAX];
   int  prefixLength;
} tAcctMatch;

typedef BOOL tCharLookup[256];

typedef struct {
  char startDate[9];
  char endDate[9];
  char trainDate[9];  /* like "19941001" */
  long day90StartDate;
  long day90EndDate;
  long day90TrainDate;  
  double pctNoFrau;
  double pctFrau;
  int daysOfFraud;
  double GmtOffSet;
  unsigned seed;           /* optional */
  int NFAcctCut;   /* Threshold for sampling non-fraud accounts */
  BOOL fraudTypes[NUM_FRAUD_TYPES]; /* Which fraud types we should include */
  int recontactDays;  /* recontact waiting period in research */
  double sampleRate;  /* scale up factor applied to NF accts in research */
  int oldFrauds;      /* 1 or 0, whether to include frauds before traindate */
  int stopAtBlock;   /* 1 or 0, whether to stop the frauds on the block date */
  int stopAtPickup; /* 1 or 0, whether to stop the frauds on the first pickup */
  int numAcctStrs; /* number of account matching strings used. 0 => use all */
  tAcctMatch acctStrs[MAX_ACCT_MATCH_STRS]; /* account prefixes to match */
  int acctAge; /* for [+/-]n, only use accounts [older/younger] than n days */
  char cardTypeStr[20];
  tCharLookup cardType;
  char cardUseStr[20];
  tCharLookup cardUse;
} modelParms;

#endif
