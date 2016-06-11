/**********************************************************************
     module names:   fEncBuff,
                     DeCryptBuffer,
                     UuEncode, and
                     UuDecode.
     type:           functions
     language:       C-370
     platform:       Mainframe (Server) and Client (PC)
     purpose:        1. To encrypt and decrypt a buffers using DES
                        (Data Encryption Statndard).  Both of these
                        functions encrypt with DES in a ECB (electronic
                        code block) mode.  The data in encrypted in
                        eight byte chunks.  Thus, the minimum buffer
                        size is eight.  If the buffer is less than eight
                        bytes long, it will be padded with NULLs.
                     2. To Uuencode and UuEncode.  This will allow the
                        data to be save in a file format.  These
                        functions work by expanding three byte chunks
                        into four byte chunks.  The formula is:
                        ((NumOfChars + 2) / 3 * 4) + 1.
     how to run:     1. The encrypt and decrypt functions both return
                        the length of the buffer.  They both have three
                        parms:  a pointer to the buffer, a pointer to
                        an int containing the length,  and a pointer to
                        the key.  The buffer has been tested for
                        legnths up to one K.  The key has been tested
                        up to 20 bytes.
                     2. The uuencode and decode both return the new
                        length.  Both function are passed to parms.
                        They are:  a pointer to the buffer and a
                        pointer to an int length.  The length will
                        change.(see purpose above)
   includes:       encrypt
***********************************************************************/
#include <stdio.h>
#include <string.h>

#include "encrypt.h"

int fEncBuff(char *pBuffer,int *pLength, char *key)
{
 register int i;
 des_cblock kk;
 des_key_schedule ks;

 DesStrToKey(key,(des_cblock *)kk);
 if (!DesChkPar((des_cblock *)kk)) printf("Key parity error\n");
 if (DesIsWeakKey((des_cblock *)kk)) printf("Weak key\n");
 DesSetKey((des_cblock *)kk,ks);

 if (*pLength < 8) *pLength = 8;
 for (i=0; i<=(*pLength-8); i+=8)
  DesEcbEncrypt(
   (des_cblock *)&(pBuffer[i]),
   (des_cblock *)&(pBuffer[i]),
   ks,DES_ENCRYPT);

 if (i<*pLength)
 {
  DesEcbEncrypt(
   (des_cblock *)&(pBuffer[*pLength-8]),
   (des_cblock *)&(pBuffer[*pLength-8]),
   ks,DES_ENCRYPT);
 }

 return(*pLength);
}

int fDecBuff(char *pBuffer,int *pLength, char *key)
{
 des_cblock kk;
 des_key_schedule ks;
 int i;

 DesStrToKey(key,(des_cblock *)kk);
 DesSetKey((des_cblock *)kk,ks);

 if (*pLength%8)
 {
  DesEcbEncrypt(
   (des_cblock *)&(pBuffer[*pLength-8]),
   (des_cblock *)&(pBuffer[*pLength-8]),
   ks,DES_DECRYPT);
 }

 for (i=0; i<=(*pLength-8); i+=8)
  DesEcbEncrypt(
   (des_cblock *)&(pBuffer[i]),
   (des_cblock *)&(pBuffer[i]),
   ks,DES_DECRYPT);


 return(*pLength);
}

int  DesEcbEncrypt(des_cblock *input,
       des_cblock *output,
       des_key_schedule ks,
       int encrypt)
 {
 register unsigned long l0,l1;
 register unsigned char *in,*out;
 unsigned long ll[2];

 in=(unsigned char *)input;
 out=(unsigned char *)output;
 c2l(in,l0);
 c2l(in,l1);
 ll[0]=l0;
 ll[1]=l1;
 DesEnc(ll,ll,ks,encrypt);
 l0=ll[0];
 l1=ll[1];
 l2c(l0,out);
 l2c(l1,out);
 l0=l1=ll[0]=ll[1]=0;
 return(0);
 }

