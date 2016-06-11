/*========================================================================*/
/*                                                                        */
/*                               mfdrive.c                                */
/*                                                                        */
/*  DESCRIPTION: get all the transactions from files in the directory 
 *  specified with dataPath and feed them to CSCORE() for account
 *  scoring, and output to DMW network, or whatever.
 *
 *
 *  NOTES: FOR HNC INTERNAL USE ONLY!!
 *                                                                        */
/*                                                                        */
/*========================================================================*/
/*                                                                        */
/*
$Source: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/mfdrive.c,v $
 *
$Log: mfdrive.c,v $
Revision 1.8  1999/08/26 00:17:42  xxs
Help message for dolLt added.

Revision 1.7  1999/07/30 16:04:03  pcd
removed hardcoded acct length in hashValue, added NULL to end of acct in checkData

Revision 1.6  1999/07/29 22:55:55  jcb
allowed to read select file for scoring a research file

 * Revision 1.5  1999/02/19  00:04:05  jcb
 * can now read compressed API files
 *
 * Revision 1.4  1998/10/08  23:28:11  jcb
 * made short score file compressed, too.
 *
 * Revision 1.3  1998/10/08  22:52:44  jcb
 * made api output file compressed
 *
 * Revision 1.2  1998/06/19  17:49:47  jcb
 * corrected score file read-in format
 *
 * Revision 1.1  1998/06/08  22:55:28  jcb
 * Initial revision
 *
 * Revision 1.5  1998/06/08  22:50:59  jcb
 * added more fields to score file
 *
 * Revision 1.4  1998/04/22  21:09:27  jcb
 * changed score file write to be number rather than string for amount
 * also added check on value of amount
 *
 * Revision 1.3  1998/04/21  18:57:47  jcb
 * added scoreall, fraudsonly, and modified score file output
 *
 * Revision 1.2  1998/01/16  22:31:56  jcb
 * changed some account number lengths from 16 to KEYLENMAX
 *
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.11  1997/03/15  06:40:18  ojo
 * Fixed the bug which prevented reading multiple compressed file sets
 * Added some code from dkg which ensures acct nums are long enough to hash
 *
 * Revision 1.10  1997/01/29  18:18:56  jcb
 * comment out frequent flyer skip
 *
 * Revision 1.9  1997/01/28  01:03:56  ojo
 * Added counts and message for Scale File Stats per jcb
 * Added ZERO_LOSS to fraudTypes per dkg
 *
 * Revision 1.8  1997/01/17  21:50:20  ojo
 * Added ability to read compressed files using zcat.  Invoked with -u flag.
 * Added -L flag to read select file when processing research records with -E.
 * Modified code to exit with warning if the inputInfo or cardArray overflow.
 *
 * Revision 1.6  1996/11/05  18:13:53  jcb
 * minor tweak on card uses print
 *
 * Revision 1.5  1996/11/05  17:55:31  jcb
 * tweak format on print of card type included
 *
 * Revision 1.4  1996/10/25  18:45:13  jcb
 * add fixes to get rid of nulls and dels in prints, also add hook for skipping mileage cards
 *
 * Revision 1.3  1996/08/22  23:12:49  kmh
 * Changed MAX_INPATH from 15 to 100 due to crashes processing fdr sources.
 *
 * Revision 1.2  1996/08/05  20:04:27  kmh
 * *** empty log message ***
 *
 * Revision 1.35  1996/02/15  00:06:01  mjk
 * Added feature enhancements to the select file: fraud type handles multiple
 * entries and allows specific exclusion; also allow similar selection on
 * card type / card use, account age, and account number prefixes.
 * Also bug fix:  Toss accounts for which first fraud date > end date.
 * Previously we treated these as non-frauds, which while technically
 * justifiable, stood to affect the validity of the non-fraud sampling rate.
 *
 * Revision 1.34  1995/12/18  23:31:06  mjk
 * Explicitly set REAL_{AUTH|POST|CARD|FRAUD}_LENGTH variable in checkData
 * even if it agrees with std.h .  Not having done this caused problems with
 * multiple data sources if one source has a "correct" file length and another
 * doesn't.
 *
 * Revision 1.33  1995/08/15  23:31:22  mjk
 * Fixed so that when -N option (don't toss transactions with missing card
 * data) is used, cardholder-based transaction data is blanked out.  Previously,
 * transaction could be populated with garbage, or the last valid cardholder's
 * information.
 *
 * Revision 1.32  1995/08/10  16:53:13  can
 * Added check for error after INITPROF.  Also, moved profOut block to better
 * place so that current profiles are output as a new account is found.
 *
 * Revision 1.31  1995/08/03  21:41:44  mjk
 * Put call to processResearch() back in main() for when we're reading
 * research file (it had been eaten up in a previous modification).
 * Also fixed bug in isValidTransaction() which caused it to reject all
 * transactions if they were in read research mode.
 *
 * Revision 1.30  1995/06/05  22:59:51  mjk
 * More bug fixes involving lingering static variables and multiple data
 * sources.  Also got rid of an extraneous error message (when using -E, it
 * complained that 'frauds.dat' was missing, and quit).
 *
 * Revision 1.29  1995/05/31  01:49:52  mjk
 * Fixed bug when concatenating multiple data sources which caused the date
 * selection to always come from the first data source.
 *
 * Revision 1.28  1995/05/24  17:26:02  can
 * Fixed bug in getFraudInfo that made it toss the first transaction of a fraud acct.
 *
 * Revision 1.27  1995/05/19  22:19:07  mjk
 * Added code to allow for concatenation of multiple data sources for model
 * generation.  Additional data sources are specified with additional instances
 * of -i<input_path> followed by corresponding -l<select_file> on the command
 * line.
 *
 * Revision 1.26  1995/05/03  16:18:09  can
 * Modified checkDates to make sure block and pickup dates are
 * set before doing any comparisons.
 *
 * Revision 1.25  1995/05/01  22:07:40  can
 * Modified getDateTime so that a date string and a time string are passed in.
 * This allows the function to be more generic.
 *
 * Revision 1.24  1995/04/29  00:46:00  can
 * Added pickupDate to make sure postings don't go out if their
 * transaction date was before the first pickup date.
 *
 * Revision 1.23  1995/04/28  23:54:00  can
 * Modified checkDates so that it uses the score date for fraud date comparison
 * when modeling, and it uses the transaction date for fraud date comparison
 * when evaluating.
 *
 * Revision 1.22  1995/04/28  17:25:41  can
 * Modified checkDates so that if the date of first fraud is after the end date,
 * then the account is not labelled PRE_FRAUD, and is used.
 * Also, made configName 256 characters so that a full path can be used.
 *
 * Revision 1.21  1995/04/24  17:33:31  can
 * Added profile size to .gen file
 *
 * Revision 1.20  1995/04/21  23:07:21  can
 * Made sure that profile names out is an option for all.
 *
 * Revision 1.19  1995/04/04  23:49:08  can
 * Added option to score isOutput transactions.
 *
 * Revision 1.18  1995/03/28  20:06:06  can
 * Modified for new zap mode.
 *
 * Revision 1.17  1995/03/27  19:44:59  can
 * Toss all transaction that are not Merch or Cash
 *
 * Revision 1.16  1995/03/22  19:52:41  can
 * Added new functionality and updated for B1_20
 *
 *
 *                                                                        */
/*========================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include "hnctype.h"
#include "std.h"
#include "falcon.h"
#include "modout.h"
#include "select.h"
/* #include "tabstrc.h" */
#include "dates.h"
#include "flib.h"
#include "maxstrc.h"

#ifdef GARBAGE
#include "zap.h"
#endif

/* handling compressed file */
#define ZCAT  "/usr/local/bin/zcat"
#define GZIP  "/usr/local/bin/gzip"
 
/* Defines the maximum record size for any file */
#define MAX_REC_SIZE   300

/* Maximum different input sources that can be concatenated together. */
/* kmh aug 22, 1996: changed from 15 to 100 */
#define MAX_INPATHS 100

/* Codes for getAuth(), getPost(), readTrans(), and getNextTran() */
#define TRANS_FOUND        0   /* Have good transaction */
#define OUT_OF_TRANS       1   /* Signal EOF found for auths and posts */
#define ACCT_CHANGE        2   /* Signal that a new account found */
#define DATE_CHANGE        3   /* Signal that a new date was found */

/* Return codes for getFraudInfo */
#define NO_FRAUD           0   /* No fraud activity for this account */
#define AGE_ONLY           1   /* Dont create dat record, but add to profile */
#define TRANS_FRAUD        2   /* Fraud transaction before daysOfFraud days */
#define OLD_FRAUD          3   /* Fraud transaction after daysOfFraud days */
#define TOSS_IT            4   /* Throw this transaction away completely */
#define PRE_FRAUD          5   /* Non-fraud trans on fraud account.  No dat */

/* File types for checkData */
#define CARD_FILE          0
#define AUTH_FILE          1
#define POST_FILE          2
#define FRAUD_FILE         3
#define NONMON_FILE        4
#define PAY_FILE           5

/* Offsets into the research output file */
#define RES_ACCOUNT       0  
#define RES_AMOUNT       20 
#define RES_CUR_DATE     34  
#define RES_LOC_DATE     43  
#define RES_TIME         52  
#define RES_FRAUDQ       59   
#define RES_FRD_DATE     61
#define RES_BLCK_DATE    70
#define RES_FRD_TYPE     79
#define RES_APFLAG       81
#define RES_SCORE        83
#define RES_AUTH_RESULT  88
#define RES_TRAN_TYPE    90
#define RES_KEY_SWIPE    92
#define RES_SIC          94
#define RES_AVL_CRED     99 
#define RES_CREDIT_LINE 107 
#define RES_POSTCODE    116 
#define RES_MRCHPOSTCODE    127 
#define RES_CNTRY_CODE  138 
#define RES_MRCHID      142 


/* Locally used struct definition */
typedef struct SInFile
{
   char inPath[512];
   char selFile[128];
   char inAPIFile[128];
   int authOnly;
   int postOnly;
   int payExists;
   int nonMonExists;
   int authRecLen;
   int authNL;
   int postRecLen;
   int postNL;
   int cardRecLen;
   int cardNL;
   int fraudRecLen;
   int fraudNL;
   int payRecLen;
   int payNL;
   int nonMonRecLen;
   int nonMonNL;
} tInFile;

/* prototypes (PRIVATE) */
int   getNextTran(char* key, char** rawTranData);
int   getProfile(char*, char*);
int   getCardInfo(char*, char*);
int   getFraudInfo(char*);
void  printParams(FILE* fp, int brief, tInFile *aInputs, int numInputs);
unsigned long getDateTime(char* dateStr, char* timeStr);
long  timeToSecs(char*);
void  parseSelectionString(char *str, tCharLookup tab);
int   acctStrCmp(const void *s1, const void *s2);

extern void research(char *Account, int bFraudacct, float Amount, int bFraudq, int Score,long Av_cred, unsigned long dateTime, char result, int bPrintIt, FILE* resFp, unsigned long tranDateTime, char tranType, long day90FraudDate, long day90BlockDate);
extern void FraudTypePerformance(char fraudType, int bFraudq, int Score,
          int bPrintIt, FILE* resFp);
extern void AmountLostWithDelay(char *Account, float Amount, int bFraudq,
          int Score, long Av_cred, unsigned long dateTime, int bPrintIt, FILE* resFp);

extern long CreateCfgFiles();


/* To reduce model files */
extern long gReduce;


/* static globals */
static FILE*        pPostFile = NULL;
static FILE*        pAuthFile = NULL;
static FILE*        pCardFile = NULL;
static FILE*        pFraudFile = NULL;
static FILE*        pNonMonFile = NULL;
static FILE*        pPayFile = NULL;
static FILE*        apiFp = NULL;
static FILE*        recFp = NULL;
static FILE*        profFp = NULL;
static FILE*        fullProfFp = NULL;
static FILE*        scrDataFp = NULL;
static FILE*        inAPIFp = NULL;
static FILE*        inResFp = NULL;
static FILE*        resFp = NULL;
static FILE*        scoreFp = NULL;
static FILE*        profNameFp = NULL;
static FILE*        profTransFp = NULL;
static FILE*        scrTransFp = NULL;
static FILE*        cardOutFp = NULL;
static FILE*        postdatwfalscoreFp = NULL;
static FILE*        authdatwfalscoreFp = NULL;

static short        tranBad = FALSE;
static long         outLimit = -1L; 
static long         numTrans = -1L; 
static long         rawAuths = 0L;
static long         rawPosts = 0L;
static long         rawNonMons = 0L;
static long         rawPays = 0L;
static char         tranType = ISAUTH;
static BOOL         researchOn = FALSE;
static BOOL         apiOut = FALSE;
static BOOL         recOut = FALSE;
static BOOL         profOut = FALSE;
static BOOL         fullProfOut = FALSE;
static BOOL         cardOut = FALSE;
static BOOL         profNameOut = FALSE;
static BOOL         scrDataOut = FALSE;
static BOOL         datwfalscoreDataOut = FALSE;
static BOOL         agethreshon = FALSE;
static BOOL         ageauths = FALSE;
static BOOL         profTransOut = FALSE;
static BOOL         scrTransOut = FALSE;
static BOOL         scoreOut = FALSE;
static BOOL         testOnly = FALSE;
static BOOL         trainOnly = FALSE;
static BOOL         mastercardOnly = FALSE;
static BOOL         visaOnly = FALSE;
static BOOL         readAPIFile = FALSE;
static BOOL         readScoreFile = FALSE;
static BOOL         not = FALSE;
static BOOL         tossNoCards = TRUE;
static BOOL         acctSelectParmsSet = FALSE;
static int          testThreshold = 25;

static int          compFlag = 0;
static char         fraudTypeTable[256];
static char         tempFraudTypes[50];   /* To read in FRAUDTYPE selection */
static int          research_brief = 0;
static int          hitPickup = FALSE;
static long         pickupDate=0L;
static char         blckDateStr[9];
static char         configName[256];
int                 authOnly=0, postOnly=0;
static int          explain=1000;
static char         inPath[200];
static char         modelOutPath[200];
static char         inAPIFile[200];
static char         inResFile[200];
static char         *profile;
static long         researchTop=0;
static void         *boot;
static long         totTime=0;
static long         splitDate=0;
static long         outAll=0;

static int          CARD_NEWLINE = 1;
static int          AUTH_NEWLINE = 1;
static int          POST_NEWLINE = 1;
static int          FRAUD_NEWLINE = 1;
static int          NONMON_NEWLINE = 1;
static int          PAY_NEWLINE = 1;

static int          REAL_CARD_LENGTH = CARD_LENGTH;
static int          REAL_AUTH_LENGTH = AUTH_LENGTH;
static int          REAL_POST_LENGTH = POST_LENGTH;
static int          REAL_FRAUD_LENGTH = FRAUD_LENGTH;
static int          REAL_NONMON_LENGTH = NONMON_LENGTH;
static int          REAL_PAY_LENGTH = PAY_LENGTH;
 
/* To determine maximum score */
int MaxScore = 0;
/* end maximum score */

/* extern globals */
extern int   gProfileSize;

/* globals */
long  recCount;
long  totIter=0;
long  totTranslate=0;
BOOL  timeIt=FALSE;

BOOL  nullFiles = FALSE;
BOOL  scoreMod = FALSE;

BOOL  CashOnly = FALSE;
BOOL  KchkOnly = FALSE;
BOOL  KeyedOnly = FALSE;
BOOL  NotKeyedOnly = FALSE;
BOOL  MrchOnly = FALSE;
BOOL  NoCash = FALSE;
BOOL  NoKchk = FALSE;
BOOL  NoMrch = FALSE;

BOOL  NoNmons = FALSE;
BOOL  NoPayms = FALSE;
BOOL  NoPosts = FALSE;

float  AmountsGe = 0.;
float  AmountsLt = 0.;

BOOL  traceOn = FALSE;
BOOL  outputOn = FALSE;
BOOL  silent = FALSE;
BOOL  doScale=FALSE;

BOOL  isFraud=FALSE;
BOOL  isOutput=FALSE;
BOOL  fraudAcct = FALSE;

BOOL  FraudsOnly = FALSE;
BOOL  ScoreAll = FALSE;

BOOL  riskOut = FALSE;
BOOL  stopAtTranslate = FALSE;
BOOL  UNCOMPRESS = TRUE;

FILE  *riskFp = NULL;
long  unsclBufSize = 70000000;
BOOL  doModel = 0;

int   countFraud=0, countNonFraud=0;

char  acctNumStr[KEYLENMAX+1];
char  fraudDateStr[9];
char  *pAcctNum;
char  *pFraudDate;

char         fraudType;
FILE         *rejectFp;
FILE         *gOutFile=NULL;
FILE         *paramFp = NULL;
FILE         *gOutDepends = NULL;

modelParms   dataSelect;

int notscored;
int scorethresh;
char acctnum[KEYLENMAX];
int hadhighscore=0;

int agethresh;

/* added for new modout.c 070197 */
int extHashVal = 0;
int extTestThreshold = 0;
/* END for new modout.c 070197 */

/* used in research.c */
long days_of_fraud;

/* add for testing */
float fGlobalFalseZeroThreshold = 0.0;


#include "error.h"
void errExit(char * str, INT32 err)
{ fprintf(stderr,"%s",str);
  if(err>=0) fprintf(stderr,"\n%lx\n\n", err);
  exit(13);
}

/* ----------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------- */
int coprime[] = {11,13,17,19,23,31};
int hashValue(char* acctNum)
{
   int i, j, k, sum=0;

   for(i=KEYLENMAX-1; (!isalnum(acctNum[i]) && acctNum[i] != '-') && i >= 0; i--);

/* dkg: Check that there are enough digits in the account number */
   if(i<12)
   {
      fprintf(stderr,"Account number found of length only %d in hashValue!\n",i+1);
      return(-1);
   }
/* end hack */

   for(k=0, j=i-12; k<6; i--, j++, k++)
     sum += (((acctNum[j] - '0') * 10) + (acctNum[i] - '0')) * coprime[k];

   return(sum%100);
}




