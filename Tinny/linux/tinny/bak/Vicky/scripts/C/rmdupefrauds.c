/*head	1.2;
access;
symbols;
locks
	jag:1.2; strict;
comment	@ * @;


1.2
date	97.02.24.20.12.23;	author jcb;	state Exp;
branches;
next	1.1;

1.1
date	96.11.06.22.33.28;	author jcb;	state Exp;
branches;
next	;


desc
@routine to remove 2nd and subsequent fraud accounts
@

1.2
log
@added looking for blank date and taking real date if exists for account
also check record length
@
text

/*************************************************************************/
/*                                                                       */
/* Routine to remove 2nd and subsequent records with the same	         */
/*  account number. Used to remove duplicate fraud accounts after        */
/*  frauds have been sorted by:                                          */
/*                                                                       */
/*  bmsort -m 100 -S -c1-19,21-49,20 -i frauds.dat | rmdupefrauds > out  */
/*                                                                       */
/* This will keep the earliest date of first fraud for any account       */
/*  that has multiple entries                                            */
/*                                                                       */
/* If there is an account with all blanks for date of first fraud, then  */
/*  an entry for that account with a date the one                        */
/*  with a date will be kept                                             */
/*                                                                       */
/* Written: 11/6/96                                                      */
/*                                                                       */
/* By: Jim Baker (jcb)                                                   */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>

void main(void)
{

  char buf[10000],prev_buf[10000];
  char acct[20],prev_acct[20];
  char written_acct[20];
  char date[9];
  char blank[9];
  int previous=0;
  int record_len=0;

  strcpy(prev_acct,"                   ");
  strcpy(written_acct,"                   ");
  strcpy(blank,"        ");

  while ( !feof(stdin) )
  {
    fgets(buf,10000,stdin);
    record_len = strlen(buf);
    if ( record_len > 100 )
    {
        fprintf(stderr,"\n\n ---> Length of fraud record = %i This is wrong.\n\n",record_len);
		  fseek(stdout,0,SEEK_SET);
		  fputs("99999999999999",stdout);
		  exit(-13);
    }
    strncpy(acct,buf,19);
    acct[19] = '\0';
    if ( strcmp(acct,written_acct) )
    {
	 	strncpy(date,buf+20,8);
	 	date[9] = '\0';
	if ( strncmp(date,blank,8) > 0 )
	{
	   if ( previous && strcmp(acct,prev_acct) )
	   {
		fputs(prev_buf,stdout);

/*  diagnostic print
 *	        fprintf(stderr," first one %s \n",prev_acct);
 */

	   }
	   fputs(buf,stdout);

/*  diagnostic print
 *	   fprintf(stderr," main one %s \n",acct);
 */

	   previous=0;
           strcpy(written_acct,acct);
           strcpy(prev_acct,acct);
	}
	else
	{
	   if ( previous && strcmp(acct,prev_acct) )
	   {

/*  diagnostic print
 *	       fprintf(stderr," second one %s \n",prev_acct);
 */

	       fputs(prev_buf,stdout);
               strcpy(written_acct,prev_acct);
	   }
	   previous=1;
	   strcpy(prev_acct,acct);
    	   strncpy(prev_buf,buf,record_len);
	}
    }
  }

  if (  strcmp(acct,written_acct) )
           fputs(buf,stdout);

}

/*
@


1.1
log
@Initial revision
@
text
@d12 4
d28 7
a34 2
  char buf[10000];
  char acct[20],old_acct[20];
d36 3
a38 1
  strcpy(old_acct,"                   ");
d40 1
a40 1
  while (!feof(stdin))
d43 8
d52 43
a94 4
    acct[19]='\0';
    if (strcmp(acct,old_acct))
	fputs(buf,stdout);
    strcpy(old_acct,acct);
d97 3
i@*/
