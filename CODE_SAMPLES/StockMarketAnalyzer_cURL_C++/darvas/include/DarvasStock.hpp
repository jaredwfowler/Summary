
#ifndef DARVAS_STOCK_HPP
#define DARVAS_STOCK_HPP

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

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Stock.hpp"
#include <string>

//Definition========================================================================================
//==================================================================================================
class DarvasStock : public Stock
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   enum ERROR_CODES 
   {
      ERR_NONE    = 0x0000,
      ERR_HISTORY = 0x0001,
      ERR_VOLUME  = 0x0002,
      ERR_PRICE   = 0x0004,
      ERR_52WEEK  = 0x0008
   };

   struct Darvas_Volume 
   {
      Darvas_Volume() 
      : ratio(0.0f)
      {}

      float ratio;
   };

   struct Darvas_Price 
   {
      Darvas_Price()
      : slope(0.0f), meanY(0.0f), weeks(0)
      {}

      float slope; // Divide this by meanY to normalize
      float meanY;
      int weeks;   // From end date
   };

   struct Darvas_52WeekHigh 
   {
      Darvas_52WeekHigh()
      : weekOffset(0), dayOffset(0)
      {}

      int weekOffset;
      int dayOffset;
      StockHistory_Day copyOfDayHistory;
   };

   static const unsigned char DARVAS_VOLUME_MIN_WEEKS = 52;
   static const unsigned char DARVAS_VOLUME_SAMPLES = 4;
   static const unsigned char DARVAS_PRICE_SAMPLES = 8;

   static const unsigned int DARVAS_PRICE_SAMPLES_WEEKS[DARVAS_PRICE_SAMPLES];

   void operator= (const DarvasStock& _ds);

   //Constructor(s)=================================================================================
   //===============================================================================================
   DarvasStock(std::string _ticker, std::string _name);

   DarvasStock(const DarvasStock& _ds);

   //Destructor=====================================================================================
   //===============================================================================================
   virtual ~DarvasStock(void);

   //Gets===========================================================================================
   //===============================================================================================
   Darvas_52WeekHigh getHigh_52Weeks(void) const { return this->high_52week; }
   std::vector<Darvas_Volume>* getCopyOfVolumeSamples() const;
   std::vector<Darvas_Price>* getCopyOfPriceSamples() const;

   //Do=============================================================================================
   //===============================================================================================
   int processAnalysis(SimpleDate _start, SimpleDate _end);

   static bool compareSymbols(const DarvasStock& _d1, const DarvasStock& _d2);
   static void writeCSVkey(const FILE* _stream);
   static void writeCSVrow(const FILE* _stream, const DarvasStock* _ds);

   void printDataCSV(const char* _dir) const;

protected:
   //Gets===========================================================================================
   //===============================================================================================
   Darvas_Volume* getVolumeSamples() const { return this->volumeSamples; }
   Darvas_Price* getPriceSamples() const { return this->priceSamples; }

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   Darvas_Volume* volumeSamples;
   Darvas_Price* priceSamples;
   Darvas_52WeekHigh high_52week;

   //Do=============================================================================================
   //===============================================================================================
   bool analyze52WeekHigh(void);
   bool analyzeVolume(void);
   bool analyzePrice(void);

   bool calculateBestFitLine(int _weeks, Darvas_Price* _dp);

   static void copyThis(DarvasStock* _dest, const DarvasStock* _src);
};
//==================================================================================================
//End Definition====================================================================================

#endif
