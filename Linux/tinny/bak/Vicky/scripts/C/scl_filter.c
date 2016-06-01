/*******************************************************************************

Filter the records in Falcon .scl and .tag files.

*******************************************************************************/

/* Header files */

#include <stdlib.h>
#include <stdio.h>

/******************************************************************************/

int main(int argc, char* argv[]){

   char fileName[100], tagLine[100], tagTag[10], filterLine[100];
   FILE *fpSclIn, *fpSclOut, *fpTagIn, *fpTagOut, *fpFilterIn;
   int recLen;
   long testOutMark, trainOutMark;
   long recInFraud=0, recInNonFraud=0;
   long recOutFraud=0, recOutNonFraud=0;
   long tagInCnt;
   long filterValue;
   long numTest=0, numTrain=0;
   long recError=0;
   long filterError;
   float *buff;

   if (argc != 4){
      fprintf(stderr, "\nUsage: scl_filter inBase outBase filterFile \n\n");
      fprintf(stderr, "inBase is the base name of the original scl and tag files.\n");
      fprintf(stderr, "outBase is the base name of the sampled scl and tag files.\n");
      fprintf(stderr, "filterFile consists of 0's and 1's \n");
      fprintf(stderr, "      1 = keep corresponding record in scale file\n");
      fprintf(stderr, "      0 = delete corresponding record in scale file\n\n");
      exit(1);
   }

   sprintf(fileName, "%s.scl", argv[1]);
   if ((fpSclIn = fopen64(fileName, "r")) == NULL){
      fprintf(stderr, "Cannot open %s\n", fileName);
      exit(1);
   }

   sprintf(fileName, "%s.scl", argv[2]);
   if ((fpSclOut = fopen64(fileName, "w")) == NULL){
      fprintf(stderr, "Cannot open %s\n", fileName);
      exit(1);
   }

   sprintf(fileName, "%s.tag", argv[1]);
   if ((fpTagIn = fopen64(fileName, "r")) == NULL){
      fprintf(stderr, "Cannot open %s\n", fileName);
      exit(1);
   }

   sprintf(fileName, "%s.tag", argv[2]);
   if ((fpTagOut = fopen64(fileName, "w")) == NULL){
      fprintf(stderr, "Cannot open %s\n", fileName);
      exit(1);
   }

   sprintf(fileName, "%s", argv[3]);
   if ((fpFilterIn = fopen64(fileName, "r")) == NULL){
      fprintf(stderr, "Cannot open %s\n", fileName);
      exit(1);
   }

   fread(&recLen, sizeof(int), 1, fpSclIn);
   fwrite(&recLen, sizeof(int), 1, fpSclOut);

   buff=(float*)malloc(recLen*sizeof(float));
   if (buff == NULL){
      fprintf(stderr, "No more memory for buff!\n");
      exit(1);
   }

   fgets(tagLine, 100, fpTagIn);
   fgets(tagLine, 100, fpTagIn);
   
   fprintf(fpTagOut, "test:        \n");
   testOutMark = ftell(fpTagOut) - 7L;

   fprintf(fpTagOut, "train:        \n");
   trainOutMark = ftell(fpTagOut) - 7L;

   while (fread(buff,sizeof(float),recLen,fpSclIn)==recLen){
      fgets(tagLine, 100, fpTagIn);
      sscanf(tagLine, "%d %s", &tagInCnt, tagTag);
      fgets(filterLine, 100, fpFilterIn);
      sscanf(filterLine, "%d", &filterValue);
      if (buff[0] > 0.0){
         recInFraud++;
         if (filterValue == 1){
            fwrite(buff,sizeof(float),recLen,fpSclOut);
            recOutFraud++;
            fprintf(fpTagOut, " %d %s\n", numTest+numTrain, tagTag);
            if (!strncmp(tagTag, "test", 4)) numTest++;
            else numTrain++;
         }
         else if(filterValue != 0){
           filterError++;
         }
      }
      else if (buff[0] < 0.0){
         recInNonFraud++;
         if (filterValue == 1){
            fwrite(buff,sizeof(float),recLen,fpSclOut);
            recOutNonFraud++;
            fprintf(fpTagOut, " %d %s\n", numTest+numTrain, tagTag);
            if (!strncmp(tagTag, "test", 4)) numTest++;
            else numTrain++;
         }
         else if(filterValue != 0){
           filterError++;
         }
      }
      else recError++;
   }

   fseek(fpTagOut, testOutMark, 0);
   fprintf(fpTagOut, "%d", numTest);

   fseek(fpTagOut, trainOutMark, 0);
   fprintf(fpTagOut, "%d", numTrain);

   if (recError)
      fprintf(stderr, "RECORDS WITH UNEXPECTED TARGETS: %10d\n", recError);
   if (filterError)
      fprintf(stderr, "RECORDS WITH UNEXPECTED FILTER VALUE: %10d\n", filterError);
   
   fprintf(stderr, "Records in:  %10d\n", recInFraud+recInNonFraud);
   fprintf(stderr, "Records out: %10d\n", recOutFraud+recOutNonFraud);
   fprintf(stderr, "Fraud records in:     %10d\n", recInFraud);
   fprintf(stderr, "Fraud records out:    %10d\n", recOutFraud);
   fprintf(stderr, "Nonfraud records in:  %10d\n", recInNonFraud);
   fprintf(stderr, "Nonfraud records out: %10d\n", recOutNonFraud);

   fclose(fpSclIn);
   fclose(fpSclOut);
   fclose(fpFilterIn);
   free(buff);

   return 0;
}

