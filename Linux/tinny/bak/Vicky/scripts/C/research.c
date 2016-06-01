/*========================================================================*/
/*                                                                        */
/*                               research.c                               */
/*                                                                        */
/*  DESCRIPTION:  Functions called from the Falcon driver to              */
/*                accumulate and output statistics on Falcon's            */
/*                performance.                                            */
/*                                                                        */
/*                                                                        */
/*  NOTES: FOR HNC INTERNAL USE ONLY!!                                    */
/*                                                                        */
/*                                                                        */
/*========================================================================*/
/*                                                                        */

/*
$Source: /work/price/falcon/dvl/ccpp/drv_falcon30/RCS/research.c,v $
 *
$Log: research.c,v $
Revision 1.1  1998/06/08 22:55:28  jcb
Initial revision

 * Revision 1.5  1998/04/22  21:09:27  jcb
 * added basis point calculation
 *
 * Revision 1.4  1998/04/21  21:34:16  jcb
 * corrected version for scoreall
 *
 * Revision 1.12  1998/04/20  17:40:08  jcb
 * added argument bFraudacct to research, can score prefraud transactions
 * with correct accounting for sampling rates
 *
 * Revision 1.10  1997/12/01  20:40:09  jcb
 * added print for no fraud auths
 *
 * Revision 1.13  1997/10/31  19:31:03  jcb
 * more tweaks on formats
 *
 * Revision 1.12  1997/10/31  17:52:26  jcb
 * minor tweak to formats
 *
 * Revision 1.11  1997/10/09  20:02:41  jcb
 * more mods to prints
 *
 * Revision 1.8  1997/10/09  20:00:25  jcb
 * more of the same
 *
 * Revision 1.7  1997/10/09  18:39:00  jcb
 * yet again print mods
 *
 * Revision 1.6  1997/10/07  22:53:10  jcb
 * more fixes on prints about detected per month
 *
 * Revision 1.5  1997/10/07  20:19:22  jcb
 * further tweaks of prints
 *
 * Revision 1.4  1997/10/06  23:27:36  jcb
 * fixed print alignment
 *
 * Revision 1.3  1997/10/06  18:38:55  jcb
 * corrected prints for multiple transactions from 6 up.
 *
 * Revision 1.2  1997/08/08  17:22:48  jcb
 * added better print for when there are no auths.
 *
 * Revision 1.1  1997/06/17  16:43:04  jcb
 * Initial revision
 *
 * Revision 1.3  1997/03/06  19:28:15  jcb
 * added more detail to type of fraud section
 *
 * Revision 1.2  1997/02/24  20:40:25  jcb
 * added additional stuff
 *
 * Revision 1.1  1996/11/26  22:29:01  jcb
 * Initial revision
 *
 * Revision 1.8  1995/03/22  17:24:02  can
 * Latest version.
 *
 *
/*                                                                        */
/*========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>   /* for INT_MIN and LONG_MIN */
#include <math.h>


#include "select.h"
extern modelParms dataSelect;

#define NSCORES    1000
#define LARGE_NUM  99999999L    /* this is a large number for use in
                                   avoiding divide by zero for TFPR and AFPR */
extern int authOnly,postOnly;
/* To determine maximum score */
extern int MaxScore;
/* End maximum score */

static int   NumApprvFraudAuths = 0;
static int   RealNumCleanAuths = 0;
static int   RealNumCleanAccounts = 0;
static int   TrueNumAccounts = 0;
static int   RealNumFraudAccounts = 0;

static double AmtFraudAppr = 0., AmtNonFraudAppr = 0.;
extern  long days_of_fraud;
static  long max_days_fraud=0, sum_days_fraud=0, sum_days_fraud_sq=0;
static  long acct_days_fraud=0, sum_acct_days_fraud=0, sum_acct_days_fraud_sq;
static  long last_acct_days_fraud=0;
static  double mean_days_fraud, sdev_days_fraud;
static  int median_days_fraud;
static  int AcctsNumDaysFraud[500];
int     SumAcctsNumDaysFraud=0;

static double histo[2][NSCORES+1];
static long ntran[2];

static double LastSampleRate = 0.;

void printHist(char * fileName)
{ FILE * fpt;
  int i;
  
  if((fpt=fopen(fileName,"w"))==NULL)
  { fprintf(stderr,"\nError: cannot open file <%s>\n\n", fileName);
    exit(13);
  }
  fprintf(fpt,"%ld %ld\n", ntran[0], ntran[1]);
  for(i=0; i<=NSCORES; i++)
    fprintf(fpt,"%04d %5.4lf %5.4lf\n", i, 
      histo[0][i]/((double)ntran[0]), 
      histo[1][i]/((double)ntran[1]));
  fclose(fpt);
}

#define DAY_SECONDS        86400L

void research (char *Account, int bFraudacct, float Amount, int bFraudq, int Score,long Av_cred, unsigned long CstDateTime, char result, int bPrintIt, FILE* resFp, unsigned long tranDateTime, char tranType, long day90FraudDate, long day90BlockDate)

