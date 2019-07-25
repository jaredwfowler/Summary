
#ifndef SCHARF_OPTION_STOCK_HPP
#define SCHARF_OPTION_STOCK_HPP

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
Extends OptionStock. Adds new variables specific to the Scharf Method, and calculates these values
based on its OptionStock values.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "OptionStock_MarketWatch.hpp"
#include "SimpleDate.hpp"
#include <string>

//Definition========================================================================================
//==================================================================================================
class ScharfOptionStock : public OptionStock_MarketWatch
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   enum ERROR_CODES 
   {
      ERR_NONE    = 0x0000,
      ERR_HISTORY = 0x0001,
      ERR_OPTIONS = 0x0002,
      ERR_SAMPLES = 0x0004,
      ERR_INDEX   = 0x0008
   };

   struct StockValueAndDate 
   {
      float value = 0;
      SimpleDate date;
   };

   struct RisesFallsAvg 
   {
      float first;
      float second;
      float third;
      float avgSecondAndThird;
   };

   void operator= (const ScharfOptionStock& _sos);

   //Constructor(s)=================================================================================
   //===============================================================================================
   ScharfOptionStock(std::string _ticker, std::string _name);

   ScharfOptionStock(const ScharfOptionStock& _sos);

   //Destructor=====================================================================================
   //===============================================================================================
   virtual ~ScharfOptionStock(void);

   //Gets===========================================================================================
   //===============================================================================================
   inline StockValueAndDate getLastWeeksClosing(void) const { return this->lastWeeksClosing; }
   inline float getMinCallStrikePrice(void) const { return this->minCallOptionStrikePrice; }
   inline float getMaxPutStrikePrice(void) const { return this->maxPutOptionStrikPrice; }
   inline StockOption getClosestStrikeCall(void) const { return *this->closestStrikeCall; }
   inline StockOption getClosestStrikePut(void) const { return *this->closestStrikePut; }
   inline float getPutRatio(void) const { return this->putRatio; }
   inline float getCallRatio(void) const { return this->callRatio; }
   inline RisesFallsAvg getRises(void) const { return this->rises; }
   inline RisesFallsAvg getFalls(void) const { return this->falls; }
   inline float getFc(void) const { return this->fc; }
   inline float getFp(void) const { return this->fp; }

   //Do=============================================================================================
   //===============================================================================================
   static bool compareSymbols(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2);
   static bool comparePutRatios(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2);
   static bool compareCallRatios(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2);
   static void writeCSVkey(const FILE* _stream);
   static void writeCSVkeySimple(const FILE* _stream);
   static void writeCSVrow(const FILE* _stream, const ScharfOptionStock* _sos);
   static void writeCSVrowSimple(const FILE* _stream, const ScharfOptionStock* _sos);

   int processAnalysis(SimpleDate _start, SimpleDate _end, SimpleDate _lastWeekCloseDate, const HttpDownloader* _request);
   void printDataCSV(const char* _dir) const;

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   StockValueAndDate lastWeeksClosing;
   float minCallOptionStrikePrice;
   float maxPutOptionStrikPrice;
   StockOption* closestStrikeCall = NULL;
   StockOption* closestStrikePut = NULL;
   float putRatio = -999;
   float callRatio = -999;
   RisesFallsAvg rises;
   RisesFallsAvg falls;
   float fc;  //Max Call Risk Factor
   float fp;  //Max Put Risk Factor
   std::vector<StockHistory_Week> openToClosePercentChangeOrdered;

   //Do=============================================================================================
   //===============================================================================================
   static void copyThis(ScharfOptionStock* _dest, const ScharfOptionStock* _src);

};
//==================================================================================================
//End Definition====================================================================================

#endif
