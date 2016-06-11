#ifndef BLTINS_H
#define BLTINS_H
/*
** $Id: bltins.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: bltins.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.2  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:55:58  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.2  1995/07/26  20:53:26  can
 * Added EVAL_IP functions.
 * Also added EVAL_EXPRESSION .
 * l.
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/


#define EVAL_NOT                 1
#define EVAL_NEGATE              2
#define EVAL_RULE                3
#define EVAL_STRING_TO_NUMERIC   4
#define EVAL_SET                 5
#define EVAL_ANDBOOL             6
#define EVAL_ORBOOL              7
#define EVAL_EQUAL               8
#define EVAL_NOTEQL              9
#define EVAL_GRTREQL            10
#define EVAL_LESSEQL            11
#define EVAL_GRTR               12
#define EVAL_LESS               13
#define EVAL_MINUS              14
#define EVAL_PLUS               15
#define EVAL_IN                 16 
#define EVAL_MULTIPLY           17
#define EVAL_DIVIDE             18
#define EVAL_IF                 19
#define EVAL_RETURN             20
#define EVAL_VAR_RETURN         21
#define EVAL_TERMINATE          22
#define EVAL_EXPRESSION         23
#define EVAL_WHILE              24
#define EVAL_FOR                25
#define EVAL_CASE               26 
#define EVAL_RBEXEC             27
#define EVAL_CALC_VAR           28
#define EVAL_POWER              29
#define EVAL_ARRAY_VAR          30
#define EVAL_SIZEOF             31
#define EVAL_RESOLVE            32
#define EVAL_EXISTS             33
#define EVAL_FLT_INCR           34
#define EVAL_FLT_IPADD          35
#define EVAL_FLT_IPSUB          36
#define EVAL_FLT_IPMUL          37
#define EVAL_FLT_IPDIV          38
#define EVAL_INCREMENT          39
#define EVAL_IPADD              40
#define EVAL_IPSUB              41
#define EVAL_IPMUL              42
#define EVAL_IPDIV              43
  
#endif