{
   #define NUMDIVM1             100
   #define INTERVAL           (1000/NUMDIVM1)
   #define NUMDIV               (NUMDIVM1+1)

   #define TRUE               1
   #define FALSE              0

   /* Method 1: Falcon running within auth system */
   /* Method 2: Falcon running one transaction behind auth system */
   /* Method 3: Falcon running at end of day */
   /* Method 4: Same as 1, except only block transactions over $50.00 */
   /* Method 5: Same as 2, except only block transactions over $50.00 */
   static struct sDivisions
      {
      int   Thresh;
      int   bTriped;
      int   NonfraudTriped;
      long  TripDay;
      float AmountLost1;                     /* Method 1 */
      float AmountLost2;                     /* Method 2 */
      float AmountLost3;                     /* Method 3 */
      float AmountLost4;                     /* Method 4 */
      float AmountLost5;                     /* Method 5 */
      double   NumFalseTrips;
      double   SumNumFalseTrips;
      int   NumValidTrips;
      int   SumWhenCaught;
      int   SumApprvWhenCaught;
      int   NumberCaught;
      int   NumFraudsInDiv;
      int   NumNonFraudsInDiv;
      int   NumFrdInDiv_FTI[10];     /* counter for number of fraud TRX in */
                                     /* division, depending on how many */
                                     /* prior frauds per account there are */
      float AmtFrdInDiv_FTI[10];     /* average dollar amount of fraud TRX in*/
                                     /* division, depending on how many */
                                     /* prior frauds per account there are */
      int   CallStatus;              /* flag indicating if we can call acct */
      int   NumFlagFraudAccts;       /* # of fraud accts flagged */        
      double   NumFlagCleanAccts;       /* # of non-fraud accts flagged */        
      double   SumNumFlagCleanAccts;       /* # of non-fraud accts flagged */        
      float AFPR;                    /* acct. false positive rate    */        
      float AcctDetectRate;          /* acct. detection rate         */        
      float AmtFraudPerDay[10];      /* amount of fraud TRX/acct per day */
      float AmtLost3PerDay[10];      /* amount lost per day of fraud */
                                     /*   using daily batch method (3) */
      float AmtL1PerMonth[12];
      float AmtL2PerMonth[12];
      float AmtL3PerMonth[12];
      int NumDPerMonth[12];
      int TripMonth;
    } Divisions[NUMDIV], Divisions_authonly[NUMDIV];

   static int   briefoutput;
   static char  AccountPast[KEYLENMAX+1] = "";
   static int   bFraudqPast = FALSE;
   static int   bFirstPass = TRUE;
   static int   NumFraudAuths = 0;
   static int   NumCleanAuths = 0;
   static int   SumNumCleanAuths = 0;
   static float SumFraudStartCredit = 0.0;
   static int   NumValidAvCredits = 0;
   static int   NumFraudAccounts = 0;
   static int   NumOfFraudsSinceFirst = 0;
   static int   NumOfApprvFraudsSinceFirst = 0;
   static int   TotalNumAccounts = 0;

   static int   NumCleanAccounts = 0;
   static int   SumNumCleanAccounts = 0;
   static long  DayOfFirstFraud = 0;
   static int   imonth=1;
   static int   icmonth=1;
   static float  Atmp1[NUMDIV];
   static float  Atmp2[NUMDIV];
   static float  Atmp3[NUMDIV];

   long Day,tranDay;
   int i,j,ii;
   int dumInt;
   float nfrd_rat, nfrd_apprv_rat;

    float ftmp2;
    float   NumCleanTRXPerCleanAcct;
    float   NumFraudTRXPerFraudAcct;
    float   NumCleanAcctPerFraudAcct;
    int idayoffraud,month90,yeartmp,daytmp;

    briefoutput = (bPrintIt == 2 ? 1: 0);

    if (bPrintIt) {
      float PercentFalseAssigns;
      float PercentValidAssigns;
      float FPosRate;

      SumNumCleanAuths += NumCleanAuths;

      RealNumCleanAccounts += (TotalNumAccounts - NumFraudAccounts);
      SumNumCleanAccounts += (TotalNumAccounts - NumFraudAccounts);

      RealNumFraudAccounts += NumFraudAccounts;

      for (i=0; i<NUMDIV; i++)
      {
        Divisions[i].SumNumFalseTrips += Divisions[i].NumFalseTrips;
        Divisions[i].SumNumFlagCleanAccts += Divisions[i].NumFlagCleanAccts;
      }

      NumCleanAuths = SumNumCleanAuths;
      NumCleanAccounts = SumNumCleanAccounts;
      NumFraudAccounts = RealNumFraudAccounts;

      TotalNumAccounts = RealNumCleanAccounts+NumFraudAccounts;

      for (i=0; i<NUMDIV; i++)
      {
        Divisions[i].NumFalseTrips = Divisions[i].SumNumFalseTrips;
        Divisions[i].NumFlagCleanAccts = Divisions[i].SumNumFlagCleanAccts;
      }
     
      if ( NumCleanAuths+NumFraudAuths == 0 )
      {
	fprintf(resFp,"\n\n");
	fprintf(resFp,"==============================================================================\n");
	fprintf(resFp,"    Error in research - no auths found at all. Check the profile period\n");
	fprintf(resFp,"                        or some other selection criteria\n");
	fprintf(resFp,"==============================================================================\n\n");
	return;
      }

      /* print table of trx false positive and detection rates */

      fprintf(resFp,"\nTRANSACTION-BASED INFORMATION:\n");

      fprintf(resFp,"Total Authorizations = %7d, Fraud Authorizations = %7d, Approved Fraud Auths = %7d\n",
             (RealNumCleanAuths+NumFraudAuths), NumFraudAuths, NumApprvFraudAuths);

      if ( NumFraudAuths == 0 )
      {
        fprintf(resFp,"\n\n");
        fprintf(resFp,"==============================================================================\n");
        fprintf(resFp,"  Problem in research - no fraud auths found at all. Check the profile period\n");
        fprintf(resFp,"                        or fraud type or some other selection criteria\n");
        fprintf(resFp,"==============================================================================\n\n");
        return;
      }

      fprintf(resFp,"\n");
      fprintf(resFp,"  (NOTE: TFPR assumes that NonFraud Accounts were sampled at rates shown above)\n");

      if (NumFraudAuths == 0)
	nfrd_rat = 0;
      else 
	nfrd_rat = (float)NumCleanAuths / (float)NumFraudAuths;

      if (NumApprvFraudAuths == 0)
	nfrd_apprv_rat = 0;
      else 
	nfrd_apprv_rat = (float)NumCleanAuths / (float)NumApprvFraudAuths;

      fprintf(resFp,"\n");
      fprintf(resFp,"  This corresponds to %7.1f  NonFraud trx per Fraud trx and\n"
                    "                      %7.1f  NonFraud trx per Approved Fraud trx\n\n", nfrd_rat, nfrd_apprv_rat);

	fprintf(resFp,"--------------------------------------------------------------------------\n");
	fprintf(resFp,"    For Approved Fraud Authorizations ............                        \n");
	fprintf(resFp,"--------------------------------------------------------------------------\n");
        fprintf(resFp,"\n");

	fprintf(resFp,"  Maximum number of days of fraud          = %5i\n",max_days_fraud);

	mean_days_fraud = (double)sum_days_fraud/(double)NumFraudAuths;
	fprintf(resFp,"\n Transaction-Based Numbers --------------\n");
	fprintf(resFp,"  Mean number of days of fraud transactions  = %5.1f\n",mean_days_fraud);

	sdev_days_fraud = (double)sum_days_fraud_sq/(double)NumFraudAuths - (double)mean_days_fraud*(double)mean_days_fraud;
	sdev_days_fraud = (double)sqrt((double)sdev_days_fraud);
	fprintf(resFp,"  Standard Dev. of number of days of fraud   = %5.1f\n",sdev_days_fraud);

	mean_days_fraud = (double)sum_acct_days_fraud/(double)NumFraudAccounts;
	fprintf(resFp,"\n Account-Based Numbers ------------------\n");
	fprintf(resFp,"  Mean number of days of fraud accounts      = %5.1f\n",mean_days_fraud);

        for (j=1;j<500;j++)
        {
/***************************************************************
          fprintf(stderr,"  j = %3i  AcctsNumDaysFraud = %6i  NumFraudAccounts = %6i\n",j,AcctsNumDaysFraud[j],NumFraudAccounts);
***************************************************************/
          SumAcctsNumDaysFraud += AcctsNumDaysFraud[j];
          median_days_fraud = j;
          if ( SumAcctsNumDaysFraud >= NumFraudAccounts/2 )
            break;
        }
	fprintf(resFp,"  Median number of days of fraud accounts    = %3i\n",median_days_fraud);

	sdev_days_fraud = (double)sum_acct_days_fraud_sq/(double)NumFraudAccounts - (double)mean_days_fraud*(double)mean_days_fraud;
	sdev_days_fraud = (double)sqrt((double)sdev_days_fraud);
	fprintf(resFp,"  Standard Dev. of number of days of fraud   = %5.1f\n",sdev_days_fraud);
	fprintf(resFp,"--------------------------------------------------------------------------\n");

      fprintf(resFp,"\n");
      fprintf(resFp,"--------------------------------------------------------------------------\n");
      fprintf(resFp,"    For All Fraud Authorizations - Approved/Declined/Referral/Pickup      \n");
      fprintf(resFp,"--------------------------------------------------------------------------\n");

      fprintf(resFp,"------------------------------------------------------------\n");
      fprintf(resFp," Threshold     False Assigns   Valid Assigns          TFPR\n");
      fprintf(resFp,"------------------------------------------------------------\n");
      for (i=0; i<NUMDIV; i++) {
	PercentFalseAssigns =
	  (float)Divisions[i].NumFalseTrips / (float)NumCleanAuths;
	PercentValidAssigns =
	  (float)Divisions[i].NumValidTrips / (float)NumFraudAuths;

	if (PercentValidAssigns > 0) {
	  FPosRate  = ( (float)Divisions[i].NumFalseTrips)/
	                (float) Divisions[i].NumValidTrips;
	}
	else
	  FPosRate  = LARGE_NUM;

	fprintf(resFp,"   %5d         %7.3f%%         %7.3f%%         %9.3f\n",
	       Divisions[i].Thresh, 100.0*PercentFalseAssigns,
	       100.0*PercentValidAssigns, FPosRate);

      }   /* end for */

      fprintf(resFp,"\n\nTotal Number of Accounts: %d\n",
	     TrueNumAccounts);
      fprintf(resFp,"Total Number of Fraudulent Accounts: %d\n\n",
	     NumFraudAccounts);

      if (!briefoutput) {
	for (i=0; i<NUMDIV; i++) {
	  fprintf(resFp,"Threshold =%5d  Num Fraud Accts Caught%5d"
		 "    %% Fraud Accts Caught %5.1f%%\n",
		 Divisions[i].Thresh,
		 Divisions[i].NumberCaught,
		 100.0*((float)Divisions[i].NumberCaught/
			(float)NumFraudAccounts ));
	
	}  /* end for */

	fprintf(resFp,"\n\nAverage number of Fraudulent transactions before"
	       " Fraud is caught: \n(using only those which are caught"
	       " within user set no. of days)\n\n");
	fprintf(resFp,"Number of Fraud Days = %5d\n",dataSelect.daysOfFraud);
	
	for (i=0; i<NUMDIV; i++){
	  fprintf(resFp,"Threshold =%5d  "
		 "Avg Num Frauds before caught %6.3f\n",
		 Divisions[i].Thresh,
		 (Divisions[i].NumberCaught) ?
		 ((float)Divisions[i].SumWhenCaught/
		  (float)Divisions[i].NumberCaught) : (float)0.0 );

	}  /* end for */
      
	fprintf(resFp,"\n\nAverage number of Approved Fraudulent transactions before"
	       " Fraud is caught: \n(using only those which are caught"
	       " within user set no. of days)\n\n");
	fprintf(resFp,"Number of Fraud Days = %d\n",dataSelect.daysOfFraud);
	fprintf(resFp,"Threshold,  Avg Num Apprv Frauds before caught\n");

	for (i=0; i<NUMDIV; i++) {
	  fprintf(resFp,"%5d %6.3f\n",
		 Divisions[i].Thresh,
		 (Divisions[i].NumberCaught) ?
		 ((float)Divisions[i].SumApprvWhenCaught/
		  (float)Divisions[i].NumberCaught) : (float)0.0 );
	}  /* end for */

        fprintf(resFp,"\n\n");
        fprintf(resFp,"===================================================================================");
        fprintf(resFp,"\n    Division Break Down:\n");
        fprintf(resFp,"===================================================================================");
        fprintf(resFp,"\n\n");
        for (i=0; i<NUMDIV; i++) {
          dumInt = Divisions[i].NumFraudsInDiv
            + Divisions[i].NumNonFraudsInDiv;
          if (!dumInt)
            dumInt = 1;
         
          fprintf(resFp,"Threshold = %4i    Frauds %8i    Non Frauds %9i    Ratio %8.2f\n",
		 Divisions[i].Thresh, Divisions[i].NumFraudsInDiv,
		 Divisions[i].NumNonFraudsInDiv,
		 (float)(1000.0*(float)Divisions[i].NumFraudsInDiv /
			 (float)dumInt )
		 );
	  
	}  /* end for */

	fprintf(resFp,"\n\n     TRX NUMBER BREAK DOWN\n");
	fprintf(resFp,"\n\nDivision Break Down Based on\n");
	fprintf(resFp,"Number of Frauds TRX into account:\n\n");
	fprintf(resFp," \n");
	fprintf(resFp," Bin Bndry   1st   2nd   3rd   4th   5th   6th   7th   8th   9th   10th+ \n");
	fprintf(resFp,"---------------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++) {
	  fprintf(resFp,"     %5i %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
		  Divisions[i].Thresh,
		  Divisions[i].NumFrdInDiv_FTI[0],
		  Divisions[i].NumFrdInDiv_FTI[1],
		  Divisions[i].NumFrdInDiv_FTI[2],
		  Divisions[i].NumFrdInDiv_FTI[3],
		  Divisions[i].NumFrdInDiv_FTI[4],
		  Divisions[i].NumFrdInDiv_FTI[5],
		  Divisions[i].NumFrdInDiv_FTI[6],
		  Divisions[i].NumFrdInDiv_FTI[7],
		  Divisions[i].NumFrdInDiv_FTI[8],
		  Divisions[i].NumFrdInDiv_FTI[9]);

	}  /* end for */

	fprintf(resFp,"\n\n     TRX AMOUNT BREAK DOWN\n");
	fprintf(resFp,"\n\nDivision Break Down Based on\n");
	fprintf(resFp,"Number of Frauds TRX into account:\n\n");
	fprintf(resFp," \n");
	fprintf(resFp," Bin Bndry     1st      2nd      3rd      4th      5th\n");
	fprintf(resFp,"----------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++)
	{
          for (j=0;j<10;j++)
	  {
	    dumInt = Divisions[i].NumFrdInDiv_FTI[j];
	    if (!dumInt)
	      dumInt = 1;
            Divisions[i].AmtFrdInDiv_FTI[j]/=((float)dumInt);
	  }
	  fprintf(resFp,"     %5i %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		  Divisions[i].Thresh,
		  Divisions[i].AmtFrdInDiv_FTI[0],
		  Divisions[i].AmtFrdInDiv_FTI[1],
		  Divisions[i].AmtFrdInDiv_FTI[2],
		  Divisions[i].AmtFrdInDiv_FTI[3],
		  Divisions[i].AmtFrdInDiv_FTI[4]);

	}  /* end for */

	fprintf(resFp,"\n\n     TRX AMOUNT BREAK DOWN\n");
	fprintf(resFp,"\n\nDivision Break Down Based on\n");
	fprintf(resFp,"Number of Frauds TRX into account:\n\n");
	fprintf(resFp," \n");
	fprintf(resFp," Bin Bndry     6th      7th      8th      9th      10th+\n");
	fprintf(resFp,"----------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++)
	{
	  fprintf(resFp,"    %5i  %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		  Divisions[i].Thresh,
		  Divisions[i].AmtFrdInDiv_FTI[5],
		  Divisions[i].AmtFrdInDiv_FTI[6],
		  Divisions[i].AmtFrdInDiv_FTI[7],
		  Divisions[i].AmtFrdInDiv_FTI[8],
		  Divisions[i].AmtFrdInDiv_FTI[9]);

          /* CAN: Reset AmtFrdInDiv_FTI values because of new intermittent */
          /* research updating scheme */
          for (j=0;j<10;j++)
	  {
	    dumInt = Divisions[i].NumFrdInDiv_FTI[j];
	    if (!dumInt)
	      dumInt = 1;
            Divisions[i].AmtFrdInDiv_FTI[j]*=((float)dumInt);
	  }

	}  /* end for */


      } /* end briefoutput */

      fprintf(resFp,"\n\nAverage Available Credit at First Fraud: $%8.2f\n",
	     (float)(SumFraudStartCredit / (float)NumValidAvCredits));

      fprintf(resFp,"\n\n   Basis Points (10000*(Approved Fraud Trx $)/All approved Trx $) =%8.3f\n",
             10000.*AmtFraudAppr/(AmtNonFraudAppr+AmtFraudAppr));


      /* new mri */

      NumCleanTRXPerCleanAcct=((float)NumCleanAuths)/((float)NumCleanAccounts);
      NumFraudTRXPerFraudAcct=((float)NumFraudAuths)/((float)NumFraudAccounts);

      if (NumFraudAccounts == 0) NumCleanAcctPerFraudAcct = 0;
      else NumCleanAcctPerFraudAcct = (float)NumCleanAccounts / (float) NumFraudAccounts;

      fprintf(resFp,"\n\nAverage Fraud Rate Statistics:\n");
      fprintf(resFp,"  Number of Non Fraud TRX per Fraud TRX ......   = %8.1f\n",nfrd_rat);
      fprintf(resFp,"  Number of Non Fraud TRX per Approved Fraud TRX = %8.1f\n",nfrd_apprv_rat);
      fprintf(resFp,"  Number of Non Fraud Accounts per Fraud Account = %8.1f\n",NumCleanAcctPerFraudAcct); 
      fprintf(resFp,"  Number of Non Fraud TRX per Non Fraud Account  = %8.1f\n",NumCleanTRXPerCleanAcct); 
      fprintf(resFp,"  Number of Fraud TRX per Fraud Account .......  = %8.1f\n",
              NumFraudTRXPerFraudAcct); 
      /* end new mri  */

      fprintf(resFp,"\n\nACCOUNT BASED INFORMATION \n"); 
      fprintf(resFp,"  AFPR = Account False Positive Rate\n");
      if(dataSelect.recontactDays<1000)
	 fprintf(resFp,"    Assumes (a) Recontact waiting period of %5d days\n",dataSelect.recontactDays);
      else
        fprintf(resFp,"    Assumes (a) Infinite recontact waiting period\n");

/******************************************
      fprintf(resFp,"            (b) %8.1f%%  NonFraud sample factor\n",dataSelect.sampleRate*100.0);
******************************************/

      fprintf(resFp,"            (c) %8.1f   NonFraud TRX per Fraud TRX \n",
                               nfrd_rat);
      fprintf(resFp,"            (d) %8.1f   NonFraud Accts per Fraud Acct\n",
                               NumCleanAcctPerFraudAcct);
      fprintf(resFp,"  ADR  = Account Detection Rate in Percent\n");
      fprintf(resFp,"  Average Loss Per Fraudulent Account in Dollars\n");
      fprintf(resFp,"    Number of Days of Fraud = %5d\n",dataSelect.daysOfFraud);
      fprintf(resFp,"    Case 1: Falcon running within auth system\n");
      fprintf(resFp,"    Case 2: Falcon running one transaction behind auth system\n");
      fprintf(resFp,"    Case 3: Falcon running at end of day\n");
      fprintf(resFp,"    Case 4: Same as 1, except only block transactions"
	      " over $50.00\n");
      fprintf(resFp,"    Case 5: Same as 2, except only block transactions"
	      " over $50.00\n\n");

      fprintf(resFp,"                               Dollars Lost\n");
      fprintf(resFp," Threshold    AFPR      ADR     Case1    Case2    Case3    Case4    Case5\n");
      fprintf(resFp,"-----------------------------------------------------------------------------\n");

      for (i=0; i<NUMDIV; i++) {

	ftmp2=(float)Divisions[i].NumFlagFraudAccts;
	ftmp2=ftmp2/((float)NumFraudAccounts);

	if(Divisions[i].NumFlagFraudAccts==0)
	  Divisions[i].AFPR=0.0;
	else
	  Divisions[i].AFPR=(float)Divisions[i].SumNumFlagCleanAccts/(float)Divisions[i].NumFlagFraudAccts;

	Divisions[i].AcctDetectRate=100.0*ftmp2;

	if (!authOnly && !postOnly)
	{
	fprintf(resFp,"  %5d   %8.2f  %8.2f  %8.2f %8.2f %8.2f %8.2f %8.2f\n",
	       Divisions[i].Thresh, Divisions[i].AFPR,  
	       Divisions[i].AcctDetectRate, 
		(Divisions[i].AmountLost1-(Divisions_authonly[i].AmountLost3-Divisions_authonly[i].AmountLost1)) / (float)NumFraudAccounts,
                (Divisions[i].AmountLost1-(Divisions_authonly[i].AmountLost3-Divisions_authonly[i].AmountLost2)) / (float)NumFraudAccounts,
                Divisions[i].AmountLost1 / (float)NumFraudAccounts,
               0.0,
               0.0);
	}
	else
	{
	fprintf(resFp,"  %5d   %8.2f  %8.2f  %8.2f %8.2f %8.2f %8.2f %8.2f\n",
	       Divisions[i].Thresh, Divisions[i].AFPR,  
	       Divisions[i].AcctDetectRate, 
               Divisions[i].AmountLost1 / (float)NumFraudAccounts,
               Divisions[i].AmountLost2 / (float)NumFraudAccounts,
               Divisions[i].AmountLost3 / (float)NumFraudAccounts,
               Divisions[i].AmountLost4 / (float)NumFraudAccounts,
               Divisions[i].AmountLost5 / (float)NumFraudAccounts);
	} 
      }  /* end for */

      if (!briefoutput) {
	fprintf(resFp,"\n\n ACCOUNT INFO FOR DAY INTO FRAUD\n");
	fprintf(resFp,"\n\n Amount approved and losses on days 1 through 5\n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold    Frd1    Lost1     Frd2    Lost2     Frd3    Lost3\n");
	fprintf(resFp,"--------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++)
	{
	  for (j=0;j<10;j++)
	  {
	    Divisions[i].AmtLost3PerDay[j] /=  (float)NumFraudAccounts;
	    Divisions[i].AmtFraudPerDay[j] /=  (float)NumFraudAccounts;
	  }
	  fprintf(resFp,"  %5i   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		  Divisions[i].Thresh,
		  Divisions[i].AmtFraudPerDay[0],
		  Divisions[i].AmtLost3PerDay[0],
		  Divisions[i].AmtFraudPerDay[1],
		  Divisions[i].AmtLost3PerDay[1],
		  Divisions[i].AmtFraudPerDay[2],
		  Divisions[i].AmtLost3PerDay[2]);

	}  /* end for */

	fprintf(resFp,"\n\n MORE DAYS OF FRAUD \n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold    Frd4    Lost4     Frd5    Lost5     Frd6    Lost6\n");
	fprintf(resFp,"--------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++)
	{
	  fprintf(resFp,"  %5i   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		 Divisions[i].Thresh,
		 Divisions[i].AmtFraudPerDay[3],
		 Divisions[i].AmtLost3PerDay[3],
		 Divisions[i].AmtFraudPerDay[4],
		 Divisions[i].AmtLost3PerDay[4],
		 Divisions[i].AmtFraudPerDay[5],
		 Divisions[i].AmtLost3PerDay[5]);

          /* CAN: Reset AmtLost3PerDay and AmtFraudPerDay values because */
          /* of new intermittent research updating scheme */ 
	  for (j=0;j<10;j++)
	  {
	    Divisions[i].AmtLost3PerDay[j] *=  (float)NumFraudAccounts;
	    Divisions[i].AmtFraudPerDay[j] *=  (float)NumFraudAccounts;
	  }

	}  /* end for */

	fprintf(resFp,"\n\n NUMBER OF ACCOUNTS DETECTED PER MONTH\n");
	fprintf(resFp,"\n\n    January Through June\n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold     Jan      Feb    March    April      May     June\n");
	fprintf(resFp,"------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++) {
	  fprintf(resFp,"  %5d   %8d %8d %8d %8d %8d %8d\n",
		 Divisions[i].Thresh,
		 Divisions[i].NumDPerMonth[0],
		 Divisions[i].NumDPerMonth[1],
		 Divisions[i].NumDPerMonth[2],
		 Divisions[i].NumDPerMonth[3],
		 Divisions[i].NumDPerMonth[4],
		 Divisions[i].NumDPerMonth[5]);
	}  /* end for */

	fprintf(resFp,"\n\n NUMBER OF ACCOUNTS DETECTED PER MONTH\n");
	fprintf(resFp,"\n\n    July Through December \n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold    July      Aug     Sept      Oct      Nov      Dec\n");
	fprintf(resFp,"------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++){
	  fprintf(resFp,"  %5d   %8d %8d %8d %8d %8d %8d\n",
		  Divisions[i].Thresh,
		  Divisions[i].NumDPerMonth[6],
		  Divisions[i].NumDPerMonth[7],
		  Divisions[i].NumDPerMonth[8],
		  Divisions[i].NumDPerMonth[9],
		  Divisions[i].NumDPerMonth[10],
		  Divisions[i].NumDPerMonth[11]);
	}  /* end for */

	fprintf(resFp,"\n\n DOLLARS LOST PER MONTH PER ACCOUNT\n");
	fprintf(resFp,"\n\n    January Through June\n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold     Jan      Feb    March    April      May     June\n");
	fprintf(resFp,"------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++) {
          for (j=0;j<12;j++) {
	    if(Divisions[i].NumDPerMonth[j]>0) {
	      Divisions[i].AmtL1PerMonth[j] /=  
		(float)Divisions[i].NumDPerMonth[j]; 
	      Divisions[i].AmtL2PerMonth[j] /=  
		(float)Divisions[i].NumDPerMonth[j]; 
	      Divisions[i].AmtL3PerMonth[j] /=  
		(float)Divisions[i].NumDPerMonth[j]; 
	    }
            else {
	      Divisions[i].AmtL1PerMonth[j] = 0.0;  
	      Divisions[i].AmtL2PerMonth[j] = 0.0;  
	      Divisions[i].AmtL3PerMonth[j] = 0.0;  
	    }
	  }
	  fprintf(resFp,"  %5i   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		 Divisions[i].Thresh, Divisions[i].AmtL1PerMonth[0],
		 Divisions[i].AmtL1PerMonth[1],
		 Divisions[i].AmtL1PerMonth[2],
		 Divisions[i].AmtL1PerMonth[3],
		 Divisions[i].AmtL1PerMonth[4],
		 Divisions[i].AmtL1PerMonth[5]);
	}  /* end for */

	fprintf(resFp,"\n\n DOLLARS LOST PER MONTH PER ACCOUNT\n");
	fprintf(resFp,"\n\n    July Through December \n");
	fprintf(resFp,"                               Dollars Lost\n");
	fprintf(resFp," Threshold    July      Aug     Sept      Oct      Nov      Dec\n");
	fprintf(resFp,"------------------------------------------------------------------\n");
	for (i=0; i<NUMDIV; i++) {
	  fprintf(resFp,"  %5i   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		 Divisions[i].Thresh,
		 Divisions[i].AmtL1PerMonth[6],
		 Divisions[i].AmtL1PerMonth[7],
		 Divisions[i].AmtL1PerMonth[8],
		 Divisions[i].AmtL1PerMonth[9],
		 Divisions[i].AmtL1PerMonth[10],
		 Divisions[i].AmtL1PerMonth[11]);

          /* CAN: Reset AmtLPerMonth values because of the new*/
          /* intermittent research updating scheme */ 
	  if(Divisions[i].NumDPerMonth[j]>0) 
          { for (j=0;j<12;j++) 
            {  Divisions[i].AmtL1PerMonth[j] *=  
	    	  (float)Divisions[i].NumDPerMonth[j]; 
	        Divisions[i].AmtL2PerMonth[j] *=  
	  	  (float)Divisions[i].NumDPerMonth[j]; 
	        Divisions[i].AmtL3PerMonth[j] *=  
	    	  (float)Divisions[i].NumDPerMonth[j]; 
            }
	  }
	  
	}  /* end for */

      } /* end if !briefoutput */

/* To determine maximum score */
      fprintf(resFp, "\n Maximum Score = %d\n", MaxScore);
/* End maximum score */

      return;
      
   }  /* end print stuff */


/*++++++++++++++++++ start of accumulations +++++++++++++++++++++*/

   if (result == 'A')
   {
      if (bFraudacct)
      {
        if (bFraudq)
          AmtFraudAppr += (double)Amount;
        else
          AmtNonFraudAppr += (double)Amount;
      }
      else
        AmtNonFraudAppr += (double)Amount/dataSelect.sampleRate;
   }

   if (bFirstPass)
      {
      bFirstPass = FALSE;
      LastSampleRate = dataSelect.sampleRate;
      for(i=0; i<2; i++) 
      { ntran[i]=0L;
        for(j=0; j<=NSCORES; j++)
          histo[i][j]=0.0;
      }
      for (i=0; i<NUMDIV; i++)
	  {
	  Divisions[i].Thresh  = i*INTERVAL;
	  Divisions[i].NumFalseTrips = 0;
	  Divisions[i].SumNumFalseTrips = 0;
	  Divisions[i].NumValidTrips = 0;
	  Divisions[i].SumWhenCaught = 0;
	  Divisions[i].NumberCaught = 0;
	  Divisions[i].NumFraudsInDiv = 0;
	  Divisions[i].NumNonFraudsInDiv = 0;
	  Divisions[i].AmountLost1 = 0.0;
	  Divisions[i].AmountLost2 = 0.0;
	  Divisions[i].AmountLost3 = 0.0;
	  Divisions[i].AmountLost4 = 0.0;
	  Divisions[i].AmountLost5 = 0.0;
	  Divisions[i].AFPR = 0.0; 
	  Divisions[i].AcctDetectRate = 0.0; 
	  Divisions[i].CallStatus = TRUE;
	  Divisions[i].NonfraudTriped = FALSE;
	  Divisions[i].NumFlagFraudAccts = 0;
	  Divisions[i].NumFlagCleanAccts = 0;
	  Divisions[i].SumNumFlagCleanAccts = 0;

	  Divisions_authonly[i].AmountLost1 = 0.0;
	  Divisions_authonly[i].AmountLost2 = 0.0;
	  Divisions_authonly[i].AmountLost3 = 0.0;
	  Divisions_authonly[i].AmountLost4 = 0.0;
	  Divisions_authonly[i].AmountLost5 = 0.0;

          for(j=0;j<12;j++)
            {
	    Divisions[i].NumDPerMonth[j] = 0;
            }
          for(j=0;j<10;j++)
            {
            Divisions[i].AmtLost3PerDay[j]=0.0;
            Divisions[i].AmtFraudPerDay[j]=0.0;
	    Divisions[i].NumFrdInDiv_FTI[j] = 0;
	    Divisions[i].AmtFrdInDiv_FTI[j] = 0.0;
            }
          for(j=0;j<10;j++)
            {
            Divisions[i].AmtL1PerMonth[j]=0.0;
            Divisions[i].AmtL2PerMonth[j]=0.0;
            Divisions[i].AmtL3PerMonth[j]=0.0;
            }

          Atmp1[i]=0.0;
          Atmp2[i]=0.0;
          Atmp3[i]=0.0;

          for (j=0;j<500;j++)
            AcctsNumDaysFraud[j]=0;
	  }
      } /*matches if bfirstPass*/

   if (strncmp(Account,AccountPast,KEYLENMAX) != 0)  /* new account */
      {
        if (tranType==ISPOST)
	{
          Day = (long)(CstDateTime / DAY_SECONDS);
          if ((Day<day90BlockDate && bFraudq) || !bFraudq)
	  {
            TrueNumAccounts++;
	    /*fprintf(stderr,"%16.16s\n",Account);*/
	  }
        }
        else
          TrueNumAccounts++;

	if ( LastSampleRate != dataSelect.sampleRate)
	{
	  SumNumCleanAuths += NumCleanAuths;

	  RealNumCleanAccounts += (TotalNumAccounts - NumFraudAccounts);
	  RealNumFraudAccounts += NumFraudAccounts;
	  SumNumCleanAccounts += (TotalNumAccounts - NumFraudAccounts);

	  for (i=0; i<NUMDIV; i++)
	  {
	    Divisions[i].SumNumFalseTrips += Divisions[i].NumFalseTrips;
	    Divisions[i].SumNumFlagCleanAccts += Divisions[i].NumFlagCleanAccts;
	  }

          LastSampleRate = dataSelect.sampleRate;
	  NumCleanAuths = 0;
	  TotalNumAccounts = 0;
	  NumFraudAccounts = 0;

          for (i=0; i<NUMDIV; i++)
	  {
	    Divisions[i].NumFalseTrips = 0;
	    Divisions[i].NumFlagCleanAccts = 0;
	  }
	}

      strncpy(AccountPast, Account, KEYLENMAX);
      AccountPast[KEYLENMAX] = '\0';
      sum_acct_days_fraud += acct_days_fraud;
      sum_acct_days_fraud_sq += acct_days_fraud*acct_days_fraud;

      if (acct_days_fraud > 499 )
	acct_days_fraud = 499;
      AcctsNumDaysFraud[acct_days_fraud]++;
      
      acct_days_fraud = 0;

      if(bFraudqPast)
      {
        for (i=0; i<NUMDIV; i++)
        {
          if(Divisions[i].bTriped)
          {
            imonth=Divisions[i].TripMonth;
            Divisions[i].AmtL1PerMonth[imonth-1]+=Atmp1[i];
            Divisions[i].AmtL2PerMonth[imonth-1]+=Atmp2[i];
            Divisions[i].AmtL3PerMonth[imonth-1]+=Atmp3[i];
            Divisions[i].NumDPerMonth[imonth-1]++;
            Atmp1[i]=0.0;
            Atmp2[i]=0.0;
            Atmp3[i]=0.0;
          }
        }
      }
      bFraudqPast = FALSE;

      if (bFraudacct)
      {
        if (tranType==ISPOST)
	{
          Day = (long)(CstDateTime / DAY_SECONDS);
          if (Day<day90BlockDate)
            TotalNumAccounts++;
        }
        else
          TotalNumAccounts++;
      }
      else
        TotalNumAccounts +=  (int) (1./LastSampleRate);

      for (i=0; i<NUMDIV; i++)
      {
	  Divisions[i].bTriped  = FALSE;
	  Divisions[i].NonfraudTriped  = FALSE;
	  Divisions[i].CallStatus = TRUE;
      }
   } /*this one matches account/accountpast comparison gg 8/27/99*/

   if(Score>=0 && Score<=NSCORES) 
     { ntran[bFraudq]++; histo[bFraudq][Score]++; }

   if (bFraudqPast)
       bFraudq = TRUE;  /* once a fraud, always a fraud */

   i =  Score/(INTERVAL);
   if (bFraudq)
   {
        if (tranType==ISPOST)
	{
          Day = (long)(CstDateTime / DAY_SECONDS);
          if (Day<day90BlockDate)
            Divisions[i].NumFraudsInDiv++;
        }
	else
           Divisions[i].NumFraudsInDiv++;
   }
   else
       Divisions[i].NumNonFraudsInDiv++;

   if (bFraudq)
   {
      Day = (long)(CstDateTime / DAY_SECONDS);
      GetYrMoDayFromDay90(Day,&month90,&yeartmp,&icmonth,&daytmp);
      tranDay=(long)(tranDateTime / DAY_SECONDS);
      for (ii=0; ii<NUMDIV; ii++)
        {
        if( (Divisions[ii].NonfraudTriped == TRUE )&&(Day >= Divisions[ii].TripDay+dataSelect.recontactDays) )
          {
           Divisions[ii].bTriped  = FALSE;
           Divisions[ii].NonfraudTriped  = FALSE;
           Divisions[ii].CallStatus  = TRUE;
          }
        }

      if (result == 'A') /* only count as loss on approves */
      {
        NumApprvFraudAuths++;
        if (days_of_fraud > max_days_fraud)
  	  max_days_fraud = days_of_fraud;
  	acct_days_fraud = days_of_fraud;
        sum_days_fraud += days_of_fraud;
        sum_days_fraud_sq += days_of_fraud*days_of_fraud;
      }

      if (tranType==ISPOST)
      {
        if (Day<day90BlockDate)
          NumFraudAuths++;
      }
      else
        NumFraudAuths++;

      if (tranType==ISPOST)
      {
        if (!bFraudqPast && Day<day90BlockDate)/* first fraud of the account */
	 {
	  NumFraudAccounts++;
	  NumOfFraudsSinceFirst = 0;
	  NumOfApprvFraudsSinceFirst = 0;

          DayOfFirstFraud=Day;
	  if (Av_cred != LONG_MIN)
	     {
	     SumFraudStartCredit += (float)Av_cred;
	     NumValidAvCredits++;
	     }
	 }
      }
      else if (!bFraudqPast)       /* first fraud of the account */
      {
	  NumFraudAccounts++;
	  NumOfFraudsSinceFirst = 0;
	  NumOfApprvFraudsSinceFirst = 0;

          DayOfFirstFraud=Day;
	  if (Av_cred != LONG_MIN)
	     {
	     SumFraudStartCredit += (float)Av_cred;
	     NumValidAvCredits++;
	     }
      }
      bFraudqPast = TRUE;

      NumOfFraudsSinceFirst += 1;    /* actually, number of frauds so far */
      if(result== 'A') NumOfApprvFraudsSinceFirst += 1;  
 
      if(bFraudq)
        {
        dumInt=NumOfFraudsSinceFirst-1;
        if(dumInt>9) dumInt=9;
        Divisions[i].NumFrdInDiv_FTI[dumInt]++;
        Divisions[i].AmtFrdInDiv_FTI[dumInt]+=Amount;
        }

      if ((result == 'A') && (tranType!=ISPOST)) /* only count as loss on approves */
      {  
	   /*it is not a post (assume auth), and now we decide if we only have
	     auths, or if we are processing auths and posts*/
	   if (authOnly)
	   {
	   for (i=0; i<NUMDIV; i++)    /* method 2, method 5 */
  	   {
	   if (!Divisions[i].bTriped || Divisions[i].NonfraudTriped)
	      {
	      Divisions[i].AmountLost2 += Amount;    /* method 2 */
	      Divisions[i].AmountLost5 += Amount;    /* method 5 */
              Atmp2[i]+=Amount;
	      }
	   else if(Amount < 50.00)
	      {
	      Divisions[i].AmountLost5 += Amount;    /* method 5 */
	      }
    	   }  /* end for */
	   }
	   else
	   {
	   /*so, we have auths and posts*/
	   for (i=0; i<NUMDIV; i++)    /* method 2, method 5 */
  	   {
	   if (!Divisions[i].bTriped || Divisions[i].NonfraudTriped)
	      {
	      Divisions_authonly[i].AmountLost2 += Amount;    /* method 2 */
	      Divisions_authonly[i].AmountLost5 += Amount;    /* method 5 */
              /*Atmp2[i]+=Amount;*/
	      }
	   else if(Amount < 50.00)
	      {
	      Divisions_authonly[i].AmountLost5 += Amount;    /* method 5 */
	      }
    	   }  /* end for */
	   }
      }
      if (tranType==ISPOST)
      {
         if (Day<day90BlockDate)
         for (i=0; i<NUMDIV; i++)
	 {
	   if (Score  >=  Divisions[i].Thresh && !Divisions[i].NonfraudTriped )
	    {
	    Divisions[i].NumValidTrips++;
	    if (!Divisions[i].bTriped)
		{
		Divisions[i].bTriped  = TRUE;
		Divisions[i].TripDay = Day;
                GetYrMoDayFromDay90(Day,&month90,&yeartmp,&imonth,&daytmp);
                Divisions[i].TripMonth = imonth;
		Divisions[i].NumberCaught += 1;
		Divisions[i].SumWhenCaught += NumOfFraudsSinceFirst;
		Divisions[i].SumApprvWhenCaught += NumOfApprvFraudsSinceFirst;
		}
	    }
	 }  /* end for */
      }/*matches ISPOST*/
      else /*tran is not a post*/
      {
      for (i=0; i<NUMDIV; i++)
	 {
	 if (Score  >=  Divisions[i].Thresh && !Divisions[i].NonfraudTriped )
	    {
	    Divisions[i].NumValidTrips++;
	    if (!Divisions[i].bTriped)
		{
		Divisions[i].bTriped  = TRUE;
		Divisions[i].TripDay = Day;
                GetYrMoDayFromDay90(Day,&month90,&yeartmp,&imonth,&daytmp);
                Divisions[i].TripMonth = imonth;
		Divisions[i].NumberCaught += 1;
		Divisions[i].SumWhenCaught += NumOfFraudsSinceFirst;
		Divisions[i].SumApprvWhenCaught += NumOfApprvFraudsSinceFirst;
		}
	    }
	 }
      }

      if (result == 'A') /* only count as loss on approves */
      {  
	   if (tranType!=ISPOST)
	   {
	   if (authOnly)
	   {
	   for (i=0; i<NUMDIV; i++)    /* method 1, method 4 */
	   {
           if (!Divisions[i].bTriped || Divisions[i].NonfraudTriped)
	      {
	      Divisions[i].AmountLost1 += Amount;    /* method 1 */
	      Divisions[i].AmountLost4 += Amount;    /* method 4 */
              Atmp1[i]+=Amount;
	      }
	   else if(Amount < 50.00)
	      {
	      Divisions[i].AmountLost4 += Amount;    /* method 4 */
	      }
	   }  /* end for */
	   }
	   else
	   {
	   for (i=0; i<NUMDIV; i++)    /* method 1, method 4 */
	   {
           if (!Divisions[i].bTriped || Divisions[i].NonfraudTriped)
	      {
	      Divisions_authonly[i].AmountLost1 += Amount;    /* method 1 */
	      Divisions_authonly[i].AmountLost4 += Amount;    /* method 4 */
              /*Atmp1[i]+=Amount;*/
	      }
	   else if(Amount < 50.00)
	      {
	      Divisions_authonly[i].AmountLost4 += Amount;    /* method 4 */
	      }
	   }  /* end for */
	   }
	   }/*matches !=ISPOST*/

           for (i=0; i<NUMDIV; i++)    /* method 3 */
	   {
           /* sum all approved losses per day */
           idayoffraud = (int)(Day-DayOfFirstFraud);
           if(idayoffraud>9) idayoffraud=9;
           if(idayoffraud<0) idayoffraud=0;
           Divisions[i].AmtFraudPerDay[idayoffraud]+=Amount;

	   if (!Divisions[i].bTriped || (Day == Divisions[i].TripDay) || Divisions[i].NonfraudTriped || (tranDay<=Divisions[i].TripDay && tranType==ISPOST))
	   {
              if (tranType!=ISPOST)
	      {
	        if (authOnly)
		       Divisions[i].AmountLost3 += Amount;
		else
		       Divisions_authonly[i].AmountLost3 += Amount;
	      }

	      /* add up daily batch loss for days of fraud */
              Divisions[i].AmtLost3PerDay[idayoffraud]+=Amount;
              Atmp3[i]+=Amount;

	      if (tranType==ISPOST && tranDay<day90BlockDate && tranDay>=day90FraudDate)
	        {
		Divisions[i].AmountLost1 += Amount;    /* method 1 */
		Divisions[i].AmountLost4 += Amount;    /* method 4 */	
		Divisions[i].AmountLost2 += Amount;    /* method 2 */
		Divisions[i].AmountLost5 += Amount;    /* method 5 */ 
		Divisions[i].AmountLost3 += Amount;    /* method 3 */ 
	        }
	   }
	   }  /* end for */
      } /* end if result == 'A' */ 

      /*For each division, if the TRX is over the threshold and */
      /*the call status is true, that means that we have successfully */
      /*caught a fraud account.  Thus we increment NumFlagFraudAccts  */
      /*by 1 and set CallStatus to false so we don't count this account */
      /*again.*/

      for (i=0; i<NUMDIV; i++)    
      {
        if (Divisions[i].bTriped && Divisions[i].CallStatus && !Divisions[i].NonfraudTriped)
          {
            Divisions[i].NumFlagFraudAccts++;    
            Divisions[i].CallStatus= FALSE;    
          }
      }
   } /*THIS MATCHES if(bFraudq) way above... gg 8/31/99*/
   else  /* non fraud */
   {
      /*reset stuff if end of recontact waiting period */
      Day = (long)(CstDateTime / DAY_SECONDS);
      for (i=0; i<NUMDIV; i++)
        {
        if( (Divisions[i].bTriped == TRUE )&&(Day >= Divisions[i].TripDay+dataSelect.recontactDays) )
          {
           Divisions[i].bTriped  = FALSE;
           Divisions[i].NonfraudTriped = FALSE;
           Divisions[i].CallStatus  = TRUE; 
          }
        }
      RealNumCleanAuths++;
      if ( bFraudacct )
        NumCleanAuths++;
      else
        NumCleanAuths += (int) (1./LastSampleRate);

      for (i=0; i<NUMDIV; i++)
	 {
	 if (Score >= Divisions[i].Thresh)
	    {
              if (bFraudacct)
	        Divisions[i].NumFalseTrips += 1.0;
	      else
	        Divisions[i].NumFalseTrips += (1./LastSampleRate);

	      /*if score > threshold AND CallStatus is true then */
	      /*we have flagged a non-fraud trx.  In this case, we */
	      /*increment NumFlagCleanAccts by 1 and set CallStatus */
	      /*to zero so we don't make the same mistake with */
	      /*this account */

            if (Divisions[i].CallStatus)
              {
		if (bFraudacct)
                  Divisions[i].NumFlagCleanAccts += 1.0;    
		else
                  Divisions[i].NumFlagCleanAccts += (1./LastSampleRate);    
                Divisions[i].CallStatus= FALSE;    
                Divisions[i].bTriped  = TRUE; 
                Divisions[i].NonfraudTriped  = TRUE; 
                Divisions[i].TripDay  = Day;  
	      }
	    }
	 }
   }
}

/***************************************************************************/
void FraudTypePerformance (char FraudType, int bFraudq, int Score,
			   int bPrintIt, FILE* resFp)
{
   #define MINTHRESH          400
   #define MAXTHRESH          990
   #define INTERVAL2           10
   #define NUMDIV2            (1 + (MAXTHRESH - MINTHRESH) / INTERVAL2)
   #define TRUE                 1
   #define FALSE                0

   static int   ThreshArray[NUMDIV2];

   typedef struct sFRAUDTYPE
      {
      char  FraudType;
      int   ValidAssigns[NUMDIV2];
      int   NumCalls;
      struct sFRAUDTYPE *next;
      } FRAUDTYPE;

   static int   bFirstPass = TRUE;
   static  FRAUDTYPE *FirstType = (FRAUDTYPE *)NULL;
   static  FRAUDTYPE *TypeLast = (FRAUDTYPE *)NULL;

   FRAUDTYPE *Type = (FRAUDTYPE *)NULL;

   int i,ii,istart,iend;

   long  Nsum = 0;
   long  NsumValid[101];


   if (bFirstPass)
      {
      bFirstPass = FALSE;
      for (i=0; i<NUMDIV2; i++)
	 {
	 ThreshArray[i] = MINTHRESH + i*INTERVAL2;
	 }
      }

/*****************************************************************/
/*    Added more detail to type of fraud    jcb    3/6/97        */
/*****************************************************************/

   if (bPrintIt)
      {
         fprintf(resFp,"\n-------------------------------------------------------------------------------------------------------\n");
         fprintf(resFp,"\n Percent successful Fraud Detection for Various\n"
		           " Types of Frauds (using different Thresholds):\n");
         for (ii=0; ii < 4; ii++)
         {
           fprintf(resFp,"\n              --------------------------------------- Threshold ---------------------------------------\n");
           fprintf(resFp," Type    Num");
	   istart = ii*15;
	   iend = istart+15;

           for (i=istart; i<iend; i++)
	 	fprintf(resFp,"%6d",ThreshArray[i]);
           fprintf(resFp,"\n-------------------------------------------------------------------------------------------------------\n");
           Type = FirstType;
	   Nsum = 0;
	   for (i=istart; i<iend; i++)
	     NsumValid[i] = 0;
           while (Type != (FRAUDTYPE *)NULL)
	   {
	      fprintf(resFp,"  %c  %7i",Type->FraudType, Type->NumCalls);
	      Nsum += Type->NumCalls;
	      for (i=istart; i<iend; i++)
	      {
	         fprintf(resFp,"%6.1f",(float)(100.0*(float)Type->ValidAssigns[i]/(float)Type->NumCalls));
		 NsumValid[i] += Type->ValidAssigns[i];
	      }
	      fprintf(resFp,"\n");
	      Type = Type->next;
	   }
	   fprintf(resFp,"Tot  %7i",Nsum);
	   for (i=istart; i<iend; i++)
	     fprintf(resFp,"%6i",NsumValid[i]);
	   fprintf(resFp,"\n");
         }
         fprintf(resFp,"\n-------------------------------------------------------------------------------------------------------\n");
         return;
      }   /* endif for printit */

   if (!bFraudq) return;       /* must have a fraud to work */

   Type = FirstType;
   while (Type != (FRAUDTYPE *)NULL)
      {
      if (FraudType == Type->FraudType)
	 break;                                       /* found it */
      Type = Type->next;
      }

   if (Type != (FRAUDTYPE *)NULL)  /* then found it */
      {
      Type->NumCalls++;
      for (i=0; i<NUMDIV2; i++)
	 {
	 if (Score > ThreshArray[i])
	    Type->ValidAssigns[i]++;
	 }
      }
   else                            /* new type */
      {
      if (FirstType == (FRAUDTYPE *)NULL)
	 {

	 FirstType = (FRAUDTYPE *)malloc(sizeof(FRAUDTYPE));
	 Type = FirstType;
	 TypeLast = FirstType;
	 }
      else
	 {
	 TypeLast->next = (FRAUDTYPE *)malloc(sizeof(FRAUDTYPE));
	 TypeLast = TypeLast->next;
	 Type = TypeLast;
	 }

      if (Type == (FRAUDTYPE *)NULL)
	 {
	 fprintf(stderr, "Ran out ot memory in FraudTypePerformance\n");
	 return;
	 }
      Type->next = (FRAUDTYPE *)NULL;
      Type->NumCalls = 1;
      for (i=0; i<NUMDIV2; i++)
	 {
	 Type->ValidAssigns[i] = 0;
	 if (Score > ThreshArray[i])
	     Type->ValidAssigns[i]++;
	 }

      Type->FraudType = FraudType;

      }   /* end of new fraud type */

}
/***************************************************************************/

void AmountLostWithDelay(char *Account, float Amount, int bFraudq, int Score,
                  long Av_cred, unsigned long CstDateTime, int bPrintIt, FILE* resFp)
{
   #define DAY_SECONDS        86400L
   #define HOUR_SECONDS       3600L
   #define NUMDIV3            100
   #define INTERVAL3          (1000/NUMDIV3)
   #define TRUE               1
   #define FALSE              0

   /* The following structure MUST be filled with increasing HourLatched */
   /* HourLatched is the time at which the data is saved to disk or tape */
   /* HourAvailable is the hour at which the data becomes available      */
   static struct sFraudLatch
     {
     int           HourLatched;
     int           HourAvailable;
     int           NeedsToBeLookedAt;
     int           WasAFraud;
     unsigned long DateTimeAvailable;
     } FraudLatch[] = {
      {18, 21, FALSE, FALSE, ULONG_MAX} };
/*                      {                      */
/*      {0,   3, FALSE, FALSE, ULONG_MAX},     */
/*      {12, 15, FALSE, FALSE, ULONG_MAX},     */
/*      {18, 21, FALSE, FALSE, ULONG_MAX} };   */
   #define  MAXLATCH    (sizeof(FraudLatch)/sizeof(struct sFraudLatch))

   static struct sDivisions
      {
      int           Thresh;
      int           bFraudDetected[MAXLATCH];
      int           bFraudCaught;
      float         AmountLost;
      int           SumWhenCaught;
      int           NumberCaught;
      int           bFirstCatch;
      } Divisions[NUMDIV3];

   static char  AccountPast[KEYLENMAX+1] = "";
   static int   bFraudqPast = FALSE;
   static int   bFirstPass = TRUE;
   static int   NumFraudAuths = 0;
   static int   NumCleanAuths = 0;
   static float SumFraudStartCredit = 0.0;
   static int   NumValidAvCredits = 0;
   static int   NumFraudAccounts = 0;
   static int   NumOfFraudsSinceFirst = 0;
   static int   TotalNumAccounts = 0;

   unsigned long Date;
   unsigned long DateTimeBeginDay;
   unsigned long SecondsIntoDay;
   unsigned long TimeAdjuster;

   int i, j;
   int dumInt;
   int iLatch;


   if (bPrintIt)
      {

      if (bFirstPass)
	 {
	 fprintf(resFp,"Error in AmountLostWithDelay\n");
	 return;
	 }


      fprintf(resFp,"\n\n\n ***** Output from AmountLostWithDelay  *****\n\n");

      fprintf(resFp,"Total Authorizations = %7d, Fraud Authorizations = %7d, Approved Fraud Auths = %7d\n",
             (RealNumCleanAuths+NumFraudAuths), NumFraudAuths, NumApprvFraudAuths);

      fprintf(resFp,"\n\nTotal Number of Accounts: %d\n", TotalNumAccounts);
      fprintf(resFp,"Total Number of Fraudulent Accounts: %d\n\n", NumFraudAccounts);


      fprintf(resFp,"\n\nAverage number of Fraudulent transactions before"
	     " Fraud is caught: \n(using only those which are caught"
	     " within %5d days)\n\n",dataSelect.daysOfFraud);

      for (i=0; i<NUMDIV3; i++)
	  {
	  fprintf(resFp,"Threshold =%5d  "
		 "Avg Num Frauds before caught %6.3f\n",
		 Divisions[i].Thresh,
		 (Divisions[i].NumberCaught) ?
		  ((float)Divisions[i].SumWhenCaught/
		  (float)Divisions[i].NumberCaught)
		  : (float)0.0 );

	  }  /* end for */


      fprintf(resFp,"\n\nAverage Available Credit at First Fraud: $%8.2f\n",
	     (float)(SumFraudStartCredit / (float)NumValidAvCredits));

      fprintf(resFp,"\n\nAverage Loss Per Fraudulent Account in %5d day period\n\n",
              dataSelect.daysOfFraud);

      fprintf(resFp," Threshold     AmountLost\n");
      fprintf(resFp,"---------------------------\n");
      for (i=0; i<NUMDIV3; i++)
	  {
/*
	  Divisions[i].AmountLost  /=  (float)NumFraudAccounts;
*/

	  fprintf(resFp,"  %5d   "
		 "   %10.2f\n",
		  Divisions[i].Thresh,
		  Divisions[i].AmountLost / (float)NumFraudAccounts);

	  }  /* end for */

      return;

      }  /* end print stuff */


   /*++++++++++++++++++ start of accumulations +++++++++++++++++++++*/

   if (bFirstPass)
      {
      bFirstPass = FALSE;
      for (i=0; i<NUMDIV3; i++)
	  {

	  Divisions[i].Thresh  = i*INTERVAL3;
	  Divisions[i].SumWhenCaught = 0;
	  Divisions[i].NumberCaught = 0;
	  Divisions[i].AmountLost = 0.0;
	  }
      }

   if (strncmp(Account,AccountPast,KEYLENMAX) != 0)  /* new account */
      {
      strncpy(AccountPast, Account, KEYLENMAX);
      AccountPast[KEYLENMAX] = '\0';
      bFraudqPast = FALSE;
      TotalNumAccounts++;
      for (i=0; i<NUMDIV3; i++)
	  {
	  Divisions[i].bFirstCatch = TRUE;
	  Divisions[i].bFraudCaught = FALSE;
	  for (j=0; j< MAXLATCH; j++)
	     {
	     Divisions[i].bFraudDetected[j] = FALSE;
	     }
	  }
      for (j=0 ; j< MAXLATCH; j++)
	 {
	     FraudLatch[j].DateTimeAvailable = ULONG_MAX;
	     FraudLatch[j].NeedsToBeLookedAt = FALSE;
	 }

      }

   if (bFraudqPast)
       bFraudq = TRUE;  /* once a fraud, always a fraud */


   Date = (unsigned long)(CstDateTime / DAY_SECONDS);
   DateTimeBeginDay = Date * DAY_SECONDS;
   SecondsIntoDay = CstDateTime - DateTimeBeginDay;



   /* Check to see if any of the past data should have already been  */
   /* available by this point in time. If so process the data        */
   for (j=0 ; j< MAXLATCH; j++)
      {
      if (FraudLatch[j].NeedsToBeLookedAt &&
	  CstDateTime > FraudLatch[j].DateTimeAvailable)
	  {
	  FraudLatch[j].NeedsToBeLookedAt = FALSE;      /* no longer needs */
							/* to be looked at */
	  /* If was a fraud, check to see if it was detected */
	  if (FraudLatch[j].WasAFraud)
	     {
	     for (i=0; i<NUMDIV3; i++)
	       {
	       if (Divisions[i].bFraudDetected[j])      /* if was detected */
		   {Divisions[i].bFraudCaught = TRUE;}  /* fraud is caught */
	       }
	     }  /* end if for was a fraud */
	  else  /* was not a fraud, so reset the Detection flag */
	     {
	     for (i=0; i<NUMDIV3; i++)
	       {
	       Divisions[i].bFraudDetected[j] = FALSE; /* reset */
	       }
	     }    /* end if for was not a fraud */
	  }    /* end if for needs to be looked at */
      }    /* end for loop */



   /* Set iLatch to current portion of the day. (iLatch corresponds       */
   /* to the data currently being collected).                             */
   /* (i.e., find the HourLatched time which is nearest the current time  */
   /* of day, yet greater than the current time of day)                   */
   if (SecondsIntoDay >
	 (unsigned long)FraudLatch[MAXLATCH-1].HourLatched * HOUR_SECONDS)
      {
      iLatch = 0;                 /* wrap back to beginning */
      TimeAdjuster = DAY_SECONDS; /* won't become available till tomorrow */
      }
   else
      {
      iLatch = 0;
      TimeAdjuster = 0L;
      while (SecondsIntoDay >
	 (unsigned long)FraudLatch[iLatch].HourLatched * HOUR_SECONDS)
	 {
	 iLatch++;
	 }
      }


   /* Set bFraudDetected for current process if score exceeds threshold   */
   /* Current process is pointed to by iLatch                             */
   for (i=0; i<NUMDIV3; i++)
     {
     if (Score  >=  Divisions[i].Thresh)
	{
	 Divisions[i].bFraudDetected[iLatch] = TRUE;
	}
     }


   /* Save the time at which the collected data will become available */
   FraudLatch[iLatch].DateTimeAvailable =  TimeAdjuster +
	 DateTimeBeginDay + FraudLatch[iLatch].HourAvailable * HOUR_SECONDS;


   /* data must be looked at sometime later */
   FraudLatch[iLatch].NeedsToBeLookedAt = TRUE;

   /* If data would not have been looked at by this point in time      */
   /* (still needs to be looked at and time less than the time         */
   /*  at which data becomes available), then check to see if the      */
   /* fraudulent activity is in progress. If so, set WasAFraud to TRUE */
   for (j=0 ; j< MAXLATCH; j++)
      {
      if (FraudLatch[j].NeedsToBeLookedAt &&
	  CstDateTime <= FraudLatch[j].DateTimeAvailable)
	  {
	  FraudLatch[j].WasAFraud = bFraudq;
	  }
      }


   if (bFraudq)
      {
      NumFraudAuths++;

      if (!bFraudqPast)       /* first fraud of the account */
	 {
	  NumOfFraudsSinceFirst = 0;
	  NumFraudAccounts++;
	  if (Av_cred != LONG_MIN)
	     {
	     SumFraudStartCredit += (float)Av_cred;
	     NumValidAvCredits++;
	     }
	 }
      bFraudqPast = TRUE;

      NumOfFraudsSinceFirst += 1;    /* actually, number of frauds so far */

      for (i=0; i<NUMDIV3; i++)
	 {
	 if (!Divisions[i].bFraudCaught)   /* if fraud not caught, */
	    {                              /* money is lost        */
	    Divisions[i].AmountLost += Amount;
	    }
	 else
	    {
	    if (Divisions[i].bFirstCatch)
	       {
	       Divisions[i].bFirstCatch = FALSE;
	       Divisions[i].NumberCaught += 1;
	       Divisions[i].SumWhenCaught += NumOfFraudsSinceFirst;
	       }
	    }
	 }
      }   /* end if for is a fraud */
   else
      {
      NumCleanAuths++;
      }


}

/***************************************************************************/