/* ----------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------- */
#define DATE_LENGTH  alen(AUTH_DATE)
int readTrans
(FILE* fp, int size, char* rawData, char* key, int readFirst, long *curDate)
{
  int  status;
  int  dateOffset;

  if (tranType == ISAUTH) 
     dateOffset = aoff(AUTH_DATE);
  else if (tranType == ISPOST) 
     dateOffset = poff(POST_POST_DATE);
  else if (tranType == ISNONMON) 
     dateOffset = noff(NONMON_TRAN_DATE);
  else 
     dateOffset = yoff(PAY_DATE);                   /* payment transaction */

  tranBad = FALSE;

  /* if file pointer is NULL, return OUT_OF_TRANS */
  if (fp == NULL)
    return(OUT_OF_TRANS);

  /* Read the data, if necessary */
  if (readFirst)
  { if (tranType == ISAUTH)
    { if (!fread(rawData, size + AUTH_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawAuths++;
    }
    else if (tranType == ISPOST)
    { if (!fread(rawData, size + POST_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawPosts++;
    }
    else if (tranType == ISNONMON)
       { if (!fread(rawData, size + NONMON_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawNonMons++;
    }
    else if (tranType == ISPAY)
       { if (!fread(rawData, size + PAY_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawPays++;
    }
  }

  status = memcmp(key, rawData, KEYLENMAX);

      /* mjk hack: if stream has "fallen behind" the key, catch up */
  while (status > 0)
  { if (tranType == ISAUTH)
    { if (!fread(rawData, size + AUTH_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawAuths++;
    }
    else if (tranType == ISPOST)
       { if (!fread(rawData, size + POST_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawPosts++;
    }
    else if (tranType == ISNONMON)
       { if (!fread(rawData, size + NONMON_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawNonMons++;
    }
    else if (tranType == ISPAY)
       { if (!fread(rawData, size + PAY_NEWLINE, 1, fp))
        return(OUT_OF_TRANS);
      else rawPays++;
    }

    status = memcmp(key, rawData, KEYLENMAX);
  }
 
  if (status < 0)
    return(ACCT_CHANGE);

  /* Check if the date has changed, if the dateStr is given */
  if (*curDate != MISSLNG)
  { long tranDate = xatoln(rawData+dateOffset, DATE_LENGTH);

    if (tranDate == MISSLNG)
    { tranBad = TRUE;
      return(TRANS_FOUND);
    }

    if ((status = (int)(*curDate - tranDate)) < 0)
      return(DATE_CHANGE);
    else if (status > 0)
      tranBad = TRUE;
  }
  else
     *curDate = xatoln(rawData+dateOffset, DATE_LENGTH);

  return(TRANS_FOUND);
}

/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getAuth(char** rawData, char* key, long *curDate, int readAuth)
{
  static char rawAuth[MAX_REC_SIZE];

  short  authFound = FALSE;
  int    status;

  while(!authFound)
  { status=readTrans(pAuthFile,REAL_AUTH_LENGTH,rawAuth,key,readAuth,curDate);
    *rawData = rawAuth;

    switch(status)
    { case TRANS_FOUND:
        authFound = TRUE;
        break;

      case ACCT_CHANGE:
      case OUT_OF_TRANS:
      case DATE_CHANGE:
        return(status);
    } /* end switch */
  } /* end while */

  return(TRANS_FOUND);
}


/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getPost(char** rawData, char* key, long *curDate, int readPost)
{
  static char rawPost[MAX_REC_SIZE]; 
  static int  postFoundForAccount = FALSE;

  short  postFound = FALSE;
  int    status;

  while(!postFound)
  { status=readTrans(pPostFile,REAL_POST_LENGTH,rawPost,key,readPost,curDate);
    *rawData = rawPost;

    switch(status)
    { case TRANS_FOUND:
        postFound = TRUE;
        break;

      case ACCT_CHANGE:
      case OUT_OF_TRANS:
      case DATE_CHANGE:
        return(status);
    } /* end switch */
  } /* end while */

  return(TRANS_FOUND);
}

/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getNonMon(char** rawData, char* key, long *curDate, int readNonMon)
{
  static char rawNonMon[MAX_REC_SIZE]; 

  short  nonMonFound = FALSE;
  int    status;

  while(!nonMonFound)
  { status=readTrans(pNonMonFile,REAL_NONMON_LENGTH,rawNonMon,key,readNonMon,curDate);
    *rawData = rawNonMon;

    switch(status)
    { case TRANS_FOUND:
        nonMonFound = TRUE;
        break;

      case ACCT_CHANGE:
      case OUT_OF_TRANS:
      case DATE_CHANGE:
        return(status);
    } /* end switch */

  } /* end while */

  return(TRANS_FOUND);
}

/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getPay(char** rawData, char* key, long *curDate, int readPay)
{
  static char rawPay[MAX_REC_SIZE]; 
  static int  PayFoundForAccount = FALSE;

  short  payFound = FALSE;
  int    status;

  while(!payFound)
  { status=readTrans(pPayFile,REAL_PAY_LENGTH,rawPay,key,readPay,curDate);
    *rawData = rawPay;

    switch(status)
    { case TRANS_FOUND:
        payFound = TRUE;
        break;

      case ACCT_CHANGE:
      case OUT_OF_TRANS:
      case DATE_CHANGE:
        return(status);
    } /* end switch */
  } /* end while */

  return(TRANS_FOUND);
}

/* ----------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------- */
#include "conv.h"
void fillTranRec(char* tran, char* raw, char* crd)
{
  int        i;
  CONV_DATA  *cD;

  memset(tran, ' ', TRAN_LENGTH);
  
  cD = convTable;
  for(i=0; i<NUM_TRAN_FIELDS; i++, cD++)
  { switch(cD->type) 
    { case CARD_RECORD:
        memcpy(tran+off(cD->tranFld), crd+coff(cD->recFld1), clen(cD->recFld1));
        break;

      case AUTH_RECORD:
        if(tranType == ISAUTH)
        { if(cD->tranFld != TRAN_CURR_CONV) 
            memcpy(tran+off(cD->tranFld), 
                           raw+aoff(cD->recFld1),alen(cD->recFld1));
        }
        break;

      case POST_RECORD:
        if(tranType == ISPOST)
          memcpy(tran+off(cD->tranFld),raw+poff(cD->recFld2),plen(cD->recFld2));
        break;

      case NONMON_RECORD:
        if(tranType == ISNONMON)
          memcpy(tran+off(cD->tranFld),raw+noff(cD->recFld3),nlen(cD->recFld3));
        break;

      case PAY_RECORD:
        if (tranType == ISPAY)
          memcpy(tran+off(cD->tranFld),raw+yoff(cD->recFld4),ylen(cD->recFld4));
        break;

      case TRAN_DEPEND:

        if(cD->tranFld == AUTH_POST_FLAG)
        { *(tran+off(AUTH_POST_FLAG)) = tranType;
          break;
        }

        /* Currently, TRAN_DEPEND fields always have a relevant source field */
        /* in the auth and post transactions, but not always in the non-mon  */
        /* and payment transactions.  We therefore check non-mons and        */
        /* payments for the NOT_AVAIL flag, to make sure we aren't populating*/
        /* them with garbage.                                                */

        if(tranType == ISAUTH && cD->recFld1 != NOT_AVAIL)
          memcpy(tran+off(cD->tranFld),raw+aoff(cD->recFld1),alen(cD->recFld1));
        else if(tranType == ISPOST)
          memcpy(tran+off(cD->tranFld),raw+poff(cD->recFld2),plen(cD->recFld2));
        else if(tranType == ISNONMON && cD->recFld3 != NOT_AVAIL)
	{
          memcpy(tran+off(cD->tranFld),raw+noff(cD->recFld3),nlen(cD->recFld3));
	}
        else if(tranType == ISPAY && cD->recFld4 != NOT_AVAIL)
          memcpy(tran+off(cD->tranFld),raw+yoff(cD->recFld4),ylen(cD->recFld4));

        if(cD->tranFld == CREDIT_LINE)
        { /* if (!memcmp(tran+off(CREDIT_LINE), "          ", len(CREDIT_LINE))) 
            memcpy(tran+off(cD->tranFld), crd+coff(CARD_CRED_LINE), clen(CARD_CRED_LINE));
	  */
	  long bob;
          if ((bob=xatoln(tran+off(CREDIT_LINE),len(CREDIT_LINE)))==MISSLNG
              || bob == 0)
             memcpy(tran+off(cD->tranFld),crd+coff(CARD_CRED_LINE),clen(CARD_CRED_LINE));
	     
        }

        break;
    }
  }
}


/* ----------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------- */
void resetTranReader(int bResetCounts)
{
   getNextTran(NULL, NULL);
   getCardInfo(NULL, NULL);
   getFraudInfo(NULL);

   /*   rewind does not work on pipes.  added 6 ints to the inFile struct
	to eliminate the second call to checkData.  so, don't need the rewinds.
	   ojo  3/3/97
   if(pPostFile) rewind(pPostFile);
   if(pAuthFile) rewind(pAuthFile);
   if(pCardFile) rewind(pCardFile);
   if(pFraudFile) rewind(pFraudFile);
   if(pNonMonFile) rewind(pNonMonFile);
   if(pPayFile) rewind(pPayFile);
   */

   if (bResetCounts)
   {  recCount = 0L;
      rawAuths = 0L;
      rawPosts = 0L;
   }
}


/* ----------------------------------------------------------------------
 *   From Mike's version of the code 12/95 for nonmons/payments
 *
 * -------------------------------------------------------------------- */
int getNextTran(char* key, char** rawTranData)
{
  int          status;
  long  authDate, postDate, nonMonDate, payDate;
  static char  *rawData = NULL;
  static int   authOut=FALSE, postOut=FALSE, nonMonOut=FALSE, payOut=FALSE;
  static int   readAuth=TRUE, readPost=TRUE, readNonMon=TRUE, readPay=TRUE;
  static long  startDate = MISSLNG;
  static long  curDate = MISSLNG;
  static long  nextDate = MISSLNG;
  static char  nextAcct[KEYLENMAX+1];	/* Implicitely initialized to zero */
  /* for paym/nonm merge test - ojo */
  static BOOL  authFoundThisAcct = FALSE, postFoundThisAcct = FALSE;
  long checkDate;

  /***************************************/
  /* Reset all static flags if necessary */
  /***************************************/
  if(key==NULL && rawTranData==NULL)
  {   *rawData = NULL;
      authOut = postOut = nonMonOut = payOut = FALSE; 
      readAuth = readPost = readNonMon = readPay = TRUE;
      startDate = MISSLNG;
      curDate = MISSLNG;
      nextDate = MISSLNG;
      memset(nextAcct, '\0', sizeof(nextAcct));

      return(OK);
  }  

  /************************************************************************/
  /* Check first to see if data will be read from consortium format input */
  /* or from a flat file (research or API layout).  If it just a flat     */
  /* file, simply read the transaction and return.                        */ 
  /************************************************************************/
  if (readAPIFile || readScoreFile)
  {  static char tran[MAX_REC_SIZE];
     FILE* inStream;

     if (readAPIFile) inStream = inAPIFp;
     else inStream = inResFp;

     *rawTranData = tran;
     if (readAuth)
     {  if (fgets(tran, MAX_REC_SIZE, inStream) == NULL)
          return(OUT_OF_TRANS);
        rawAuths++;
     }


     if (memcmp(key, tran, KEYLENMAX))
     {  readAuth = FALSE;
        memcpy(key, tran, KEYLENMAX);
        return(ACCT_CHANGE);
     }

     memcpy(key, tran, KEYLENMAX);
     key[KEYLENMAX] = '\0';

     readAuth = TRUE;
     return(TRANS_FOUND);
  }


  /**************************************************/
  /* Set up startDate from the data select.txt file */
  /**************************************************/
  if (startDate == MISSLNG)
  {  startDate = atol(dataSelect.startDate);
     curDate = startDate;
  }

  status = MISSLNG;			/* Make sure we enter this loop... */
  while (status != TRANS_FOUND)
  { /**************************************************************************/
	 /* First, look for authorizations.  The process is basically driven by    */
	 /* the auth stream.  When the date or account changes, it goes to look for*/
	 /* postings for the same date and account.  If there are no such trans.   */
	 /* it returns a code and loops back again with the last date read as the  */
	 /* new date and/or the last account read as the new account               */
	 /**************************************************************************/
	  if (tranType == ISAUTH)
	  { /* Get transaction for the current account (key) & current Date        */
	    /* (curDate) readAuth is a flag telling whether a new record should    */
	    /* be read from the file, or the current data (statically held in      */
	    /* rawData) should be checked for account/date validity.  Status gives */
	    /* the results of the check.                                           */
		 status = getAuth(&rawData, key, &curDate, readAuth);
		 *rawTranData = rawData;

		 switch (status)
		 { case TRANS_FOUND:  /* account and record date match curDate and key */
			  /* Will be reading next auth */
			  readAuth = TRUE;
			  break;

			case OUT_OF_TRANS: /* End of auth file found */

			  authOut = TRUE;

                          /* If the postings, nonmons, and payments have also ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (postOut && nonMonOut && payOut)
                             return(OUT_OF_TRANS);

			  /* Postings are to be read next */
                          if (!postOut)
                            tranType = ISPOST;
			  else if (!nonMonOut)
                            tranType = ISNONMON;
                          else
                            tranType = ISPAY;
			  break;

			case DATE_CHANGE:  /* Date of auth is different from current date */
			  authDate = xatoln(rawData+aoff(AUTH_DATE), DATE_LENGTH);

			  /* Set next date if the next auth is sooner than any other future */
			  /* transaction that we know of.                                   */
			  if (nextDate == MISSLNG || authDate < nextDate)
			    nextDate = authDate;

			  /* Look at postings next */
			  if (!postOut)
			    tranType = ISPOST;
			  else if (!nonMonOut)
			    tranType = ISNONMON;
			  else
			    tranType = ISPAY;

			  /* Will not read the next auth, since we already have the data in */
			  /* rawData (actually, it is stored in rawAuth in getAuth) */
			  readAuth = FALSE;
			  break;

			case ACCT_CHANGE:   /* Current acct is different from key */
			  /* Set up the nextAcct data if the new account comes before any */
			  /* other new accounts we've seen.                               */
/*-------------------------------------------------------------------------------------------------*/
                          /**********************************************************************/
                          /* If we are using account prefix matching, we don't want to wait for */
                          /* this code to reject every single transaction whose account number  */
                          /* is higher than our highest prefix.  This kludge allows us to quit  */
                          /* early if appropriate.                                              */
                          /**********************************************************************/
                          if (dataSelect.numAcctStrs > 0)
                          {  int lastPref = dataSelect.numAcctStrs - 1;
                             if (memcmp(rawData, dataSelect.acctStrs[lastPref].acctPrefix,
                                                  dataSelect.acctStrs[lastPref].prefixLength) > 0)
                             {  /* May the gods of flow control have mercy on my wretched soul */
                                authOut = TRUE;
                                if (postOut && nonMonOut && payOut)
                                   return(OUT_OF_TRANS);
                                else
                                {  /* Whatever the OUT_OF_TRANS case does should be duped here */
                                   if (!postOut)
                                     tranType = ISPOST;
                                   else if (!nonMonOut)
                                     tranType = ISNONMON;
                                   else
                                     tranType = ISPAY;
			           readAuth = FALSE;
			           authFoundThisAcct = FALSE;
                                   break;
                                }
                             }
                          }
/*-------------------------------------------------------------------------------------------------*/
			  if (nextAcct[0] == '\0' || memcmp(rawData, nextAcct, KEYLENMAX) < 0)
			    memcpy(nextAcct, rawData, KEYLENMAX); 

			  /* Look at posts next */
                          if (!postOut)
                            tranType = ISPOST;
                          else if (!nonMonOut)
                            tranType = ISNONMON;
                          else
                            tranType = ISPAY;

			  /* Will not read the next auth, since we already have the data in */
			  /* rawData (actually, it is stored in rawAuth in getAuth) */
			  readAuth = FALSE;


			  /* used for Paym/Nonm merge test - ojo */
			  authFoundThisAcct = FALSE;

			  break;
		 } /* end switch */
	  } /* end if ISAUTH */

		
	  /***********************************************************************/
	  /* If the global tranType flag is ISPOST, that indicates that the auth */
	  /* stream has either run out or has hit a date or account change.  If  */
	  /* it has run out, then posting will be read until they run out.  If   */
	  /* the auth date or account changed, then postings will be read until  */
	  /* the posting account changes or the posting date == last auth date   */
	  /***********************************************************************/
	  if (tranType == ISPOST)
	  { /* Same general idea as above.  Read next posting, and use it if the */
		 /* date and account numbers match the current ones.  Otherwise,      */
		 /* continue on to non-monetary transactions.                         */
		 status = getPost(&rawData, key, &curDate, readPost);
		 *rawTranData = rawData;

		 switch (status)
		 { case TRANS_FOUND:   /* transaction matches on key and curDate */
			  /* Will be reading next posting */
			  readPost = TRUE;
			  break;

			case DATE_CHANGE:   /* the posting date > current date */
			  /* Read the posting date for date comparison */
			  postDate = xatoln(rawData+poff(POST_POST_DATE), DATE_LENGTH);

			  if (nextDate == MISSLNG || postDate < nextDate)
				 nextDate = postDate;

			  /* Want to move on to non-mons. */
                          if (!nonMonOut)
                            tranType = ISNONMON;
                          else
                            tranType = ISPAY;

			  /* Will not be reading next post (have data in rawData) */
			  readPost = FALSE;
			  break;

			case ACCT_CHANGE:  /* posting account is different from key */
/*-------------------------------------------------------------------------------------------------*/
                          /**********************************************************************/
                          /* If we are using account prefix matching, we don't want to wait for */
                          /* this code to reject every single transaction whose account number  */
                          /* is higher than our highest prefix.  This kludge allows us to quit  */
                          /* early if appropriate.                                              */
                          /**********************************************************************/
                          if (dataSelect.numAcctStrs > 0)
                          {  int lastPref = dataSelect.numAcctStrs - 1;
                             if (memcmp(rawData, dataSelect.acctStrs[lastPref].acctPrefix,
                                                  dataSelect.acctStrs[lastPref].prefixLength) > 0)
                             {  /* May the gods of flow control have mercy on my wretched soul */
                                postOut = TRUE;
                                if (authOut && nonMonOut && payOut )
                                   return(OUT_OF_TRANS);
                                else
                                {  /* Whatever the OUT_OF_TRANS case does should be duped here */
                                   if (!nonMonOut)
                                     tranType = ISNONMON;
                                   else
                                     tranType = ISPAY;
			           readPost = FALSE;
                                   break;
                                }
                             }
                          }
/*-------------------------------------------------------------------------------------------------*/
			  if (nextAcct[0] == '\0' || memcmp(rawData, nextAcct, KEYLENMAX) < 0)
				 memcpy(nextAcct, rawData, KEYLENMAX);

			  /* Check if there are other tran types remaining */
			  /* Will be moving on to non-mons next time through, so set */
			  /* global tranType is ISNONMON */
                          if (!nonMonOut)
                            tranType = ISNONMON;
                          else
                            tranType = ISPAY;

			  /* Will not be reading next post (have data in rawData) */
			  readPost = FALSE;

			  /* for paym/nonm merge test - ojo */
			  postFoundThisAcct = FALSE;

			  break;

			case OUT_OF_TRANS:  /* End of posting file found */

			  /* Set flag to indicate that the postings have run out */
			  postOut = TRUE;

                          /* If the auths, nonmons, and payments have also ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (authOut && nonMonOut && payOut )
                             return(OUT_OF_TRANS);

			  /* Other tran types may remain, so set up global tranType flag to */
			  /* signal that the non-mon code should be used next time through */
                          if (!nonMonOut)
			    tranType = ISNONMON;
                          else
			    tranType = ISPAY;
			  break;

		 } /* end switch */
	  } /* end if ISPOST */

	  if (tranType == ISNONMON)

	  { /* This is similar to the above two if statements, except that if we */
		 /* encounter a new account, we don't want to look at it if there are */
		 /* no auths or postings to the same account (since non-mons and pays */
		 /* are used for profiling only, there's no point in profiling an     */
		 /* account that will never be scored).                                */

		 status = getNonMon(&rawData, key, &curDate, readNonMon);
		 *rawTranData = rawData;

		 switch (status)
		 { case TRANS_FOUND:       /* Transaction matches on key and curDate. */
			  /* Will be reading next non-mon */
			  readNonMon = TRUE;
			  break;

		   case OUT_OF_TRANS: /* End of non-mon file found */

			  /* Set flag to signal that non-mons are done, but there are still, */
			  /* possibly, other tran types to read */
			  nonMonOut = TRUE;

                          /* If the auths, postings, and payments have also ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (authOut && postOut && payOut)
                             return(OUT_OF_TRANS);

			  /* Indicate that payments are to be read next */
			  tranType = ISPAY; 

			  break;

		   case DATE_CHANGE:  /* Date of non-mon is different from current date */
			  nonMonDate = xatoln(rawData+noff(NONMON_TRAN_DATE), DATE_LENGTH);
			  if (nextDate == MISSLNG || nonMonDate < nextDate)
				 nextDate = nonMonDate;

			  /* Do payments next */
			  tranType = ISPAY;

			  /* Will not read the next non-mon, since we already have the data */
			  /* in rawData (actually, it is stored in rawNonMon in getNonMon) */
			  readNonMon = FALSE;
			  break;

		   case ACCT_CHANGE:   /* Current acct is different from key */
			  /* Note that we do NOT update newAcct.  If we want to read the   */
			  /* next non-mon account then there will be at least one auth or  */
			  /* posting to that account.                                      */
/*-------------------------------------------------------------------------------------------------*/
                          /**********************************************************************/
                          /* If we are using account prefix matching, we don't want to wait for */
                          /* this code to reject every single transaction whose account number  */
                          /* is higher than our highest prefix.  This kludge allows us to quit  */
                          /* early if appropriate.                                              */
                          /**********************************************************************/
                          if (dataSelect.numAcctStrs > 0)
                          {  int lastPref = dataSelect.numAcctStrs - 1;
                             if (memcmp(rawData, dataSelect.acctStrs[lastPref].acctPrefix,
                                                  dataSelect.acctStrs[lastPref].prefixLength) > 0)
                             {  /* May the gods of flow control have mercy on my wretched soul */
                                nonMonOut = TRUE;
                                if (authOut && postOut && payOut)
                                   return(OUT_OF_TRANS);
                                else
                                {  /* Whatever the OUT_OF_TRANS case does should be duped here */
                                   tranType = ISPAY;
			           readAuth = FALSE;
                                   break;
                                }
                             }
                          }
/*-------------------------------------------------------------------------------------------------*/

                          /* If the auths, postings, and payments have ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (authOut && postOut && payOut)
                             return(OUT_OF_TRANS);

			  tranType = ISPAY; 
			  readNonMon = FALSE;

			  break;
		 } /* end switch */
	  }

	  if (tranType == ISPAY)
	  { /* Again we are reading transactions as above, and again, we don't  */
		 /* consider new accounts unless there is also an auth or posting to */
		 /* the same account.  However, here we update curDate and/or key if */
		 /* we have exhausted a day or an account.                           */
		 status = getPay(&rawData, key, &curDate, readPay);
		 *rawTranData = rawData;

		 switch (status)
		 { case TRANS_FOUND:   /* transaction matches on key and curDate */
			  /* Will be reading next payment */
			  readPay = TRUE;
			  break;

			case DATE_CHANGE:   /* the payment date > current date */
			  /* Read the posting date for date comparison */
			  payDate = xatoln(rawData+yoff(PAY_DATE), DATE_LENGTH);
			  if (payDate == MISSLNG || payDate < nextDate)
				 nextDate = payDate;

			  /* Want to move back on to auths */
			  tranType = ISAUTH;

			  /* Will not be reading next payment (have data in rawData) */
			  readPay = FALSE;

			  /* Update current date so that on next pass we can pick up correct */
			  /* transactions.                                                   */
			  curDate = nextDate;
			  nextDate = MISSLNG;
			  break;

			case ACCT_CHANGE:  /* payment account is different from key */
			  /* Again, don't update nextAccount; will skip until we get one  */
			  /* that has auths and/or postings.                              */
/*-------------------------------------------------------------------------------------------------*/
                          /**********************************************************************/
                          /* If we are using account prefix matching, we don't want to wait for */
                          /* this code to reject every single transaction whose account number  */
                          /* is higher than our highest prefix.  This kludge allows us to quit  */
                          /* early if appropriate.                                              */
                          /**********************************************************************/
                          if (dataSelect.numAcctStrs > 0)
                          {  int lastPref = dataSelect.numAcctStrs - 1;
                             if (memcmp(rawData, dataSelect.acctStrs[lastPref].acctPrefix,
                                                  dataSelect.acctStrs[lastPref].prefixLength) > 0)
                             {  /* May the gods of flow control have mercy on my wretched soul */
                                payOut = TRUE;
                                if (authOut && postOut && nonMonOut)
                                   return(OUT_OF_TRANS);
                                else
                                {  /* Whatever the OUT_OF_TRANS case does should be duped here */
			           tranType = ISAUTH;
			           if (nextDate != MISSLNG)
			           { curDate = nextDate;
				          nextDate = MISSLNG;
			           }
			           else
			           { memcpy(key, nextAcct, KEYLENMAX);
				          nextAcct[0] = '\0';
				          curDate = startDate;
				          /* nextDate stays equal to MISSLNG */
				          return(ACCT_CHANGE);
			           }
                                   break;
                                }
                             }
                          }
/*-------------------------------------------------------------------------------------------------*/
                          /* If the auths, postings, and nonmons have ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (authOut && postOut && nonMonOut)
                             return(OUT_OF_TRANS);

			  /* Go back to looking at auths on next call */
			  tranType = ISAUTH;

			  /* Will not be reading next payment (have data in rawData) */
			  readPay = FALSE;

			  /* if nextDate hasn't been set, that means we've run out of     */
			  /* transactions on the current account.  Move on to the next    */
			  /* account, resetting dates as well.                            */
			  if (nextDate == MISSLNG)
			  { if (nextAcct[0] == '\0' || memcmp(rawData,nextAcct,KEYLENMAX) < 0)
					memcpy(nextAcct, rawData, KEYLENMAX);

				 memcpy(key, nextAcct, KEYLENMAX);
				 nextAcct[0] = '\0';
				 curDate = startDate;
				 /* note that nextDate stays equal to MISSLNG. */
				 return(ACCT_CHANGE);
			  }
			  else
			  { /* Since nextDate has been set, we update the current date and */
				 /* make another pass through the while loop.                   */
				 curDate = nextDate;
				 nextDate = MISSLNG;
			  }
			  break;

			case OUT_OF_TRANS:  /* End of payment file found */

			  /* Set flag to indicate that the payments have run out */
			  payOut = TRUE;

                          /* If the auths, postings, and nonmons have also ended, return */
                          /* OUT_OF_TRANS to signal that all transactions have been read */
                          if (authOut && postOut && nonMonOut)
                             return(OUT_OF_TRANS);

			  /* Other tran types may remain, so set up global tranType flag to */
			  /* signal that the auth should be used next time through */
			  tranType = ISAUTH;

			  /* if nextDate has not been set, we have run out of days for the */
			  /* current account.  Update key.  Otherwise, we just update the  */
			  /* current date (curDate) and continue through the loop.         */
			  if (nextDate != MISSLNG)
			  { curDate = nextDate;
				 nextDate = MISSLNG;
			  }
			  else
			  { memcpy(key, nextAcct, KEYLENMAX);
				 nextAcct[0] = '\0';
				 curDate = startDate;
				 /* nextDate stays equal to MISSLNG */
				 return(ACCT_CHANGE);
			  }
			  break;
		 } /* end switch */
	  } /* end if ISPAY */
  }

  /* If here, then a valid transaction of type *tranType* was found */
  return(TRANS_FOUND);
} /* ............................................ endfun( getNextTran ) */


/* ----------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------- */
int isValidTransaction(char* acctNum, char* tran)
{
   int status;

   float amount;

   static int printit1=1, printit2=1, printit3=1, printit4=1, printit5=1;
   static int printit6=1, printit7=1, printit8=1, printit9=1, printit10=1;
   static int printit11=1, printit12=1;

   fraudAcct = FALSE;

   status = getFraudInfo(tran);

/**** if it is posted, it has been approved, need to set this to get losses for postings *****/
/****      jcb    7/30/97    *****************************************************************/
   if (tranType == ISPOST)
       *(tran+off(AUTH_RESULT)) = 'A';

/***************************************************************************/
/*    jcb  1/15/97   for High Dollar Trx   */
/*
 * if(*(tran+off(TRAN_AMOUNT)) < 1000.00 )
 * {
 *   status = AGE_ONLY;
 *   if (printit)
 *	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all amounts less than 1000 dollars\n\n");
 *   printit=0;
 * }
 */
/***************************************************************************/

/**********************************************/
/*  Add tran type selection                   */
/*    jcb  3/3/97                             */
/*                                            */
/* FDR sets Kchecks to type C and SIC = 0003  */
/*                  or type M and SIC = 0004  */
/*                                            */
/**********************************************/

 if (tranType == ISAUTH || tranType == ISPOST)
 {

/********************************************************************************************************/
/************** Change all FDR convenience checks to tran type K .....   jcb   8/13/97 ******************/
/********************************************************************************************************/

   if( !strncmp(tran+off(MERCH_CAT),"0003",4) || !strncmp(tran+off(MERCH_CAT),"0004",4) )
   {
     *(tran+off(TRAN_TYPE)) = 'K';
     if (printit10)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting tran type to %c for SIC = 0003 or 0004\n\n",*(tran+off(TRAN_TYPE)));
     printit10=0;
   }

/********************************************************************************************************/
/******* Change all convenience checks that are not SIC 0003 to SIC 0004  .....   jcb   8/13/97 *********/
/********************************************************************************************************/
   if( *(tran+off(TRAN_TYPE)) == 'K' && strncmp(tran+off(MERCH_CAT),"0003",4) )
   {
     strncpy(tran+off(MERCH_CAT),"0004",4);
     if (printit11)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting SIC code to %4.4s for K transactions that are not SIC = 0003\n\n",tran+off(MERCH_CAT));
     printit11=0;
   }

   if( CashOnly && (*(tran+off(TRAN_TYPE)) != 'C' || !strncmp(tran+off(MERCH_CAT),"0003",4)) )
   {
     status = AGE_ONLY;
     if (printit1)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all non-Cash transactions, tran type = %c\n\n",*(tran+off(TRAN_TYPE)));
     printit1=0;
   }
   else if ( KchkOnly && (*(tran+off(TRAN_TYPE)) != 'K' && (strncmp(tran+off(MERCH_CAT),"0003",4) && strncmp(tran+off(MERCH_CAT),"0004",4))) )
   {
     status = AGE_ONLY;
     if (printit2)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all non-Kcheck transactions, tran type = %c SIC = %4.4s\n\n",*(tran+off(TRAN_TYPE)),(tran+off(MERCH_CAT)));
     printit2=0;
   }
   else if ( MrchOnly && (*(tran+off(TRAN_TYPE)) != 'M' || !strncmp(tran+off(MERCH_CAT),"0004",4)) )
   {
     status = AGE_ONLY;
     if (printit3)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all non-Merchandise transactions, tran type = %c\n\n",*(tran+off(TRAN_TYPE)));
     printit3=0;
   }

   if ( NoCash && *(tran+off(TRAN_TYPE)) == 'C' && strncmp(tran+off(MERCH_CAT),"0003",4) )
   {
     status = TOSS_IT;
     if (printit4)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting TOSS_IT true for all Cash transactions, tran type = %c SIC = %4.4s\n\n",*(tran+off(TRAN_TYPE)),(tran+off(MERCH_CAT)));
     printit4=0;
   }

   if ( NoKchk && (*(tran+off(TRAN_TYPE)) == 'K' || !strncmp(tran+off(MERCH_CAT),"0003",4) || !strncmp(tran+off(MERCH_CAT),"0004",4)) )
   {
     status = TOSS_IT;
     if (printit5)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting TOSS_IT true for all Kcheck transactions, tran type = %c\n\n",*(tran+off(TRAN_TYPE)));
     printit5=0;
   }

   if ( NoMrch && *(tran+off(TRAN_TYPE)) == 'M' && strncmp(tran+off(MERCH_CAT),"0004",4) )
   {
     status = TOSS_IT;
     if (printit6)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting TOSS_IT true for all Merchandise transactions, tran type = %c\n\n",*(tran+off(TRAN_TYPE)));
     printit6=0;
/**********************     printit=0; */
   }

   if ( AmountsGe > 0. )
     if( amount = (float)xatofn((char*)tran+off(TRAN_AMOUNT), len(TRAN_AMOUNT)) < AmountsGe )
     {
       status = AGE_ONLY;
       if (printit7)
    	  fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all transactions with amounts < %f\n\n",AmountsGe);
       printit7=0;
     }

   if ( AmountsLt > 0. )
     if( amount = (float)xatofn((char*)tran+off(TRAN_AMOUNT), len(TRAN_AMOUNT)) >= AmountsLt )
     {
       status = AGE_ONLY;
       if (printit12)
    	  fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all transactions with amounts >= %f\n\n",AmountsLt);
       printit12=0;
     }

   if( KeyedOnly && *(tran+off(AUTH_KEYED_SWIPED)) != 'K' )
   {
     status = AGE_ONLY;
     if (printit8)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all non-keyed transactions\n\n");
     printit8=0;
   }
   else if( NotKeyedOnly && *(tran+off(AUTH_KEYED_SWIPED)) == 'K' )
   {
     status = AGE_ONLY;
     if (printit9)
  	fprintf(stderr,"\n\n  ---> NOTE: isValidTransaction is setting AGE_ONLY true for all keyed transactions\n\n");
     printit9=0;
   }

 }

/***************************************************************************/

/************************************************************
   if ( KchkOnly && *(tran+off(TRAN_TYPE)) == 'K' )
        *(tran+off(TRAN_TYPE)) = 'M';
**************************************************************/

/***************************************************************************/
/*
 *  if(*(tran+off(TRAN_TYPE)) == 'K' )
 *     *(tran+off(TRAN_TYPE)) == 'M';
 */
/***************************************************************************/

/*   end   */

   switch(status)
   {  case NO_FRAUD:
        if (FraudsOnly)
        {
          isFraud = FALSE;
          isOutput = FALSE;
          return(FALSE);
        }
        else
        {
          isFraud = FALSE;
          isOutput = TRUE;
          break;
        }
      case TRANS_FRAUD:
         fraudAcct = TRUE;
         isFraud = TRUE;
         isOutput = TRUE;
         break;
      case PRE_FRAUD:
        fraudAcct = TRUE;
        isFraud = FALSE;
        if (FraudsOnly || ScoreAll)
        {
          isOutput = TRUE;
        }
        else
        {
          isOutput = FALSE;
        }
        break;
      case AGE_ONLY:
         isFraud = FALSE;
         isOutput = FALSE;
         break;
      case TOSS_IT:
      case OLD_FRAUD:
         return(FALSE);
   }

   if (!fraudAcct) if (hashValue(acctNum) > dataSelect.NFAcctCut) return(FALSE);

   if (fraudAcct && isOutput && 
                  !dataSelect.fraudTypes[fraudTypeTable[fraudType]])
       return(FALSE);

   if (isFraud && dataSelect.stopAtPickup && hitPickup)
       return(FALSE);

   if (isFraud && dataSelect.stopAtBlock && hitPickup)
       return(FALSE);

   if (!readScoreFile)
   {  
      if (tranType == ISAUTH || tranType == ISPOST)
      {
        if(*(tran+off(TRAN_TYPE)) == 'M' || *(tran+off(TRAN_TYPE)) == 'C' ||
           *(tran+off(TRAN_TYPE)) == 'B' || *(tran+off(TRAN_TYPE)) == 'K')
           return(TRUE);
        else 
  	   return(FALSE);
      }
      else if (tranType == ISNONMON)
      {
        if(*(tran+off(TRAN_TYPE)) == 'A' || *(tran+off(TRAN_TYPE)) == 'C' ||
           *(tran+off(TRAN_TYPE)) == 'F' || *(tran+off(TRAN_TYPE)) == 'K' ||
           *(tran+off(TRAN_TYPE)) == 'L' || *(tran+off(TRAN_TYPE)) == 'M' ||
           *(tran+off(TRAN_TYPE)) == 'N' || *(tran+off(TRAN_TYPE)) == 'O' ||
           *(tran+off(TRAN_TYPE)) == 'P' || *(tran+off(TRAN_TYPE)) == 'Q' ||
           *(tran+off(TRAN_TYPE)) == 'R' || *(tran+off(TRAN_TYPE)) == 'S' ) 
           	return(TRUE);
        else 
  	   	return(FALSE);
      }
      else if (tranType == ISPAY)
      {
        if(*(tran+off(TRAN_TYPE)) == 'R' || *(tran+off(TRAN_TYPE)) == 'N' )
           	return(TRUE);
        else 
  	   	return(FALSE);
      }
   }
   else
   {  if (*(tran+RES_TRAN_TYPE) == 'M' || *(tran+RES_TRAN_TYPE) == 'C' ||
          *(tran+RES_TRAN_TYPE) == 'B' || *(tran+RES_TRAN_TYPE) == 'K')
         return(TRUE);
      else return(FALSE);
   }
}


#define HOUR_SECONDS  3600
#define MIN_SECONDS     60
#define DAY_SECONDS  86400L
long timeToSecs(char* timeStr)
{
  long hrs, mins, secs, retTime;

  if (timeStr)
  {  hrs = xatoin(timeStr, 2);
     mins = xatoin(timeStr+2, 2);
     secs = xatoin(timeStr+4, 2);
   
     retTime = hrs*HOUR_SECONDS + mins*MIN_SECONDS + secs;
  }
  else
    retTime = DAY_SECONDS - 1;

  return(retTime);
}

unsigned long getDateTime(char* dateStr, char* timeStr)
{
  unsigned long retDate;

  retDate = GetDay90(dateStr) * DAY_SECONDS;
  if(timeStr) retDate += timeToSecs(timeStr);
  else retDate += DAY_SECONDS - 1;

  return((unsigned long)retDate);
}

void writeResearch(int sig)
{
  if (!researchOn) return;

  if (sig == SIGUSR1)
    fprintf(stderr, "\n\nUpdating research file\n\n");

  fseek(resFp, researchTop, 0);

  research(NULL, 0, 0.0, 0, 0, 0L, 0L, ' ', 1+research_brief, resFp,0,0,0,0);
  if (!research_brief) 
  {  FraudTypePerformance(' ', 0, 0, (int)TRUE, resFp);
     AmountLostWithDelay(NULL, 0.0, 0, 0, 0L, 0L, (int)TRUE, resFp); 
  }

  fflush(resFp);
}


void cleanup(int sig)
{
   INT32 pError, newError;

   fprintf(stderr," -----> Job finished processing\n");

   if(timeIt)
   {  fprintf(stdout, "Total time in CSCORE %.2f secs\n", totTime/1000000.0);
      fprintf(stdout, "Total time in Iteration %.2f secs\n", totIter/1000000.0);
      fprintf(stdout, "Total time in Translation %.2f secs\n", totTranslate/1000000.0);
   }

   if (sig == SIGINT)
     fprintf(stderr, "\n\nProcessing stopped after %d records.  Have a nice day!!\n\n", recCount); 
  
   if(doModel)
   {  if((newError=CreateCfgFiles())!=0L)
         errExit("\nError: CreateCfgFiles failed", newError);
   }

   if(researchOn) 
      writeResearch(0);

   if (!readScoreFile)
   {
     free((void *)(profile));

     FREENET(boot, &pError);
     if(isFatal(pError))
        errExit("\nError: FREENET failed", pError);
   }


   if (rejectFp) fclose(rejectFp);
   if (pCardFile) fclose(pCardFile);
   if (pAuthFile) fclose(pAuthFile);
   if (pPostFile) fclose(pPostFile);
   if (pFraudFile) fclose(pFraudFile);
   if (inAPIFp) fclose(inAPIFp);
   if (inResFp) fclose(inAPIFp);
   if (resFp) fclose(resFp);

   if (apiFp) fclose(apiFp);
   if (recFp) fclose(recFp);
   if (profFp) fclose(profFp);
   if (scrDataFp) fclose(scrDataFp);

   if (paramFp) fclose(paramFp);
   exit(0);
} 


/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
void processResearch()
{
   char         acct[KEYLENMAX+1];
   INT32        newError, pError;
   int          status;
   char         *tranData;
   int          ct=0;

   /* Setup to allow for proper cleanup upon Ctrl-C termination */
   signal(SIGINT, cleanup);
   signal(SIGUSR1, writeResearch);
   fprintf(stderr, "\n<Hit Ctrl-C at any time to stop processing>\n\n");

   memset(acct, '\0', KEYLENMAX+1);

   while((status=getNextTran(acct, &tranData))!=OUT_OF_TRANS)
   {  if (outLimit == 0L) break;

      if (testOnly && hashValue(acct) >= testThreshold)
         continue;
      else if (trainOnly && hashValue(acct) < testThreshold)
         continue;

      if (mastercardOnly && acct[0] != '5') continue;
      if (visaOnly && acct[0] != '4') continue;

      if (status == ACCT_CHANGE)
      {  hitPickup = FALSE;
         pickupDate = 0L;
         acctSelectParmsSet = FALSE;
         continue;
      }

      if (*(tranData+off(AUTH_RESULT)) == PICKUP) 
      {  if(pickupDate == 0L)
            pickupDate = GetDay90(tranData+off(TRAN_DATE));
          hitPickup=TRUE;
      }

      if(isValidTransaction(acct, tranData)) 
      {  if (outLimit != -1 && (outLimit%5000) == 0 && outLimit != 0 && !silent)
            fprintf(stderr, "%8ld\t%8ld\n", outLimit, ct);
         else if (outLimit == -1 && (ct%5000)==0 && ct!=0 && !silent) 
            fprintf(stderr, "%8ld\n", ct);

	if ( tranType == ISAUTH )
	{
         ct++; recCount++;

/***************  jcb   4/4/97   Do not write research out on an interim basis *****/
/***************  if ((ct%100000)==0 && ct!=0 && researchOn) writeResearch(0); *****/

         if (outLimit != -1) outLimit--;
	}

         if (researchOn && isOutput)
         {  float amount;
            long  availCred, score;
            unsigned long  dateTime;
       
            amount = (float)xatofn(tranData+RES_AMOUNT, 13);
            availCred = xatoln(tranData+RES_AVL_CRED, 10);
            score = xatoin(tranData+RES_SCORE, 4);
  
            if(tranData[RES_TIME] == '.') 
               dateTime = getDateTime(tranData+RES_CUR_DATE, NULL); 
            else 
               dateTime = getDateTime(tranData+RES_CUR_DATE, tranData+RES_TIME); 

            research(acct, fraudAcct,
                      amount,
                      isFraud,
                      score,
                      availCred,
                      dateTime,
                      tranData[RES_AUTH_RESULT],
                      (int)FALSE, 
                      resFp, 0, 0,0,0);
    
            FraudTypePerformance(fraudType, 
                      isFraud, 
                      score,
                      (int)FALSE, 
                      resFp);
   
/********************************************************
            AmountLostWithDelay(acct,
                      amount,
                      isFraud,
                      score,
                      availCred,
                      dateTime, 
                      (int)FALSE,
                      resFp);
********************************************************/

         }  
      } /* ............................. endif( isValidTransaction ) */
   } /* .............................. endwhile((acct=getNextTran()) */
} 



char* expStrs[] = {
"TODW", "SICB", "RISK", "HIDA", "AMTS", "LOCA", 
"DECL", "AVEL", "PVEL", "TTYP", "MISC", "NOEXP", "XXXX"
};

int  printExpStr(int explainGrp)
{ 
   fprintf(stdout, "%s ", expStrs[explainGrp-1]);
   return 0;
}



/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
void processTransactions()
{
   long day90FraudDate = 0L;
   long day90BlockDate;
   static int printit = 0;
   static int lastPrint = 0;

   static int   firstCall = 1;
   static long  ct=0, tag=0;
   int          status, newAcct=FALSE, i, APIscored = FALSE;
   INT32        newError, pError;
   char         acct[KEYLENMAX+1], fileName[256], type;
   char         cardRec[MAX_REC_SIZE];
   char         tranRec[MAX_REC_SIZE];
   char         *rawTranData;
   SCORESTRUCT  oldScr, score, *scorePtr = &score;
   long         start, end;
   int          cutoff, hashVal;
   char         str_gmt_offset[15];

   if (firstCall)
   {
      if((rejectFp=fopen("rejects.dat", "w"))==NULL)
      {  fprintf(stderr,"\nError: cannot w-open rejects file\n");
         exit(13);
      }

      boot = NULL;
      INITNET(configName, &boot, &pError);
      if(isFatal(pError)) errExit("\nError: INITNET failed", pError);
      printf("SUCCESSFULLY INITIALIZED the NET....\n");
      /* set the GMT Offset for this particular data set */
      sprintf (str_gmt_offset, "%5.1lf", dataSelect.GmtOffSet);      
      setModelVar(boot, "GMT_OFFSET", str_gmt_offset, &pError);

      profile = (char*)malloc(sizeof(char)*gProfileSize);
      fprintf(stderr,"\nPROFILE SIZE: %5d  bytes, %5d  variables\n", gProfileSize, gProfileSize/4);
      if(paramFp) fprintf(paramFp,"\nPROFILE SIZE: %5d  bytes, %5d  variables\n", gProfileSize, gProfileSize/4);

      /* Setup to allow for proper cleanup upon Ctrl-C termination */
      signal(SIGINT, cleanup);
      signal(SIGUSR1, writeResearch);
      fprintf(stderr, "\n<Hit Ctrl-C at any time to stop processing>\n\n");
      firstCall = 0;
   } else {
      resetTranReader(0);
      /* set the GMT Offset for this particular data set */
      sprintf (str_gmt_offset, "%5.1lf", dataSelect.GmtOffSet);      
      setModelVar(boot, "GMT_OFFSET", str_gmt_offset, &pError);
   }

   memset(acct, '\0', KEYLENMAX+1);
   /* getNextTran() reads the auth and posting files and returns transaction */
   /* data via rawTranData, and the current account via the acct parameter.  */
   /* The transaction stream is sorted by scoring date (transaction date for */
   /* auths and posting date for postings).  In the event that there are     */
   /* multiple transactions in a day, all the auths will arrive in           */
   /* transaction time order, followed by the postings.  Non-zero return     */
   /* codes signal date changes, account changes, and EOF.  */
   while((status=getNextTran(acct, &rawTranData))!=OUT_OF_TRANS)
   {  
      if (outLimit == 0L) break;

      hashVal = hashValue(acct);
      if (testOnly && hashVal >= testThreshold) continue;
      else if (trainOnly && hashVal < testThreshold) continue;

      /***************************/
      /* for new modout.c        */
      /***************************/
      extHashVal = hashVal;
      extTestThreshold = testThreshold;
      /***************************/

      if (mastercardOnly && acct[0] != '5') continue;
      if (visaOnly && acct[0] != '4') continue;

      if (status == DATE_CHANGE)
         continue;

      if (status == ACCT_CHANGE)
      {  newAcct = TRUE;
         hitPickup = FALSE;
         pickupDate = 0L;

/*
         if (profOut && isOutput && !isFraud)
            PrintTemplateData(profile, "PROF", boot, 1);
*/

         acctSelectParmsSet = FALSE;
         continue;
      }

      if (!readAPIFile) {
         if (getCardInfo(cardRec, rawTranData)) {
	   if(tossNoCards) {
	     continue;
	   } else {
	     memset(cardRec, ' ', REAL_CARD_LENGTH);
	     memcpy(cardRec+coff(CARD_ACCT_ID), acct, KEYLENMAX);
	     memcpy(cardRec+coff(CARD_OPEN_DATE), "19800101", 8);
	   }
	 }

/*
 *   jcb: test for mileage portfolios    12/5/96
 *   == 'F' => only non-mileage accounts are processed
 *   != 'F' => only     mileage accounts are processed
 *   comment out to process all accounts..... THERE ARE TWO LOCATIONS OF THIS
 */
 
/*
 *    if(*(cardRec+coff(CARD_INCENTIVE)) == 'F')
 *    {
 *      if ( !printit )
 *      {
 *        printit = 1;
 *        fprintf(stderr,"\n\n -----> No Frequent Flyer cards (i.e., Mileage Cards) Processed\n\n");
 *      }
 *      continue;
 *    }
 */

/* jcb: end mileage test */

	 fillTranRec(tranRec, rawTranData, cardRec);
      }
      else 
      {  memcpy(tranRec, rawTranData, ((tMax*)boot)->tranLen);
      /* memcpy(tranRec, rawTranData, TRAN_LENGTH); */
         APIscored = sscanf(rawTranData + ((tMax*)boot)->tranLen + 1,
      /*		    std[NUM_TRAN_FIELDS].offset+std[NUM_TRAN_FIELDS].length, */
			    "%d %d %d %d", 
                                    &scorePtr->pFraudScore,
                                    &scorePtr->reasons[0],
                                    &scorePtr->reasons[1],
                                    &scorePtr->reasons[2]) == 4;
      }

      if (*(tranRec+off(AUTH_RESULT)) == PICKUP) 
      {  if(pickupDate == 0L)
            pickupDate = GetDay90(tranRec+off(TRAN_DATE));
          hitPickup=TRUE;
      }

      notscored=1;

      if(isValidTransaction(acct, tranRec)) 
      {  if (outLimit != -1 && (outLimit%1000) == 0 && outLimit != 0 && !silent && ct != lastPrint)
         {
            fprintf(stderr, "%8ld\t%8ld\n", outLimit, ct);
            lastPrint = outLimit;
         }
         else if (outLimit == -1 && (ct%5000)==0 && ct!=0 && !silent && ct != lastPrint)
         {
            fprintf(stderr, "%8ld\n", ct);
            lastPrint = ct;
         }
        /*added ISPOST since want to count postings too: gg 8/25/99*/
	if ( tranType == ISAUTH || tranType == ISPOST)
	{
         ct++; recCount++;

/***************  jcb   4/4/97   Do not write research out on an interim basis *****/
/***************  if ((ct%100000)==0 && ct!=0 && researchOn) writeResearch(0); *****/

         if (outLimit != -1) outLimit--;
	}

         /***************************/
         /* Copy global information */
         /***************************/
         memcpy(pAcctNum, acct, KEYLENMAX); 
         pAcctNum[KEYLENMAX] = '\0';

         pError=OK;

         if(newAcct)
         {  if(!getProfile(acct, profile))
            {  INITPROF(profile,
                         (BYTE*)tranRec,
                         boot,
                         &pError);
 
               if((pError != OK) && notWarning(pError))
               {  if(isFatal(pError)) errExit("\nError: score() failed", pError);
                  else if(isNoScore(pError))
                  {  int ch = *(tranRec+std[AUTH_POST_FLAG].offset);
   
                     if (outLimit != -1 && tranType == ISAUTH ) outLimit++;
                     recCount--;
                     
                     if (auth(tranRec)) fprintf(rejectFp,"%06ld:%06ld %c %x\n", 
                             recCount, rawAuths, ch, pError);
                     else fprintf(rejectFp,"%06ld:%06ld %c %x\n", 
                             recCount, rawPosts, ch, pError);
                     fflush(rejectFp);
                     continue;
                  }
               }
               else
                  newAcct = FALSE;
            }
         }

         if(timeIt) start = clock();

         if(isOutput || outAll) CSCORE((BYTE*)tranRec,profile,scorePtr,boot,explain,&pError); 
         else CSCORE((BYTE*)tranRec,profile,NULL,boot,explain,&pError);


         if(timeIt)
         {  end = clock();
            totTime += end-start;
         }

         if ((isOutput || outAll) && scoreOut)  
         {  fprintf(scoreFp, "%19.19s%8lx %4d %2d %2d %2d %-10.10s",
                                            tranRec,
                                            pError,
                                            scorePtr->pFraudScore,
                                            scorePtr->reasons[0],
                                            scorePtr->reasons[1],
                                            scorePtr->reasons[2],
                                            scorePtr->modelName);
               if (isFraud)
                 fprintf(scoreFp," 1\n");
               else if (fraudAcct)
                 fprintf(scoreFp," 2\n");
               else
                 fprintf(scoreFp," 0\n");

         }

         if((pError != OK) && notWarning(pError) )
         {  if(isFatal(pError)) errExit("\nError: score() failed", pError);
            else if(isNoScore(pError))
            {  int ch = *(tranRec+std[AUTH_POST_FLAG].offset);

               if (outLimit != -1 && tranType == ISAUTH ) outLimit++;
               recCount--;
               
               if (auth(tranRec)) fprintf(rejectFp,"%06ld:%06ld %c %x\n", 
                       recCount, rawAuths, ch, pError);
               else fprintf(rejectFp,"%06ld:%06ld %c %x\n", 
                       recCount, rawPosts, ch, pError);
               fflush(rejectFp);

               if (apiOut)
               { fwrite(tranRec, 1, ((tMax*)boot)->tranLen, apiFp);
		 /*  fwrite(tranRec, 1, TRAN_LENGTH, apiFp);  */
#ifndef NOITER
                  fwrite((char*)&pError, 1, sizeof(long), apiFp);
                  fprintf(apiFp, "NOSCOR");
#endif
                  fprintf(apiFp, "\n");
               }
            }
         } 
         else 
         {  
            if(recOut)
            {  if (isFraud) type = fraudType; 
               else type = ' ';

               fprintf(recFp, "%6.6s %9.9s %8.8s %8.8s %1.1s %8.8s %8.8s %5.5s %5.5s %8.8s %3.3s %8.8s %6.6s %8.2f %1.1s %1.1s %4.4s %9.9s %3.3s %1.1s %1.1s %1.1s %d %c\n", tranRec+10, 
                         tranRec+off(POSTCODE),
                         tranRec+off(ACCOUNT_OPEN_DATE),
                         tranRec+off(NEW_PLASTIC_DATE),
                         tranRec+off(PLASTIC_ISSUE_TYPE),
                         tranRec+off(ACCOUNT_EXPIRE_DATE),
                         tranRec+off(CARD_EXPIRE_DATE),
                         tranRec+off(AVAIL_CREDIT)+5,
                         tranRec+off(CREDIT_LINE)+5,
                         tranRec+off(BIRTHDATE),
                         tranRec+off(NUM_CARDS),
                         tranRec+off(TRAN_DATE),
                         tranRec+off(TRAN_TIME),
                         xatofn(tranRec+off(TRAN_AMOUNT), len(TRAN_AMOUNT)),
                         tranRec+off(AUTH_RESULT),
                         tranRec+off(TRAN_TYPE),
                         tranRec+off(MERCH_CAT),
                         tranRec+off(MERCH_POST_CODE),
                         tranRec+off(MERCH_CNTRY_CODE),
                         tranRec+off(AUTH_PIN_VERIFY),
                         tranRec+off(AUTH_CVV_VERIFY),
                         tranRec+off(AUTH_KEYED_SWIPED),
                         isFraud,
                         type);
            } /* end if(recOut) */ 

            if (researchOn && isOutput)
            {  float amount;
               long  availCred;
               unsigned long  dateTime, tranDateTime;
       
               amount = (float)xatofn(tranRec+off(TRAN_AMOUNT), len(TRAN_AMOUNT));
               availCred = xatoln(tranRec+off(AVAIL_CREDIT), len(AVAIL_CREDIT));
   
               if(auth(tranRec)) 
                  dateTime = getDateTime(tranRec+off(TRAN_DATE), tranRec+off(TRAN_TIME)); 
               else
               {  dateTime = getDateTime(tranRec+off(POST_DATE), NULL); 
                  tranDateTime = getDateTime(tranRec+off(TRAN_DATE), NULL); 
               }
 
/* To determine maximum score */
               if (scorePtr->pFraudScore > MaxScore)
                  MaxScore = scorePtr->pFraudScore;
/* End maximum score */

      	       day90FraudDate = (long)GetDay90(pFraudDate);
	       day90BlockDate = (long)GetDay90(blckDateStr);

               research(acct, fraudAcct,
                         amount,
                         isFraud,
                         scorePtr->pFraudScore,
                         availCred,
			 dateTime,  /*this is the posting date for posts*/
                         *(tranRec+off(AUTH_RESULT)),
                         (int)FALSE,
                         resFp,
			 tranDateTime,
			 tranType,
			 day90FraudDate,
			 day90BlockDate
			 );

               FraudTypePerformance(fraudType, 
                         isFraud, 
                         scorePtr->pFraudScore,
                         (int)FALSE,
                         resFp);
      
/********************************************************
               AmountLostWithDelay(acct,
                         amount,
                         isFraud,
                         scorePtr->pFraudScore,
                         availCred,
                         dateTime, 
                         (int)FALSE,
                         resFp);
********************************************************/
            }

            if (apiOut)
            {  fwrite(tranRec, 1, ((tMax*)boot)->tranLen, apiFp);
	    /* fwrite(tranRec, 1, TRAN_LENGTH, apiFp); */
#ifndef NOITER
               fprintf(apiFp, "%4d%2d%2d%2d", 
                                            scorePtr->pFraudScore,
                                            scorePtr->reasons[0],
                                            scorePtr->reasons[1],
                                            scorePtr->reasons[2]);
#endif
               fprintf(apiFp, "\n");
            }

            if (isOutput)
	        notscored=0;

            if (scrDataOut && (isOutput || outAll))
            {  double amount;
               long  availCred,creditLine;

               if ((amount=(double)xatofn((char*)tranRec+off(TRAN_AMOUNT),len(TRAN_AMOUNT))) == MISSDBL)
			amount=0.;
               if ((availCred=(long)xatoln((char*)tranRec+off(AVAIL_CREDIT),len(AVAIL_CREDIT))) == MISSLNG)
			availCred = 0;
               if ((creditLine=(long)xatoln((char*)tranRec+off(CREDIT_LINE),len(CREDIT_LINE))) == MISSLNG)
			creditLine = 0;

               fprintf(scrDataFp, "%19.19s ", acct);
               fprintf(scrDataFp, "%13.2f ", amount);

               if(post(tranRec))
               { fprintf(scrDataFp,"%8.8s ", tranRec+off(POST_DATE));
                 fprintf(scrDataFp,"%8.8s ", tranRec+off(TRAN_DATE));
                 fprintf(scrDataFp,".      ");
               }
               else
               { fprintf(scrDataFp,"%8.8s ", tranRec+off(TRAN_DATE));
                 fprintf(scrDataFp,".        ");
                 fprintf(scrDataFp,"%6.6s ", tranRec+off(TRAN_TIME));
               }

               if (isFraud)
               { fprintf(scrDataFp,"1 ");
                 fprintf(scrDataFp,"%8.8s ", pFraudDate);
                 fprintf(scrDataFp,"%8.8s ", blckDateStr);
                 fprintf(scrDataFp,"%c ", fraudType);
               }
               else if (fraudAcct)
               { fprintf(scrDataFp,"2 ");
                 fprintf(scrDataFp,"%8.8s ", pFraudDate);
                 fprintf(scrDataFp,"%8.8s ", blckDateStr);
                 fprintf(scrDataFp,"%c ", fraudType);
               }
               else
               { fprintf(scrDataFp,"0 ");
                 fprintf(scrDataFp, ".        ");
                 fprintf(scrDataFp, ".        ");
                 fprintf(scrDataFp,". ");
               }

               if(post(tranRec)) fprintf(scrDataFp,"0 ");
               else fprintf(scrDataFp,"1 ");

               fprintf(scrDataFp, "%4d ", scorePtr->pFraudScore);
               fprintf(scrDataFp, "%c ", *(tranRec+off(AUTH_RESULT)));
               fprintf(scrDataFp, "%c ", *(tranRec+off(TRAN_TYPE)));
               /* new field for key/swipe research */
               fprintf(scrDataFp, "%c ", *(tranRec+off(AUTH_KEYED_SWIPED)));

               fprintf(scrDataFp, "%4.4s ", tranRec+off(MERCH_CAT));
               fprintf(scrDataFp, "%7li ", availCred);
               fprintf(scrDataFp, "%7li  ", creditLine);
               fprintf(scrDataFp, "%9.9s  ", tranRec+off(POSTCODE));
               fprintf(scrDataFp, "%9.9s  ", tranRec+off(MERCH_POST_CODE));
               fprintf(scrDataFp, "%3.3s ", tranRec+off(MERCH_CNTRY_CODE));
               fprintf(scrDataFp, "%16.16s\n", tranRec+off(MERCH_ID));

               fflush(scrDataFp);
            }

         } /* .............................endif was error in CSCORE */
      } /* ............................. endif( isValidTransaction ) */

            if ((strncmp(rawTranData,acctnum,19)!=0) && datwfalscoreDataOut)
	    {
	      /*The account number has changed*/

	      fclose (postdatwfalscoreFp);
	      fclose (authdatwfalscoreFp);

              if (hadhighscore==1)
	      {
                /*Had a high score*/
                hadhighscore=0;
                system("cat auths_high.tmp >> auths_high.dat");
                system("cat posts_high.tmp >> posts_high.dat");
              }
              postdatwfalscoreFp = fopen("posts_high.tmp", "w");
              authdatwfalscoreFp = fopen("auths_high.tmp", "w");
	    }

	    if (notscored)
	    {
		/*the trans doesn't have a score, so set it to 0*/
		scorePtr->pFraudScore=0;
	    }

            if ((int)(scorePtr->pFraudScore)>scorethresh)
              hadhighscore=1;

            if (datwfalscoreDataOut)
            {
              /*these lines output new auths and posts files with the falcon score placed in the avail-cred field.  gg 8/28/98*/
               if(post(tranRec))
	       {
	          fprintf(postdatwfalscoreFp,"%70.70s      %4d          %68.68s\n",rawTranData,scorePtr->pFraudScore,rawTranData+90);
                  fflush(postdatwfalscoreFp);
	       }
               else
	       {
	          fprintf(authdatwfalscoreFp,"%64.64s      %4d %68.68s\n",rawTranData,scorePtr->pFraudScore,rawTranData+75);
                  fflush(authdatwfalscoreFp);
	       }
            }

            strncpy(acctnum,rawTranData,KEYLENMAX);

   } /* .............................. endwhile((acct=getNextTran()) */
   printf("Finished processing this data set after totaling %d records read and %d processed.\n",
     rawAuths, recCount);
} 


/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getProfile(char* key, char* profRec)
{
   return(0);
}


/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int checkDates(char* fraudDate, char *tran)
{
   float agetag;
   char  scoreDate[9], tranDate[9];
   long  day90FraudDate=0L, day90ScoreDate, day90BlockDate, day90TranDate;
   long  check_dates_days_of_fraud;

   if (!readScoreFile)
   { 
     if (!(post(tran))) 
     {
        memcpy(scoreDate, tran+off(TRAN_DATE), len(TRAN_DATE));
        memcpy(tranDate, scoreDate, 8);
     }
     else /* Tran is a posting */
     {
        memcpy(scoreDate, tran+off(POST_DATE), len(POST_DATE));
        memcpy(tranDate, tran+off(TRAN_DATE), len(TRAN_DATE));
     }
   }
   else
   {  
      memcpy(scoreDate, tran+RES_CUR_DATE, 8);
      if (tran[RES_APFLAG] == '0') memcpy(tranDate,  tran+RES_LOC_DATE, 8);
      else memcpy(tranDate, scoreDate, 8);
   }

   scoreDate[8] = '\0';
   tranDate[8] = '\0';

   if ((day90ScoreDate = (long)GetDay90(scoreDate)) == MISSLNG)
     return(TOSS_IT);

   if ((day90TranDate  = (long)GetDay90(tranDate)) == MISSLNG)
     day90TranDate = day90ScoreDate;
   

   if(fraudDate)
   {  
      day90FraudDate = (long)GetDay90(fraudDate);
      day90BlockDate = (long)GetDay90(blckDateStr);
      if(day90FraudDate>dataSelect.day90EndDate)
         return(TOSS_IT);
   }

   if(day90ScoreDate < dataSelect.day90StartDate) return(TOSS_IT);
   else if(day90ScoreDate < dataSelect.day90TrainDate) return(AGE_ONLY);
   else if(day90ScoreDate > dataSelect.day90EndDate) return(TOSS_IT);

   agetag =  xatofn(tran+off(AVAIL_CREDIT), len(AVAIL_CREDIT));

   if (fraudDate)
   {  
      if(dataSelect.oldFrauds == 0)
         if(day90FraudDate < dataSelect.day90TrainDate) return(TOSS_IT);
  
      if(dataSelect.stopAtBlock)
      {  
	if (!(post(tran)))
	{
           if(day90BlockDate>0 && day90ScoreDate>day90BlockDate)
               return(TOSS_IT);
	}
	else /*it's a post*/
	{
	if(day90BlockDate>0 && (day90TranDate >= day90BlockDate) && doScale)
	    return(TOSS_IT);
	else if (day90BlockDate>0 && (day90TranDate < day90FraudDate) && doScale)
	    return(AGE_ONLY);
	}
      }

      if(dataSelect.stopAtPickup && pickupDate>0 && day90ScoreDate>=pickupDate)
            return(TOSS_IT);

      if(day90ScoreDate < day90FraudDate && (day90TranDate < day90FraudDate || !(doScale) || !(post(tran)))) return(PRE_FRAUD);
      /*note: the above if part after the && is only needed to match the ifal code on postings (ifal scores posts with tran date in the frd window but post date outside the window (i.e. if the data has an error and score date is before tran date)) 19991206 gg*/

      days_of_fraud = day90ScoreDate-day90FraudDate+1;
      
      if (doScale && post(tran))
	check_dates_days_of_fraud = day90TranDate-day90FraudDate;
      else
	check_dates_days_of_fraud = day90ScoreDate-day90FraudDate;

      if(check_dates_days_of_fraud >= dataSelect.daysOfFraud)
	return(OLD_FRAUD);
      else if (tranType == ISNONMON || tranType == ISPAY || 0 > dataSelect.pctFrau || (agetag <= agethresh && agethreshon) || (auth(tran) && ageauths))
        return(AGE_ONLY);
      else 
	return(TRANS_FRAUD);
   }/*this matches the if fraudDate*/                                             else                                                                      
   {  
      if(tranType == ISNONMON || tranType == ISPAY || ranPct() > dataSelect.pctNoFrau || (agetag <= agethresh && agethreshon) || (auth(tran) && ageauths))
        return(AGE_ONLY);
      else
      {
	return(NO_FRAUD);
      }    
   }
}


int getFraudInfo(char* tran)
{
   static char* curFraudAcct = NULL;
   static char  fraud[MAX_REC_SIZE];
   static long  firstFraudDate=MISSLNG;
   static long  blockDate=MISSLNG;
   static int   readFraud = TRUE;
   static int   curAcctIndex = 0;
   static BOOL  tossThisAcct = FALSE;
   int  status;
   long tmpVal;

   /**********************************/
   /* Check if need to reset statics */
   /**********************************/
   if(tran==NULL)
   {  if (curFraudAcct != NULL)
      memset (curFraudAcct, (int)0, KEYLENMAX+1); 
      firstFraudDate=MISSLNG;
      blockDate=MISSLNG;
      readFraud = TRUE;
      tossThisAcct = FALSE;
      curAcctIndex = 0;
      return(OK);
   }

   /************************************************************************/
   /* Any selection parameters which are effectively account-based should  */
   /* be set here upon reading the first transaction of the account.       */
   /* Using the ACCTSTR, CARDTYPE, and/or CARDUSE params, we set the       */
   /* tossThisAcct flag.                                                   */
   /************************************************************************/
   if (!acctSelectParmsSet)
   {  tossThisAcct = FALSE;

      /* Account prefix matching */
      if (dataSelect.numAcctStrs > 0)
      {  while (curAcctIndex < dataSelect.numAcctStrs)
         { if ((status=memcmp(tran,dataSelect.acctStrs[curAcctIndex].acctPrefix,
                          dataSelect.acctStrs[curAcctIndex].prefixLength)) <= 0)
              break;
           else
              curAcctIndex++;
         }
         if (status != 0)
            tossThisAcct = TRUE;
      }

      /* Card type and card use selection */
      if (!dataSelect.cardType[tran[off(CARD_TYPE)]])
         tossThisAcct = TRUE;
      else if (!dataSelect.cardUse[tran[off(CARD_USE)]])
         tossThisAcct = TRUE;

      acctSelectParmsSet = TRUE;
   }

   if (tossThisAcct)
      return(TOSS_IT);

   /************************************************************************/
   /* Account age selection works as follows:  if the ACCTAGE parameter in */
   /* the select file is set to 0, no age-based account exclusion is done. */
   /* Otherwise, a negative number indicates that we should use only       */
   /* accounts that are younger in days than the magnitude of the number   */
   /* given, and a positive number indicates that we should use only       */
   /* accounts older than the number of days indicated.                    */
   /************************************************************************/
   if (dataSelect.acctAge != 0)
   {  long openDate, curDate, age;
      openDate = GetDay90(tran+off(ACCOUNT_OPEN_DATE));
      if (auth(tran))
         curDate = GetDay90(tran+off(TRAN_DATE));
      else if (post(tran))
         curDate = GetDay90(tran+off(POST_DATE));
      if (openDate == MISSLNG || curDate == MISSLNG)
      {  tossThisAcct = TRUE;
         return(TOSS_IT);
      }
      else
      {  age = curDate - openDate;
         if (dataSelect.acctAge < 0 && age > -dataSelect.acctAge)
         {  tossThisAcct = TRUE;    /* Toss this and all future transactions */
            return(TOSS_IT);
         }
         else if (dataSelect.acctAge > 0 && age < dataSelect.acctAge)
            return(AGE_ONLY);
      }
   }
   
   pFraudDate[0] = '\0';
   blckDateStr[0] = '\0';

   /* If reading research file, then get fraud info from it */
   if (readScoreFile)
   { memcpy(blckDateStr, tran+RES_BLCK_DATE, 8);  
     memcpy(pFraudDate, tran+RES_FRD_DATE, 8);  
     fraudType = tran[RES_FRD_TYPE];
     if (fraudType == '.') fraudType = ' ';

     if (tran[RES_FRD_DATE] == '.') return(checkDates(NULL, tran));
     else return(checkDates(pFraudDate, tran));
   }

   if (pFraudFile == NULL)
      return(checkDates(NULL, tran)); 

   if (curFraudAcct == NULL)
   {  curFraudAcct = (char*)malloc(KEYLENMAX+1);
      memset(curFraudAcct, '\0', KEYLENMAX+1);
   }
     
   status = memcmp(curFraudAcct, tran+off(ACCOUNT_NUMBER), len(ACCOUNT_NUMBER));

   if (status != 0)
      firstFraudDate = MISSLNG;

   while(status < 0)
   {  if (readFraud)
      {  if (!fread(fraud, REAL_FRAUD_LENGTH + FRAUD_NEWLINE, 1, pFraudFile))
            return(checkDates(NULL, tran)); 
      }
      else readFraud = TRUE;

      memcpy(curFraudAcct, fraud+foff(FRAUD_ACCT_ID), flen(FRAUD_ACCT_ID));
      status=memcmp(curFraudAcct,tran+off(ACCOUNT_NUMBER),len(ACCOUNT_NUMBER));
   }

   /******************************************************************************/
   /* Put readFraud = FALSE here because we have either read, or will be reading */
   /* the next fraud account from the file.                                      */
   /******************************************************************************/
   readFraud = FALSE;

   if (status > 0)
      return(checkDates(NULL, tran)); 

   if (firstFraudDate == MISSLNG)
   {  firstFraudDate=xatoln(fraud+foff(FRAUD_TRAN_DATE),flen(FRAUD_TRAN_DATE));
      blockDate=xatoln(fraud+foff(FRAUD_BLOCK_DATE),flen(FRAUD_BLOCK_DATE));
      /************************************************************************/
      /* If the first fraud date field is badly populated or missing in the   */
      /* data, it can cause a problem since the value MISSLNG is used to      */
      /* indicate that this is the first transaction on a fraud account.  To  */
      /* avoid this confusion, we set bad firstFraudDate values to EndDate+1, */
      /* which will cause them to be tossed by checkDates().                  */
      /************************************************************************/
      if (firstFraudDate == MISSLNG)
         firstFraudDate = dataSelect.day90EndDate + 1;

      fraudType = *(fraud+foff(FRAUD_TYPE));

      while(status == 0)
      {  if (!fread(fraud, REAL_FRAUD_LENGTH + FRAUD_NEWLINE, 1, pFraudFile))
         {  sprintf(pFraudDate, "%ld", firstFraudDate);
            if (blockDate == MISSLNG) sprintf(blckDateStr, "        ");
            else sprintf(blckDateStr, "%ld", blockDate);
            return(checkDates(pFraudDate, tran));
         }

         status = memcmp(fraud+foff(FRAUD_ACCT_ID), tran, len(ACCOUNT_NUMBER));
      }
   }

  /* dpp hack Feb. 19, 1996 */
  /* set first fraud date to open date if app fraud */
  /* FUSA denotes appfraud by the fraud code 'F' */
  /* if -N option is specified, use first fraud date */

   /* if ((fraudType==fraudCodes[APPFRAUD]) || (fraudType=='F')) 
        tmpVal = xatoln(tran+off(ACCOUNT_OPEN_DATE),len(ACCOUNT_OPEN_DATE));
        if (tmpVal!=MISSLNG || tossNoCards) firstFraudDate = tmpVal;
      } */

   sprintf(pFraudDate, "%ld", firstFraudDate);
   if (blockDate == MISSLNG) sprintf(blckDateStr, "        ");
   else sprintf(blckDateStr, "%ld", blockDate);
   return(checkDates(pFraudDate, tran));
}


/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
#define MAXCARDCUTS  50
int getCardInfo(char* cardRec, char* tran)
{
   static char  cardBuf[MAX_REC_SIZE], cardBufPrev[MAX_REC_SIZE];
   static char  cardArray[MAXCARDCUTS+1][MAX_REC_SIZE];
   static int   numCards = 0, usePrev = FALSE;

   int    i=0, j=0, status, newAccount = FALSE;
   int    tranMonth, tranYear, cardYear, cardMonth;
   long   tranDate, cardDate;
   char   cardIssType;

   if (pCardFile == NULL)
      return(-1);

   /**********************************/
   /* Check if need to reset statics */
   /**********************************/
   if(cardRec==NULL && tran==NULL)
   {  numCards = 0;
      usePrev = FALSE;
      return(OK);
   }

   if (numCards != 0) {
     status = memcmp(cardArray[0], tran, KEYLENMAX);

     if (status < 0) newAccount = TRUE;
      else if (status > 0) return(-1);
   } else {
     status = -1;
     newAccount = TRUE;
   }

   while(status < 0) {
       if (!usePrev) {
	   if (fread(cardBuf, REAL_CARD_LENGTH + CARD_NEWLINE, 1, pCardFile) != 1)
           return(-1);
       } else {
	   strcpy(cardBuf, cardBufPrev);
	   usePrev = FALSE;
       }
       status = memcmp(cardBuf, tran, KEYLENMAX);
   }

   if (status > 0) {
       strcpy(cardBufPrev, cardBuf);
       usePrev = TRUE;
       /* above two line replace backing up fileptr with fseek 
                   fseek(pCardFile, -(REAL_CARD_LENGTH+CARD_NEWLINE), 1); */
       return(-1);
   }

   if (newAccount)
   {  memcpy(cardArray[0], cardBuf, REAL_CARD_LENGTH);

      for(i=1; i<MAXCARDCUTS; i++)  {
	 if (!usePrev) {
	     if (fread(cardArray[i], REAL_CARD_LENGTH+CARD_NEWLINE, 1, pCardFile) != 1)
                 break;
	} else {
	     strcpy(cardArray[i], cardBufPrev);
	     usePrev = FALSE;
	}
         status = memcmp(cardArray[i], tran, KEYLENMAX);
        if (status != 0) {
	    strcpy(cardBufPrev, cardArray[i]);
	    usePrev = TRUE;
	    /* Roll back the extra card read it */
            /* fseek(pCardFile, -(REAL_CARD_LENGTH + CARD_NEWLINE), 1); */
            break;
         }
      }

      numCards = i;
      if (numCards > MAXCARDCUTS) {
	  errExit("\nAn account has more than 50 card records associated with it.\n Increase MAXCARDCUTS in mfdrive.c and recompile.\n", 201);
      }
      if(cardOut)
      {  fwrite((char*)&(cardArray[numCards-1]), 1, 150, cardOutFp);
         fprintf(cardOutFp, "\n");
      }
   }

   if (tranType==ISAUTH) 
      tranDate = xatoln(tran+aoff(AUTH_DATE), alen(AUTH_DATE));
   else if (tranType==ISPOST)
      tranDate = xatoln(tran+poff(POST_POST_DATE), plen(POST_POST_DATE));
   else if (tranType==ISNONMON)
      tranDate = xatoln(tran+noff(NONMON_TRAN_DATE), nlen(NONMON_TRAN_DATE));
   else       /* tranType==ISPAYMENT */
      tranDate = xatoln(tran+yoff(PAY_DATE), ylen(PAY_DATE));

   if (tranDate == MISSLNG)
      return(-1);

   tranYear = (int)(tranDate / 10000L);
   tranMonth = (int)(tranDate % 10000L) / 100;


   for(i=0; i<numCards; i++)
   {  cardDate = xatoln(cardArray[i]+coff(CARD_REC_DATE), clen(CARD_REC_DATE));
      cardYear = (int)(cardDate / 10000L);
      cardMonth = (int)(cardDate % 10000L) / 100;

/************** jcb   5/19/98  ....  Always use previous record ***************/
/***************************************  turned off ....
      if (cardYear == tranYear && cardMonth >= tranMonth)
      {  if(i>0) i--;
         break;
      }
      else if (cardYear > tranYear)
      {  if(i>0) i--;
         break;
      }
***************************************/
 
      if (cardMonth == tranMonth && cardYear == tranYear)
         break;
      else if (cardYear > tranYear)
      {  if(i>0) i--;
         break;
      }
      else if (cardYear == tranYear && cardMonth > tranMonth)
      {  if(i>0) i--;
         break;
      }
   }

   if (i==numCards)
   {  /*************************************************************/
      /* Hit end of card array with no correct card date           */
      /* Use the last account in the array (assume is most recent) */ 
      /*************************************************************/
      i--;
   }


   memcpy(cardRec, cardArray[i], REAL_CARD_LENGTH);

   cardDate = xatoln(cardRec+coff(CARD_ISS_DATE), clen(CARD_ISS_DATE));
   cardIssType = *(cardRec+coff(CARD_ISS_TYPE));

   if (cardDate > tranDate)
   {  for(j=i-1; j >= 0; j--) 
      {  cardDate=xatoln(cardArray[j]+coff(CARD_ISS_DATE), clen(CARD_ISS_DATE));
         cardIssType = *(cardArray[j]+coff(CARD_ISS_TYPE));
         if (cardDate < tranDate) break;
      } 

      if (cardDate != MISSLNG)
         sprintf(cardRec+coff(CARD_ISS_DATE), "%8.8ld", cardDate); 

      *(cardRec+coff(CARD_ISS_TYPE)) = cardIssType;
   }

   return(0);
}


/* ----------------------------------------------------------------------
 * parseSelectionString()
 *
 * Takes a string of the form "[!][{A-Z}...]" and populates tab such that
 * for a character ch, tab[ch] == TRUE if ch is in the set of characters
 * indicated by the string, and FALSE otherwise.
 * -------------------------------------------------------------------- */
void parseSelectionString(char *str, tCharLookup tab)
{
   BOOL notInStrVal;
   char *pch = str;

   /* if empty string, all characters are included */
   if (str[0] == '\0')
   {  memset(tab, TRUE, sizeof(tCharLookup));
      return;
   }
   if (str[0] == '!')
   {  notInStrVal = TRUE;
      pch++;
   }
   else notInStrVal = FALSE;

   memset(tab, notInStrVal, sizeof(tCharLookup));
   for (; *pch != '\0'; pch++)
      tab[*pch] = !notInStrVal;
}

/* ----------------------------------------------------------------------
 * This is a callback function that gets passed to qsort().  Performs a
 * simple account number string comparison (for possibly different length
 * strings).
 * -------------------------------------------------------------------- */
int acctStrCmp(const void *s1, const void *s2)
{
   tAcctMatch *p1 = (tAcctMatch *)s1, *p2 = (tAcctMatch *)s2;
   int minLen;

   minLen = (p1->prefixLength < p2->prefixLength) ?
               p1->prefixLength : p2->prefixLength;

   return memcmp(p1->acctPrefix, p2->acctPrefix, minLen);
}

/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
int getSelectParms(char* selFile)
{
  FILE*   modelFp;
  int     i, n;
  double  ranDouble;
  time_t  tloc, tsecs;
  char    buf[1024], tempstr[1024];
  char*   pFrdStr, *pch;

  if (selFile[0] != '\0')
  { if((modelFp=fopen(selFile,"r"))==NULL)
    { fprintf(stderr,"\nError: cannot open file %s\n\n", selFile);
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " STARTDATE= %s ", dataSelect.startDate)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data STARTDATE \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " ENDDATE= %s ", dataSelect.endDate)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data ENDDATE \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " TRAINDATE= %s ", dataSelect.trainDate)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data TRAINDATE \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " PCTFRAUD= %lf ", &dataSelect.pctFrau)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data PCTFRAUD \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " PCTNOFRAUD= %lf ", &dataSelect.pctNoFrau)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data PCTNOFRAUD \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " NUMFRAUDAYS= %d ", &dataSelect.daysOfFraud)!=1)
    { fprintf(stderr,"\nError: Cannot read model-data NUMFRAUDAYS \n\n");
      return(1);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " RANDOMSEED= %u ", &dataSelect.seed)!=1)
    { if((tsecs=time(&tloc)) == ((time_t)(-1))) dataSelect.seed = 1;
      else dataSelect.seed = (int)(tsecs&0xFFFF);
    }
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " NFACCTCUT= %d ", &dataSelect.NFAcctCut)!=1)
       dataSelect.NFAcctCut = 100;
 
      /* Parsing the fraud types */
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " FRAUDTYPE=%s", tempFraudTypes)!=1)
       memset(dataSelect.fraudTypes, TRUE, NUM_FRAUD_TYPES);
    else
    {  not = (tempFraudTypes[0] == '!');
       pFrdStr = tempFraudTypes + (not ? 1 : 0);
       memset(dataSelect.fraudTypes,not,NUM_FRAUD_TYPES);
       for (i = 0; pFrdStr[i] != '\0'; i++)
       { if (fraudTypeTable[pFrdStr[i]] != UNKNOWN_FRD)
            dataSelect.fraudTypes[fraudTypeTable[pFrdStr[i]]] = !not;
         /*
         for (iType = 0; iType < NUM_FRAUD_TYPES; iType++)
            if (pFrdStr[i] == fraudCodes[iType])
            { isKnownFrdType = TRUE;
              dataSelect.fraudTypes[iType] = !not;
              break;
            }
         if (!isKnownFrdType)
            badFrdType = TRUE;
         */
       }
    }

    /* Seed the random function */
    srand((unsigned int)dataSelect.seed);

    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " RECONTACT= %d ", &dataSelect.recontactDays)!=1)
         dataSelect.recontactDays = 30;
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " SAMPLERATE= %lf ", &dataSelect.sampleRate)!=1)
         dataSelect.sampleRate = .01;
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " OLDFRAUDS= %d ", &dataSelect.oldFrauds)!=1)
         dataSelect.oldFrauds = 0;
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " STOPATBLCK= %d ", &dataSelect.stopAtBlock)!=1)
         dataSelect.stopAtBlock = 0;
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " STOPATPCKUP= %d ", &dataSelect.stopAtPickup)!=1)
         dataSelect.stopAtPickup = 0;
 
    if (fgets(buf,1024,modelFp) == NULL
        || sscanf(buf, " ACCTSTRS= %s ", tempstr)!=1)
         dataSelect.numAcctStrs = 0;
    else
    {  /* Parse the comma-separated list of account prefixes */
       pch = strchr(buf, '=') + 1;
       (void) strtok(pch, ",\n");
       for (i=dataSelect.numAcctStrs=0;pch!=NULL&&i<MAX_ACCT_MATCH_STRS;i++)
       {  dataSelect.numAcctStrs++;
          dataSelect.acctStrs[i].prefixLength = strlen(pch);
          strncpy(dataSelect.acctStrs[i].acctPrefix, pch,
                  dataSelect.acctStrs[i].prefixLength);
          pch = strtok(NULL, ",\n");
       }
       qsort(dataSelect.acctStrs, dataSelect.numAcctStrs, sizeof(tAcctMatch),
             acctStrCmp);
    }
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " ACCTAGE= %d ", &dataSelect.acctAge)!=1)
       dataSelect.acctAge = 0;
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " CARDTYPE= %s ", dataSelect.cardTypeStr)!=1)
    {  memset(dataSelect.cardType, TRUE, sizeof(dataSelect.cardType));
       dataSelect.cardTypeStr[0] = '\0';
    }
    else
       parseSelectionString(dataSelect.cardTypeStr, dataSelect.cardType);
 
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " CARDUSE= %s ", dataSelect.cardUseStr)!=1)
    {  memset(dataSelect.cardUse, TRUE, sizeof(dataSelect.cardUse));
       dataSelect.cardUseStr[0] = '\0';
    }
    else
       parseSelectionString(dataSelect.cardUseStr, dataSelect.cardUse);

    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " GMTOFFSET= %lf ", &dataSelect.GmtOffSet)!=1)
    {  
 	   fprintf(stderr,"\n\n ---> NOTE: no GMTOFFSET in select file, must be set before continuing.\n"
	               "                 It should be the last selection in the file.\n"
                       "                 It will over-ride whatever is set in the .tab file.\n\n");
	   exit(13);
    }

