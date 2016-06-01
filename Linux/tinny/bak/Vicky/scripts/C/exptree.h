#ifndef EXPTREE_H
#define EXPTREE_H

/*
** $Id: exptree.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: exptree.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.3  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
 * Revision 2.1  1996/04/22  17:56:11  can
 * Several fixes and changes for 2.1
 *
 * Revision 1.7  1995/08/25  01:25:48  can
 * Fixed compile warning messages
 *
 * Revision 1.6  1995/08/16  17:59:31  can
 * Make all tree freeing functions externally linkable, for use in
 * the parse routines.
 *
 * Revision 1.5  1995/08/02  00:07:53  can
 * Took out include of varuse.h because it is no longer needed
 *
 * Revision 1.4  1995/07/21  16:24:19  can
 * Added support for variable and parameter usage
 *
 * Revision 1.3  1995/07/16  18:24:58  can
 * Added CheckExpressionFunction prototype
 *
 * Revision 1.2  1995/07/09  23:07:20  can
 * Fixed some bugs
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

/* HNC Rules engine include files */
#include "engine.h"

/* HNC Common include files */
#include "hncrtyp.h"

/***********************************************************/
/* Enumerated type for operators.  They are given in order */
/* of their precedence.                                    */
/***********************************************************/
typedef enum eOp {
   NOOP=0,
   SET,
   ANDBOOL,
   ORBOOL,
   EQUAL,
   NOTEQL,
   GRTREQL,
   LESSEQL,
   GRTR,
   LESS,
   PLUS,
   MINUS,
   MULTIPLY,
   DIVIDE,
   POWER
} tOp;

/**************************/
/* Expression branch type */
/**************************/
#define NOTYPE      0

/*********************************/
/* Flags for expression branches */
/*********************************/
#define  EXPR_NOT        0x000001
#define  EXPR_NEGATE     0x000002
#define  EXPR_CONSTANT   0x000004
#define  EXPR_VARBLE     0x000008
#define  EXPR_FIELD      0x000010
#define  EXPR_REFERENCE  0x000020
#define  EXPR_TREE       0x000040
#define  EXPR_PAREN      0x000080
#define  EXPR_UNUSED     0x000100
#define  EXPR_FUNCTION   0x000200
#define  EXPR_IN         0x000400
#define  EXPR_RULE       0x000800
#define  EXPR_RBEXEC     0x001000
#define  EXPR_ARRAYVAR   0x002000
#define  EXPR_SIZEOF     0x004000
#define  EXPR_RESOLVE    0x008000
#define  EXPR_EXISTS     0x010000
#define  EXPR_ERRSTR     0x020000
#define  EXPR_ERRNO      0x040000


/*************************************************************/
/* Structures to hold information about an expression branch */ 
/*************************************************************/
typedef struct sParamExprs {
   struct sExprTree   *exprTree;
   struct sParamExprs *next;
} tParamExpr;

typedef struct sFuncCall {
   tFunc      *funcData;
   tRuleData  *ruleData;
   tParamExpr *paramList;
} tFuncCall;

typedef struct sRuleCall {
   tRuleData  *ruleData;
   tParamExpr *paramList;
} tRuleCall;

#define SIZE_DIRECT    1
#define SIZE_ARRAYPROC 2
typedef struct sSizeOf {
   long type;
   char *size;    
} tSizeOf;

typedef struct sArrayUse {
   /* This is a void because it is a tExprTree at parse time, but */
   /* will be a calcNode for the expression at run time           */
   void       *indexExpr; 
   tArrayData *arrayData;
} tArrayUse;

typedef struct sArrayProc {
   long      fldIndex;
   tArrayUse *arrayUse;
   
   struct sArrayProc *next;
} tArrayProc;

typedef struct sRecUse {
   tFldData   *fldData;
   short      noFree;
   short      type;
   char       fldPermission;
   char       recPermission;
   char       *fieldName;
   char       *recordName;
   tArrayProc *arrayProc;
} tRecUse;

typedef struct sInCall {
   struct sExprTree *keyExpr;
   tParamExpr *paramList;
} tInCall;

typedef struct sResolve {
   struct sExprTree *lenExpr;
   struct sExprTree *addrExpr;
   tRecData  *recData;    
} tResolve;

/************************************************/
/* Structure to hold the rule base data and the */
/* corresponding run-time handles               */
/************************************************/
typedef struct sEngRBase {
   tRuleBaseData *RBData;
   int           parseIt;
   void          *handle;

   struct sEngRBase *next;
} tEngRBData;

typedef struct sExprBranch {
   int   type;
   long  flags;
   union {
      tEngNum    num;
      char       *str;
      float      fVal;
      long       lVal;
      tFuncCall  *funcCall;
      tRuleCall  *ruleCall;
      tInCall    *inCall;
      tRecUse    *dataFld;
      tEngRBData *RBExec;
      tEngVar    *var;
      tArrayUse  *arrayUse;
      tSizeOf    *sizeOp;
      tResolve   *resolve;
      struct sExprTree *tree;
  } val;
} tExprBranch;

typedef struct sExprTree {
   tExprBranch left;
   tOp         op;
   tExprBranch right;
   int         type;
   int         reference;
   int         *numParens;
   struct sExprTree *parent;
} tExprTree;


long AllocateExprTree(tExprTree **exprTree);
long CheckExpressionFunction(tExprTree *exprTree);
long FreeExprTree(tExprTree *exprTree);
long FreeExprBranch(tExprBranch exprBranch);
long FreeParamList(tParamExpr *paramList);


#endif
