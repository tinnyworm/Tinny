#ifndef STD_H
#define STD_H

/* -------------------------------------------------------------------------
 *
 *  FILE: std.h 
 *
 *  DESCRIPTION: standard interface controller. 
 *
 * ----------------------------------------------------------------------- */

/* Define the size of each record type (transaction, card, posting, auth) */
#define TRAN_LENGTH   273
#define CARD_LENGTH   186 
#define AUTH_LENGTH   164
#define POST_LENGTH   143
#define FRAUD_LENGTH   49
#define NONMON_LENGTH  47
#define PAY_LENGTH    107

/* Length of key used to identify accounts */
#define KEYLENMAX     19

typedef enum {
IS_DATE=0,
IS_TIME,
IS_NUMERIC,
IS_ALPHNUM,
IS_AorP,
IS_IRorF,
IS_MorF,
IS_SGorO,
IS_PBorO,
IS_ADIPRorV,
IS_CMKTRorO,
IS_IorV,
IS_IVNorZ,
IS_KSU,
IS_NorY,
IS_ACLNOSorT,
IS_CorS,
IS_RorN,
IS_ACFKLMNOPQRorS,
IS_ACFKLMNOPQRSorT,
IS_ATPRDorO
} fType;

typedef struct {
  char  name[25];
  int   offset;
  int   length;
  fType type;
} FIELD_DATA;

typedef enum {
ACCOUNT_NUMBER=0,
AUTH_POST_FLAG,
POSTCODE,
CARD_CNTRY_CODE,
ACCOUNT_OPEN_DATE,
NEW_PLASTIC_DATE,
PLASTIC_ISSUE_TYPE,
ACCOUNT_EXPIRE_DATE,
CARD_EXPIRE_DATE,
AVAIL_CREDIT,
CREDIT_LINE,
GENDER,
BIRTHDATE,
NUM_CARDS,
INCOME,
CARD_TYPE,
CARD_USE,
TRAN_DATE,
TRAN_TIME,
TRAN_AMOUNT,
TRAN_CURR_CODE,
TRAN_CURR_CONV,
AUTH_RESULT,
TRAN_TYPE,
MERCH_CAT, 
MERCH_POST_CODE,
MERCH_CNTRY_CODE,
AUTH_PIN_VERIFY,
AUTH_CVV_VERIFY,
AUTH_KEYED_SWIPED,
POST_DATE,
POST_AUTHED,
NEW_POSTCODE,
NEW_CREDIT_LINE,
MINIMUM_DUE,
TOTAL_DUE,
BALANCE,
PAYMENT_DUE_DATE,
STATEMENT_DATE,
DEPOSIT_METHOD,
MERCH_ID,

NUM_TRAN_FIELDS
} TRAN_FIELDS;


/**************************************************/
/* Identifiers for possible incoming field values */
/**************************************************/
/* For AUTH_POST_FLAG */
#define ISAUTH      'A'
#define ISPOST      'P'
#define ISNONMON    'N'
#define ISPAY       'Y'

/* Catch-all types for all fields */
#define UNKNOWN     ' '
#define OTHER       'O'

/* For PLASTIC_ISSUE_TYPE */
#define INITISSUE   'I'
#define REISSUE     'R'
#define FORCEMBOS   'F'

/* For GENDER */
#define MALE        'M'
#define FEMALE      'F'
#define I_MALE      10 
#define I_FEMALE     0 

/* For CARD_TYPE */
#define STANDARD    'S'
#define GOLD        'G'

/* For CARD_USE */
#define PERSONAL    'P'
#define BUSINESS    'B'

/* For AUTH_RESULT */
#define APPROVE     'A'
#define DECLINE     'D'
#define APPRPOSID   'I'
#define PICKUP      'P'
#define REFERRAL    'R'

/* For TRAN_TYPE */
#define T_CASH      'C'
#define T_MERCH     'M'
#define T_CASHBACK  'B'
#define T_CONVCHECK 'K'
#define T_BALTRANS  'T'
#define T_CREDIT    'R'
#define T_REVERSE   'V'
#define T_ADDRVER   'A'
#define T_CODE10    'S'

