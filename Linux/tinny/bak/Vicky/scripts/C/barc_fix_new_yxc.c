#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

unsigned char   visaPref0[7] ="492940";
unsigned char   visaPref1[7] ="492941";
unsigned char   visaPref2[7] ="492942";
unsigned char   visaPref3[7] ="492943";
unsigned char   visaPref4[7] ="492944";
unsigned char   visaPref5[7] ="492945";
unsigned char   visaPref6[7] ="492946";
unsigned char   visaPref7[7] ="492947";
unsigned char   visaPref8[7] ="492948";
unsigned char   visaPref9[7] ="492949";
unsigned char     mcPrefA[7] ="530125";
unsigned char  debitPrefB[7] ="453978";
unsigned char  debitPrefC[7] ="453979";
unsigned char  corp1PrefD[7] ="425757";
unsigned char  corp2PrefE[7] ="471532";

#define FRAUD_REC_LEN 49
#define AUTHS_REC_LEN 164
#define CARDS_REC_LEN 49 
#define BYTE_LEN 1
#define MAX_REC_LEN 300
#define ACCT_LEN 19
#define PREFIX_LEN 6


#define NEWLINECHAR     0x0a    /* decimal  10 */
#define SPACECHAR       0x20    /* decimal  32 */
#define FIRSTASCII      0x20    /* decimal  32 = space char */
#define LASTASCII       0x7e    /* decimal 126 = tilde '~' char */

#define GZIP "/usr/local/bin/gzip "
#define REJECTS "rejects.gz"

void     printUsage();
void     checkArg(int);

/****************************************************************************\
* char ebcdic_to_ascii(unsigned char c)
* 
* This function converts a single uppercase letter, number and space in EBCDIC
* to an ASCII character.
\****************************************************************************/
static unsigned char e2a[256] = {
          0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
         16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
        128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
        144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
         32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
         38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
         45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
        186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
        195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
        202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
        209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
        216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
        123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
        125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
         92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
         48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};

char ebcdic_to_ascii(const unsigned char c)
{
        return e2a[c];
}

/*char ebcdic_to_ascii(unsigned char c) {
	int num;

	num=(int)c;
	if (num == 64) 
		num=32;
	else
	if (num == 96)
		num=45;
	else
	if (num == 75)
		num=46;
	else
	if (num > 239 && num < 250)
		num=num-192;
	else
	if (num > 192 && num < 202)
		num=num-128;
	else
	if (num > 208 && num < 218)
		num=num-135;
	else
	if (num > 225 && num < 234)
		num=num-143;
	return (char)num;
}
/****************************************************************************\
 This function coverts a string of uppercase letters, numbers and spaces in
 EBCDIC to ASCII characters.
 add an ebcdic to ascii lookup table, relationship is in etoa[]. 11/07/96
\****************************************************************************/
/*void ebcdic_to_ascii_string(char* new_string, char* string, int len) {
	unsigned int num;
	int i;
	unsigned char c;

	for(i=0;i<len;i++) {
		c=(unsigned) string[i];
		num=(unsigned int)c;

		new_string[i]=(char)etoa[num];
	}
        new_string[len] = '\0';
}*/




