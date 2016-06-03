#TITLE: Build predictive models to predict customer attritions, based on IRM's definitions
#AUTHOR: Tyler Wayne, Zach Mayer
#DATE: 2012-09-27
#DATE Modified: 2012-12-05

#TODO:
#specifiy training AND test period

##########################################
# Setup
##########################################

#Safety (preventy accidently running the whole script)
stop('You probably dont want to run this from the top')

#Setup
setwd('C:/AP_SVN/irm')
rm(list = ls(all = TRUE)) 
gc(reset=TRUE)
set.seed(63163) #From random.org

#Load Libraries
library(data.table)
library(caret)
library(doParallel)
library(Metrics)
source('src/R/Useful Functions.R')

#Define training and test set:
train_months <- c(112:135) #Train through December 2011
test_months <- c(135) #2011-09

#Choose target
#MUST SET ALL THREE PARAMETERS CORRECTLY!
TARGET <- 'attrit_pay_three_month'
MAXMONTH <- 'term_pay_month'  #SHOULD MATCH TARGET
add_not_in_model <- c('manager_init_term', 'manager_prob_init_term')  #If target is term_pay, 
                                                                      #must put term_init based vars here
#Set parameters
cvFolds <- 8     #Number of cross-validation folds
PARALLEL <- TRUE  #Set to FALSE for a 100% reproducible model, or if you keep running out of RAM
CORES <- detectCores()
SUB_SAMPLE <- TRUE
MONTHS_2011 <- 124:135
WRITE_DATA_SUMMARY <- FALSE

##########################################
# Load data and subset
##########################################

load("data/IRM Data-RM.RData")
not_in_model <- c(not_in_model, add_not_in_model)
unique(IRM[,c('month_count', 'month_year')])

#Remove entries dated after a customer has attrited
#BE SURE to change this if we use attrit_pay_three_month for attritions
IRM <- IRM[IRM$month_count < IRM[,MAXMONTH],]

#Split train/test
train <- subset(IRM, month_count %in% train_months)
test <- subset(IRM, month_count %in% test_months)
table(train$month_count, train[,TARGET])
table(test$month_count, test[,TARGET])

#Calculate which customers attrited and which customers did not
custs <- data.table(train, key='imsight_num')
custs <- custs[,list(atrit=max(get(TARGET)=='Yes')), by='imsight_num']
custs <- as.data.frame(custs)

#Resample the data so we have equal numbers of attrited and non-attrited customers
mySample <- unique(custs$imsight_num)
if (SUB_SAMPLE){
  custs_attr <- subset(custs, atrit==1)$imsight_num
  custs_no_attr <- subset(custs, atrit==0)$imsight_num
  custs_no_attr <- sample(custs_no_attr, length(custs_attr))
  mySample <- c(custs_attr, custs_no_attr)
  custs <- subset(custs, imsight_num %in% mySample)
  train <- subset(train, imsight_num %in% mySample)
}

##########################################
# Setup cross-validation
##########################################

#Make CV fold based on Customer Number
folds <- createFolds(factor(custs$atrit), k=cvFolds, list=TRUE, returnTrain=TRUE)

#Use the indexes in each fold to determine the customers in that fold
folds <- lapply(folds, function(x) custs$imsight_num[x])

#Determine the observations from the training set within each fold
folds <- lapply(folds, function(x) which(train$imsight_num %in% x))

#Built train control
myControl <- trainControl(method='cv', number=cvFolds, summaryFunction=mySummary, 
                          classProbs=TRUE, index=folds, savePredictions=TRUE, 
                          predictionBounds=c(0,1))

#Split X/Y
xvars <- setdiff(names(train), not_in_model)
X <- train[,xvars]
Y <- train[,TARGET]

##########################################
# Run models
##########################################

#Start parallel cluster
if (PARALLEL){
  cl <- makeCluster(CORES)
  registerDoParallel(cl)
}

#Fit GBM
set.seed(95514)
model_gbm    <- train(X, Y, method='gbm', metric='ROC', trControl=myControl,
                      distribution='adaboost',
                      tuneGrid=expand.grid(.n.trees=1:250, .interaction.depth=21, .shrinkage=.15))
model_gbm$time
plot(model_gbm, metric='ROC')
confusionMatrix(model_gbm)

#Select Variables
gbm_vars <- summary(model_gbm$finalModel, plotit=FALSE)
gbm_vars <- as.character(gbm_vars[gbm_vars$rel.inf >= 0.5, 'var'])

#Fit glmnet
set.seed(96208)
model_glmnet    <- train(X[,gbm_vars], Y, method='glmnet', metric='ROC', trControl=myControl,
                      tuneGrid=expand.grid(.alpha=0:1, .lambda=seq(0.1, 50, length=10)))
model_glmnet$time
plot(model_glmnet, metric='Pos.Pred.Value')
confusionMatrix(model_glmnet)

#Stop parallel cluster
if (PARALLEL){
  stopCluster(cl)
}

#Save Models
save(model_gbm, TARGET, test_months, xvars, mySample, file='output/model_gbm.RData')
save(model_glmnet, TARGET, test_months, gbm_vars, mySample, file='output/model_glmnet.RData')

##########################################
# Output GBM
##########################################

#Output the standard model output
outname <- paste('output/GBM Output-RM-pay-', as.Date(Sys.time()), '.xlsx', sep='')
PvA <- modelOutPut(model_gbm, train, test, xvars, yVar=TARGET, outname=outname, adjust=FALSE, 
                   extra_vars=targets)

#Print condusion matricies
confusionMatrix(PvA[PvA$trainset==0,'pred'], 
                PvA[PvA$trainset==0, TARGET], "Yes")

confusionMatrix(PvA[PvA$trainset==0,'pred_adj'], 
                PvA[PvA$trainset==0, TARGET], "Yes")
confusionMatrix(PvA[PvA$trainset==0,'pred_adj'], 
                PvA[PvA$trainset==0, 'attrit_pay_six_month'], "Yes")
#AUC
auc(PvA[PvA$trainset==0, TARGET]=='Yes', PvA[PvA$trainset==0,'prob'])


PvA_gbm <- PvA

#Marginal plots for important variables

##########################################
# Output glmnet
##########################################

#Output the standard model output
outname <- paste('output/glmnet Output-RM-pay-', as.Date(Sys.time()), '.xlsx', sep='')
PvA <- modelOutPut(model_glmnet, train, test, gbm_vars, yVar=TARGET, outname=outname, adjust=FALSE
                   , extra_vars=c('attrit_pay_one_month', 'attrit_pay_six_month'))

#Print condusion matricies
confusionMatrix(PvA[PvA$trainset==0,'pred'], 
                PvA[PvA$trainset==0, TARGET], "Yes")

confusionMatrix(PvA[PvA$trainset==0,'pred_adj'], 
                PvA[PvA$trainset==0, TARGET], "Yes")

PvA_glmnet <- PvA
