/**********************************************************************/
/*                                                                    */
/* Function to take packed 5-digit (or upper-case letter) ZIP codes   */
/*  from Falcon and turn them into the real ZIP codes.                */
/*                                                                    */
/*  Input: 9-digit integers from stdin                                */
/*  Ouput: 5-(digit,letter) ZIP codes into stdout                     */
/*                                                                    */
/*  Written by: Jim Baker                                             */
/*  Date:  3/11/98                                                    */
/*                                                                    */
/*  unpackPostalCode function from drv_model{gen,use} code            */
/*   appears in geoinfo.c in 1.3x code                                */
/*                                                                    */
/**********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#define MISSLNG   LONG_MIN

long unpackPostalCode(char* , long );
long packPostalCode(char* );

int main(int argc, char* argv[])
{
  long i,j;
  char Postal[10];
  char buff[100];

  if ( argc >= 2 )
  {
     fprintf(stderr,"\n\n  USAGE:\n"
                    "          Function to take packed 5-digit (or upper-case letter) ZIP codes\n"
                    "          from Falcon and turn them into the real ZIP codes or vice versa.\n\n"
                    "          cat packed_zip_file   | unpack_zips > unpacked_zip_file\n"
                    "                 or\n"
                    "          cat unpacked_zip_file | pack_zips   > packed_zip_file\n\n\n");
     exit(-13);
  }

  while ( fgets(buff,100,stdin) != NULL )
  {
    j = unpackPostalCode(Postal,atol(buff));
    printf("%5.5s\n",Postal);
  }
}

/* =========================================================================
 * Function unpackPostalCode
 * ======================================================================== */

char charMap[27] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

long unpackPostalCode(char* unpacked, long packed)
{
   unsigned char  chr;
   int            i;
 
   unpacked[5] = '\0';
 
   /***********************************************************/
   /* Unpack a long integer into a five character postal code */
   /***********************************************************/
   for(i=4; i>=0 && packed >= 0; i--)
   {  chr = packed % 0x0000003F;
      packed = packed / 0x0000003F;

      /*
      if (chr<26) chr=charMap[chr];
      else if ( chr < 36) chr = (chr-26) + '0';
      else chr = 32;
      */
      if (chr<26) chr=charMap[chr];
      else chr = (chr-26) + '0';

      unpacked[i] = chr;
   }
 
   return(0);
}

/* =========================================================================
 * Function packPostalCode
 * ======================================================================== */

long packPostalCode(char* postalCode)
{
   char  chr;
   long  packed = 0;
   int   index, val;
 
   /*********************************************************/
   /* Pack a five character postal code into a long integer */
   /*********************************************************/
/*************************************************
   for(index=0; index<5 && postalCode[index] != ' '; index++)
*************************************************/
   for(index=0; index<5 ; index++)
   {  chr = postalCode[index];
 
      if (!isalnum(chr) && chr != ' ') return(MISSLNG);

      if ( chr == ' ')
        val = 36;
      else if (isalpha(chr))
      {  int first=0, last=25, i=0, found=0;
         chr = toupper(chr);
 
         while(first <= last && !found)
         {  i = (first+last)/2;
 
            if (chr == charMap[i])
            {  found=1;
               break;
            }
 
            else if (chr < charMap[i]) last = i - 1;
            else first = i + 1;
         }
 
         val = i;
      }
      else
         val = (chr-'0') + 26;
 
      if(index < 5) packed *= 0x0000003F;
      packed+=val;
   }
 
   return(packed);
}