int  DesEnc(unsigned long *input,
       unsigned long *output,
       des_key_schedule ks,
       int encrypt)
 {
 register unsigned long l,r,t,u;
 register int i;
 register unsigned long *s;

 l=input[0];
 r=input[1];

 /* do IP */
 PERM_OP(r,l,t, 4,0x0f0f0f0f);
 PERM_OP(l,r,t,16,0x0000ffff);
 PERM_OP(r,l,t, 2,0x33333333);
 PERM_OP(l,r,t, 8,0x00ff00ff);
 PERM_OP(r,l,t, 1,0x55555555);
 /* r and l are reversed - remember that :-) - fix
  * it in the next step */

 /* Things have been modified so that the initial rotate is
  * done outside the loop.  This required the
  * des_SPtrans values in sp.h to be rotated 1 bit to the right.
  * One perl script later and things have a 5% speed up on a sparc2.
  * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
  * for pointing this out. */
 t=(r<<1)|(r>>31);
 r=(l<<1)|(l>>31);
 l=t;

 /* clear the top bits on machines with 8byte longs */
 l&=0xffffffff;
 r&=0xffffffff;

 s=(unsigned long *)ks;
 /* I don't know if it is worth the effort of loop unrolling the
  * inner loop */
 if (encrypt)
  {
  for (i=0; i<32; i+=4)
   {
   D_ENCRYPT(l,r,i+0); /*  1 */
   D_ENCRYPT(r,l,i+2); /*  2 */
   }
  }
 else
  {
  for (i=30; i>0; i-=4)
   {
   D_ENCRYPT(l,r,i-0); /* 16 */
   D_ENCRYPT(r,l,i-2); /* 15 */
   }
  }
 l=(l>>1)|(l<<31);
 r=(r>>1)|(r<<31);
 /* clear the top bits on machines with 8byte longs */
 l&=0xffffffff;
 r&=0xffffffff;

 /* swap l and r
  * we will not do the swap so just remember they are
  * reversed for the rest of the subroutine
  * luckily FP fixes this problem :-) */

 PERM_OP(r,l,t, 1,0x55555555);
 PERM_OP(l,r,t, 8,0x00ff00ff);
 PERM_OP(r,l,t, 2,0x33333333);
 PERM_OP(l,r,t,16,0x0000ffff);
 PERM_OP(r,l,t, 4,0x0f0f0f0f);

 output[0]=l;
 output[1]=r;
 l=r=t=u=0;
 return(0);
 }

/* return 0 if key parity is odd (correct),
 * return -1 if key parity error,
 * return -2 if illegal weak key.
 */
int  DesSetKey(des_cblock *key,des_key_schedule schedule)
 {
 register unsigned long c,d,t,s;
 register unsigned char *in;
 register unsigned long *k;
 register int i;

 k=(unsigned long *)schedule;
 in=(unsigned char *)key;

 c2l(in,c);
 c2l(in,d);

 /* do PC1 in 60 simple operations */
/* PERM_OP(d,c,t,4,0x0f0f0f0f);
 HPERM_OP(c,t,-2, 0xcccc0000);
 HPERM_OP(c,t,-1, 0xaaaa0000);
 HPERM_OP(c,t, 8, 0x00ff0000);
 HPERM_OP(c,t,-1, 0xaaaa0000);
 HPERM_OP(d,t,-8, 0xff000000);
 HPERM_OP(d,t, 8, 0x00ff0000);
 HPERM_OP(d,t, 2, 0x33330000);
 d=((d&0x00aa00aa)<<7)|((d&0x55005500)>>7)|(d&0xaa55aa55);
 d=(d>>8)|((c&0xf0000000)>>4);
 c&=0x0fffffff; */

 /* I now do it in 47 simple operations :-)
  * Thanks to John Fletcher (john_fletcher@lccmail.ocf.llnl.gov)
  * for the inspiration. :-) */
 PERM_OP (d,c,t,4,0x0f0f0f0f);
 HPERM_OP(c,t,-2,0xcccc0000);
 HPERM_OP(d,t,-2,0xcccc0000);
 PERM_OP (d,c,t,1,0x55555555);
 PERM_OP (c,d,t,8,0x00ff00ff);
 PERM_OP (d,c,t,1,0x55555555);
 d= (((d&0x000000ff)<<16)| (d&0x0000ff00)     |
   ((d&0x00ff0000)>>16)|((c&0xf0000000)>>4));
 c&=0x0fffffff;

 for (i=0; i<ITERATIONS; i++)
  {
  if (shifts2[i])
   { c=((c>>2)|(c<<26)); d=((d>>2)|(d<<26)); }
  else
   { c=((c>>1)|(c<<27)); d=((d>>1)|(d<<27)); }
  c&=0x0fffffff;
  d&=0x0fffffff;
  /* could be a few less shifts but I am to lazy at this
   * point in time to investigate */
  s= des_skb[0][ (c    )&0x3f                ]|
   des_skb[1][((c>> 6)&0x03)|((c>> 7)&0x3c)]|
   des_skb[2][((c>>13)&0x0f)|((c>>14)&0x30)]|
   des_skb[3][((c>>20)&0x01)|((c>>21)&0x06) |
        ((c>>22)&0x38)];
  t= des_skb[4][ (d    )&0x3f                ]|
   des_skb[5][((d>> 7)&0x03)|((d>> 8)&0x3c)]|
   des_skb[6][ (d>>15)&0x3f                ]|
   des_skb[7][((d>>21)&0x0f)|((d>>22)&0x30)];

  /* table contained 0213 4657 */
  *(k++)=((t<<16)|(s&0x0000ffff))&0xffffffff;
  s=     ((s>>16)|(t&0xffff0000));

  s=(s<<4)|(s>>28);
  *(k++)=s&0xffffffff;
  }
 return(0);
 }

