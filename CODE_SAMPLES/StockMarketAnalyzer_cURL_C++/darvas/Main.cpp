/***************************************************************************************************
***********************         |\/| o  _ ._ _  |  _   _   _.  _            ************************
***********************         |  | | (_ | (_) | (/_ (_| (_| (_ \/         ************************
***********************                                _|        /          ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  March 16, 2016
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: March 16, 2016
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "DarvasMethod.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sys/stat.h>  // mkdir

#if defined(_MSC_VER) || defined(__WINDOWS__)
#include <Windows.h>
#endif

int main(int argc, char* argv[]) 
{
   // Get the start time
   time_t startTime = time(NULL);
   struct tm* timeInfo = localtime(&startTime);

   char baseDir[512] = {0};
   char inputFile[512] = {0};
   char outFile[600] = {0};

   if (3 == argc) {
      strcpy(inputFile, argv[1]);
      strcpy(baseDir, argv[2]);
   } else {
      fprintf(stdout, "Try Again. Format: ./darvas <inputFile> <outputDir>\n");
      exit(1);
   }

   #if defined(_MSC_VER) || defined(__WINDOWS__)

   wchar_t wideCharOutput[512] = { 0 };
   wchar_t wideCharBaseDir[512] = { 0 };
   size_t convertedChars = 0;
   mbstowcs_s(&convertedChars, wideCharBaseDir, 512, baseDir, _TRUNCATE);

   #endif

   std::sprintf(baseDir, "%s/darvasOutput_%04d-%02d-%02d_%02d-%02d-%02d", baseDir,
                                                                          1900 + timeInfo->tm_year,
                                                                          (timeInfo->tm_mon + 1),
                                                                          timeInfo->tm_mday,
                                                                          timeInfo->tm_hour,
                                                                          timeInfo->tm_min,
                                                                          timeInfo->tm_sec);

   #if defined(_MSC_VER) || defined(__WINDOWS__)

   mbstowcs_s(&convertedChars, wideCharBaseDir, 512, baseDir, _TRUNCATE);
   (void)CreateDirectory(wideCharBaseDir, NULL);

   #else

   (void)mkdir(baseDir, 0755);

   #endif

   std::sprintf(outFile, "%s/errors.txt", baseDir);
   std::freopen(outFile, "w", stderr);

   std::vector<std::string> tickerList;

   // Read the input file. Get vector of tickers ready.
   FILE* fdOptions = fopen(inputFile, "r");
   if (fdOptions == NULL) 
   {
      fprintf(stderr, "Failed to open %s \n", inputFile);
      exit(1);
   }
   char buffer[64] = { 0 };
   while (!feof(fdOptions)) 
   {
      if (fgets(buffer, 64, fdOptions) != NULL) 
      {
         std::string str(buffer);
         str = str.substr(0, str.length() - 1);
         tickerList.push_back(str);
      }
   }
   fclose(fdOptions);

   DarvasMethod mainVar(&tickerList);
   mainVar.processDarvasStocks();

   // Output failures to stderr
   fprintf(stderr, "%s\n", mainVar.getFailureString().c_str());

   FILE* fd = NULL;

   // Output darvas analysis
   std::sprintf(outFile, "%s/resultsDarvas.csv", baseDir);
   if ((fd = std::fopen(outFile, "w")) == NULL)
   {
      fprintf(stdout, "Failed to open output file");
   }
   mainVar.writeCSVresults(fd);
   std::fclose(fd);

   // Output stock history data
   std::sprintf(outFile, "%s/data", baseDir);

   #if defined(_MSC_VER) || defined(__WINDOWS__)

   mbstowcs_s(&convertedChars, wideCharOutput, 512, outFile, _TRUNCATE);
   (void)CreateDirectory(wideCharOutput, NULL);

   #else

   (void)mkdir(outFile, 0755);

   #endif

   mainVar.writeCSVdata(outFile);

   std::fclose(stderr);

   // Get the end time
   time_t endTime = time(NULL);
   fprintf(stdout, "Run Time: %lu seconds\n", (long unsigned int)(endTime - startTime));

   return 0;
}