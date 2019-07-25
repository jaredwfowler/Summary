
#ifndef STOCK_HISTORY_WEEK_HPP
#define STOCK_HISTORY_WEEK_HPP

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
Manages up to 5 StockHistory_Day objects. 
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "StockHistory_Day.hpp"
#include <vector>
#include <stdio.h>

//Definition========================================================================================
//==================================================================================================
class StockHistory_Week
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   bool operator< (const StockHistory_Week& _stk) const;
   void operator= (const StockHistory_Week& _stk);

   //Constructor(s)=================================================================================
   //===============================================================================================
   StockHistory_Week();
   StockHistory_Week(const StockHistory_Week& _stk);

   //Destructor=====================================================================================
   //===============================================================================================
   ~StockHistory_Week();

   //Gets===========================================================================================
   //===============================================================================================
   inline SimpleDate getDate(void) const { return this->date; }
   inline SimpleDate getCloseDate(void) const { return this->closeDate; }
   inline float getOpen(void) const { return this->open; }
   inline float getClose(void) const { return this->close; }
   inline float getHigh(void) const { return this->high; }
   inline float getLow(void) const { return this->low; }
   inline unsigned long getAvgVolume(void) const { return this->avgVolume; }
   inline unsigned long getTotalVolume(void) const { return this->totalVolume; }
   inline int getStockDays(void) const { return static_cast<int>(this->stockDays); }
   inline float getPercentChange(void) const { return this->percentChange; }

   StockHistory_Day* getDayHistory(unsigned int _i);

   //Do=============================================================================================
   //===============================================================================================
   bool addDayHistory(const StockHistory_Day* _stk);
   bool addDayHistory(const std::vector<StockHistory_Day>* _stk);

   static bool sortByOCpercentChange(const StockHistory_Week& _s1, const StockHistory_Week& _s2);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   SimpleDate date;           //Date of earliest day of week.
   SimpleDate closeDate;      //Date of latest day of week.
   float open;                //Opening price of earliest day of week.
   float close;               //Closing price for last day of week.
   float high;                //Highest price during the week.
   float low;                 //Lowest price during the week.
   unsigned long totalVolume; //Total volume for the week
   unsigned char stockDays;   //Total number of stock days this week
   unsigned long avgVolume;   //Avg. daily volume of trades/week
   float percentChange;       //From week's open to close
   StockHistory_Day* dayHistory[5];

   //Sets===========================================================================================
   //===============================================================================================
   inline void setDate(SimpleDate date){ this->date = date; }
   inline void setCloseDate(SimpleDate date){ this->closeDate = date; }
   inline void setOpen(float x){ this->open = x; }
   inline void setClose(float x){ this->close = x; }
   inline void setHigh(float x){ this->high = x; }
   inline void setLow(float x){ this->low = x; }
   inline void setStockDays(unsigned char x){ this->stockDays = x; }
   inline void setTotalVolume(unsigned long x){ this->totalVolume = x; }
   inline void setAvgVolume(unsigned long x){ this->avgVolume = x; }
   inline void setPercentChange(float x){ this->percentChange = x; }

   //Do=============================================================================================
   //===============================================================================================
   void updateWeekValues(void);
   int getDayOffset(const StockHistory_Day* _stk) const;
   bool dayHistorySanityCheck(const StockHistory_Day* _stk) const;
   void updateDate(void);
   void updateCloseDate(void);
   void updateOpen(void);
   void updateClose(void);
   void updateHigh(void);
   void updateLow(void);
   void updateVolume(void);
   void updatePercentChange(void);

   static void copyThis(StockHistory_Week* _dest, const StockHistory_Week* _src, bool _init);

};
//==================================================================================================
//End Definition====================================================================================

#endif
