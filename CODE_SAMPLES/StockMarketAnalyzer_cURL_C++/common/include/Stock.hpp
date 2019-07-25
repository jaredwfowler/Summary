
#ifndef STOCK_HPP
#define STOCK_HPP

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
Stock keeps track of a particular stock's weekly history for x amount of time. (Normally 52).
Tools are provided to download the info via curl and parse out the info. Print functions are
available to output the gathered information.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <vector>
#include <string>
#include "SimpleDate.hpp"
#include "StockHistory_Week.hpp"
#include "StockRating.hpp"

// Google and Yahoo no longer provide a nice API for stock historical information. -December 5, 2017
#define GOOGLE_YAHOO_HISTORY_API       0
#define YAHOO_HISTORY_PURE_SCRAPE      0

//Definition========================================================================================
//==================================================================================================
class Stock
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   Stock(std::string _ticker, std::string _name);

   //Destructor=====================================================================================
   //===============================================================================================
   virtual ~Stock(){}

   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   typedef struct
   {
      std::string scharf;
      std::string google;
      std::string yahoo;
      std::string fidelity;
      std::string marketWatch;
   } alternative_symbol_names_t;

   typedef enum
   {
      SOURCE_GOOGLE,
      SOURCE_YAHOO,
      SOURCE_FIDELITY,
      SOURCE_MARKETWATCH
   } ticker_source_t;

   static const alternative_symbol_names_t alternative_symbol_names_table[];

   //Gets===========================================================================================
   //===============================================================================================
   inline std::string getTicker(void) const { return this->ticker; }
   inline std::string getName(void) const { return this->name; }
   inline std::vector<StockHistory_Week> getAllStockHistory(void) const { return this->history; }
   inline std::vector<StockRating> getAllStockRating(void) const { return this->rating; }

   StockRating getStockRating(std::string _rater) const;
   SimpleDate getHistoryStartDate(void) const;
   SimpleDate getHistoryEndDate(void) const;

   static std::string getAlternativeName(std::string _name, ticker_source_t _source);

   //Do=============================================================================================
   //===============================================================================================
   bool addHistory(SimpleDate _start, SimpleDate _end);
   bool addRating(const StockRating* _stkRating);
   bool clearHistory(void);

   static void historyTableOutput(const FILE* _stream, const std::vector<StockHistory_Week>* _vec);
   static void historyCSVOutput(const FILE* _stream, const std::vector<StockHistory_Week>* _vec);
   static std::vector<StockHistory_Week>* downloadParseReturnHistory(std::string _ticker,
                                                                     SimpleDate _start,
                                                                     SimpleDate _end);

protected: //#######################################################################################
   //Gets===========================================================================================
   //===============================================================================================
   inline const std::vector<StockHistory_Week>* getHistoryPtr(void) const { return &(this->history); }

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   std::string ticker;
   std::string name;
   std::vector<StockHistory_Week> history;
   std::vector<StockRating> rating;

   //Sets===========================================================================================
   //===============================================================================================
   inline void setTicker(std::string _x){ this->ticker = _x; }
   inline void setName(std::string _x){ this->name = _x; }

   //Do=============================================================================================
   bool insertRating(const StockRating* _r);
   bool insertHistory(const StockHistory_Week* _stk);
   void sortRating(void);
   void sortHistory(void);

#if GOOGLE_YAHOO_HISTORY_API
   static std::vector<StockHistory_Week>* getGoogleFinanceHistory(std::string _ticker, SimpleDate _start, SimpleDate _end);
   static std::vector<StockHistory_Week>* getYahooFinanceHistory(std::string _ticker, SimpleDate _start, SimpleDate _end);
#elif YAHOO_HISTORY_PURE_SCRAPE
    static std::vector<StockHistory_Week>* getYahooFinanceHistory_PurePageScrape(std::string _ticker);
#else
   static std::vector<StockHistory_Week>* getYahooFinanceHistory_PageScrape(std::string _ticker, SimpleDate _start, SimpleDate _end);
#endif
};
//==================================================================================================
//End Definition====================================================================================

#endif