/* For NONMON_TRAN_TYPE */
#define N_ADDRCHG   'A'
#define N_PINCHG    'C'
#define N_FIRSTPIN  'F'
#define N_CHECKREQ  'K'
#define N_CRLINCHG  'L'
#define N_MOTHERCHG 'M'
#define N_NAMECHG   'N'
#define N_PHONCHG   'P'
#define N_PLSTREQ   'Q'
#define N_PLSTNORM  'R'
#define N_SSNCHG    'S'

/* For AUTH_PIN_VERIFY and AUTH_CVV_VERIFY */
#define INVALID     'I'
#define VALID       'V'
#define NOTPRESENT  'N'
#define VALUEZERO   'Z'
#define NOTCHECKED  ' '

/* For AUTH_KEYED_SWIPED */
#define KEYED       'K'
#define SWIPED      'S'
#define READ_UNALTERED  'U'

/* For POST_AUTHED */
#define YES         'Y'
#define NO          'N'

/* For PAY_REVERSAL */
#define Y_REVERSAL  'R'
#define Y_NOREVERSE 'N'

/* For PAY_DEPOSIT_METH */
#define Y_ATM       'A'
#define Y_TELLER    'T'
#define Y_POSTAL    'P'
#define Y_ACCTXFER  'R'
#define Y_DROPBOX   'D'

/******************************************************/
/* macros for accessing transaction field information */
/******************************************************/
#define off(fld) std[fld].offset
#define len(fld) std[fld].length
#define name(fld) std[fld].name
#define post(trans) *(trans+std[AUTH_POST_FLAG].offset) == ISPOST
#define auth(trans) *(trans+std[AUTH_POST_FLAG].offset) == ISAUTH
#define nonmon(trans) *(trans+std[AUTH_POST_FLAG].offset) == ISNONMON
#define payment(trans) *(trans+std[AUTH_POST_FLAG].offset) == ISPAY
#define keyed(trans) *(trans+std[AUTH_KEYED_SWIPED].offset) == KEYED

/*************************************/
/* Transaction data controller table */
/*************************************/
static FIELD_DATA std[NUM_TRAN_FIELDS] = {
{"ACCOUNT_NUMBER",       0,  19,  IS_ALPHNUM}, 
{"AUTH_POST_FLAG",      19,   1,  IS_AorP},
{"POSTCODE",            20,   9,  IS_ALPHNUM},
{"CARD_CNTRY_CODE",     29,   3,  IS_NUMERIC},
{"ACCOUNT_OPEN_DATE",   32,   8,  IS_DATE},	
{"NEW_PLASTIC_DATE",    40,   8,  IS_DATE},
{"PLASTIC_ISSUE_TYPE",  48,   1,  IS_IRorF},	
{"ACCOUNT_EXPIRE_DATE", 49,   8,  IS_DATE},
{"CARD_EXPIRE_DATE",    57,   8,  IS_DATE},
{"AVAIL_CREDIT",        65,  10,  IS_NUMERIC},
{"CREDIT_LINE",         75,  10,  IS_NUMERIC},
{"GENDER",              85,   1,  IS_MorF},  
{"BIRTHDATE",           86,   8,  IS_DATE},
{"NUM_CARDS",           94,   3,  IS_NUMERIC},
{"INCOME",              97,  10,  IS_NUMERIC},
{"CARD_TYPE",          107,   1,  IS_SGorO},
{"CARD_USE",           108,   1,  IS_PBorO},
{"TRAN_DATE",          109,   8,  IS_DATE},
{"TRAN_TIME",          117,   6,  IS_TIME},
{"TRAN_AMOUNT",        123,  13,  IS_NUMERIC},
{"TRAN_CURR_CODE",     136,   3,  IS_ALPHNUM},
{"TRAN_CURR_CONV",     139,  13,  IS_NUMERIC},
{"AUTH_RESULT",        152,   1,  IS_ADIPRorV},
{"TRAN_TYPE",          153,   1,  IS_ACFKLMNOPQRSorT},
{"MERCH_CAT",          154,   4,  IS_NUMERIC}, 
{"MERCH_POST_CODE",    158,   9,  IS_ALPHNUM},   
{"MERCH_CNTRY_CODE",   167,   3,  IS_NUMERIC}, 
{"AUTH_PIN_VERIFY",    170,   1,  IS_IorV},
{"AUTH_CVV_VERIFY",    171,   1,  IS_IVNorZ},
{"AUTH_KEYED_SWIPED",  172,   1,  IS_KSU},
{"POST_DATE",          173,   8,  IS_DATE},
{"POST_AUTHED",        181,   1,  IS_NorY},
{"NEW_POSTCODE",       182,   9,  IS_ALPHNUM},
{"NEW_CREDIT_LINE",    191,  10,  IS_NUMERIC},
{"MINIMUM_DUE",        201,  13,  IS_NUMERIC},
{"TOTAL_DUE",          214,  13,  IS_NUMERIC},
{"BALANCE",            227,  13,  IS_NUMERIC},
{"PAYMENT_DUE_DATE",   240,   8,  IS_DATE},
{"STATEMENT_DATE",     248,   8,  IS_DATE},
{"DEPOSIT_METHOD",     256,   1,  IS_ATPRDorO},
{"MERCH_ID",           257,  16,  IS_ALPHNUM}
};


