/*==========================================================================*/
/*                                                                          */
/*                                 engerr.h                                 */
/*                                                                          */
/*  DESCRIPTION:   Error codes rule enginecode
 *
 *  NOTES:
 *                                                                          */
/*                                                                          */
/*==========================================================================*/
/*                                                                          */
/*
$Source:
 *
$Log:
 *
 *                                                                          */
/*                                                                          */
/*==========================================================================*/
#ifndef ENGERR_H
#define ENGERR_H

/*
** $Id: engerr.h,v 1.1 1998/06/08 22:55:28 jcb Exp $
** $Author: jcb $
** $Date: 1998/06/08 22:55:28 $
** $Locker:  $
** $Log: engerr.h,v $
** Revision 1.1  1998/06/08 22:55:28  jcb
** Initial revision
**
 * Revision 1.1  1998/01/14  21:43:15  jcb
 * Initial revision
 *
 * Revision 2.1  1997/01/16  22:44:06  can
 * Lots of changes for 2.1 release
 *
** Revision 2.0  1996/03/18 18:45:44  gak
** GUTS 2.0.1 alpha release
**
 * Revision 1.8  1995/08/23  17:23:53  can
 *  Made all lines less than 100 bytes
 *
 * Revision 1.7  1995/08/11  18:27:31  can
 * Modified MakeRuleErrorString to take error string buffer instead of
 * returning it.
 *
 * Revision 1.6  1995/08/07  23:41:13  can
 * Added message for no return from a rule
 *
 * Revision 1.5  1995/08/02  00:06:08  can
 * Modified call to MakeParseErrorString to put rule type in
 *
 * Revision 1.3  1995/07/21  16:24:06  can
 * Added error message codes
 *
 * Revision 1.2  1995/07/16  18:23:31  can
 * Added error messages for the check to ensure that an expression does
 * something (must set a value, call a function, or call a rule)
 *
 * Revision 1.1  1995/07/09  23:07:20  can
 * Initial revision
 *
 * Revision 1.1  1995/07/03  21:18:26  wle
 * Initial revision
 *
*/

#include "error.h"

#define RBENG   0x02010000


#include "calctab.h"
#include "parstrc.h"
#include "engine.h"
#include "calctab.h"

long  MakeEngineError(long status, tCalcTable *calcTab);
long  AddErrMsg(long errCode, char* token, int errData, tParse *parse);
long  GetHighestParseSeverity(tRuleData *ruleData);
long  MakeRuleErrorString(char* buf, char* ruleName, tRuleErr *error, int giveLines, int type);
long  MakeParseError(long status, tRuleBaseData *ruleBaseData, int giveLines);




/*-------------------- ERROR CODES ----------------------- */
/* KEEP THIS STRUCTURE INTACT SO THAT THE ERROR.D FILE CAN */
/* BE EASILY GENERATED.                                    */
/*-------------------------------------------------------- */

/* SEVERITY */
#define FATAL_ENGINE          (RBENG|FATAL)
#define FAIL_ENGINE           (RBENG|FAILURE)
#define WARN_ENGINE           (RBENG|WARN)
#define INFORM_ENGINE         (RBENG|INFORM)


/* CLASS */
#define LEX_RULE             0x0100       
#define PARSE_EXPR           0x0200
#define PARSE_FUNC           0x0300
#define PARSE_SET            0x0400
#define PARSE_STATMNT        0x0500
#define PARSE_STATMNT_LST    0x0600
#define PARSE_ELSE           0x0700
#define PARSE_IF             0x0800
#define PARSE_IN             0x0900
#define PARSE_RULE           0x0A00
#define PARSE_RULE_CALL      0x0B00
#define PARSE_RETURN         0x0C00
#define PARSE_LOCVARS        0x0D00
#define PARSE_RULE_BASE      0x0E00
#define PARSE_CASE           0x0F00
#define PARSE_WHILE          0x1000
#define PARSE_FOR            0x1100
#define PARSE_LOCTYPE        0x1200
#define PARSE_LOCVAR         0x1300
#define PARSE_LOCCONST       0x1400
#define CREATE_TMPVAR        0x1500
#define PARSE_RECORD         0x1600
#define PARSE_ARRAY          0x1700
#define PARSE_TERMINATE      0x1800
#define PARSE_RBEXEC         0x1900
#define PARSE_ARRAY_IDX      0x1A00
#define CHECK_FIELD          0x1B00
#define PARSE_SIZEOF         0x1C00
#define PARSE_RESOLVE        0x1D00
#define PARSE_EXISTS         0x1E00
#define PARSE_RESERROR       0x1F00

