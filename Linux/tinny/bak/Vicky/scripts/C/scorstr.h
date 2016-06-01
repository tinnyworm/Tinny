/* ------------------------------------------------------------------------
 *
 *  FILE:  hscorstr
 *
 *  DESCRIPTION:  Falcon Score data structure
 *
 * ----------------------------------------------------------------------- */

#ifndef _SCORE_STRU
#define _SCORE_STRU

#define MAXREASONS  3

typedef struct {
  int pFraudScore;
  short reasons[MAXREASONS];
  char  modelName[10];
} SCORESTRUCT;

#endif

