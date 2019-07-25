
#ifndef STOCK_OPTION_HPP
#define STOCK_OPTION_HPP

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
** Revision Personel: JFowler, 
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Holds information pertaining to a stock option, either call or put. This is a simple holder class.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <string>
#include "SimpleDate.hpp"

//Definition========================================================================================
//==================================================================================================
class StockOption
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   enum OPTION_TYPE 
   {
      PUT, CALL
   };

   bool operator< (const StockOption& _stk) const;
   bool operator== (const StockOption& _stk) const;

   //Constructor(s)=================================================================================
   //===============================================================================================
   StockOption(OPTION_TYPE _ot, std::string _contractName, float _price, float _change, 
      float _changePercent, float _bid, float _ask, float _openInterest, unsigned long _volume, 
      float _strike, SimpleDate _expDate, SimpleDate _todaysDate);

   //Gets===========================================================================================
   //===============================================================================================
   inline OPTION_TYPE getType(void) const { return this->type; }
   inline SimpleDate getDate(void) const { return this->todaysDate; }
   inline SimpleDate getExpiration(void) const { return this->expirationDate; }
   inline float getStrike(void) const { return this->strike; }
   inline float getPrice(void) const { return this->price; }
   inline float getBid(void) const { return this->bid; }
   inline float getAsk(void) const { return this->ask; }
   inline float getChange(void) const { return this->change; }
   inline float getChangePercent(void) const { return this->changePercent; }
   inline float getOpenInterest(void) const { return this->openInterest; }
   inline unsigned long getVolume(void) const { return this->volume; }
   inline std::string getContractName(void) const { return this->contractName; }

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   OPTION_TYPE type;
   SimpleDate todaysDate;
   SimpleDate expirationDate;
   std::string contractName;
   float change;
   float changePercent;
   float openInterest;
   unsigned long volume;
   float strike;
   float price;
   float bid;
   float ask;

   //Sets===========================================================================================
   //===============================================================================================
   inline void setType(OPTION_TYPE x){ this->type = x; }
   inline void setDate(SimpleDate d){ this->todaysDate = d; }
   inline void setExpiration(SimpleDate d){ this->expirationDate = d; }
   inline void setStrike(float x){ this->strike = x; }
   inline void setPrice(float x){ this->price = x; }
   inline void setBid(float x){ this->bid = x; }
   inline void setAsk(float x){ this->ask = x; }
   inline void setChange(float x){ this->change = x; }
   inline void setChangePercent(float x){ this->changePercent = x; }
   inline void setopenInterest(float x){ this->openInterest = x; }
   inline void setVolume(unsigned long x){ this->volume = x; }
   inline void setContractName(std::string x){ this->contractName = x; }

};
//==================================================================================================
//End Definition====================================================================================

#endif
