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
** Version Date: May 24, 2016
** Revision Personnel: JFowler, 
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
ScharfOption UnitTests
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "ScharfOptionStock.hpp"


class ScharfOptionStockTest : public ScharfOptionStock
{
public: 
   ScharfOptionStockTest(std::string _ticker, std::string _name) 
      : ScharfOptionStock(_ticker, _name) {}
};

bool ScharfOptionStock_Test_Copy() {
   bool success = true;

   return success;
}

bool ScharfOptionStock_Test_Analyzer() {
   bool success = true;

   return success;
}

bool ScharfOptionStock_Test_SybmolCompare() {
   bool success = true;

   ScharfOptionStockTest ds1("AAPL","AAPL");
   ds1.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1), SimpleDate(2015, 1, 1));
   ScharfOptionStockTest ds2("DIS","DIS");
   ds2.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1), SimpleDate(2015, 1, 1));
   ScharfOptionStockTest ds3("GILD","GILD");
   ds3.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1), SimpleDate(2015, 1, 1));

   if(!ScharfOptionStock::compareSymbols(ds1, ds2) ||
      !ScharfOptionStock::compareSymbols(ds1, ds3) ||
      ScharfOptionStock::compareSymbols(ds2, ds1)  ||
      !ScharfOptionStock::compareSymbols(ds2, ds3) ||
      ScharfOptionStock::compareSymbols(ds3, ds1)  ||
      ScharfOptionStock::compareSymbols(ds3, ds2)) {
      success = false;
   }

   return success;
}
