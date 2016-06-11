#ifndef HNCDEFINES_H
#define HNCDEFINES_H
/*
** $Id: hncdef.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: hncdef.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 1.1  1996/04/24  18:55:32  can
 * Initial revision
 *
 * Revision 1.7  1995/12/13  20:14:29  wle
 * Corrected #define statement to prevent multiple inclusion of file.
 *
 * Revision 1.6  1995/12/13  18:36:55  jrw
 * added definitions for use when obtaining database name parameter
 *
 * Revision 1.5  1995/08/18  18:10:26  gak
 * Added hncBROADCAST
 *
 * Revision 1.4  1995/07/25  16:25:16  chaz
 * Added hncMASTER constant.  First required by hncShell.
 *
 * Revision 1.3  1995/07/05  15:19:47  wle
 * Increased size of template name length to 16.
 *
 * Revision 1.2  1995/06/19  23:29:22  wle
 * Whole bunch of changes
 *
 * Revision 1.1  1995/06/15  22:34:07  wle
 * Initial revision
 *
 * Revision 1.1  1995/06/14  21:34:31  wle
 * Initial revision
 *
*/

#define hncMAX_PRODUCTS 16
#define hncMAX_PROCESSES 99 

#define hncPROCESSNAMELEN 16
#define hncSERVICENAMELEN 16
#define hncHOSTNAMELEN 31
#define hncTEMPLATENAMELEN 16

#define hncERRORCODELEN 10

#define hncDBNAME "DATABASE"
#define hncDBNAMELEN 17

#define hncDBUSERIDLEN 20
#define hncDBPASSWORDLEN 20

#define hncMASTER "hncMaster"
/* Universal constant for name of the master process */

#define hncBROADCAST "BROADCAST" /* broadcast service */
#endif
