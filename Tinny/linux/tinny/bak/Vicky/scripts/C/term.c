/* ======================================================================*/
/*  c routine term to be called by c as term(0);
/* ======================================================================*/

#include <stdio.h>
#include <sys/signal.h>
int *j;

void   term(i)
int *i;
{
       int termit();
       j =  i;

       fflush(stdout);
       setbuf(stdout,NULL);

       fflush(stderr);
       setbuf(stderr,NULL);
}

void   termit(sig)
        int sig;
{
        printf("  termit called \n");
        fflush(stdout);
        *j = 1;
/*        exit_(); */
}
