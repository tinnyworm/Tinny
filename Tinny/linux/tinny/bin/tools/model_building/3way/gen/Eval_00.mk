##########################
## Settings for evaluation
##########################

# Sampling rate
RATE = 0.03
 
# Global data modifiers.Comma separated list:
#      acctlen=value - Overrides the default value for account number
#         length for comparing and creating short score file. It
#         does NOT affect size of account number field in the API.
#      std_kchk - standardize KCHK 0003 and 0004 to 0004
#      defaultcard - use default card data for records missing
#         card file entries
#      buildcard30 - generate/select card records the like
#         drvfalcon30u does
#   Use $(SCL_MOD) to get the same as used for training.
EVL_MOD = $(SCL_MOD)
# Transaction filtering: Comma separated list:
#  NOTE: cash, kchk, and mrch are mutually exclusive
#      cash - Only cash transactions and not SIC 0003 are trained
#           or scored.  All others are profiled.
#      kchk - Only K tran type, or SIC 0003 or 0004 are trained
#           or scored.  All others are profiled.
#      mrch - Only M tran type and not SIC 0004 are trained
#           or scored.  All others are profiled.
#
#      nocash - Cash trans that are not SIC 0003 are not processed.
#      nokchk - Kchk tran type, SIC 003, or SIC 0004 are not processed.
#      nomrch - M tran type and not SIC 0004 are not processed.
#
#  NOTE: the next two are mutually exclusive
#      keyed - Keyed trans are trained or scored.
#              All others are profiled.
#      nokeyed - Non keyed trans are trained or scored.
#              Keyed trans are profiled.
#
#      amountGE=value - Transactions with amounts greater than or
#              equal to the value will be scored.
#              All others are profiled.
#      amountLT=value - Transactions with amounts less than the value
#              will be scored. All others are profiled.
#      fraudsonly - only transactions from fraud accounts will be
#              considered for processing
#  NOTE: these two suboptions are not supported yet
#      noauths - no auth files will be processed
#      noposts - no post files will be processed
#  Use $(SCL_FILTER) to get the same as used for training.
EVL_FILTER = $(SCL_FILTER)
# Max number to process. Leave blank for all
EVL_NUMBER =
# Percentage of accounts to use for testing
EVL_TEST_PCT = $(SCL_TEST_PCT)
# Score all transactions? [YES/NO]
EVL_SCOREALL = NO
# Generate a bigScrFile? [YES/NO]
EVL_GEN_BIGSCR = YES
# Generate an API file? [YES/NO]
EVL_GEN_API = NO
# Generate a short score file? [YES/NO]
EVL_GEN_SCR = NO
# Choose a name for this evaluation set
EVL_NAME = 00
