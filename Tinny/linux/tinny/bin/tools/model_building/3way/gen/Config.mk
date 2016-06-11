##################
## Global Settings
##################

##########################################################################
# All data should be under ${DATA_DIR} directory, one directory for each
# data set.
# The select files are using the same index name as data dir
##########################################################################

## Project name (model name)
PROJECT = uk2004fp
## Basename for new model segment (requires 8 chars) (segment name)
BASENAME = gen_2004
## Project root directory
ROOT = /work/frauda2/FP_UK/segments/16-gen-144vars-3way-new
## New name for risk file
NEW_RISK = ukcrdrsktnew
## Path to new risk file
NEW_RISK_PATH = /work/frauda2/FP_UK/risk
## NN Model configuration file: Space separated list of indices of include
## files containing NNET training configuration options, e.g.
## MODEL_IX = 00 02
## will train two nets using the settings in Model00.mk and Model01.mk
MODEL_IX = 21_0 22_0
## Evaluation descriptions: Space separated list of indices of include
## files containing evaluation configuration options, e.g.
## EVAL_IX = 00
## will run evaluation runs as described in Eval00.mk and Eval01.mk for each
## model as specified by MODEL_IX.
EVAL_IX = 00

###########################################################################
# DATA and SELECT files
#
# The last part of data directory (the part after DATA_DIR)should be the 
# same as the last part of select file.
#
# example:
# 		data dir:    ${DATA_DIR}/rbs_0 
# 		select file: select.mgen.rbs_0
###########################################################################
DATA_DIR = /work/frauda2/FP_UK/01_data
SELECT_DIR = /work/frauda2/FP_UK/segments/16-gen-144vars-3way-new/select
DATA = cap1 \
       fdi_0 \
		 fdi_1 \
		 fdi_2 \
		 fdi_3 \
		 fdi_4 \
		 fdi_5 \
		 fdi_6 \
		 fdi_7 \
		 fdi_8 \
		 fdi_9 \
       rbs_0 \
		 rbs_1 \
		 rbs_2 \
		 rbs_3 \
		 rbs_4 \
		 rbs_5 \
		 rbs_6 \
		 rbs_7 \
		 rbs_8 \
		 rbs_9 

#######################################
## Settings for generating a scale file
#######################################
# 3 way split data? [YES/NO] 
# YES:
#		-t train option for both counting and scaling. It scores 75% training
#		data, and labelled all as train in the tag file. The tag file will be
#		re-labelled later using 3way scripts
# NO:
#		take SCL_TEST_PCT (see below) for testing and the rest for training.
3WAY = YES
# FP? [YES/NO]
FP = YES
# Global data modifiers.Comma separated list:
#      acctlen=value - Overrides the default value for account number
#         length for comparing and creating short score file. It
#         does NOT affect size of account number field in the API.
#      std_kchk - standardize KCHK 0003 and 0004 to 0004
#      defaultcard - use default card data for records missing
#         card file entries
#      buildcard30 - generate/select card records the like
#         drvfalcon30u does
SCL_MOD = std_kchk,buildcard30
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
#      sic=value (or range1-range2) - Transactions matching the SIC codes
#              will be scored, all other transactions profiled.
#      nosic=value (or range1-range2) - Transactions matching the SIC codes
#              will be profiled, and all others will be scored.
#      fraudsonly - only transactions from fraud accounts will be
#              considered for processing
#  NOTE: these two suboptions are not supported yet
#      noauths - no auth files will be processed
#      noposts - no post files will be processed
SCL_FILTER = noposts
# Max number to process. Leave blank for all
SCL_NUMBER =
# Percentage of accounts to use for testing
SCL_TEST_PCT = 25
# How to scale. Comma separated combination of: scl,dat,nozerocalc,binary
#     scl - produce the .scl file, .cfg, .tag
#     dat - produce .dat and .scl, .cfg, .tag
#     nozerocalc - Don't include zero values in means and std
#           deviation calculations
#     binary - detect and process binary input variables differently.
SCL_Z = scl,nozerocalc,binary
# Generate a bigScrFile? [YES/NO]
SCL_GEN_BIGSCR = NO

# Perform sensitivity analysis? [YES/NO]
SENSANA = YES
