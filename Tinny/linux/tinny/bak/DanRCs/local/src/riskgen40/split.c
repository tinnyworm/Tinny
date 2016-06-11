/**
 * @file
 * @author Danfeng Li <danfengli@fairisaac.com>
 * @date
 *
 * @brief
 *
 * $Log:$
 *
 * Copied from http://funknet.net/rfunk/Linux/progs/split.c
 *
 */
 
/*
  split.c - attempted implementation of perl's split function

  Rob Funk, 1996
*/

#include "split.h"

char **split (const char *delim, const char *string)
{
  static char **splitarray;
  static char junkstr[MAXLEN];
  int numfields;
  int i=0;
  char *p;
  int dlen; /* length of delimiter (usually will be 1) */

  strcpy (junkstr, string);
  dlen = strlen (delim);
  numfields = 1+countdelim(delim,junkstr);
#ifdef DEBUG
  printf ("%d fields\n",numfields);
#endif /* DEBUG */

  /* allocate array of pointers to field strings */
  splitarray = (char **)malloc(numfields*sizeof(char *));
  if (splitarray == NULL)
    {
      perror("split");
      exit(-1);
    }

  /* parse out the fields into the array */
  splitarray[i] = junkstr;
  p = strstr (splitarray[i], delim);
  while (p != NULL)
    {
      i++;
      *p = '\0';
      p += dlen;
      splitarray[i] = p;
      p = strstr (splitarray[i], delim);
    }
  splitarray[numfields] = NULL; /* signal end of array */

  return (splitarray);
}

int countdelim (const char *delim, const char *string)
/* count the number of times delim appears in string */
{
  int ctr=0; /* counter of delimiter appearances */
  int dlen; /* length of delimiter (usually will be 1) */
  int slen; /* length of string */
  int pos=0; /* string position */

  if ((string == NULL) || (delim == NULL)) return (0);
  dlen = strlen(delim);
  slen = strlen(string);
  if (dlen<1) return (0);

  while (pos<slen)
    {
      if (! strncmp(delim, &(string[pos]), dlen))
	{
	  ctr++;
	  pos += dlen;
	}
      else
	pos++;
    }
  return (ctr);
}
 

