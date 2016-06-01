
#define HDR_LGTH          64
#define ITM_ACLG          8
#define MARKER_POINTER    0xC8689449
#define CONST_QUOTE       125
#define CONST_AMPERS      80
#define CONST_SPACE       64
#define INQIR_FUNTION     "INQIRE_ADDRESS"
#define RESET_FUNTION     "RESETE_ADDRESS"
#define SYNCP_FUNTION     "SYNCPT_TABLEEE"
#define LGADDR_FUNTION    "LGD"
#define FRADDR_FUNTION    "FRD"
#define CLOSFL_FUNTION    "CLS"
#define PAGE_4K           0x00001111
#define ENTRIES_256       0x00000111
#define INDEX_LIMIT       1024*128

#define ASMSOURC          {"DD:ASMSOURC"}
#define LINKCARD          {"DD:LINKCARD"}
#define ADDR_FILE_NAME    "DD:ADDRFILE"
#define FREE_FILE_NAME    "DD:FREEFILE"
#define CS_LN   "         CSECT                                                                  "
#define AM_LN   "         AMODE 31                                                               "
#define RM_LN   "         RMODE ANY                                                              "
#define EN_CS   "         END                                                                    "
#define TI_LN   "         TITLE 'XXXXXXXX PROFIT MAX INITIAL TIME MEMORY MODULE        '         "
#define DT_LN   "         DC C'                                                        '         "
#define LI_LN   " NAME XXXXXXXX(R)                                                               "




typedef  struct
{
#ifdef LMT128K
char ITM_PAGE_021[512*256];
char ITM_PAGE_025[4096* 16];
#endif
#ifdef LMT1M
char ITM_PAGE_021[4096*256];
char ITM_PAGE_025[4096* 16];
#endif
#ifdef LMT2M
char ITM_PAGE_021[4096*256];
char ITM_PAGE_022[4096*256];
char ITM_PAGE_025[4096* 16];
#endif
#ifdef LMT3M
char ITM_PAGE_021[4096*256];
char ITM_PAGE_022[4096*256];
char ITM_PAGE_023[4096*256];
char ITM_PAGE_025[4096* 16];
#endif
#ifdef LMT4M
char ITM_PAGE_021[4096*256];
char ITM_PAGE_022[4096*256];
char ITM_PAGE_023[4096*256];
char ITM_PAGE_024[4096*256];
char ITM_PAGE_025[4096* 16];
#endif
}tLNGARR;

typedef  struct
{
long int   offs_cell[INDEX_LIMIT];
}tITM_INDEX;

typedef struct
{
char * tablebegin;
char * nextavail;
long int eyecatcher;
long int  tablesize;
long int  curr_cell;
char * pNet;    /* do not forget that we have to translate it also */
char CLNTNUMB[8];
char reser01[24];
tITM_INDEX dITM_INDEX;
double dummy;
tLNGARR dLNGARR;
}tITM_PAGE;


typedef struct
{
char TEST_NAME[4];
long int local_addr;
long int totl;
void *   forward;
}tTestEntry;


long int ITM_ARRAY_2_ASM_SOURCE(char * clntname);
long int ITM_ABS_2_OFFS();
void *   ITM_OFFS_2_ABS(char *tabbegin);
void *   ITM_FREE(void *ptr);
void *   ITM_MALLOC(size_t);
void *   ITM_MAKE_LOADNAME(char * inname,    char * loadname);
void *   ITM_LOGADDR(void * pTrsl); /* log it for addresses to be translated*/

long     ITM_SET_GLBL(void *mem);
long     ITM_SET_PNET(void *pNet);
long     ITM_GET_PNET(void **pNet);
long     ITM_COPY_AREA(void *ptr);
void *   ITM_GETSTRT();
char *   ITM_GETSTRTPTR();
long     ITM_GETTOTAL();
long     ITM_FREEALL();

void *   ITM_UTILIT(char *  fnc);
void *   ITM_ATTCH_PAGE(char * Table_Name,
                char hold_indc, long int *pLeng);
static   tITM_PAGE *  pGLBL_ITM_PAGE;
static tITM_PAGE  dITM_PAGE;

