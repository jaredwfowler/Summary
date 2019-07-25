
#ifndef STOCK_HISTORY_DAY_HPP
#define STOCK_HISTORY_DAY_HPP

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
Holds a single day's statistical information pertaining to a stock.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "SimpleDate.hpp"

//Definition========================================================================================
//==================================================================================================
class StockHistory_Day
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   StockHistory_Day(SimpleDate _date = SimpleDate(1, 1, 1), float _open = 0, 
      float _close = 0, float _high = 0, float _low = 0, unsigned long _volume = 0);

   //Gets===========================================================================================
   //===============================================================================================
   inline SimpleDate getDate(void) const { return this->date; }
   inline float getOpen(void) const { return this->open; }
   inline float getClose(void) const { return this->close; }
   inline float getHigh(void) const { return this->high; }
   inline float getLow(void) const { return this->low; }
   inline unsigned long getVolume(void) const { return this->volume; }

   //Sets===========================================================================================
   //===============================================================================================
   inline void setDate(SimpleDate date){ this->date = date; }
   inline void setOpen(float x){ this->open = x; }
   inline void setClose(float x){ this->close = x; }
   inline void setHigh(float x){ this->high = x; }
   inline void setLow(float x){ this->low = x; }
   inline void setVolume(unsigned long x){ this->volume = x; }

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   SimpleDate date;
   float open;
   float close;
   float high;
   float low;
   unsigned long volume;

};
//==================================================================================================
//End Definition====================================================================================

#endif