/***********************************************/
/* macros for accessing card field information */
/***********************************************/
#define coff(fld) card[fld].offset
#define clen(fld) card[fld].length
#define cname(fld) card[fld].name

typedef enum {
CARD_ACCT_ID=0,
CARD_POSTAL_CODE,
CARD_CNTRY,
CARD_OPEN_DATE,
CARD_ISS_DATE,
CARD_ISS_TYPE,
CARD_EXPR_DATE,
CARD_CRED_LINE,
CARD_GENDER,
CARD_BIRTHDATE,
CARD_INCOME,
CRD_TYPE,
CRD_USE,
CARD_NUM_CARDS,
CARD_REC_DATE,
CARD_ADDR_1,
CARD_ADDR_2,
CARD_SSN_5,
CARD_ASSOC,
CARD_INCENTIVE,
CARD_XFER_P_ACCT,
CARD_STATUS,
CARD_STATUS_DATE,

NUM_CARD_FIELDS
} CARD_FIELDS;

/******************************/
/* Card file controller table */
/******************************/
static FIELD_DATA card[NUM_CARD_FIELDS] = {
{"CARD_ACCT_ID",       0,  19,  IS_ALPHNUM}, 
{"CARD_POSTAL_CODE",  19,   9,  IS_ALPHNUM},
{"CARD_CNTRY",        28,   3,  IS_NUMERIC},
{"CARD_OPEN_DATE",    31,   8,  IS_DATE},	
{"CARD_ISS_DATE",     39,   8,  IS_DATE},
{"CARD_ISS_TYPE",     47,   1,  IS_DATE},
{"CARD_EXPR_DATE",    48,   8,  IS_DATE},
{"CARD_CRED_LINE",    56,  10,  IS_NUMERIC},
{"CARD_GENDER",       66,   1,  IS_MorF},  
{"CARD_BIRTHDATE",    67,   8,  IS_DATE},
{"CARD_INCOME",       75,  10,  IS_NUMERIC},
{"CRD_TYPE",          85,   1,  IS_SGorO},
{"CRD_USE",           86,   1,  IS_PBorO},
{"CARD_NUM_CARDS",    87,   3,  IS_NUMERIC},
{"CARD_REC_DATE",     90,   8,  IS_NUMERIC},
{"CARD_ADDR_1",       98,  26,  IS_ALPHNUM},
{"CARD_ADDR_2",      124,  26,  IS_ALPHNUM},
{"CARD_SSN_5",       150,   5,  IS_ALPHNUM},
{"CARD_ASSOC",       155,   1,  IS_ALPHNUM},
{"CARD_INCENTIVE",   156,   1,  IS_ALPHNUM},
{"CARD_XFER_P_ACCT", 157,  19,  IS_ALPHNUM},
{"CARD_STATUS",      176,   2,  IS_ALPHNUM},
{"CARD_STATUS_DATE", 178,   8,  IS_DATE}
};


/***********************************************/
/* macros for accessing auth field information */
/***********************************************/
#define aoff(fld) aut[fld].offset
#define alen(fld) aut[fld].length
#define aname(fld) aut[fld].name

