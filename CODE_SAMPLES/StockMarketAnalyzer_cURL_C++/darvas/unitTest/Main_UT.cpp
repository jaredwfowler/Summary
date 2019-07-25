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
****  Date:  May 3, 2016
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: May 3, 2016
** Revision Personnel: JFowler, 
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "DarvasMethod_UT.hpp"
#include "DarvasStock_UT.hpp"

#include <stdio.h>
#include <string.h>

int main(){
   std::freopen("unitTest/errors.txt", "w", stderr);

   char testName[64] = {0};

   strcpy(testName, "DarvasStock_Test_Copy");
   if(DarvasStock_Test_Copy()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }
   
   strcpy(testName, "DarvasStock_Test_Analyzer");
   if(DarvasStock_Test_Analyzer()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }

   strcpy(testName, "DarvasStock_Test_SybmolCompare");
   if(DarvasStock_Test_SybmolCompare()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }

   strcpy(testName, "DarvasMethod_Test_Dates");
   if(DarvasMethod_Test_Dates()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }

   strcpy(testName, "DarvasMethod_Test_SingleThreadJob");
   if(DarvasMethod_Test_SingleThreadJob()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }

   strcpy(testName, "DarvasMethod_Test_MultiThreadJob");
   if(DarvasMethod_Test_MultiThreadJob()) {
      fprintf(stdout, "%-64s: PASS\n", testName);
   } else {
      fprintf(stdout, "%-64s: FAIL\n", testName);
   }

   return 0;
}
