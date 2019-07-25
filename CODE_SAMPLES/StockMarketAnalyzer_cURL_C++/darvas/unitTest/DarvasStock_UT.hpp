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
#include "Helpers_UT.hpp"
#include "DarvasStock.hpp"

class DarvasStockTest : public DarvasStock
{
public: 
   DarvasStockTest(std::string _ticker, std::string _name) 
      : DarvasStock(_ticker, _name) {}
   
   Darvas_Volume* GetVolumeSamples() const { return this->getVolumeSamples(); }
   Darvas_Price* GetPriceSamples() const { return this->getPriceSamples(); }
};

bool DarvasStock_Test_Copy() {
   bool success = true;

   DarvasStockTest s1("AAPL","AAPL");
   s1.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));
   DarvasStockTest s2("DIS", "DIS");
   s2.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));
   s2 = s1;
   DarvasStockTest s3(s2);

   
   DarvasStock::Darvas_Volume* volumeSamples1 = s1.GetVolumeSamples();
   DarvasStock::Darvas_Price* priceSamples1 = s1.GetPriceSamples();
   DarvasStock::Darvas_Volume* volumeSamples2 = s2.GetVolumeSamples();
   DarvasStock::Darvas_Price* priceSamples2 = s2.GetPriceSamples();
   DarvasStock::Darvas_Volume* volumeSamples3 = s3.GetVolumeSamples();
   DarvasStock::Darvas_Price* priceSamples3 = s3.GetPriceSamples();

   if((s1.getHigh_52Weeks().weekOffset != s2.getHigh_52Weeks().weekOffset) ||
      (s2.getHigh_52Weeks().weekOffset != s3.getHigh_52Weeks().weekOffset) ||
      (s1.getHigh_52Weeks().dayOffset != s2.getHigh_52Weeks().dayOffset) ||
      (s2.getHigh_52Weeks().dayOffset != s3.getHigh_52Weeks().dayOffset)) {
      success = false;
   }

   if((volumeSamples1 != volumeSamples2) && (volumeSamples2 != volumeSamples3)) {
      for(int i = 0; i < DarvasStock::DARVAS_VOLUME_SAMPLES; i++) {
         if((volumeSamples1[i].ratio != volumeSamples2[i].ratio) ||
            (volumeSamples2[i].ratio != volumeSamples3[i].ratio)) {
            success = false;
            break;
         }
      }
   } else {
      success = false;
   }

   if((priceSamples1 != priceSamples2) && (priceSamples2 != priceSamples3)) {
      for(int i = 0; i < DarvasStock::DARVAS_PRICE_SAMPLES; i++) {
         if((priceSamples1[i].slope != priceSamples2[i].slope) ||
            (priceSamples2[i].slope != priceSamples3[i].slope)) {
            success = false;
            break;
         }
      }
   } else {
      success = false;
   }

   return success;
}

bool DarvasStock_Test_Analyzer() {
   bool success = true;

   //DO NOT CHANGE THESE PARAMETERS
   DarvasStockTest ss("AAPL","AAPL");
   ss.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));

   //Examine Results
   DarvasStock::Darvas_Volume* volumeSamples = ss.GetVolumeSamples();
   DarvasStock::Darvas_Price* priceSamples = ss.GetPriceSamples();
   DarvasStock::Darvas_52WeekHigh high_52week = ss.getHigh_52Weeks();

   //Verify Price Analysis Slopes
   const float slopes[DarvasStock::DARVAS_PRICE_SAMPLES] = 
      { -1.765, -0.0969, 0.2419, 0.0576, -0.0572, 0.1603, 0.1223, 0.1237 };

   for(int i = 0; i < DarvasStock::DARVAS_PRICE_SAMPLES; i++) {
      if(abs(priceSamples[i].slope - slopes[i]) > .001) {
         success = false;
         fprintf(stdout, "DarvasStock_Test_Analyzer - Slope Compare Fail\n");
      }
   }

   return success;
}

bool DarvasStock_Test_SybmolCompare() {
   bool success = true;

   DarvasStockTest ds1("AAPL","AAPL");
   ds1.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));
   DarvasStockTest ds2("DIS","DIS");
   ds2.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));
   DarvasStockTest ds3("GILD","GILD");
   ds3.processAnalysis(SimpleDate(2014, 1, 1), SimpleDate(2015, 1, 1));

   if(!DarvasStock::compareSymbols(ds1, ds2) ||
      !DarvasStock::compareSymbols(ds1, ds3) ||
      DarvasStock::compareSymbols(ds2, ds1)  ||
      !DarvasStock::compareSymbols(ds2, ds3) ||
      DarvasStock::compareSymbols(ds3, ds1)  ||
      DarvasStock::compareSymbols(ds3, ds2)) {
      success = false;
   }

   return success;
}
