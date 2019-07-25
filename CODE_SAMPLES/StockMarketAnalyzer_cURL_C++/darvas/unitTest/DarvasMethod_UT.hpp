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
****  Date:  May 19, 2016
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
DarvasStock UnitTests
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "DarvasMethod.hpp"
#include <stdio.h>

class DarvasMethodTest : public DarvasMethod
{
public: 
   DarvasMethodTest(const std::vector<std::string>* _tickerList) : DarvasMethod(_tickerList) {}
   
   void DownloadParseStoreStockInfo_thread(std::string _str, 
      std::vector<DarvasStock>* _ptr, SimpleDate _start, SimpleDate _end) {
      downloadParseStoreStockInfo_thread(_str, _ptr, _start, _end);
   }
};

bool DarvasMethod_Test_Dates() {
   bool success = true;

   //Create a new input vector
   std::vector<std::string> inVec;
   inVec.push_back("AAPL");

   DarvasMethodTest testVar = DarvasMethodTest(&inVec);

   SimpleDate endDate = SimpleDate::today();
   SimpleDate startDate = SimpleDate(endDate.getYear() - 1, endDate.getMonth(), endDate.getDayM());
   startDate = startDate.addDays(2 - startDate.getDayW());

   if((testVar.getStartDate() != startDate) ||
      (testVar.getEndDate() != endDate)) {
      success = false;
   }

   return success;
}

bool DarvasMethod_Test_SingleThreadJob() {
   bool success = true;

   //Create a new input vector -- dummy input
   std::vector<std::string> inVec;
   inVec.push_back("AAPL");

   DarvasMethodTest testVar = DarvasMethodTest(&inVec);
   std::vector<DarvasStock>* vecPtr = new std::vector<DarvasStock>();
   
   SimpleDate startDate = testVar.getStartDate();
   SimpleDate endDate = testVar.getEndDate();

   //Attempt to run the function and check if vector is incremented
   testVar.DownloadParseStoreStockInfo_thread("AAPL", vecPtr, startDate, endDate);

   if(vecPtr->size() != 1) {
      success = false;
   }

   testVar.DownloadParseStoreStockInfo_thread("DIS", vecPtr, startDate, endDate);

   if(vecPtr->size() != 2) {
      success = false;
   }

   delete vecPtr;
   vecPtr = NULL;

   return success;
}

bool DarvasMethod_Test_MultiThreadJob() {
   bool success = true;

   //Create a new input vector
   std::vector<std::string> inVec;
   inVec.push_back("AAPL");
   inVec.push_back("NOTAREALSTOCK");
   inVec.push_back("GILD");
   inVec.push_back("DIS");

   DarvasMethodTest testVar = DarvasMethodTest(&inVec);
   testVar.processDarvasStocks();

   //Verify that we got an error string
   std::string errorStr = testVar.getFailureString();
   if("NOTAREALSTOCK,1\n" != errorStr) {
      success = false;
   }

   return success;
}
