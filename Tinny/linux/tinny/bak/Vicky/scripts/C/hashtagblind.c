#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// $COMPILE$ gcc -O -o hashtagblind hashtagblind.c

#define ACCT_LEN 19
#define BUFF_SIZE 1024
#define BLIND_THRESHOLD 25
#define TEST_DEFAULT 35

int hashValue(char*);

//*****************************************************************************

int main(int argc, char **argv){
  char              buff[BUFF_SIZE];
  unsigned long int i=0, iTest=0, iTrain=0, iBlind=0;
  int               testThreshold;
  int               hval;
  
  // Check Command-line arguments
  if(argc > 1){
    if(!strcmp(argv[1],"--help")){
      fprintf(stderr,"Use: %s [test]\n\n"
	      "\tWhere 0 <= test <= 75. (Default 10)\n"
	      "\tWill read from STDIN and generate to STDOUT\n"
	      "\t[line_number]\ttest|train|blind\n"
	      "\tso that 25%% of accounts get labeled 'blind',\n"
	      "\t[test] get labeled 'test', the rest 'train'.\n",
	      argv[0]);
      exit(1);
    } else {
      testThreshold = BLIND_THRESHOLD + atoi(argv[1]);
    }
  } else {
    testThreshold = TEST_DEFAULT;
  }
  
  if(testThreshold < BLIND_THRESHOLD || testThreshold > 99){
    fprintf(stderr,"ERROR: [test] out of [0:75] range.\n");
    exit(1);
  }
  
  while (fgets(buff,BUFF_SIZE,stdin)){
    hval = hashValue(buff);
    if (hval < BLIND_THRESHOLD) {
      fprintf(stdout, " %lu blind\n", i);
      iBlind++;
    } else if (hval < testThreshold) {
      fprintf(stdout, " %lu test\n", i);
      iTest++;
    } else {
      fprintf(stdout, " %lu train\n", i);
      iTrain++;
    }
    i++;
  }
  fprintf(stderr, "test:\t%.1f\ntrain:\t%.1f\nblind:\t%.1f\n\n",
	  100.*iTest/i, 100.*iTrain/i, 100.*iBlind/i);
  fprintf(stderr, "test:\t%lu\ntrain:\t%lu\nblind:\t%lu\n",
	  iTest, iTrain, iBlind);
  return 0;
}

//****************************************************************************

int hashValue(char* acctNum){
  static int coprime[] = {11,13,17,19,23,31};
  int i, j, k, sum=0;
  
  for (i=ACCT_LEN-1; !isalnum((int)acctNum[i]) && acctNum[i] != '-'; i--);
  
  if (i<11){
    fprintf(stderr,
	    "Account number found of length only %d in hashValue!\n", i+1);
    return(-1);
  }
  
  if (i==11){
    for (k=0, j=i-11; k<6; i--, j++, k++)
      sum += (((acctNum[j] - '0') * 10) + (acctNum[i] - '0')) * coprime[k];
  }else{
    for (k=0, j=i-12; k<6; i--, j++, k++)
      sum += (((acctNum[j] - '0') * 10) + (acctNum[i] - '0')) * coprime[k];
  }
  
  return(sum%100);
}

