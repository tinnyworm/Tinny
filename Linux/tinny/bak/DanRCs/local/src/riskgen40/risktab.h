/**
 * @file
 * @author Danfeng Li <danfengli@fairisaac.com>
 * @date
 *
 * @brief
 *
 * $Log:$
 *
 */


#ifndef RISKTAB_H
#define RISKTAB_H

/* input transaction information */
typedef struct {
  int					tag;	/* fraud tag, 1 -- fraud, 0 -- nonfraud */
  unsigned long 	amt;	/* tran amt in US penny up to 4,294,967,295 */
  unsigned long	dist;	/* distance from home */
  int					key;	/* is_keyed, 1 -- keyed, 0 -- not keyed */
  unsigned int		how;	/* hour of week */
  unsigned int		sic;	
  char				post[10];	/* postal code, will only use zip3 */
  unsigned int		cntry;/* merchant cntry code */
  unsigned int		curr;	/* tran currency code */
} tranInfo;



#endif /* RISKTAB_H */