int main(int argc, char *argv[])
{
  unsigned char  buffer_in[MAX_REC_LEN+1];
  unsigned char buffer_out[MAX_REC_LEN+1];
  unsigned char acct[2000];
  unsigned char hexHold[3];
  unsigned char prefix[PREFIX_LEN+1];
  unsigned char temp[2];
  int argNum=1;
  char rest[1000];
  int i, rec_length=0;
  char cmmd[256];
  int fileerror;

  FILE *rejectsFp=NULL;

 sprintf(cmmd, "%s > %s", GZIP, REJECTS);
//sprintf(cmmd, " > %s" REJECTS);  
  if ( (rejectsFp = popen(cmmd, "w")) == NULL)
    {
      fprintf(stderr, "Trouble opening pipe to '%s'.\n", REJECTS);
      fprintf(stderr, "  Check permissions.\n");
      exit(-5);
    } 

  /* Prevent runaways */
  memset(buffer_in,  '\0', MAX_REC_LEN+1);
  memset(buffer_out, '\0', MAX_REC_LEN+1);
  prefix[PREFIX_LEN]= '\0';

  /* Command line processing */

  if (argc <= 1)
  {
    printUsage();
    exit( 4 );
  }
  while(argNum < argc)
    {
      switch(argv[argNum][1])
	{
	case 'h':
	  printUsage();
	  exit(4);
	  break;
	case 'a':
	  checkArg(rec_length);
	  rec_length= AUTHS_REC_LEN;
	  break;
	case 'c':
	  checkArg(rec_length);
	  rec_length= CARDS_REC_LEN;
	  break;
	case 'f':
	  checkArg(rec_length);
	  rec_length= FRAUD_REC_LEN;;
	  break;

	case 'r':
	  checkArg(rec_length);
	  rec_length = atoi( &(argv[argNum][2]) );
	  if (rec_length < 1)
	    {
	      fprintf(stderr, "The record length you gave is garbled.\n\n");
	      exit(-1);
	    }
	  if (rec_length >= (MAX_REC_LEN - PREFIX_LEN))
	    {
	      fprintf(stderr, "The record length you gave exceeds MAXIMUM (%d) expected.\n\n",
		      (MAX_REC_LEN - PREFIX_LEN));
	      exit(-2);
	    }
	  break;

	  
	default:
	  fprintf(stderr, "Unrecognized option '-%c'.\n\n", argv[argNum][1]);
	  printUsage();
	  exit(-3);
	}
      argNum++;
    } 
  
  /* fix account number */

  while (fread(buffer_in, (size_t) BYTE_LEN, (size_t) rec_length, stdin))
    {
      /* Prevent runaways */
      memset(acct,  '\0', ACCT_LEN+1);
      memset(prefix, '\0', PREFIX_LEN);
      /* build the acct prefix */
/*      memcpy(prefix, buffer_in, PREFIX_LEN);
      
     if ( (memcmp(prefix, debitPref, PREFIX_LEN)) &&
	   (memcmp(prefix, corp1Pref, PREFIX_LEN)) &&
	   (memcmp(prefix, corp2Pref, PREFIX_LEN)) &&
	   (memcmp(prefix, visaPref,  PREFIX_LEN)) &&
	   (memcmp(prefix, mcPref,    PREFIX_LEN)) )
	{
	  
	  fwrite(buffer_in, (size_t) BYTE_LEN, (size_t) rec_length, rejectsFp);
	  fputc('\n', rejectsFp);
	  continue;
	}
      
      // the MC/VISA byte */
/*      acct[0] = buffer_in[0];
      
      // assume the bin byte differs in position 5 only */
//      acct[1] = buffer_in[5]; 
     
      /* convert the encoded bytes to HEX equivalent and copy */

      /* yxc modified the BIN conversion. Totally there are 11 BINs  */

      for (i=0; i <6; i++) {
         sprintf(temp, "%c",buffer_in[i]);
         temp[0]=ebcdic_to_ascii(temp[0]);
         temp[1]='\0';
         strcat(prefix, temp);
     // printf("%s\n",prefix);
      }
      fileerror=0;
      
      if      ( !(memcmp(prefix, visaPref0, PREFIX_LEN)))  acct[0]='0';                    
      else if ( !(memcmp(prefix, visaPref1, PREFIX_LEN)))  acct[0]='1';                    
      else if ( !(memcmp(prefix, visaPref2, PREFIX_LEN)))  acct[0]='2';                    
      else if ( !(memcmp(prefix, visaPref3, PREFIX_LEN)))  acct[0]='3';                    
      else if ( !(memcmp(prefix, visaPref4, PREFIX_LEN)))  acct[0]='4';                    
      else if ( !(memcmp(prefix, visaPref5, PREFIX_LEN)))  acct[0]='5';                    
      else if ( !(memcmp(prefix, visaPref6, PREFIX_LEN)))  acct[0]='6';                    
      else if ( !(memcmp(prefix, visaPref7, PREFIX_LEN)))  acct[0]='7';                    
      else if ( !(memcmp(prefix, visaPref8, PREFIX_LEN)))  acct[0]='8';                    
      else if ( !(memcmp(prefix, visaPref9, PREFIX_LEN)))  acct[0]='9';                    
      else if ( !(memcmp(prefix,   mcPrefA, PREFIX_LEN)))  acct[0]='A';                    
      else if ( !(memcmp(prefix,debitPrefB, PREFIX_LEN)))  acct[0]='B';
      else if ( !(memcmp(prefix,debitPrefC, PREFIX_LEN)))  acct[0]='C';
      else if ( !(memcmp(prefix,corp1PrefD, PREFIX_LEN)))  acct[0]='D';
      else if ( !(memcmp(prefix,corp2PrefE, PREFIX_LEN)))  acct[0]='E';
      else {acct[0]='X'; fileerror=1; }

      
      for (i = 6; i < 15; i++) {
	sprintf(hexHold,"%02X", buffer_in[i]);
	strcat(acct, hexHold);
      }
    
    //  printf ("%s\n", prefix);
      assert ( strlen(acct) == 19 );
      
      /* blank the last three digits */
//      memset(acct+16, 0x20, 3);
      
//      assert ( strlen(acct) == ACCT_LEN );
      
      
      /* search the rest of the line for non-ASCII chars, which include newlines */
     
       memset(rest,  '\0', rec_length-19+1);
       
      for(i=19; i < rec_length; i++)
	{
	    sprintf(temp, "%c", buffer_in[i]);
            temp[0]=ebcdic_to_ascii(temp[0]);
            temp[1]='\0';
            strcat(rest, temp);
          /*  if((temp[i] < FIRSTASCII) || (temp[0] > LASTASCII))
	    {
	      rest[i] = SPACECHAR;
	    }
	  else
	    {
	      buffer_out[i] = buffer_in[i];
	    }*/
	}


/* next loop added by jwb to add bin to record */

    /*  for(i=rec_length; i < rec_length + 6 ; i++)
	{
	  buffer_out[i] = prefix[i - rec_length ];
	}*/
      strcat(acct,rest );
    //  printf("%s\n",rest);
      strcat(acct,prefix);
      /* copy fixed account number to bufOUT */
       memcpy(buffer_out, acct, rec_length+6);
    
      if (fileerror == 1) {
     	  fwrite(buffer_out, (size_t) BYTE_LEN, (size_t) rec_length+6, rejectsFp);
	  fputc('\n', rejectsFp);
//	  fprintf(rejectsFp, "%s\n", acct);
        //  printf("Error: %s\n",acct);
    }
 
                      
    if (fileerror != 1) {
      fwrite(buffer_out, (size_t) BYTE_LEN, (size_t) rec_length + 6 , stdout);
      fputc('\n', stdout);
    }
    }   
  pclose(rejectsFp);
  return 0;
}

void printUsage()
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "barc_fix_new_yxc [-r<record length>] | [-a/c/f/N/p/P] \n");
  fprintf(stderr, "         -a    indicates auth records.\n");
  fprintf(stderr, "         -c    indicates card records.\n");
  fprintf(stderr, "         -f    indicates fraud records.\n");
  fprintf(stderr, "         -N    indicates nonmon records.\n");
  fprintf(stderr, "         -p    indicates payment records.\n");
  fprintf(stderr, "         -P    indicates posting records.\n\n");
}

void checkArg(int rec_length)
{
  if (rec_length)
    {
      fprintf(stderr, "Incompatible argument: Record length already specified through -a/c/f option.\n\n");
      exit(-2);
    }
}

