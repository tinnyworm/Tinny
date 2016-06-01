/**
 * @file
 * @author Danfeng Li <dli1@ifp.uiuc.edu>
 * @date
 *
 * @brief
 *
 * $Log:$
 *
 */
 
#include "stats.hpp"
#include<stdio.h>

void stats::accumulate(const long x)
{
   count++;
   sum += x;
   sum2 += x*x;
}

int main(int argc, char *argv[]) 
{
   stats s();
   long a[]={1,2,3,4,5};

   for(int i=0; i<5; i++) {
      s.accumulate(a[i]);
   }

   printf("mean=%f\n",s.mean());
   printf("std=%f\n",s.std());
}

   