typedef enum {
AUTH_ACCT_ID=0,
AUTH_DATE,
AUTH_TIME,
AUTH_AMOUNT,
AUTH_CURR_CODE,
AUTH_CURR_RATE,
AUTH_DECISION,
AUTH_TRAN_TYPE,
AUTH_AVAIL_CRED,
AUTH_CRED_LINE,
AUTH_SIC,
AUTH_POSTAL_CODE,
AUTH_MRCH_CNTRY,
AUTH_PIN_VER,
AUTH_CVV,
AUTH_KEY_SWIPE,
AUTH_MSG_EXPIRE_DATE,
AUTH_AUTH_ID,
AUTH_MERCH_ID,

NUM_AUTH_FIELDS
} AUTH_FIELDS;

/***************************************/
/* Authorization file controller table */
/***************************************/
static FIELD_DATA aut[NUM_AUTH_FIELDS] = {
{"AUTH_ACCT_ID",           0,  19,  IS_ALPHNUM}, 
{"AUTH_DATE",             19,   8,  IS_DATE},
{"AUTH_TIME",             27,   6,  IS_TIME},
{"AUTH_AMOUNT",           33,  13,  IS_NUMERIC},
{"AUTH_CURR_CODE",        46,   3,  IS_ALPHNUM},
{"AUTH_CURR_RATE",        49,  13,  IS_NUMERIC},
{"AUTH_DECISION",         62,   1,  IS_ADIPRorV},
{"AUTH_TRAN_TYPE",        63,   1,  IS_CMKTRorO},
{"AUTH_AVAIL_CRED",       64,  10,  IS_NUMERIC},
{"AUTH_CRED_LINE",        74,  10,  IS_NUMERIC},
{"AUTH_SIC",              84,   4,  IS_NUMERIC}, 
{"AUTH_POSTAL_CODE",      88,   9,  IS_ALPHNUM},   
{"AUTH_MRCH_CNTRY",       97,   3,  IS_NUMERIC}, 
{"AUTH_PIN_VER",         100,   1,  IS_IorV},
{"AUTH_CVV",             101,   1,  IS_IVNorZ},
{"AUTH_KEY_SWIP",        102,   1,  IS_KSU},
{"AUTH_MSG_EXPIRE_DATE", 103,   8,  IS_DATE},
{"AUTH_AUTH_ID",         111,   6,  IS_NUMERIC},
{"AUTH_MERCH_ID",        123,  16,  IS_ALPHNUM}
};


/***********************************************/
/* macros for accessing auth field information */
/***********************************************/
#define poff(fld) pst[fld].offset
#define plen(fld) pst[fld].length
#define pname(fld) pst[fld].name

typedef enum {
POST_ACCT_ID=0, 
POST_AMOUNT, 
POST_CURR_CODE,
POST_CURR_CONV,
POST_POST_DATE,
POST_TRAN_DATE,
POST_TRAN_TIME,
POST_AVAIL_CRED,
POST_CRED_LINE,
POST_TRAN_TYPE,
POST_AUTHORIZED,
POST_SIC,
POST_POSTAL_CODE,
POST_MRCH_CNTRY,
POST_AUTH_ID,
POST_POST_ID,
POST_MERCH_ID,

NUM_POST_FIELDS
} POST_FIELDS;

/*********************************/
/* Posting data controller table */
/*********************************/
static FIELD_DATA pst[NUM_POST_FIELDS] = {
{"POST_ACCT_ID",        0,  19,  IS_ALPHNUM}, 
{"POST_AMOUNT",        19,  13,  IS_NUMERIC},
{"POST_CURR_CODE",     32,   3,  IS_NUMERIC},
{"POST_CURR_RATE",     35,  13,  IS_NUMERIC},
{"POST_POST_DATE",     48,   8,  IS_DATE},
{"POST_TRAN_DATE",     56,   8,  IS_DATE},
{"POST_TRAN_TIME",     64,   6,  IS_TIME},
{"POST_AVAIL_CRED",    70,  10,  IS_NUMERIC},
{"POST_CRED_LINE",     80,  10,  IS_NUMERIC},
{"POST_TRAN_TYPE",     90,   1,  IS_CMKTRorO},
{"POST_AUTHORIZED",    91,   1,  IS_NorY},
{"POST_SIC",           92,   4,  IS_NUMERIC}, 
{"POST_POSTAL_CODE",   96,   9,  IS_ALPHNUM},   
{"POST_MRCH_CNTRY",   105,   3,  IS_NUMERIC}, 
{"POST_AUTH_ID",      108,   6,  IS_NUMERIC},
{"POST_POST_ID",      114,   6,  IS_NUMERIC},
{"POST_MERCH_ID",     120,  16,  IS_ALPHNUM}
};