/********************************************************************************************************/
/*******************  jcb   7/30/97    comment this out shouldn't have to be in each select file ********/
/********************************************************************************************************/

/*********************************************************************************************/
    /* KMH ADDED 7/24/1997: DIRTY HACK FOR CONTROLING THE THRESHOLD FOR FALSE ZEROS. */
    /* jcb modified 8/4/97   comment this out, since it is now a command line option */
/*********************************************************************************************
    if (fgets(buf,128,modelFp) == NULL
        || sscanf(buf, " FALSEZERO= %f ", &fGlobalFalseZeroThreshold)!=1)
       fGlobalFalseZeroThreshold = 0.0;
*********************************************************************************************/
    /* KMH END */
/**********************************************************************************************/


    fclose(modelFp);
  }
  else
  { fprintf(stderr, "No data selection file specified.  Using default data selection parameters\n");
    strcpy(dataSelect.startDate, "19800101"); 
    strcpy(dataSelect.endDate, "25000101");
    strcpy(dataSelect.trainDate, "19800101");
    dataSelect.pctFrau = 1;
    dataSelect.pctNoFrau = 1;
    dataSelect.daysOfFraud = 1000;
    dataSelect.NFAcctCut = 100;
    dataSelect.sampleRate = .01;
    memset(dataSelect.fraudTypes,TRUE,NUM_FRAUD_TYPES);
    dataSelect.recontactDays = 30;
    dataSelect.oldFrauds = 0;
    dataSelect.stopAtBlock = 0;
    dataSelect.stopAtPickup = 0;
    dataSelect.numAcctStrs = 0;
    dataSelect.acctAge = 0;
    dataSelect.cardTypeStr[0] = '\0';
    memset(dataSelect.cardType,TRUE,sizeof(dataSelect.cardType));
    dataSelect.cardUseStr[0] = '\0';
    memset(dataSelect.cardUse,TRUE,sizeof(dataSelect.cardUse));
  }

  if ((dataSelect.day90StartDate = (long)GetDay90(dataSelect.startDate)) == MISSLNG)
  { fprintf(stderr,"\nError: Invalid STARTDATE in %s\n\n", selFile);
    return(1);
  }
  if ((dataSelect.day90EndDate = (long)GetDay90(dataSelect.endDate)) == MISSLNG)
  { fprintf(stderr,"\nError: Invalid ENDDATE in %s\n\n", selFile);
    return(1);
  }
  if ((dataSelect.day90TrainDate = (long)GetDay90(dataSelect.trainDate)) == MISSLNG)
  { fprintf(stderr,"\nError: Invalid TRAINDATE in %s\n\n", selFile);
    return(1);
  }

  return(OK);
}