int  DesIsWeakKey(des_cblock *key)
 {
 int i;

 for (i=0; i<NUM_WEAK_KEY; i++)
  /* Added == 0 to comparision, I obviously don't run
   * this section very often :-(, thanks to
   * engineering@MorningStar.Com for the fix
   * eay 93/06/29 */
  if (memcmp(weak_keys[i],key,sizeof(key)) == 0)
   return(1);
 return(0);
 }

int  DesStrToKey(char *str,des_cblock *key)
 {
 des_key_schedule ks;
 int i,length;
 register unsigned char j;

 memset(key,0,(size_t)(8));
 length=strlen(str);
 for (i=0; i<length; i++)
  {
  j=str[i];
  if ((i%16) < 8)
   (*key)[i%8]^=(j<<1);
  else
   {
   /* Reverse the bit order 05/05/92 eay */
   j=((j<<4)&0xf0)|((j>>4)&0x0f);
   j=((j<<2)&0xcc)|((j>>2)&0x33);
   j=((j<<1)&0xaa)|((j>>1)&0x55);
   (*key)[7-(i%8)]^=j;
   }
  }
 DesSetOddParity((des_cblock *)key);
 DesSetKey((des_cblock *)key,ks);
 DesCbcCksum((des_cblock *)str,(des_cblock *)key,(long)length,ks,
  (des_cblock *)key);
 memset(ks,0,sizeof(ks));
 DesSetOddParity((des_cblock *)key);
 return(0);
 }

unsigned long  DesCbcCksum(des_cblock *input,
          des_cblock *output,
          long length,
          des_key_schedule schedule,
          des_cblock *ivec)
 {
 register unsigned long tout0,tout1,tin0,tin1;
 register long l=length;
 unsigned long tin[2],tout[2];
 unsigned char *in,*out,*iv;

 in=(unsigned char *)input;
 out=(unsigned char *)output;
 iv=(unsigned char *)ivec;

 c2l(iv,tout0);
 c2l(iv,tout1);
 for (; l>0; l-=8)
  {
  if (l >= 8)
   {
   c2l(in,tin0);
   c2l(in,tin1);
   }
  else
   c2ln(in,tin0,tin1,l);

  tin0^=tout0;
  tin1^=tout1;
  tin[0]=tin0;
  tin[1]=tin1;
  DesEnc((unsigned long *)tin,(unsigned long *)tout,
   schedule,DES_ENCRYPT);
  /* fix 15/10/91 eay - thanks to keithr@sco.COM */
  tout0=tout[0];
  tout1=tout[1];
  }
 if (out != NULL)
  {
  l2c(tout0,out);
  l2c(tout1,out);
  }
 tout0=tin0=tin1=tin[0]=tin[1]=tout[0]=tout[1]=0;
 return(tout1);
 }

void  DesSetOddParity(des_cblock *key)
 {
 int i;

 for (i=0; i<DES_KEY_SZ; i++)
  (*key)[i]=odd_parity[(*key)[i]];
 }

int DesChkPar(des_cblock *key)
 {
 int i;

 for (i=0; i<DES_KEY_SZ; i++)
  {
  if ((*key)[i] != odd_parity[(*key)[i]])
   return(0);
  }
 return(1);
 }

int fUuEncode(unsigned char *pIn,int *pNum)
 {
 int n,tot=0;
 unsigned long l;
 unsigned char *p,*i;
 unsigned char pOut[BUFSIZE];
 p=pOut;
 i=pIn;

 if (*pNum%3) *(p++)=(3 - (*pNum%3)) + ' ';
 else *(p++)=0+' ';
 for (n=0; n<*pNum; n+=3)
  {
  ccc2l(i,l);
  *(p++)=(unsigned char)((l>>18)&0x3f)+' ';
  *(p++)=(unsigned char)((l>>12)&0x3f)+' ';
  *(p++)=(unsigned char)((l>> 6)&0x3f)+' ';
  *(p++)=(unsigned char)((l    )&0x3f)+' ';
  tot+=4;
  }
 tot+=1;
 *p='\0';
 l=0;
 strcpy(pIn,pOut);
 *pNum = tot;
 return(tot);
 }


int fUuDecode(unsigned char *in,int *pNum)
 {
 #define blank '\n'-' '
 int j,i,rem,n;
 unsigned int space=0;
 unsigned long l;
 unsigned long w,x,y,z;
 unsigned char *out;

  out=in;

  rem= *(in++)-' ';
  n = ((*pNum-1)*3/4) - rem;
  if (n == blank)
   {
   n=0;
   in--;
   }

  for (i=0, j=0; i<n; j+=4,i+=3)
   {
    w= *(in++)-' ';
    x= *(in++)-' ';
    y= *(in++)-' ';
    z= *(in++)-' ';
    l=(w<<18)|(x<<12)|(y<< 6)|(z    );
    l2ccc(l,out);
   }
 *out='\0';
 w=x=y=z=0;
 *pNum = n;
 return(n);
 }