/***********************************************/
/* macros for accessing fraud field information */
/***********************************************/
#define foff(fld) frd[fld].offset
#define flen(fld) frd[fld].length
#define fname(fld) frd[fld].name

typedef enum {
FRAUD_ACCT_ID=0,
FRAUD_TYPE,
FRAUD_TRAN_DATE,
FRAUD_FIND_METHOD,
FRAUD_TRAN_TIME,
FRAUD_BLOCK_DATE,
FRAUD_BLOCK_TIME,

NUM_FRAUD_FIELDS
} FRAUD_FIELDS;

/******************************/
/* Fraud file controller table */
/******************************/
static FIELD_DATA frd[NUM_FRAUD_FIELDS] = {
{"FRAUD_ACCT_ID",       	 0,	19,	IS_ALPHNUM}, 
{"FRAUD_TYPE",			19,	 1,	IS_ACLNOSorT},		
{"FRAUD_TRAN_DATE",		20,	 8,     IS_DATE},	
{"FRAUD_FIND_METHOD",		28,	 1,	IS_CorS},
{"FRAUD_TRAN_TIME",		29,	 6,	IS_TIME},
{"FRAUD_BLOCK_DATE",            35,      8,     IS_DATE},
{"FRAUD_BLOCK_TIME",            43,      6,     IS_TIME}
};


/***********************************************/
/* macros for accessing non-monetary field information */
/***********************************************/
#define noff(fld) non[fld].offset
#define nlen(fld) non[fld].length
#define nname(fld) non[fld].name

typedef enum {
NONMON_ACCT_ID=0,
NONMON_TRAN_TYPE,
NONMON_TRAN_DATE,
NONMON_POSTAL_CODE,
NONMON_CRED_LINE,

NUM_NONMON_FIELDS
} NONMON_FIELDS;

/******************************/
/* Non-monetary file controller table */
/******************************/
static FIELD_DATA non[NUM_NONMON_FIELDS] = {
{"NONMON_ACCT_ID",      0,	19,	IS_ALPHNUM}, 
{"NONMON_TRAN_TYPE",   19,	 1,	IS_ACFKLMNOPQRorS},		
{"NONMON_TRAN_DATE",   20,	 8,	IS_DATE},
{"NONMON_POSTAL_CODE", 28,	 9,	IS_ALPHNUM},	
{"NONMON_CRED_LINE",   37,	10,	IS_NUMERIC}
};


/***********************************************/
/* macros for accessing payment field information */
/***********************************************/
#define yoff(fld) pay[fld].offset
#define ylen(fld) pay[fld].length
#define yname(fld) pay[fld].name

typedef enum {
PAY_ACCT_ID=0,
PAY_AMOUNT,
PAY_MIN_DUE,
PAY_TOTAL_DUE,
PAY_BALANCE,
PAY_CRED_LINE,
PAY_DATE,
PAY_DUE_DATE,
PAY_STATE_DATE,
PAY_REVERSAL,
PAY_DEPOSIT_METH,

NUM_PAY_FIELDS
} PAY_FIELDS;

/******************************/
/* Non-monetary file controller table */
/******************************/
static FIELD_DATA pay[NUM_PAY_FIELDS] = {
{"PAY_ACCT_ID",        0,   19,   IS_ALPHNUM}, 
{"PAY_AMOUNT",        19,   13,   IS_NUMERIC},
{"PAY_MIN_DUE",       32,   13,   IS_NUMERIC},
{"PAY_TOTAL_DUE",     45,   13,   IS_NUMERIC},
{"PAY_BALANCE",       58,   13,   IS_NUMERIC},
{"PAY_CRED_LINE",     71,   10,   IS_NUMERIC},
{"PAY_DATE",          81,    8,   IS_DATE},
{"PAY_DUE_DATE",      89,    8,   IS_DATE},
{"PAY_STATE_DATE",    97,    8,   IS_DATE},
{"PAY_REVERSAL",     105,    1,   IS_RorN},
{"PAY_DEPOSIT_METH", 106,    1,   IS_ATPRDorO}
};

#endif
