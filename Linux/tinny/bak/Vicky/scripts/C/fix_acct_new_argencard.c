#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


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
// New conversion table extracted from FDR reader. 
unsigned char etoa[256] = {
'\x20',
'\x01',
'\x02',
'\x03',
'\x9C',
'\x09',
'\x86',
'\x7F',
'\x97',
'\x8D',
'\x8E',
'\x0B',
'\x0C',
'\x0D',
'\x0E',
'\x0F',
'\x10',
'\x11',
'\x12',
'\x13',
'\x9D',
'\x85',
'\x08',
'\x87',
'\x18',
'\x19',
'\x92',
'\x8F',
'\x1C',
'\x1D',
'\x1E',
'\x1F',
'\x80',
'\x81',
'\x82',
'\x83',
'\x84',
'\x0A',
'\x17',
'\x1B',
'\x88',
'\x89',
'\x8A',
'\x8B',
'\x8C',
'\x05',
'\x06',
'\x07',
'\x90',
'\x91',
'\x16',
'\x93',
'\x94',
'\x95',
'\x96',
'\x04',
'\x98',
'\x99',
'\x9A',
'\x9B',
'\x14',
'\x15',
'\x9E',
'\x1A',
'\x20',
'\xA0',
'\xA1',
'\xA2',
'\xA3',
'\xA4',
'\xA5',
'\xA6',
'\xA7',
'\xA8',
'\x5B',
'\x2E',
'\x3C',
'\x28',
'\x2B',
'\x7C',
'\x26',
'\xA9',
'\xAA',
'\xAB',
'\xAC',
'\xAD',
'\xAE',
'\xAF',
'\xB0',
'\xB1',
'\x21',
'\x24',
'\x2A',
'\x29',
'\x3B',
'\x5E',
'\x2D',
'\x2F',
'\xB2',
'\xB3',
'\xB4',
'\xB5',
'\xB6',
'\xB7',
'\xB8',
'\xB9',
'\x5D',
'\x2C',
'\x25',
'\x5F',
'\x3E',
'\x3F',
'\xBA',
'\xBB',
'\xBC',
'\xBD',
'\xBE',
'\xBF',
'\xC0',
'\xC1',
'\xC2',
'\x60',
'\x3A',
'\x23',
'\x40',
'\x27',
'\x3D',
'\x22',
'\xC3',
'\x61',
'\x62',
'\x63',
'\x64',
'\x65',
'\x66',
'\x67',
'\x68',
'\x69',
'\xC4',
'\xC5',
'\xC6',
'\xC7',
'\xC8',
'\xC9',
'\xCA',
'\x6A',
'\x6B',
'\x6C',
'\x6D',
'\x6E',
'\x6F',
'\x70',
'\x71',
'\x72',
'\xCB',
'\xCC',
'\xCD',
'\xCE',
'\xCF',
'\xD0',
'\xD1',
'\x7E',
'\x73',
'\x74',
'\x75',
'\x76',
'\x77',
'\x78',
'\x79',
'\x7A',
'\xD2',
'\xD3',
'\xD4',
'\xD5',
'\xD6',
'\xD7',
'\xD8',
'\xD9',
'\xDA',
'\xDB',
'\xDC',
'\xDD',
'\xDE',
'\xDF',
'\xE0',
'\xE1',
'\xE2',
'\xE3',
'\xE4',
'\xE5',
'\xE6',
'\xE7',
'\x7B',
'\x41',
'\x42',
'\x43',
'\x44',
'\x45',
'\x46',
'\x47',
'\x48',
'\x49',
'\xE8',
'\xE9',
'\xEA',
'\xEB',
'\xEC',
'\xED',
'\x7D',
'\x4A',
'\x4B',
'\x4C',
'\x4D',
'\x4E',
'\x4F',
'\x50',
'\x51',
'\x52',
'\xEE',
'\xEF',
'\xF0',
'\xF1',
'\xF2',
'\xF3',
'\x5C',
'\x9F',
'\x53',
'\x54',
'\x55',
'\x56',
'\x57',
'\x58',
'\x59',
'\x5A',
'\xF4',
'\xF5',
'\xF6',
'\xF7',
'\xF8',
'\xF9',
'\x30',
'\x31',
'\x32',
'\x33',
'\x34',
'\x35',
'\x36',
'\x37',
'\x38',
'\x39',
'\xFA',
'\xFB',
'\xFC',
'\xFD',
'\xFE',
'\xFF'
};


char ebcdic_to_ascii(unsigned char c)
{
  unsigned int num = (unsigned int)c;
  return (char)etoa[num];
}
/****************************************************************************
 This function coverts a string of uppercase letters, numbers and spaces in
 EBCDIC to ASCII characters.
 add an ebcdic to ascii lookup table, relationship is in etoa[]. 11/07/96
/****************************************************************************/
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

         sprintf(temp, "%c",buffer_in[0]);
		   temp[0]=ebcdic_to_ascii(temp[0]);
			temp[1]='\0';
         sprintf(temp, "%c",buffer_in[5]);
		   temp[0]=ebcdic_to_ascii(temp[0]);
			temp[1]='\0';
			
			fileerror=0;
      
      /*if      ( !(memcmp(prefix, visaPref0, PREFIX_LEN)))  acct[0]='0';                    
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
*/
      
      for (i = 6; i < 13; i++) {
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

/*void printUsage()
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "barc_fix_new_yxc [-r<record length>] | [-a/c/f/N/p/P] \n");
  fprintf(stderr, "         -a    indicates auth records.\n");
  fprintf(stderr, "         -c    indicates card records.\n");
  fprintf(stderr, "         -f    indicates fraud records.\n");
  fprintf(stderr, "         -N    indicates nonmon records.\n");
  fprintf(stderr, "         -p    indicates payment records.\n");
  fprintf(stderr, "         -P    indicates posting records.\n\n");
}*/

void printUsage()
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "barc_fix_new_yxc [-r<record length>] \n");
}


void checkArg(int rec_length)
{
  if (rec_length)
    {
      fprintf(stderr, "Incompatible argument: Record length already specified through -a/c/f option.\n\n");
      exit(-2);
    }
}

