
#ifndef STOCK_MARKET_NYSE_HPP
#define STOCK_MARKET_NYSE_HPP

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
Basic functions specific to the New York Stock Exchange
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <cstdlib>
#include "HttpDownloader.hpp"

//Definition========================================================================================
//==================================================================================================
class StockMarket_NYSE
{
public: //##########################################################################################
   //Do=============================================================================================
   //===============================================================================================
   static time_t getNewYorkTime(void);

};
//==================================================================================================
//End Definition====================================================================================

#endif