#define CREAT_RB             0x2000
#define PARSE_RB             0x2100
#define CREATE_TABLE         0x2200
#define RUN_SUPPORT_FUNC     0x2300
#define RUN_CALC_TABLE       0x2400
#define EXEC_RB              0x2500
#define FREE_RB              0x2600
#define EVAL_ATOM            0x2700
#define CALC_NODE            0x2800
#define GET_MEMORY           0x2900
#define RESOLVE_REC          0x2A00
#define GET_POOL_DATA        0x2B00
#define RESOLVE_TMPLT        0x2C00



/* LOW */
#define UNKNWN_CHAR             0x0001
#define UNMTCH_DQUOTE           0x0002
#define TOO_MANY_DOTS           0x0003

#define UNMTCH_SQUOTE           0x0005
#define UNMTCH_RPAREN           0x0006
#define UNMTCH_LPAREN           0x0007
#define UNEXPCT_KEYWD           0x0008
#define OPERATR_EXPCT           0x0009
#define UNMTCH_RBRACK           0x000A
#define UNMTCH_LBRACK           0x000B
#define EARLY_TERM              0x0010
#define OPERAND_EXPCT           0x0011
#define PARAM_NOT_ARRAY         0X0012
#define PARAM_NOT_RECARRAY      0X0013
#define BAD_STATMNT             0x0014
#define EMPTY_STATMNT           0x0015
#define UNMTCH_BEGIN            0x0016
#define UNMTCH_END              0x0017
#define SYNTX_ERR               0x0018
#define IF_NO_THEN              0x0019
#define IGNORE_AFTER_LINE       0x001A
#define IGNORE_AFTER_TOK        0x001B
#define RULE_NOT_FOUND          0x001C
#define FUNC_NOT_FOUND          0x001D
#define BAD_STRING_OP           0x0020
#define CANT_NEG_STR            0x0021
#define OUT_OF_MEMORY           0x0022
#define BAD_NUM_PARMS           0x0023
#define PARAM_NOT_REF           0x0024
#define ILLEGAL_LVALUE          0x0025
#define SET_READ_ONLY_TMPLT     0x0027
#define SET_READ_ONLY_FLD       0x0028
#define REF_READ_ONLY_TMPLT     0x0029
#define REF_READ_ONLY_FLD       0x002A
#define ILLEGAL_IN              0x002B
#define EQUAL_NOT_SET           0x002C
#define EXPR_NO_FUNCTION        0x002D
#define TYPE_MISMATCH_DT        0X002E
#define TYPE_MISMATCH_DN        0X002F
#define TYPE_MISMATCH_DS        0X0030
#define TYPE_MISMATCH_TD        0X0031
#define TYPE_MISMATCH_TN        0X0032
#define TYPE_MISMATCH_TS        0X0033
#define TYPE_MISMATCH_ND        0X0034
#define TYPE_MISMATCH_NT        0X0035
#define TYPE_MISMATCH_NS        0X0037
#define TYPE_MISMATCH_SN        0X0038
#define TYPE_MISMATCH_DNONE     0X0039
#define TYPE_MISMATCH_TNONE     0X003A
#define TYPE_MISMATCH_NNONE     0X003B
#define TYPE_MISMATCH_SNONE     0X003C
#define TYPE_MISMATCH_NONE      0X003D
#define SET_PARAMETER           0X003E
#define NOT_STRING              0X003F
#define BAD_VARDEF_TOK          0X0040
#define VAR_EXISTS_PARAM        0X0041
#define MISSING_COLON           0X0043
#define MISSING_SEMI_COLON      0X0044
#define BAD_VAR_TYPE            0X0045
#define EMPTY_BLOCK             0X0046
#define DUP_RULE                0X0047
#define NO_RETURN               0X0048
#define VAR_NOT_ARRAY           0X0049
#define CASE_NO_OF              0x004A
#define CASE_NOT_NUMERIC        0x004B
#define CASE_NOT_STRING         0x004C
#define BAD_CASE_TOK            0x004D
#define CASE_NO_END             0x004E
#define FOR_USE_EQL             0x004F
#define FOR_BAD_INIT            0x0050
#define FOR_NOT_NUMERIC         0x0051
#define MISSING_TO              0x0052
#define FOR_NO_DO               0x0053
#define WHILE_NO_DO             0x0054
#define TYPE_EXISTS_TMPLT       0x0055
#define VAR_EXISTS_TMPLT        0x0056
#define FLD_EXISTS              0x0057
#define BAD_REC_TERM            0x0058
#define BAD_FLD_TYPE            0x0059
#define TYPE_DEF_USE_EQUAL      0x005A
#define MISSING_SET             0x005B
#define TYPE_EXISTS_LOCTYPE     0x005C
#define TYPE_EXISTS_GLOBTYPE    0x005D
#define CONST_DEF_USE_EQUAL     0x005E
#define CONST_EXISTS_PARAM      0x005F
#define CONST_EXISTS_LOCCONST   0x0060
#define CONST_EXISTS_LOCVAR     0x0061
#define CONST_EXISTS_GLOBCONST  0x0062
#define CONST_EXISTS_GLOBVAR    0x0063
#define BAD_CONST_VALUE         0x0064
#define VAR_EXISTS_LOCCONST     0x0066
#define VAR_EXISTS_LOCVAR       0x0067
#define VAR_EXISTS_GLOBCONST    0x0068
#define VAR_EXISTS_GLOBVAR      0x0069
#define BAD_STRLEN              0x006A
#define STRLEN_NO_END           0x006B
#define VAR_NOT_FOUND           0x006C
#define FLD_NOT_FOUND           0x006D
#define FLD_NOT_RECORD          0x006E
#define PARAM_NOT_RECORD        0x006F
#define VAR_NOT_RECORD          0x0070
#define RECORD_NOT_DEFINED      0x0071
#define SET_RECMISMATCH         0x0072
#define BAD_RECORD_OP           0x0073
#define BAD_TYPE                0x0074
#define BAD_CASE_TYPE           0x0075
#define CASE_VAL_DUP            0x0076
#define CANT_MOD_LOOPVAR        0x0077
#define LOOP_NOTVAR             0x0078
#define TERM_NOT_NUMERIC        0x0079
#define FOR_NO_RUN              0x007A
#define FOR_ZERO_STEP           0x007B
#define EMPTY_CASE              0x007C
#define BAD_WHILE_LIMIT         0x007D
#define END_IN_LIST             0x007E
#define WARN_CALC_RULE          0x007F
#define FAIL_CALC_RULE          0x0080
#define FATAL_CALC_RULE         0x0081
#define EXEC_NOT_NAME           0x0082
#define EXEC_NOT_FOUND          0x0083
#define BAD_INDEX_TYPE          0x0084
#define INDEX_OB_LO             0x0085
#define INDEX_OB_HI             0x0086
#define MISSING_LBRACKET        0x0087
#define BAD_START_INDEX         0x0088
#define MISSING_DOTDOT          0x0089
#define BAD_END_INDEX           0x008A
#define BAD_RANGE               0x008B
#define MISSING_RBRACKET        0x008C
#define MISSING_OF              0x008D
#define BAD_ARRAY_TYPE          0x008E
#define SET_ARRMISMATCH         0x008F
#define VAR_NOT_RECARRAY        0x0090
#define FLD_NOT_RECARRAY        0x0091
#define FLD_NOT_ARRAY           0x0092
#define ARRAY_OF_ARRAY          0x0093
#define UNDEF_SUB_TMPLT         0x0094
#define SUBTMPLT_NO_REC         0x0095
#define REC_NO_SUBTMPLT         0x0096
#define ALWAYS_ONE              0x0097
#define NO_LPAREN               0x0098
#define NO_RPAREN               0x0099
#define RESOLVE_BADTYPE         0x009A
#define RESOLVE_NOCOMMA         0x009B
#define RESOLVE_NONUMLEN        0x009C

#define BAD_STATEMENT_TYPE      0x00A0
#define UNKNOWN_OPERATOR        0x00A1
#define NO_CALC_TABLE           0x00A2
#define FUNCTION_FAILED         0x00A3
#define BAD_ATOM_TYPE           0x00A4
#define UNKNOWN_BLTIN           0x00A5
#define MEM_POOL_EXCEEDED       0x00A6
#define NO_EVAL_RULES           0x00A7
#define RULE_PARAM_MISMTCH      0x00A8
#define WHILE_LIMIT_HIT         0x00A9
#define UNKNOWN_FLD             0x00AA
#define NO_REQ_FLD              0x00AB
#define BAD_LENGTH              0x00AC
#define SIZE_TOO_BIG            0x00AD
#define BAD_MALLOC              0x00AE
#define NO_REC_DATA             0x00AF
#define MAX_ELEM_REACHED        0x00B0
#define BAD_CALC_TYPE           0x00B1


#endif
