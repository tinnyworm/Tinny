/* -------------------------------------------------------------------------
 *
 *  FILE: conv.h 
 *
 *  DESCRIPTION: Controller table for conversion of separate consortium
 *  structures into single transaction structure 
 *
 * ----------------------------------------------------------------------- */

typedef enum {
CARD_RECORD=0,
POST_RECORD,
AUTH_RECORD,
NONMON_RECORD,
PAY_RECORD,
TRAN_DEPEND
} recType;

typedef struct {
  int     tranFld;
  int     recFld1;
  int     recFld2;
  int     recFld3;
  int     recFld4;
  recType type;
} CONV_DATA;

#define NOT_AVAIL  -99

static CONV_DATA convTable[NUM_TRAN_FIELDS] = {
{ ACCOUNT_NUMBER,        CARD_ACCT_ID,         NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ AUTH_POST_FLAG,        NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ POSTCODE,              CARD_POSTAL_CODE,     NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ CARD_CNTRY_CODE,       CARD_CNTRY,           NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ ACCOUNT_OPEN_DATE,     CARD_OPEN_DATE,       NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ NEW_PLASTIC_DATE,      CARD_ISS_DATE,        NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,          CARD_RECORD },
{ PLASTIC_ISSUE_TYPE,    CARD_ISS_TYPE,        NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ ACCOUNT_EXPIRE_DATE,   CARD_EXPR_DATE,       NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ CARD_EXPIRE_DATE,      AUTH_MSG_EXPIRE_DATE, NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         AUTH_RECORD },
{ AVAIL_CREDIT,          AUTH_AVAIL_CRED,      POST_AVAIL_CRED,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ CREDIT_LINE,           AUTH_CRED_LINE,       POST_CRED_LINE,
    NOT_AVAIL,          PAY_CRED_LINE,     TRAN_DEPEND },
{ GENDER,                CARD_GENDER,          NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ BIRTHDATE,             CARD_BIRTHDATE,       NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ NUM_CARDS,             CARD_NUM_CARDS,       NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ INCOME,                CARD_INCOME,          NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ CARD_TYPE,             CRD_TYPE,             NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ CARD_USE,              CRD_USE,              NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         CARD_RECORD },
{ TRAN_DATE,             AUTH_DATE,            POST_TRAN_DATE,
    NONMON_TRAN_DATE,   PAY_DATE,          TRAN_DEPEND },
{ TRAN_TIME,             AUTH_TIME,            POST_TRAN_TIME,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ TRAN_AMOUNT,           AUTH_AMOUNT,          POST_AMOUNT,
    NOT_AVAIL,          PAY_AMOUNT,        TRAN_DEPEND },
{ TRAN_CURR_CODE,        AUTH_CURR_CODE,       POST_CURR_CODE,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ TRAN_CURR_CONV,        AUTH_CURR_RATE,       POST_CURR_CONV,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ AUTH_RESULT,           AUTH_DECISION,        NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         AUTH_RECORD },
{ TRAN_TYPE,             AUTH_TRAN_TYPE,       POST_TRAN_TYPE,
    NONMON_TRAN_TYPE,   PAY_REVERSAL,      TRAN_DEPEND },
{ MERCH_CAT,             AUTH_SIC,             POST_SIC,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ MERCH_POST_CODE,       AUTH_POSTAL_CODE,     POST_POSTAL_CODE,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ MERCH_CNTRY_CODE,      AUTH_MRCH_CNTRY,      POST_MRCH_CNTRY,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND },
{ AUTH_PIN_VERIFY,       AUTH_PIN_VER,         NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         AUTH_RECORD },
{ AUTH_CVV_VERIFY,       AUTH_CVV,             NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         AUTH_RECORD },
{ AUTH_KEYED_SWIPED,     AUTH_KEY_SWIPE,       NOT_AVAIL,
    NOT_AVAIL,          NOT_AVAIL,         AUTH_RECORD },
{ POST_DATE,             NOT_AVAIL,            POST_POST_DATE,
    NONMON_TRAN_DATE,   PAY_DATE,          TRAN_DEPEND },
{ POST_AUTHED,           NOT_AVAIL,            POST_AUTHORIZED,
    NOT_AVAIL,          NOT_AVAIL,         POST_RECORD },
{ NEW_POSTCODE,          NOT_AVAIL,            NOT_AVAIL,
    NONMON_POSTAL_CODE, NOT_AVAIL,         NONMON_RECORD },
{ NEW_CREDIT_LINE,       NOT_AVAIL,            NOT_AVAIL,
    NONMON_CRED_LINE,   NOT_AVAIL,         NONMON_RECORD },
{ MINIMUM_DUE,           NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_MIN_DUE,       PAY_RECORD },
{ TOTAL_DUE,             NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_TOTAL_DUE,     PAY_RECORD },
{ BALANCE,               NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_BALANCE,       PAY_RECORD },
{ PAYMENT_DUE_DATE,      NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_DUE_DATE,      PAY_RECORD },
{ STATEMENT_DATE,        NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_STATE_DATE,    PAY_RECORD },
{ DEPOSIT_METHOD,        NOT_AVAIL,            NOT_AVAIL,
    NOT_AVAIL,          PAY_DEPOSIT_METH,  PAY_RECORD },
{ MERCH_ID,              AUTH_MERCH_ID,        POST_MERCH_ID,
    NOT_AVAIL,          NOT_AVAIL,         TRAN_DEPEND  },

};
