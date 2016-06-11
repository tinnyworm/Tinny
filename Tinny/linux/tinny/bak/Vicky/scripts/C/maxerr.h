/*==========================================================================*/
/*                                                                          */
/*                               maxerr.h                                   */
/*                                                                          */
/*  DESCRIPTION:   Error codes for ProfitMax
 *
 *  NOTES:
 *                                                                          */
/*                                                                          */
/*==========================================================================*/
#ifndef MAXERR_H
#define MAXERR_H

/*
** $Id: maxerr.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: maxerr.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
** Revision 1.2  1996/05/01 01:07:16  can
** Added NO_TRAN_TYPE for check of tran type in MAXPINIT
**
*/

#include "error.h"

#define MAX    0x01010000


/*-------------------- ERROR CODES ----------------------- */
/* KEEP THIS STRUCTURE INTACT SO THAT THE ERROR.D FILE CAN */
/* BE EASILY GENERATED.                                    */
/*-------------------------------------------------------- */

/* SEVERITY */
#define FATAL_MAX          (MAX|FATAL)
#define FAIL_MAX           (MAX|FAILURE)
#define WARN_MAX           (MAX|WARN)
#define INFORM_MAX         (MAX|INFORM)


/* CLASS */
#define MAX_INIT           0x0100
#define MAX_PINIT          0x0200
#define MAX_SCORE          0x0300
#define MAX_FREE           0x0400

#define RULES_INIT         0x1000
#define GET_TMPLTS         0x1100
#define GET_RBS            0x1200
#define PROC_DATA          0x1300
#define FREE_RULES         0x1400


/* LOW */
#define OUT_OF_MEMORY      0x0001
#define NO_TRAN_TEMPLATE   0x0002
#define NO_PROF_TEMPLATE   0x0003
#define NO_SCORE_TEMPLATE  0x0004
#define NO_EXPLN_TEMPLATE  0x0005
#define NO_TRAN_TYPE       0x0006
#define TMPLT_NOT_FOUND    0x0007

#define BAD_HANDLE           0x0010
#define CANT_OPEN_FILE       0x0011
#define BAD_TMPLTFILE_PARM   0x0012
#define BAD_RBFILE_PARM      0x0013
#define MODEL_ERR            0x0014
#define CANT_READ_RULEBASES  0x0015
#define CANT_READ_TEMPLATES  0x0016
#define BAD_RULE_PARSE       0x0017

#endif
