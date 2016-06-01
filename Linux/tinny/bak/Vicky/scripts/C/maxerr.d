#ifndef MAXERR_D
#define MAXERR_D

#include "maxerr.h"

errStruct errOutSeverity[] = {
{ FATAL_MAX,                        "FATAL_MAX",                        TRUE },
{ FAIL_MAX,                         "FAIL_MAX",                         TRUE },
{ WARN_MAX,                         "WARN_MAX",                         TRUE },
{ INFORM_MAX,                       "INFORM_MAX",                       TRUE },
{ (-99),                            "",                                 TRUE }
};

errStruct errOutClass[] = {
{ MAX_INIT,                         "MAX_INIT",                         TRUE },
{ MAX_PINIT,                        "MAX_PINIT",                        TRUE },
{ MAX_SCORE,                        "MAX_SCORE",                        TRUE },
{ MAX_FREE,                         "MAX_FREE",                         TRUE },
{ RULES_INIT,                       "RULES_INIT",                       TRUE },
{ GET_TMPLTS,                       "GET_TMPLTS",                       TRUE },
{ GET_RBS,                          "GET_RBS",                          TRUE },
{ PROC_DATA,                        "PROC_DATA",                        TRUE },
{ FREE_RULES,                       "FREE_RULES",                       TRUE },
{ (-99),                            "",                                 TRUE }
};

errStruct errOutLow[] = {
{ OUT_OF_MEMORY,                    "OUT_OF_MEMORY",                    TRUE },
{ NO_TRAN_TEMPLATE,                 "NO_TRAN_TEMPLATE",                 TRUE },
{ NO_PROF_TEMPLATE,                 "NO_PROF_TEMPLATE",                 TRUE },
{ NO_SCORE_TEMPLATE,                "NO_SCORE_TEMPLATE",                TRUE },
{ NO_EXPLN_TEMPLATE,                "NO_EXPLN_TEMPLATE",                TRUE },
{ NO_TRAN_TYPE,                     "NO_TRAN_TYPE",                     TRUE },
{ BAD_HANDLE,                       "BAD_HANDLE",                       TRUE },
{ CANT_OPEN_FILE,                   "CANT_OPEN_FILE",                   TRUE },
{ BAD_TMPLTFILE_PARM,               "BAD_TMPLTFILE_PARM",               TRUE },
{ BAD_RBFILE_PARM,                  "BAD_RBFILE_PARM",                  TRUE },
{ MODEL_ERR,                        "MODEL_ERR",                        TRUE },
{ CANT_READ_RULEBASES,              "CANT_READ_RULEBASES",              TRUE },
{ CANT_READ_TEMPLATES,              "CANT_READ_TEMPLATES",              TRUE },
{ BAD_RULE_PARSE,                   "BAD_RULE_PARSE",                   TRUE },
{ (-99),                            "",                                 TRUE }
};

#endif
