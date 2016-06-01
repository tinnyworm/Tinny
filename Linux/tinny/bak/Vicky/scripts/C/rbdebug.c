#include <stdio.h>

short debug=0;


long debug_out(char* msg, void* arg)
{
   if(debug && arg)
      fprintf(stderr, msg, arg); 

   return(0);
}
