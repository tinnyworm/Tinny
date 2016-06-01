/*==========================================================================*/
/*                                                                          */
/*                               error.h                                    */
/*                                                                          */
/*  DESCRIPTION:   Generally useful error stuff
 *
 *  NOTES:
 *                                                                          */
/*                                                                          */
/*==========================================================================*/
#ifndef ERROR_H
#define ERROR_H

/*
** $Id: error.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: error.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.0  1996/03/18  18:45:44  gak
 * GUTS 2.0.1 alpha release
 *
*/

#define FATAL     0x10000000
#define NOSCORE   0x20000000
#define FAILURE   0x20000000
#define WARN      0x30000000
#define INFORM    0x40000000

#define notWarning(err) (((err) & 0xF0000000) != WARN)
#define isWarning(err)  (((err) & 0xF0000000) == WARN)
#define notFatal(err)   (((err) & 0xF0000000) != FATAL)
#define isFatal(err)    (((err) & 0xF0000000) == FATAL)
#define notNoScore(err) (((err) & 0xF0000000) != NOSCORE)
#define isNoScore(err)  (((err) & 0xF0000000) == NOSCORE)
#define notFailure(err) (((err) & 0xF0000000) != FAILURE)
#define isFailure(err)  (((err) & 0xF0000000) == FAILURE)
#define notInform(err)  (((err) & 0xF0000000) != INFORM)
#define isInform(err)   (((err) & 0xF0000000) == INFORM)

#ifndef OK
#define OK 0L
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct {
  long errNum;
  char * errStr;
  int expected;
} errStruct;

#endif