/* ----------------------------------------------------------------------
 *
 *
 *
 * -------------------------------------------------------------------- */
void argErr(void)
{
   fprintf(stderr,"\nUsage: drv_falcon\n\n");
   fprintf(stderr," REQUIRED PARAMETERS\n");
   fprintf(stderr,"   -i<in_path>     Path to input data files\n");
   fprintf(stderr,
      "   -m<config>      Specify the configuration file.  This file contains the\n"
      "                   names of all the models, the name of the template file,\n"
      "                   and the name of the control rule base file\n"); 

   fprintf(stderr,"\n INPUT OPTIONS\n");
   fprintf(stderr,
      "  [-E<res_input>]  Read data from a file containing tran date, score,\n"
      "                   amount, etc. in the format created via the -s option\n"
      "                   from this driver.  The transactions are NOT scored, but\n"
      "                   rather run through research, so a model is not required.\n");
   fprintf(stderr,
      "  [-A<api_file>]   Read data from a file containing api transactions, and,\n"
      "                   optionally, scores and reasons, instead of consortium\n"
      "                   format files.  Note: the input data directory is still\n"
      "                   required for the fraud information.\n");

   fprintf(stderr,"\n INPUT FILTER OPTIONS\n");
   fprintf(stderr,
      "  [-l<sel_file>]   Specify the data selection parameters file.  If no -l is\n"
      "                   specified, the default is to use all the data in the\n"
      "                   directory (or file)\n");
   fprintf(stderr,
      "  [-T<tst_thrsh>]  Specify the cutoff for the test data.  For example, if\n"
      "                   25%% of the accounts are for testing, 75%% for training,\n"
      "                   use -T25\n");
   fprintf(stderr,
      "  [-test]          Use only the test data.  If -T is not specified, then\n"
      "                   will use the default of 25%% of accounts.\n");
   fprintf(stderr,
      "  [-train]         Use only the training data.  If -T is not specified, then\n"
      "                   will use the default of 75%% of accounts.\n");
   fprintf(stderr,
      "  [-u]             The input .dat files are compressed as *.dat.gz  \n"
      "                   and must be uncompressed before reading\n");
   fprintf(stderr,"  [-n<num_recs>]   Number of records to process\n");
   fprintf(stderr,
      "  [-N]             Indicate the records without cardholder information\n"
      "                   should NOT be tossed.\n");

   fprintf(stderr,"\n MODEL GENERATION OPTIONS\n");
   fprintf(stderr,"  [-M]             Indicates that models are to be built using the .dat file format\n");
   fprintf(stderr,"  [-G]             Indicates that models are to be built, going directly to .scl files\n");
   fprintf(stderr,"  [-B]             Size of buffer to use (in megabytes) for holding unscaled\n"
	          "                   data.  This is only used with the -G option.\n");
   fprintf(stderr,"  [-nullfiles]     Use /dev/null for .scl,.hdr,.tag,TEMP files\n");

   fprintf(stderr,"\n SCORING OPTIONS\n");
   fprintf(stderr,"  [-e<explain>]    Specify the explanation threshold\n");
   fprintf(stderr,"  [-fraudsonly]    Only score fraud accounts, and include the pre-fraud transactions\n");
   fprintf(stderr,"  [-scoreall]      Score all accounts, and include the pre-fraud transactions as well\n");
   fprintf(stderr,
      "  [-O]             Indicate that all transactions are to be scored,\n"
      "                   including those transactions from the training period\n");

   fprintf(stderr,"\n OUTPUT OPTIONS\n");
   fprintf(stderr,
      "  [-o<data file>]  Create a file containing an ascii version of the model\n"
      "                   input vector.\n");
   fprintf(stderr,
      "  [-a<api_file>]   Create a file containing each api transaction along with\n"
      "                   the score and reason codes for that transaction, if any\n");
   fprintf(stderr,
      "  [-s<scr_file>]   Create a file containing scores, dollars, date, and fraud\n"
      "                   information.  The file can be used for input to the sas\n"
      "                   evaluation tools, or for input to this driver for\n"
      "                   re-running research only without scoring\n");
   fprintf(stderr,
      "  [-c<scores>]     Create a file containing account, scores, reasons codes,\n"
      "                   and the model ID\n");
   fprintf(stderr,
      "  [-r<res_file>]   Run the research routines, with output sent to res_file\n");
   fprintf(stderr,"  [-b]             Use brief form of research\n");
   fprintf(stderr,"  [-p<prof_out>]   Create a file containing ascii profile data.\n");
   fprintf(stderr,"  [-F<full_prof>]  Create a file containing profile information\n");
   fprintf(stderr,"                   on each transaction.\n");
   fprintf(stderr,
      "  [-C<card_out>]   Create a file containing ascii cardholder data.\n");
   fprintf(stderr,"  [-P<prof_names>] Create a file containing all the profile names\n");
   fprintf(stderr,"  [-R<risk_file>]  Specify the file containing risk data\n");
   fprintf(stderr,
      "  [-H]             Stop after translation.  This will only work with the\n");
   fprintf(stderr,"                   -R option, that is, when generating risk data.\n");
   fprintf(stderr,"  [-h<threshold>]  Create new data files (auths_high.dat and posts_high.dat)\n                   which contain transactions for accounts which have\n                   at least one score greater than <threshold>.  The Falcon\n                   score is placed in the available credit field.\n");

   fprintf(stderr,"\n MODEL DEVELOPMENT OPTIONS\n");
   fprintf(stderr,"  [-dolGe<amount>] Only use transactions with dollar amounts"
	          "                    ge amount for training and scoring\n");
   fprintf(stderr,"  [-dolLt<amount>] Only use transactions with dollar amounts"
	          "                    less-than amount for training and scoring\n");
   fprintf(stderr,"  [-CASH]          Only use Cash transactions for training\n"
	          "                    and scoring, but profile everything\n");
   fprintf(stderr,"  [-KCHK]          Only use Kcheck transactions for training\n"
	          "                    and scoring, but profile everything\n");
   fprintf(stderr,"  [-KEYED]         Only use keyed transactions for training\n"
	          "                    and scoring, but profile everything\n");
   fprintf(stderr,"  [-MRCH]          Only use Merchandise transactions for training\n"
	          "                    and scoring, but profile everything\n");
   fprintf(stderr,
    "  [-NOCASH]        Do Not use Cash transactions for training and scoring\n");
   fprintf(stderr,
    "  [-NOKCHK]        Do Not use Kcheck transactions for training and scoring\n");
   fprintf(stderr,
    "  [-NOMRCH]        Do Not use Merchandise transactions for training and scoring\n");
   fprintf(stderr,"  [-NOTKEYED]      Only use not-keyed transactions for training\n"
	          "                    and scoring, but profile everything\n");
   fprintf(stderr,"  [-NONMON]        Do Not even OPEN any nonMon files\n");
   fprintf(stderr,"  [-NOPMT]         Do Not even OPEN any payment files\n");
   fprintf(stderr,"  [-NOPOST]        Do Not even OPEN any posting files\n");
   fprintf(stderr,
    "  [-Z<float_limit>]   If float_limit != 0., do not include zero values\n"
    "                        in the means and std deviations\n"
    "                        if float_limit > 0. give false zeroes a scaled value of\n"
    "                           zero if the total number of records for a particular\n"
    "                           variable is less than the given threshold limit\n"
    "                        if float_limit < 0. leave false zeroes as they are\n");
   fprintf(stderr,"  [-j<threshold>]  Just score those transactions with a value greater \n                   than <threshold> in the available credit field.  Other\n                   transactions are aged (profiled).\n");
   fprintf(stderr,"  [-v]             Age all auths.\n");
   fprintf(stderr,"\n MISC OPTIONS\n");
   fprintf(stderr,"  [-S]             Run in silent mode\n");
   fprintf(stderr,"  [-t]             Run in trace mode\n");
   fprintf(stderr,"  [-I]             To time the process\n");
   fprintf(stderr,"  [-R]             To reduce the model files\n");
   fprintf(stderr,"  [-D<fname>]      To print listing of input var dependencies\n");

   fprintf(stderr,"\n");

   exit(13);
}

