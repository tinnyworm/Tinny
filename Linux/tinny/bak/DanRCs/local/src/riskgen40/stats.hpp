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


#ifndef STATS_HPP
#define STATS_HPP

class stats {
   unsigned long        count;   /* total # of transactions */
   long long            sum;     /* sum */
   unsigned long long   sum2;    /* sum of x^2 */
//   float                mu;      /* mean */
//   float                std;     /* standard deviation */
public :
   stats() {count=0; sum=0; sum2=0;}
   ~stats();
   void accumulate(const long x);
//   {
//      count++;
//      sum += x;
//      sum2 += x*x;
//   }
   float mean() {return (float)(sum)/(float)count;}
   float std() {return (float)(sum2)/(float)count - mean()*mean();}
};



#endif /* STATS_HPP */

