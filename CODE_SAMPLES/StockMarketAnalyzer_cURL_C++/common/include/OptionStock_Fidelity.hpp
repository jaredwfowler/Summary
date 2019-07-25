
#ifndef OPTION_STOCK_FIDELITY_HPP
#define OPTION_STOCK_FIDELITY_HPP

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
****  Date:  January 12, 2017
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: January 12, 2017
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Derived from class OptionStock. OptionStock_Fidelity adds support for Fidelity option downloads.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "OptionStock.hpp"
#include "HttpDownloader_Fidelity.hpp"

//Definition========================================================================================
//==================================================================================================
class OptionStock_Fidelity : public OptionStock
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   OptionStock_Fidelity(std::string _ticker, std::string _name);

   //Destructor=====================================================================================
   //===============================================================================================
   virtual ~OptionStock_Fidelity() {}

   //Do=============================================================================================
   //===============================================================================================
   bool addOptions_Fidelity(const HttpDownloader_Fidelity* _httpRequest);

   static std::vector<StockOption>* getThisWeeksOptions_Fidelity(std::string _ticker, const HttpDownloader_Fidelity* _httpRequest);

private: //#########################################################################################
   //Do=============================================================================================
   //===============================================================================================
   static std::vector<StockOption>* getFidelityWeekOptions(std::string _ticker, const HttpDownloader_Fidelity* _httpRequest);

};
//==================================================================================================
//End Definition====================================================================================

#endif