/* Utility function to check if files in .dat sets exist and are regular.
     ojo   1/2/97  */
int checkFile(char *fileName, int fileType, int dataSetNumb) {
  struct stat fileStat;
  mode_t fileMode;

  if (stat(fileName, &fileStat) == -1) {
    switch ( fileType ) {
     case AUTH_FILE:
      printf("\n -----> NO AUTHs file found in data set number %2d\n", dataSetNumb);
      break;
     case POST_FILE:
      printf("\n -----> NO POSTs file found in data set number %2d", dataSetNumb);
      break;
     case NONMON_FILE:
      printf("\n -----> NO NONMONs file found in data set number %2d\n", dataSetNumb);
      break;
     case PAY_FILE:
      printf("\n -----> NO PAYMENTs file found in data set number %2d\n", dataSetNumb);
      break;
    }
    switch (errno) {
     case EACCES:
      printf("\n  Check permissions on search path.\n");
      break;
     case ENOENT:
      printf("  If this bothers you, check the file names (esp. for '.gz' tail).\n");
      break;
     default:
      printf("  Run 'man stat' and read the ERRORS section for possible problems.\n\n");
    }
    return 0;
  }
  fileMode = fileStat.st_mode & S_IFMT;
  if ( (fileMode == S_IFREG) || (fileMode == S_IFLNK) ) {
    switch ( fileType ) {
     case AUTH_FILE:
      printf("\n -----> AUTHs file found in data set number %2d\n", dataSetNumb);
      break;
     case POST_FILE:
      printf("\n -----> POSTs file found in data set number %2d\n", dataSetNumb);
      break;
     case NONMON_FILE:
      printf("\n -----> NONMONs file found in data set number %2d\n", dataSetNumb);
      break;
     case PAY_FILE:
      printf("\n -----> PAYMENTs file found in data set number %2d\n", dataSetNumb);
      break;
    }
    return 1;
  } else {
    switch ( fileType ) {
     case AUTH_FILE:
      printf("\n -----> NO AUTHs file found in data set number %2d.\n", dataSetNumb);
      break;
     case POST_FILE:
      printf("\n -----> NO POSTs file found in data set number %2d.\n", dataSetNumb);
      break;
     case NONMON_FILE:
      printf("\n -----> NO NONMONs file found in data set number %2d.\n", dataSetNumb);
      break;
     case PAY_FILE:
      printf("\n -----> NO PAYMENTs file found in data set number %2d.\n", dataSetNumb);
      break;
    }
    printf("\n ----->  A file with same name exists, but is not a regular file or link.\n\n");
    return 0;
  }

}

