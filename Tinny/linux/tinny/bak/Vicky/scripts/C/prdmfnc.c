#include "prdmfnc.h"

/* Include for the specific product that is being used */
#include "falmfnc.h"


long LoadProductModelFunctions(tFuncInit *funcInit, tFunc **funcList)
{
   return(LoadFalconModelFunctions(funcInit, funcList));
}
