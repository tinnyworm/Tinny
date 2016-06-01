/*==========================================================================*/
/*                                                                          */
/*                               optinet.h                                  */
/*                                                                          */
/*  DESCRIPTION:   Error codes for the OptiNet neural net code
 *
 *  NOTES:
 *                                                                          */
/*                                                                          */
/*==========================================================================*/
#ifndef OPTIERR_H
#define OPTIERR_H

/*
** $Id: optierr.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: optierr.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.2  1997/01/18  03:28:39  can
 * Added new error codes for missing value and div/range
 *
 * Revision 1.1  1997/01/17  19:17:40  can
 * Initial revision
 *
**
*/

#include "error.h"

#define NET    0x01010000


/*-------------------- ERROR CODES ----------------------- */
/* KEEP THIS STRUCTURE INTACT SO THAT THE ERROR.D FILE CAN */
/* BE EASILY GENERATED.                                    */
/*-------------------------------------------------------- */

/* SEVERITY */
#define FATAL_NET          (NET|FATAL)
#define FAIL_NET           (NET|FAILURE)
#define WARN_NET           (NET|WARN)
#define INFORM_NET         (NET|INFORM)


/* CLASS */
#define NET_INIT           0x0100
#define NET_ITER           0x0200
#define NET_FREE           0x0300


/* LOW */
#define BAD_CTA_FORMAT            0x01
#define BAD_WTA_FORMAT            0x02
#define CANT_OPEN_CFG             0x03
#define CANT_OPEN_CTA             0x04
#define CANT_OPEN_WTA             0x05
#define CFG_NODE_MISMATCH         0x06
#define IN_WEIGHT_SCAN_ERROR      0x07
#define NO_NET_MEMORY             0x08
#define NUM_HIDDEN_MISMATCH       0x09
#define NUM_WEIGHT_MISMATCH       0x0A
#define OUT_WEIGHT_SCAN_ERROR     0x0B
#define BAD_CFG_TYPE              0x0C
#define BAD_CFG_SLAB              0x0D
#define BAD_CFG_LENGTH            0x0E
#define BAD_CFG_MINMAX            0x0F
#define BAD_CFG_MISSVAL           0x10
#define BAD_CFG_MEAN              0x11
#define BAD_CFG_STDDEV            0x12
#define BAD_CFG_NUMSYM            0x13
#define BAD_SYM_READ              0x14
#define BAD_SYM_INDEX             0x15
#define BAD_SYM_LENGTH            0x16
#define BAD_CFG_SCALEFN           0x17
#define BAD_CFG_DIVRANGE          0x18
#define BAD_CFG_RECCNT            0x19


#endif
