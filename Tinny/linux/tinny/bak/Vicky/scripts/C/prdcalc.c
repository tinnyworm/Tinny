#include "prdcalc.h"

/* Include for the specific product that is being used */
#include "falcalc.h"


long ValidateProductFunction(tModelVar *modelVar, short funcCode, char *fileName, char *errMsg)
{
   return(ValidateFalconFunction(modelVar, funcCode, fileName, errMsg));
}