#define DATE     1
#define AMOUNT   2
static int monthDays[12] = {
 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
int chkFld(char* fld, int type)
{
  int year, mon, day;
  int hour, min, sec;
  float amnt;

  switch(type)
  { case DATE:
      year = xatoin(fld, 4);
      mon = xatoin(fld+4, 2);
      day = xatoin(fld+6, 2);
/******* This isn't necessary ** Select file handles it *** jcb  3/13/98 ****
      if (year == MISSINT || year < 1970 || year > 2009) return(1);
*****************************************************************************/
      if (year == MISSINT ) return(1);
      if (mon == MISSINT || mon < 1 || mon > 12) return(1);
      if (day == MISSINT || day < 1) return(1);
      if (day > monthDays[mon-1]) return(1);
      break;
    case AMOUNT:
/********* This isn't necessary   jcb   3/13/98 ***********************
      if (*(fld+10) != '.') return(1);
***********************************************************************/
      amnt = xatofn(fld, 13);
      if (amnt == MISSFLT) return(1);
      break;
  }

  return(0);
}

int chkNewLine(FILE* fp, int fileType)
{
  int  i;
  char ch, lineHold[500];

  for(i=0; i<MAX_REC_SIZE; i++)
    if ((ch=(char)fgetc(fp)) == '\n') break;

  if (i==MAX_REC_SIZE) {
    fgets(lineHold, MAX_REC_SIZE, fp);
    /* rewind(fp); - rewind doesn't work for pipes.  Since fixed length record,
	don't really need to rewind anyway. Read to end of curr rec.  ojo 12/30/96 */
    return(-1);
  }

  if (fileType == CARD_FILE) REAL_CARD_LENGTH = i;
  else if (fileType == AUTH_FILE) REAL_AUTH_LENGTH = i;
  else if (fileType == POST_FILE) REAL_POST_LENGTH = i;
  else if (fileType == FRAUD_FILE) REAL_FRAUD_LENGTH = i;
  else if (fileType == NONMON_FILE) REAL_NONMON_LENGTH = i;
  else if (fileType == PAY_FILE) REAL_PAY_LENGTH = i;

  fgets(lineHold, MAX_REC_SIZE, fp);
  /* rewind(fp); - rewind doesn't work for pipes.  Since fixed length record,
	don't really need to rewind anyway. Read to end of curr rec.  ojo 12/30/96 */
  return(OK);
}

#define CHANGED       1
#define UNDETERMINED  2
int checkData(FILE* fp, int fileType, int recl)
{
  char recsBuf[500], lineHold[500];
  char acct[KEYLENMAX+1];
  int  status=0, chk=0;

  if (fp==NULL) return(OK);

  fread(recsBuf, recl, 2, fp); 
  fgets(lineHold, MAX_REC_SIZE, fp);
  /* rewind(fp); - rewind doesn't work for pipes.  Since fixed length record,
	don't really need to rewind anyway, just read not next newline. ojo 12/30/96 */

  if (recsBuf[recl] == '\n')
  { if (fileType == CARD_FILE) REAL_CARD_LENGTH = recl;
    else if (fileType == AUTH_FILE) REAL_AUTH_LENGTH = recl;
    else if (fileType == POST_FILE) REAL_POST_LENGTH = recl;
    else if (fileType == FRAUD_FILE) REAL_FRAUD_LENGTH = recl;
    else if (fileType == NONMON_FILE) REAL_NONMON_LENGTH = recl;
    else if (fileType == PAY_FILE) REAL_PAY_LENGTH = recl;
    return(OK);
  }

  if (chkNewLine(fp, fileType) == OK) return(CHANGED);

  memcpy(acct, recsBuf, KEYLENMAX);
  acct[KEYLENMAX] = '\0';

  switch(fileType)
  {  case CARD_FILE:
       CARD_NEWLINE = 0;
#define NUM_CARD_CHECKS 2
       status=NUM_CARD_CHECKS;
       while(status==NUM_CARD_CHECKS)
       {  status = 0;
          status += chkFld(recsBuf+recl+chk+coff(CARD_OPEN_DATE), DATE);
          if (memcmp(acct, recsBuf+recl+chk, 6)) status++;

          if (status==NUM_CARD_CHECKS)
          {  if(chk > -50 && chk <= 0) chk--; 
             else if(chk==-50) chk=1;
             else if(chk < 50) chk++;
             else break;
          }
       }

       if (status==NUM_CARD_CHECKS)
       {  fprintf(stderr, "Couldn't determine record length for cards.  "
                          "It isn't %d, as specified in std.h\n", recl);
          return(UNDETERMINED);
       }
       else 
       {  REAL_CARD_LENGTH = recl+chk;
          if (chk!=0) return(CHANGED);
          else return(OK);
       }
     case AUTH_FILE:
       AUTH_NEWLINE = 0;
#define NUM_AUTH_CHECKS  3
       status=NUM_AUTH_CHECKS;
       while(status==NUM_AUTH_CHECKS)
       {  status = 0;
          status += chkFld(recsBuf+recl+chk+aoff(AUTH_AMOUNT), AMOUNT);
          status += chkFld(recsBuf+recl+chk+aoff(AUTH_DATE), DATE);
          if (memcmp(acct, recsBuf+recl+chk, 6)) status++;

          if(status==NUM_AUTH_CHECKS)
          {  if(chk > -50 && chk <= 0) chk--; 
             else if(chk==-50) chk=1;
             else if(chk < 50) chk++;
             else break;
          }
       }

       if (status==NUM_AUTH_CHECKS)
       {  fprintf(stderr, "Couldn't determine record length for auths.  "
                          "It isn't %d, as specified in std.h\n", recl);
          return(UNDETERMINED);
       }
       else
       {  REAL_AUTH_LENGTH = recl+chk;
          if (chk!=0) return(CHANGED);
          else return(OK);
       }
     case POST_FILE:
       POST_NEWLINE = 0;
#define NUM_POST_CHECKS  3
       status=NUM_POST_CHECKS;
       while(status==NUM_POST_CHECKS)
       {  status = 0;
          status += chkFld(recsBuf+recl+chk+poff(POST_AMOUNT), AMOUNT);
          status += chkFld(recsBuf+recl+chk+poff(POST_POST_DATE), DATE);
          if (memcmp(acct, recsBuf+recl+chk, 6)) status++;

          if(status==NUM_POST_CHECKS)
          {  if(chk > -50 && chk <= 0) chk--; 
             else if(chk==-50) chk=1;
             else if(chk < 50) chk++;
             else break;
          }
       }

       if (status==NUM_POST_CHECKS)
       {  fprintf(stderr, "Couldn't determine record length for postings.  "
                          "It isn't %d, as specified in std.h\n", recl);
          return(UNDETERMINED);
       }
       else 
       {  REAL_POST_LENGTH = recl+chk;
          if (chk!=0) return(CHANGED);
          else return(OK);
       }
     case FRAUD_FILE:
       FRAUD_NEWLINE = 0;
#define NUM_FRAUD_CHECKS  3
       status=NUM_FRAUD_CHECKS;
       while(status==NUM_FRAUD_CHECKS)
       {  status=0;
          status += chkFld(recsBuf+recl+poff(FRAUD_TRAN_DATE), DATE);
          if (*(recsBuf+recl+foff(FRAUD_TYPE)) != 'A' &&
              *(recsBuf+recl+foff(FRAUD_TYPE)) != 'L' &&
              *(recsBuf+recl+foff(FRAUD_TYPE)) != 'S' &&
              *(recsBuf+recl+foff(FRAUD_TYPE)) != 'C' &&
              *(recsBuf+recl+foff(FRAUD_TYPE)) != 'O' &&
              *(recsBuf+recl+foff(FRAUD_TYPE)) != 'N') status++;
          if (memcmp(acct, recsBuf+recl+chk, 5)) status++;

          if(status==NUM_FRAUD_CHECKS)
          {  if(chk > -50 && chk <= 0) chk--; 
             else if(chk==-50) chk=1;
             else if(chk < 50) chk++;
             else break;
          }
       } 

       if (status==NUM_FRAUD_CHECKS)
       {  fprintf(stderr, "Couldn't determine record length for frauds.  "
                          "It isn't %d, as specified in std.h\n", recl);
          return(UNDETERMINED);
       }
       else 
       {  REAL_FRAUD_LENGTH = recl+chk;
          if (chk!=0) return(CHANGED);
          else return(OK);
       }
  }
}


void printParams(FILE* fp, int brief, tInFile *aInputs, int numInputs)
{
   char*   crdTypeStr, *datTypeStr; 
   int whichInput, i;
   BOOL badFrdPrinted;

   if (!brief)
   {  if(doModel)
         fprintf(fp, "             PURPOSE: To generate a DAT file for a new model\n");
      else
         fprintf(fp, "             PURPOSE: To run data through a model for analysis\n");
   }


   for (whichInput = 0; whichInput < numInputs; whichInput++)
   {
      if (numInputs > 1)
         fprintf(fp, "\nINPUT SOURCE #%d:  %s\n", whichInput+1, aInputs[whichInput].inPath);
      if (readAPIFile)
         fprintf(fp, "          INPUT DATA: %s (API format)\n", inAPIFile);  
      else if (readScoreFile)
         fprintf(fp, "          INPUT DATA: %s (Research format)\n", inResFile);
      else 
         fprintf(fp, "          INPUT DATA: %s (Consortium format)\n", aInputs[whichInput].inPath);

      if (!readScoreFile) fprintf(fp,"         BOOT CONFIG: %s\n",configName);

      if (NoNmons) {
          fprintf(fp, "         NonMon files ignored!\n");
      }
      if (NoPayms) {
          fprintf(fp, "         Payment files ignored!\n");
      }
      if (NoPosts) {
          fprintf(fp, "         Posting files ignored!\n");
      }

      (void) getSelectParms(aInputs[whichInput].selFile);
      fprintf(fp, "          START DATE: %s\n", dataSelect.startDate);
      fprintf(fp, "         ENDING DATE: %s\n", dataSelect.endDate);
      fprintf(fp, "       TRAINING DATE: %s\n", dataSelect.trainDate);
      fprintf(fp,"       %% FRAUD TRANS: %10.6f %%\n", dataSelect.pctFrau*100);
      fprintf(fp,"    %% NONFRAUD TRANS: %10.6f %%\n", dataSelect.pctNoFrau*100);
      fprintf(fp, "       DAYS OF FRAUD: %-4i\n", dataSelect.daysOfFraud);
      if (dataSelect.NFAcctCut < 100)
        fprintf(fp, "   %% NONFRAUD ACCTS: %4i\n", dataSelect.NFAcctCut);
      fprintf(fp, "     FRAUD TYPE USED:");
      for(i=0; i< NUM_FRAUD_TYPES; i++)
         if (dataSelect.fraudTypes[i])
            fprintf(fp, " %c ", fraudCodes[i]);
      fprintf(fp,"\n");
      fprintf(fp, " FRAUD TYPE EXCLUDED:");
      for(i=0; i< NUM_FRAUD_TYPES; i++)
         if (!dataSelect.fraudTypes[i])
            fprintf(fp, " %c ", fraudCodes[i]);
      fprintf(fp,"\n");
      for(i=0, badFrdPrinted = FALSE; tempFraudTypes[i] != '\0'; i++)
      {  if (isalpha(tempFraudTypes[i])
             && fraudTypeTable[tempFraudTypes[i]] == UNKNOWN_FRD)
         {  if (!badFrdPrinted)
            {  fprintf(fp, "  UNKNOWN FRAUD TYPE:");
               badFrdPrinted = TRUE;
            }
            fprintf(fp, " %c ", tempFraudTypes[i]);
         }
      }
      if (badFrdPrinted)
         fprintf(fp, "\n");
    

      fprintf(fp, "           RECONTACT: %d\n", dataSelect.recontactDays);
      fprintf(fp, "         SAMPLE RATE: %5.1f %%\n", 100.*dataSelect.sampleRate);
      fprintf(fp,"     USE OLD FRAUDS?: %s\n", dataSelect.oldFrauds?"Yes":"No");
      fprintf(fp, "      STOP AT BLOCK?: %s\n", dataSelect.stopAtBlock?"Yes":"No");
      fprintf(fp, "     STOP AT PICKUP?: %s\n", dataSelect.stopAtPickup?"Yes":"No");
      fprintf(fp, "    ACCOUNT PREFIXES: ");
      if (dataSelect.numAcctStrs == 0)
         fprintf(fp, "All account numbers");
      else
         fprintf(fp, "%.*s", dataSelect.acctStrs[0].prefixLength,
                             dataSelect.acctStrs[0].acctPrefix);
      for (i = 1; i < dataSelect.numAcctStrs; i++)
         fprintf(fp, ", %.*s", dataSelect.acctStrs[i].prefixLength,
                               dataSelect.acctStrs[i].acctPrefix);
      fprintf(fp, "\n");

      if (dataSelect.acctAge >= 0)
         fprintf(fp, " MINIMUM ACCOUNT AGE: ");
      else
         fprintf(fp, " MAXIMUM ACCOUNT AGE: ");
      fprintf(fp, "%d days\n", abs(dataSelect.acctAge));

      if (dataSelect.cardTypeStr[0] == '!')
      {  fprintf(fp, " CARD TYPES EXCLUDED: %c", dataSelect.cardTypeStr[1]);
         for (i = 2; dataSelect.cardTypeStr[i] != '\0'; i++)
            fprintf(fp, ", %c", dataSelect.cardTypeStr[i]);
      }
      else
      {
         fprintf(fp, " CARD TYPES INCLUDED: ");
         if (dataSelect.cardTypeStr[0] == '\0')
            fprintf(fp, "All");
         else
            for (i = 0; dataSelect.cardTypeStr[i] != '\0'; i++)
              if (i == 0)
                fprintf(fp, "%c", dataSelect.cardTypeStr[i]);
              else
                fprintf(fp, ", %c", dataSelect.cardTypeStr[i]);
      }
      fprintf(fp, "\n");
 
      if (dataSelect.cardUseStr[0] == '!')
      {  fprintf(fp, "  CARD USES EXCLUDED: %c", dataSelect.cardUseStr[1]);
         for (i = 2; dataSelect.cardUseStr[i] != '\0'; i++)
            fprintf(fp, ", %c", dataSelect.cardUseStr[i]);
      }
      else
      {
         fprintf(fp, "  CARD USES INCLUDED: ");
         if (dataSelect.cardUseStr[0] == '\0')
            fprintf(fp, "All");
         else
            for (i = 0; dataSelect.cardUseStr[i] != '\0'; i++)
              if (i == 0)
                fprintf(fp, "%c", dataSelect.cardUseStr[i]);
              else
                fprintf(fp, ", %c", dataSelect.cardUseStr[i]);
      }
      fprintf(fp, "\n");

      fprintf(fp, "          GMT Offset: %5.1lf", dataSelect.GmtOffSet);
      fprintf(fp, "\n");

      if (datwfalscoreDataOut)
         fprintf(fp, "  HI SCORE DATASET THRESHOLD: %d\n",scorethresh);

      if (agethreshon)
         fprintf(fp, "  SCORE/AGE THRESHOLD: %d\n",agethresh);

      if (ageauths)
         fprintf(fp, "  AGE AUTHS ON.\n");

      if (mastercardOnly) crdTypeStr = " MasterCard ";
      else if (visaOnly) crdTypeStr = " Visa ";
      else crdTypeStr = " ";

      if (aInputs[whichInput].postOnly) datTypeStr = "Postings only";
      else if (aInputs[whichInput].authOnly) datTypeStr = "Auths only";
      else datTypeStr = "Auths and Postings";

      if (testOnly)
         fprintf(fp, "           DATA USED: %s, %d%% of the%saccounts (the test set)\n", 
                                 datTypeStr, testThreshold, crdTypeStr);  
      else if (trainOnly)
         fprintf(fp, "           DATA USED: %s, %d%% of the%saccounts (the training set)\n", 
                                 datTypeStr, 100 - testThreshold, crdTypeStr);
      else 
         fprintf(fp, "           DATA USED: %s, all%saccounts\n", datTypeStr,
                                 crdTypeStr);  

      /* KMH ADDED 7/24/1997 */
      fprintf(fp, "\n   False Zero Threshold: %5.3lf\n", fGlobalFalseZeroThreshold);
      fprintf(fp, "\n");
      /* END KMH */

      if (numInputs > 1)
         fprintf(fp, "==================================================================\n");
   }

   fprintf(fp, "\n");
}


/* ----------------------------------------------------------------------
 *
 *  Main driver.  Requires a single parameter which is the full path name
 *  to where the data files are located.  processTransactions() is used
 *  to read the files and call CSCORE()
 *
 * -------------------------------------------------------------------- */
/* getopt input bitflag */
#define INPUT_PATH         1
#define OUTPUT_FILE        2
#define NUM_RECS           4
#define CONFIG_NAME        8
#define RESEARCH          16
#define SELECT_FILE       32
#define TEST_THRESH       64
#define ERROR           1024
int main(int argc, char* argv[])
{
   int     i, argNum=1, found=0, changed=0, firstMsgPrinted=0;
   char    outFileName[256], file[256], zcatCmmd[256];
   char    selFile[256], resFile[256], profFile[256], outPath[256];
   char    profNameFile[256], scrDataFile[256], scoreFile[256];
   char    sasFile[256], recFile[50], apiFile[256], riskFile[256];
   char    profTransFile[256], scrTransFile[256], cardOutFile[256];
   char    fullProfFile[256];
   time_t  cTime = time((long*)0);
   tInFile inputInfo[MAX_INPATHS];
   int     numInputPaths=0, whichInput;
   FILE    *fp;
   char    buf[256];

   term(0);

   pAcctNum = acctNumStr;
   pFraudDate = fraudDateStr;
   memset((char*)inputInfo, '\0', sizeof(inputInfo));

   /* Initialize fraud type tables, defaulting to UNKNOWN_FRD */
   memset(fraudTypeTable, (char)UNKNOWN_FRD, sizeof(fraudTypeTable));
   fraudTypeTable[fraudCodes[APPFRAUD]]    = APPFRAUD;
   fraudTypeTable[fraudCodes[COUNTERFEIT]] = COUNTERFEIT;
   fraudTypeTable[fraudCodes[LOST]]        = LOST;
   fraudTypeTable[fraudCodes[NONRECEIPT]]  = NONRECEIPT;
   fraudTypeTable[fraudCodes[OTHER_FRD]]   = OTHER_FRD;
   fraudTypeTable[fraudCodes[STOLEN]]      = STOLEN;
   fraudTypeTable[fraudCodes[TAKEOVER]]    = TAKEOVER; 
   fraudTypeTable[fraudCodes[MAILPHONE]]   = MAILPHONE;
   fraudTypeTable[fraudCodes[CONVCHECK]]   = CONVCHECK;
   fraudTypeTable[fraudCodes[FRIENDLY]]    = FRIENDLY;
   fraudTypeTable[fraudCodes[ZERO_LOSS]]   = ZERO_LOSS;

   while(argNum < argc)
   {  switch(argv[argNum][1])
      { case 'i':
          /* Create a new "input context." */
          strcpy(inputInfo[numInputPaths++].inPath, *(argv+argNum)+2);
          found |= INPUT_PATH;
	  if (numInputPaths > MAX_INPATHS) {
	      errExit("\nDrv_model* will process only(!!) 100 file sets per run.\n" 
		      "If needed, increase MAX_INPATHS in mfdrive.c and recompile.\n", 101);
	  }
          break;
        case 'Z':
          fGlobalFalseZeroThreshold = (float) atof(*(argv+argNum)+2);
          break;
        case 'n':
          if (!strcmp(*(argv+argNum)+1, "nullfiles")) nullFiles = TRUE;
          else
	  {
             numTrans = atoi(*(argv+argNum)+2);
             outLimit = numTrans;
             found |= NUM_RECS;
	  }
          break;
        case 'T':
          testThreshold = atoi(*(argv+argNum)+2);
          found |= TEST_THRESH;
          break;
        case 'R':
	  {
          /* if ( *(argv+argNum)+2 != NULL ) {
	    riskOut = TRUE;
	    strcpy(riskFile, *(argv+argNum)+2);
	  } else { */
	    gReduce = 1;
	  }
	  break;
        case 'O':
          outAll = TRUE;
          fprintf(stderr, " outAll set TRUE\n\n");
          break;
        case 'N':
          if (!strcmp(*(argv+argNum)+1, "NOCASH")) 
	  {
		NoCash = TRUE;
                printf("Ignoring any cash transactions TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NOKCHK")) 
	  {
		NoKchk = TRUE;
                printf("Ignoring any kcheck transactions TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NOMRCH")) 
	  {
		NoMrch = TRUE;
                printf("Ignoring any merchandise transactions TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NOTKEYED")) 
	  {
		NotKeyedOnly = TRUE;
                printf("Ignoring any non-keyed transactions TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NONMON")) 
	  {
		NoNmons = TRUE;
                printf("Ignoring any NonMon files TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NOPMT")) 
	  {
		NoPayms = TRUE;
                printf("Ignoring any Payment files TOTALLY!\n");
	  }
          else if (!strcmp(*(argv+argNum)+1, "NOPOST")) 
	  {
		NoPosts = TRUE;
                printf("Ignoring any Postings files TOTALLY!\n");
	  }
	  else
          {
          	tossNoCards = FALSE;
                fprintf(stderr,"\n\n tossNoCards set to FALSE - should do auths whether card account exists or not\n\n");
          }
          break;
        case 't':
          if (!strcmp(*(argv+argNum)+1, "test")) testOnly = TRUE;
          else if (!strcmp(*(argv+argNum)+1, "train")) trainOnly = TRUE;
          else traceOn = TRUE;
          break;
        case 'd':
          if (!strncmp(*(argv+argNum)+1, "dolGe",5)) 
	  {
		AmountsGe = strtod( *(argv+argNum)+6 , (char **) NULL);
	  }
          else if (!strncmp(*(argv+argNum)+1, "dolLt",5)) 
	  {
		AmountsLt = strtod( *(argv+argNum)+6 , (char **) NULL);
	  }
          break;
        case 'K':
          if (!strcmp(*(argv+argNum)+1, "KCHK")) 
	  {
		KchkOnly = TRUE;
	  	if ( (KchkOnly && MrchOnly) || (KchkOnly && CashOnly) )
	  	{
			fprintf(stderr,"\n\n ---> KCHK set with either CASH or MRCH. Not possible.\n\n");
			exit(13);
	  	}
	  }
          else if (!strcmp(*(argv+argNum)+1, "KEYED")) 
	  {
		KeyedOnly = TRUE;
	  }
          break;
        case 'M':
          if (!strcmp(*(argv+argNum)+1, "MRCH")) 
	  {
		MrchOnly = TRUE;
	  	if ( (MrchOnly && KchkOnly) || (MrchOnly && CashOnly) )
	  	{
			fprintf(stderr,"\n\n ---> MRCH set with either CASH or KCHK. Not possible.\n\n");
			exit(13);
	  	}
	  }
          else
          {
          	doModel = TRUE;
	  }
          break;
        case 'S':
          silent = TRUE;
          break;
        case 'l':
          /* Copy file name into current "input context" */
          if (!numInputPaths && !readScoreFile)
          {  fprintf(stderr, "\nError:  -i<input_path> option must come before "
                          "any other\ninput file specifications: -l or -A or -E\n\n");
             argErr();
          }
          else
          {  strcpy(inputInfo[numInputPaths-1].selFile, *(argv+argNum)+2);
             found |= SELECT_FILE;
          }
          break;
        case 'p':
          profOut = TRUE;
          strcpy(profFile, *(argv+argNum)+2);
          break;
        case 'm':
          strcpy(configName, *(argv+argNum)+2);
          found |= CONFIG_NAME;
          break;
        case 'f':
          if (!strcmp(*(argv+argNum)+1, "fraudsonly"))
          {
            FraudsOnly = TRUE;
            fprintf(stderr,"\n\n  ------->  FraudsOnly set TRUE to only score fraud accounts, both prefraud and fraud auths\n\n");
          }
          break;
        case 's':
 	  if (!strcmp(*(argv+argNum)+1, "scoremod")) 
          {
	    scoreMod = TRUE;
            fprintf(stderr,"\n\n  ------->  scoreMod set TRUE to modify scores\n\n");
          }
          else if (!strcmp(*(argv+argNum)+1, "scoreall"))
          {
            ScoreAll = TRUE;
            fprintf(stderr,"\n\n  ------->  ScoreAll set TRUE to score all accounts: nonfraud, prefraud and fraud auths\n\n");
          }
          else
          {
            scrDataOut = TRUE;
            strcpy(scrDataFile, *(argv+argNum)+2);
          }
          break;
        case 'h':
          datwfalscoreDataOut = TRUE;
          scorethresh=atoi(*(argv+argNum)+2);
          break;
        case 'j':
          agethreshon = TRUE;
          agethresh=atoi(*(argv+argNum)+2);
          break;
        case 'v':
          ageauths = TRUE;
          break;
        case 'c':
          scoreOut = TRUE;
          strcpy(scoreFile, *(argv+argNum)+2);
          break;
        case 'C':
          if (!strcmp(*(argv+argNum)+1, "CASH")) 
	  {
		CashOnly = TRUE;
	  	if ( (CashOnly && KchkOnly) || (CashOnly && MrchOnly) )
	  	{
			fprintf(stderr,"\n\n ---> CASH set with either KCHK or MRCH. Not possible.\n\n");
			exit(13);
	  	}
	  }
          else
          {
          	cardOut = TRUE;
          	strcpy(cardOutFile, *(argv+argNum)+2);
	  }
          break;
        case 'u':
          UNCOMPRESS = FALSE;
          break;
        case 'F':
          fullProfOut = TRUE;
          strcpy(fullProfFile, *(argv+argNum)+2);
          break;
        case 'H':
          stopAtTranslate = TRUE;
          break;
        case 'o':
          strcpy(outPath, *(argv+argNum)+2);
          found |= OUTPUT_FILE;
          break;
        case 'a':
          apiOut = TRUE;
          strcpy(apiFile, *(argv+argNum)+2);
          break;
        case 'P':
          profNameOut = TRUE;
          strcpy(profNameFile, *(argv+argNum)+2);
          break;
        case 'E':
          readScoreFile = TRUE;
          strcpy(inResFile, *(argv+argNum)+2);
          if (numInputPaths) {
              fprintf(stderr, "\nError:  Only one input source allowed when "
                              "reading from research file.\n");
              argErr();
          }
          numInputPaths = 1;
          break;
        case 'r':
          strcpy(resFile, *(argv+argNum)+2);
          found |= RESEARCH;
          break;
        case 'b':
          research_brief = 1;
          break;
        case 'e':
          explain = atoi(*(argv+argNum)+2);
          break;
        case 'I':
          timeIt = TRUE;
          break;
        case 'D':
          if((gOutDepends = fopen(argv[argNum]+2, "w"))==NULL)
          {  fprintf(stderr, "\nError: Can't create file '%s'\n", argv[argNum]+2);
             argErr();
          }
          break;
        case 'A':
          /* Put API file in current "input context" */
          if (!numInputPaths) {
             fprintf(stderr, "\nError:  -i<input_path> option must come before "
                             "any other\ninput file specifications: -l or -A\n\n");
             argErr();
          } else {
              readAPIFile = TRUE;
              strcpy(inputInfo[numInputPaths-1].inAPIFile, *(argv+argNum)+2);
          }
          break;
        case 'G':
          doModel = TRUE;
          doScale = TRUE;
          break;
        case 'B':
          unsclBufSize = atoi(*(argv+argNum)+2);
          if(unsclBufSize <= 0)
          {  fprintf(stderr, "\nError:  Bad size, %d,  for unscaled buffer\n\n", unsclBufSize);
             argErr();
          }
          unsclBufSize *= 1024*1024;
          break;
        default:
          found |= ERROR; 
      }
      argNum++;
   }

   if (found&ERROR)
   { fprintf(stderr,"Error in argument list\n\n");
     argErr();
   }

   if (!(found&CONFIG_NAME) && !readScoreFile)
   { fprintf(stderr,"Must provide a config file\n\n");
     argErr();
   }


   if (readAPIFile && readScoreFile)
   { fprintf(stderr,"Error: Can't specify both research input and API file input\n\n");
     argErr();
   }

   if (readScoreFile)
   {     if (UNCOMPRESS){
            sprintf(zcatCmmd,"%s %s.gz", ZCAT, inResFile);
            inResFp = popen(zcatCmmd, "r");
            if (inResFp == NULL){
               fprintf(stderr, "\nCan't zcat score file input file <%s.gz>\n\n", inResFile);
               argErr();
            }
         }
         else {
            inResFp = fopen(inResFile, "r");
            if (inResFp == NULL){
               fprintf(stderr, "\nCan't open flat score file input file <%s>\n\n", inResFile);
               argErr();
            }
         }
   }
   else if (!(found&INPUT_PATH) && !readScoreFile)
   { fprintf(stderr,"Bad INPUT PATH\n\n");
     argErr();
   }

   if (found&OUTPUT_FILE)
   {  gOutFile = fopen(outPath, "w");
      if (gOutFile == NULL)
      {  fprintf(stderr, "\nError opening <%s> for writing\n", outPath);
         argErr();
      }
   }


   /* create gen file during modeling */
   if (doModel)
   {
      sprintf(file, "%s.gen",configName);
      paramFp = fopen(file, "w");
      if (paramFp == NULL)
      {  fprintf(stderr, "\nError opening <%s> for writing\n", file);
         argErr();
      }
      
      /* write the content of the config file into the gen file */
      fprintf(paramFp, "The content of the config file :\n");
      fp = fopen(configName, "r");
      if (fp == NULL)
      {  fprintf(stderr, "\nError opening the config file <%s> for reading\n", configName);
         argErr();
      }
      while(fgets(buf,128,fp))
          fprintf(paramFp,buf);
      fclose(fp);
      fprintf(paramFp, "End of config file\n\n");


      fprintf(paramFp, "This data was generated on %s\n", asctime(localtime(&cTime)));
      fprintf(paramFp, "The %s file was created by '%s' using the following parameters:\n\n", doScale?".scl":".dat", argv[0]);
      printParams(paramFp, 1, inputInfo, numInputPaths);
      
   }


   if (mastercardOnly && visaOnly) 
   {  fprintf(stderr, "Can't use both -M and -V; use neither to get all accts.");
      exit(13);
   }

   if (found&RESEARCH)
   {  if ((resFp = fopen(resFile, "w")) == NULL)
      {  fprintf(stderr, "\nError opening <%s> for writing\n", resFile);
         argErr();
      }
      researchOn = TRUE;
   }

   if (found&NUM_RECS)
   {  if (outLimit <= 0)
      {  fprintf(stderr, "\nError:  Number of records must be a number > 0\n");
         argErr();
      }
   }

   if (apiOut)
   {   sprintf(zcatCmmd, "%s -c > %s.gz", GZIP, apiFile);
       if ((apiFp = popen(zcatCmmd, "w")) == NULL) 
       {
         fprintf(stderr,"\nError: cannot open gzip pipe to file '%s'\n", apiFile);
         argErr();
       }
   }

   if (recOut)
   {  if ((recFp = fopen(recFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", recFile);
         argErr();
      }
   }

   if (profOut)
   {  if ((profFp = fopen(profFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", profFile);
         argErr();
      }
   }

   if (fullProfOut)
   {  if ((fullProfFp = fopen(fullProfFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", fullProfFile);
         argErr();
      }
   }

   if (profNameOut)
   {  if ((profNameFp = fopen(profNameFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", profNameFile);
         argErr();
      }
   }

   if (cardOut)
   {  if ((cardOutFp = fopen(cardOutFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", cardOutFile);
         argErr();
      }
   }

   if (scrDataOut) {
     if (UNCOMPRESS) {
       sprintf(zcatCmmd, "%s -c > %s.gz", GZIP, scrDataFile);
       if ((scrDataFp = popen(zcatCmmd, "w")) == NULL) {
         fprintf(stderr,"\nError: cannot open gzip pipe to file '%s'\n", scrDataFile);
         exit(-1);
       }
     } else {
       if ((scrDataFp = fopen(scrDataFile, "w")) == NULL) {
         fprintf(stderr,"\nError: cannot open file '%s'\n", scrDataFile);
         argErr();
       }
     }
   }

   if (datwfalscoreDataOut)
   {
      /*clear the data files which contain the hi scoring account trans*/
      postdatwfalscoreFp = fopen("auths_high.dat", "w");
      authdatwfalscoreFp = fopen("posts_high.dat", "w");
   }

   if (scoreOut)
   {  
     if (UNCOMPRESS) {
       sprintf(zcatCmmd, "%s -c > %s.gz", GZIP, scoreFile);
       if ((scoreFp = popen(zcatCmmd, "w")) == NULL) {
         fprintf(stderr,"\nError: cannot open gzip pipe to file '%s'\n", scoreFile);
         exit(-1);
       }
     } else {
       if ((scoreFp = fopen(scoreFile, "w")) == NULL) {
         fprintf(stderr,"\nError: cannot open file '%s'\n", scoreFile);
         argErr();
       }
     }
   }

   if (riskOut)
   {  if ((riskFp = fopen(riskFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", riskFile);
         argErr();
      }
   }

   if (scrTransOut)
   {  if(splitDate==0)
      {  fprintf(stderr, "\nMust specify split date with score transaction file\n\n");
         argErr();
      }
      if((scrTransFp = fopen(scrTransFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", scrTransFile);
         argErr();
      }
   }

   if (profTransOut)
   {  if(splitDate==0)
      {  fprintf(stderr, "\nMust specify split date with profile transaction file\n\n");
         argErr();
      }

      if((profTransFp = fopen(profTransFile, "w")) == NULL)
      {  fprintf(stderr,"\nError: cannot open file '%s'\n", profTransFile);
         argErr();
      }
   }

   fraudType = ' ';

   for (whichInput = 0; whichInput < numInputPaths; whichInput++)
   {
      if (!readScoreFile && !readAPIFile)
      {
   /*  cardholder data file */
         if (UNCOMPRESS) {
       sprintf(zcatCmmd, "/usr/local/bin/zcat %s/cards.dat.gz", inputInfo[whichInput].inPath);
       fp = popen(zcatCmmd, "r");
    } else {
       sprintf(file, "%s/cards.dat", inputInfo[whichInput].inPath);
            fp = fopen(file, "r");
    }
         switch(checkData(fp, CARD_FILE, CARD_LENGTH))
         { case OK:
             break;
           case CHANGED:
             changed = 1; 
             break;
           case UNDETERMINED:
             argErr();
         }
         if (fp != NULL) {
        if (UNCOMPRESS) pclose(fp);
          else  fclose(fp);
        inputInfo[whichInput].cardRecLen = REAL_CARD_LENGTH;
        inputInfo[whichInput].cardNL = CARD_NEWLINE;
    }

   /*  authorization data file */
         if (UNCOMPRESS) sprintf(file, "%s/auths.dat.gz", inputInfo[whichInput].inPath);
      else sprintf(file, "%s/auths.dat", inputInfo[whichInput].inPath);
    if (checkFile(file, AUTH_FILE, whichInput)) {
      if (UNCOMPRESS) {
        sprintf(zcatCmmd, "/usr/local/bin/zcat %s", file);
        fp = popen(zcatCmmd, "r");
      } else {
        fp = fopen(file, "r");
      }
           switch(checkData(fp, AUTH_FILE, AUTH_LENGTH))
           { case OK:
               break;
             case CHANGED:
               changed = 1;
               break;
             case UNDETERMINED:
               argErr();
           }
      if (fp != NULL) {
        if (UNCOMPRESS) pclose(fp);
          else  fclose(fp);
        inputInfo[whichInput].authRecLen = REAL_AUTH_LENGTH;
        inputInfo[whichInput].authNL = AUTH_NEWLINE;
      }
    } else {
        inputInfo[whichInput].postOnly = 1;
    }  /* end if checkFile for auths */

   /*  posting data file */
       if ( !(NoPosts) ) {
         if (UNCOMPRESS)  sprintf(file, "%s/posts.dat.gz", inputInfo[whichInput].inPath);
	   else sprintf(file, "%s/posts.dat", inputInfo[whichInput].inPath);
	 if (checkFile(file, POST_FILE, whichInput)) {
	   if (UNCOMPRESS) {
	     sprintf(zcatCmmd, "/usr/local/bin/zcat %s", file);
	     fp = popen(zcatCmmd, "r");
	   } else {
		   fp = fopen(file, "r");
	   }
         switch(checkData(fp, POST_FILE, POST_LENGTH))
         { case OK:
             break;
           case CHANGED:
             changed = 1; 
             break;
           case UNDETERMINED:
             argErr();
         }
	   if (fp != NULL) {
	     if (UNCOMPRESS) pclose(fp);
	       else  fclose(fp);
	     inputInfo[whichInput].postRecLen = REAL_POST_LENGTH;
	     inputInfo[whichInput].postNL = POST_NEWLINE;
	   }
	 } else {
	    inputInfo[whichInput].authOnly = 1;
	 }  /*  end if checkFile for posts */
        } /* end if Do want to process postings */
        else 
          inputInfo[whichInput].authOnly = 1;

	/*  nonmon data file */
	if ( !(NoNmons) ) {
         if (UNCOMPRESS)  sprintf(file, "%s/nonmons.dat.gz",
				  inputInfo[whichInput].inPath);
	   else sprintf(file, "%s/nonmons.dat", inputInfo[whichInput].inPath);
	 if (inputInfo[whichInput].nonMonExists =
	     checkFile(file, NONMON_FILE, whichInput)) {
	   if (UNCOMPRESS) {
	     sprintf(zcatCmmd, "/usr/local/bin/zcat %s", file);
	     fp = popen(zcatCmmd, "r");
	   } else {
	     fp = fopen(file, "r");
	   }
	   switch(checkData(fp, NONMON_FILE, NONMON_LENGTH)) {
	   case OK:
	     break;
	   case CHANGED:
	     changed = 1;
	     break;
	   case UNDETERMINED:
	     argErr();
	   }
	   if (fp != NULL) {
	     if (UNCOMPRESS) pclose(fp);
	       else  fclose(fp);
	     inputInfo[whichInput].nonMonRecLen = REAL_NONMON_LENGTH;
	     inputInfo[whichInput].nonMonNL = NONMON_NEWLINE;
	   }
	 }  /*  end if checkFile for nonmons */
        } /* end if Do want to process nonmons */

	/*  payments data file */
	if ( !(NoPayms) ) {
         if (UNCOMPRESS)  sprintf(file, "%s/paymnts.dat.gz",
				  inputInfo[whichInput].inPath);
	   else sprintf(file, "%s/paymnts.dat", inputInfo[whichInput].inPath);
	 if (inputInfo[whichInput].payExists = checkFile(file, PAY_FILE, whichInput)) {
	   if (UNCOMPRESS) {
	     sprintf(zcatCmmd, "/usr/local/bin/zcat %s", file);
	     fp = popen(zcatCmmd, "r");
	   } else {
	     fp = fopen(file, "r");
	   }
	   switch(checkData(fp, PAY_FILE, PAY_LENGTH)) {
	   case OK:
	     break;
	   case CHANGED:
	     changed = 1;
	     break;
	   case UNDETERMINED:
	     argErr();
	   }
	   if (fp != NULL) {
	     if (UNCOMPRESS) pclose(fp);
	       else  fclose(fp);
	     inputInfo[whichInput].payRecLen = REAL_PAY_LENGTH;
	     inputInfo[whichInput].payNL = PAY_NEWLINE;
	   }
	 }  /*  end if checkFile for payments */
        } /* end if Do want to process payments */

      } /*   end of not read research and not read api files   */

      if (inputInfo[whichInput].postOnly && inputInfo[whichInput].authOnly)
       {  fprintf(stderr,"\nError: cannot find auth or posting data\n");
          argErr();
       }

       if (!readScoreFile)
       {
	/*  frauds  data check */
         if (UNCOMPRESS) {
	    sprintf(zcatCmmd, "/usr/local/bin/zcat %s/frauds.dat.gz", inputInfo[whichInput].inPath);
	    fp = popen(zcatCmmd, "r");
	 } else {
		   sprintf(file, "%s/frauds.dat", inputInfo[whichInput].inPath);
		   fp = fopen(file, "r");
	 }
	  	   switch(checkData(fp, FRAUD_FILE, FRAUD_LENGTH))
         { case OK:
             break;
           case CHANGED:
             changed = 1; 
             break;
           case UNDETERMINED:
             argErr();
         }
	 if (fp != NULL) {
	     if (UNCOMPRESS) pclose(fp);
	       else  fclose(fp);
	     inputInfo[whichInput].fraudRecLen = REAL_FRAUD_LENGTH;
	     inputInfo[whichInput].fraudNL = FRAUD_NEWLINE;
	 }
       }


      if (changed)
      {
          if (!firstMsgPrinted)
          {  fprintf(stderr,
	      "\n  The following files appear to have record lengths different from\n"
	      "  those specified in std.h.  This is just a warning, but be aware that\n"
	      "  the record lengths I calculated may not be correct, so double check\n"
	      "  your data.  Modify std.h and recompile to blow away this warning\n\n");
             firstMsgPrinted = 1;
          }

          fprintf(stderr, "%s:\n", inputInfo[whichInput].inPath);

          if (FRAUD_LENGTH != REAL_FRAUD_LENGTH) 
             fprintf(stderr, "\tCalculated Fraud record length is %d -- %d in std.h\n", 
                                               REAL_FRAUD_LENGTH, FRAUD_LENGTH);

          if (CARD_LENGTH != REAL_CARD_LENGTH) 
             fprintf(stderr, "\tCalculated Card record length is %d -- %d in std.h\n", 
                                               REAL_CARD_LENGTH, CARD_LENGTH);

          if (AUTH_LENGTH != REAL_AUTH_LENGTH) 
             fprintf(stderr, "\tCalculated Auth record length is %d -- %d in std.h\n", 
                                               REAL_AUTH_LENGTH, AUTH_LENGTH);

          if (POST_LENGTH != REAL_POST_LENGTH) 
             fprintf(stderr, "\tCalculated Posting record length is %d -- %d in std.h\n",
                                               REAL_POST_LENGTH, POST_LENGTH);
          if (PAY_LENGTH != REAL_PAY_LENGTH) 
             fprintf(stderr, "\tCalculated Payment record length is %d -- %d in std.h\n", 
                                               REAL_PAY_LENGTH, PAY_LENGTH);
          if (NONMON_LENGTH != REAL_NONMON_LENGTH) 
             fprintf(stderr, "\tCalculated NonMon record length is %d -- %d in std.h\n", 
                                               REAL_NONMON_LENGTH, NONMON_LENGTH);
          fprintf(stderr, "\n");
	  changed = 0;
      }    /* End of if (changed) */
   }         /* End of for loop */

   /* Output current parameters with warning message before begin main loop */
   /* This will allow user to peruse setup before wasting several hours     */
   fprintf(stderr, "\nYou are running with the following parameters.\n"
                   "If something does not look right... stop now!\n\n");

   fprintf(stderr, "  ---> Created by executable: '%s'\n\n", argv[0]);

   printParams(stderr, 0, inputInfo, numInputPaths);

   /* Output the current parameters to the research file as a permanent log */
   if (researchOn)
   {  fprintf(resFp, "This analysis was run on %s\n", asctime(localtime(&cTime)));
      fprintf(resFp, "Created by '%s' using the following parameters:\n\n", argv[0]);

      printParams(resFp, 1, inputInfo, numInputPaths);

      researchTop = ftell(resFp);
   }

   if (readScoreFile)
      numInputPaths = 1;         /* Make the following loop run once */

   for (whichInput = 0; whichInput < numInputPaths; whichInput++)
   {
      if (readAPIFile){
         if (UNCOMPRESS){
            if (inAPIFp != NULL) pclose(inAPIFp);
            sprintf(zcatCmmd,"%s %s.gz", ZCAT, inputInfo[whichInput].inAPIFile);
            inAPIFp = popen(zcatCmmd, "r");
            if (inAPIFp == NULL){
               fprintf(stderr, "\nCan't zcat API input file <%s.gz>\n\n",
                               inAPIFile);
               argErr();
            }
         }
         else {
            if (inAPIFp != NULL) fclose(inAPIFp);
            inAPIFp = fopen(inputInfo[whichInput].inAPIFile, "r");
            if (inAPIFp == NULL){
               fprintf(stderr, "\nCan't open flat API input file <%s>\n\n",
                               inAPIFile);
               argErr();
            }
         }
      }

      if(getSelectParms(inputInfo[whichInput].selFile))
        exit(13);

      strcpy(inPath, inputInfo[whichInput].inPath);
      if (!readAPIFile && !readScoreFile)
      {
         authOnly = inputInfo[whichInput].authOnly;
         postOnly = inputInfo[whichInput].postOnly;
	 /*  by restoring calculated record lengths from previous checkData calls
	     eliminate need for another round of rec length calls - ojo 12/31/96 */
	 /*  also needed to restore the NEWLINE globals -   OOPS    ojo  2/17/97 */
         REAL_AUTH_LENGTH  = inputInfo[whichInput].authRecLen;
         AUTH_NEWLINE      = inputInfo[whichInput].authNL;
         REAL_POST_LENGTH  = inputInfo[whichInput].postRecLen;
         POST_NEWLINE      = inputInfo[whichInput].postNL;
         REAL_FRAUD_LENGTH = inputInfo[whichInput].fraudRecLen;
         FRAUD_NEWLINE     = inputInfo[whichInput].fraudNL;
         REAL_CARD_LENGTH  = inputInfo[whichInput].cardRecLen;
         CARD_NEWLINE      = inputInfo[whichInput].cardNL;
         REAL_NONMON_LENGTH = inputInfo[whichInput].nonMonRecLen;
         NONMON_NEWLINE     = inputInfo[whichInput].nonMonNL;
         REAL_PAY_LENGTH   = inputInfo[whichInput].payRecLen;
         PAY_NEWLINE       = inputInfo[whichInput].payNL;

         if (pCardFile != NULL) {
	     if (UNCOMPRESS)  pclose(pCardFile);
	       else fclose(pCardFile);
	 }
         if (UNCOMPRESS) {
	    sprintf(zcatCmmd, "/usr/local/bin/zcat %s/cards.dat.gz", inPath);
	    if ( (pCardFile = popen(zcatCmmd, "r")) == NULL)
            {  fprintf(stderr,"\nError: cannot open pipe for 'cards.dat'\n");
               argErr();
            }
	 } else {
         sprintf(file, "%s/cards.dat", inPath);
            if((pCardFile = fopen(file, "rb")) == NULL)
         {  fprintf(stderr,"\nError: cannot open file 'cards.dat'\n");
            argErr();
         }
	 }

         if (!postOnly) {
             if (pAuthFile != NULL) {
	         if (UNCOMPRESS)  pclose(pAuthFile);
	           else fclose(pAuthFile);
	     }
            if (UNCOMPRESS) {
	        sprintf(zcatCmmd, "/usr/local/bin/zcat %s/auths.dat.gz", inPath);
	        if ( (pAuthFile = popen(zcatCmmd, "r")) == NULL)
                {  fprintf(stderr,"\nError: cannot open pipe for 'auths.dat'\n");
                   argErr();
                }
	    } else {
                sprintf(file, "%s/auths.dat", inPath);
                if((pAuthFile = fopen(file, "rb")) == NULL)
            {  fprintf(stderr, "\nError: cannot open file 'auths.dat'\n");
               argErr();
            }
         }
         }

         if (!authOnly) {
	 if ( !(NoPosts) ) {
             if (pPostFile != NULL) {
	         if (UNCOMPRESS)  pclose(pPostFile);
	           else fclose(pPostFile);
	     }
            if (UNCOMPRESS) {
	        sprintf(zcatCmmd, "/usr/local/bin/zcat %s/posts.dat.gz", inPath);
	        if ( (pPostFile = popen(zcatCmmd, "r")) == NULL)
                {  fprintf(stderr,"\nError: cannot open pipe for 'posts.dat'\n");
                   argErr();
                }
	    } else {
	        sprintf(file, "%s/posts.dat", inPath);
                if((pPostFile = fopen(file, "rb")) == NULL)
            {  fprintf(stderr, "\nError: cannot open file 'posts.dat'\n");
               argErr();
            }
         }
         }
         }

	 if ( !(NoPayms) ) {
         if (inputInfo[whichInput].payExists) {
             if (pPayFile != NULL) {
	         if (UNCOMPRESS)  pclose(pPayFile);
	           else fclose(pPayFile);
	     }
            if (UNCOMPRESS) {
	        sprintf(zcatCmmd, "/usr/local/bin/zcat %s/paymnts.dat.gz", inPath);
	        if ( (pPayFile = popen(zcatCmmd, "r")) == NULL)
                {  fprintf(stderr,"\nError: cannot open pipe for 'paymnts.dat'\n");
                   argErr();
                }
	    } else {
		sprintf(file, "%s/paymnts.dat", inPath);
                if((pPayFile = fopen(file, "rb")) == NULL)
                {  fprintf(stderr, "\nError: cannot open file 'paymnts.dat'\n");
                   argErr();
                }
	    }
	 }
	 } /* end if Do want to process payments */

	 if ( !(NoNmons) ) {
         if (inputInfo[whichInput].nonMonExists) {
	    if (pNonMonFile != NULL) {
	         if (UNCOMPRESS)  pclose(pNonMonFile);
	           else fclose(pNonMonFile);
	    }
            if (UNCOMPRESS) {
	        sprintf(zcatCmmd, "/usr/local/bin/zcat %s/nonmons.dat.gz", inPath);
	        if ( (pNonMonFile = popen(zcatCmmd, "r")) == NULL)
                {  fprintf(stderr,"\nError: cannot open pipe for 'nonmons.dat'\n");
                   argErr();
                }
	    } else {
	        sprintf(file, "%s/nonmons.dat", inPath);
                if((pNonMonFile = fopen(file, "rb")) == NULL)
                {  fprintf(stderr, "\nError: cannot open file 'nonmons.dat'\n");
                   argErr();
                }
	    }
         }
	 } /* end if Do want to process nonmons */
     }   /*  end if (not read API) and (not read Research File)  */

      if (!readScoreFile) {
          if (pFraudFile != NULL) {
	      if (UNCOMPRESS)  pclose(pFraudFile);
	        else fclose(pFraudFile);
	  }
         if (UNCOMPRESS) {
	    sprintf(zcatCmmd, "/usr/local/bin/zcat %s/frauds.dat.gz", inPath);
	    if ( (pFraudFile = popen(zcatCmmd, "r")) == NULL) {
	       fprintf(stderr,"\nError: cannot open pipe for 'frauds.dat'\n");
               argErr();
            }
	 } else {
	    sprintf(file, "%s/frauds.dat", inPath);
            if((pFraudFile = fopen(file, "rb")) == NULL) {
            fprintf(stderr,"\nCannot open file 'frauds.dat'\n");
	       argErr();
	    }
	 }
      }

      fprintf(stderr,"\n\n==========================================================================\n");
      fprintf(stderr,    " ----> Processing data:   %s\n", inPath);
      fprintf(stderr,    "==========================================================================\n");
      fprintf(stderr,"      NOTE: The GMT offset for this dataset is: %5.1lf hours\n",dataSelect.GmtOffSet);
      fprintf(stderr,    "==========================================================================\n\n");

      if (!readScoreFile)
         processTransactions();
      else
         processResearch();
   }      /* End of for loop */

   /* function exits in cleanup() */
   cleanup(0);
}
