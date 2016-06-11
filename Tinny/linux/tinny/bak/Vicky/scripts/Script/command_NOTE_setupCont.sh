#! /usr/local/bin/bash

#Steps:
#1. create a sub-directory
mkdir switch_model; 

#2.  Copy the final model
model_copy40.sh ../06_merge/FFMUSCredit140.cnf switch_model; 
cd switch_model;
mkdir old_model;
model_copy40.sh  $USCRT/model_temp/USCredit_v13.0/FFMUSCredit130.cnf old_model;

#3. Compare the difference of common files b/w old and new. If differ, keep one with more flexiablility. 
cp -r old_model/vars/ vars_13;
cp -r old_model/tabs/ tabs_13;
cp -r old_model/nets/ nets_13;
cp -r old_model/func/ func_13;
cp old_model/fal4tmplt fal4tmplt_13; # However, we only can keep one tmplate

cp old_model/gencr130 .; 
cp old_model/hidcr130 .; 
cp old_model/rapcr130 .; 
cp old_model/yamcr130 .;

#4.Need to change model files accordingly for the old model.  

#5.Need to compose the switch.rul file.
# 5.1 change switch.cnf like
#####################################################
#MODEL_SET = gencr140,hidcr140,ugscr140,tnecr140
#MODEL_SET = gencr130,hidcr130,rapcr130,yamcr130
#TEMPLATE_FILE =  fal4tmplt
#RULE_BASE_FILE = switch.rul
#####################################################
# requires a switch model in a subdirectory switch_model 
# with cnf file switch.cnf.
# switch.rul should have the date tag ___SWITCH_DATE___
# requires a Makefile script in the switch_model subdirectory 
# that creates big score files in ./out/bScr.{name}
# requires a bin subdirectory containing scripts 
# getFraudCases and getNonFraudCases
# input the initial switch date
# usage:   /home/yxc/models/getMake-ContinuityTest.pl -d 20050901 -s 0.05  
#####################################################
#
#NOTE: Before run getMake-ContinuityTest.pl. A few things need to be checked. 
# 1. Check the name setting for Makefile whether it is consistent with this
# code. eg. bigScore files needs to be in eval/ and with name bScr.*gz 
# 2. It will run 10 weeks + 1 week (old/baseline).
# 3. This code can be only run at SunOS not Linux because of getCases.
# 4. This code need to be run in the same directory the switch_model in. 
