
#ifndef DARVAS_METHOD_HPP
#define DARVAS_METHOD_HPP

/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
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
Rules of Stock Consideration:
1. Stock shall not be over-the-counter, aka, shall have the ability to be sold at any given time.
2. Sudden rises in Volume with an increase in price shall be noted.
3. Shall take into consideration a stock's earning report, as to satisfy the fundamental aspect.
4. Use the Dow-Jones Industrial Average to determine the general strength of the market
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <thread>
#include <mutex>
#include "DarvasStock.hpp"
#include "SimpleDate.hpp"

//Definition========================================================================================
//==================================================================================================
class DarvasMethod
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================

   //Constructor(s)=================================================================================
   //===============================================================================================
   DarvasMethod(const std::vector<std::string>* _tickerList);

   //Get============================================================================================
   //===============================================================================================
   SimpleDate getStartDate(void) const { return this->startDate; }
   SimpleDate getEndDate(void) const { return this->endDate; }
   std::string getFailureString(void) const { return failureString; }

   //Do=============================================================================================
   //===============================================================================================
   void processDarvasStocks();
   void writeCSVdata(const char* _dir) const;
   void writeCSVresults(const FILE* _stream) const;
   void clearFailureString(void) { failureString = ""; }

protected: //#######################################################################################
   //Do=============================================================================================
   //===============================================================================================
   static void downloadParseStoreStockInfo_thread(std::string _str, 
      std::vector<DarvasStock>* _ptr, SimpleDate _start, SimpleDate _end);


private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   static std::mutex darvasMutex;
   static const unsigned int DARVAS_THREADS = 40;
   static unsigned int totalJobs;
   static unsigned int jobsDone;
   static unsigned int printJobActive;
   static std::mutex darvasFailureStrMutex;
   static std::string failureString;

   std::thread* t;
   std::thread* printThread;
   SimpleDate startDate;
   SimpleDate endDate;
   std::vector<std::string> stockSymbols;
   std::vector<DarvasStock> darvasStockList;

   //Do=============================================================================================
   //===============================================================================================
   bool outputFilter(int _i) const;
   static void printPercentDone_thread();

};
//==================================================================================================
//End Definition====================================================================================

#endif
