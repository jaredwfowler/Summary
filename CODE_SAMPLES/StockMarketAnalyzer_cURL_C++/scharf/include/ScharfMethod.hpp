
#ifndef SCHARF_METHOD_HPP
#define SCHARF_METHOD_HPP

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
****  Date:  December 7, 2015
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: December 7, 2015
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Scharf's plan analyzes the stock history for the last year, is highest and lowest extremes, and
based off such information, recommends the best call and put strike prices. Assuming a stock is
not very volatile, this method should allow the user to gain money 50/52 weeks/year. The volatility
factor is extremely important. If a stock is too volatile, it's probably not very safe to buy
options against.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <thread>
#include <mutex>
#include "ScharfOptionStock.hpp"
#include "SimpleDate.hpp"

//Definition========================================================================================
//==================================================================================================
class ScharfMethod
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   ScharfMethod(const std::vector<std::string>* _tickerList);

   //Destructor=====================================================================================
   //===============================================================================================
   virtual ~ScharfMethod();

   //Get============================================================================================
   //===============================================================================================
   SimpleDate getStartDate(void) const { return this->startDate; }
   SimpleDate getEndDate(void) const { return this->endDate; }
   SimpleDate getLastCloseDate(void) const { return this->lastWeekCloseDate; }
   std::string getFailureString(void) const { return failureString; }

   //Do=============================================================================================
   //===============================================================================================
   void processOptions();
   void writeCSVdata(const char* _dir) const;
   void writeCSVresults(const FILE* _stream) const;
   void writeCSVresultsSimpleFiltered(const FILE* _stream) const;
   void clearFailureString(void) { failureString = ""; }

protected: //#######################################################################################
   //Do=============================================================================================
   //===============================================================================================
   static void downloadParseStoreStockInfo_thread(std::string _str,
     std::vector<ScharfOptionStock>* _ptr, SimpleDate _start, SimpleDate _end,
     SimpleDate _lastClosing, const HttpDownloader* _request);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   static std::mutex scharfMutex;
   static const unsigned int SCHARF_THREADS = 40;
   static unsigned int totalJobs;
   static unsigned int jobsDone;
   static unsigned int printJobActive;
   static std::mutex scharfFailureStrMutex;
   static std::string failureString;

   std::thread* t;
   std::thread* printThread;
   std::vector<std::string> optionSymbols;
   std::vector<ScharfOptionStock> optionsList;
   SimpleDate startDate;
   SimpleDate endDate;
   SimpleDate lastWeekCloseDate;
   HttpDownloader* authenticatedHttpRequest;

   //Do=============================================================================================
   //===============================================================================================
   bool outputFilter(int _i) const;
   bool outputFilterSimple(int _i) const;
   static void printPercentDone_thread();

};
//==================================================================================================
//End Definition====================================================================================

#endif
