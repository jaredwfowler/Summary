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
****  Date:  May 3, 2016
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: May 3, 2016
** Revision Personel: JFowler, 
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "SimpleDate.hpp"
#include <stdio.h>

bool SimpleDate_Test() {
   bool success = true;

   if((SimpleDate::getMonthName(1) != "JANUARY") ||
      (SimpleDate::getMonthName(12) != "DECEMBER"))
   {
      fprintf(stdout, "SimpleDate::getMonthName Fail\n");
      success = false;
   }

   if((SimpleDate::getMonthAbr(1) != "JAN") ||
      (SimpleDate::getMonthAbr(12) != "DEC"))
   {
      fprintf(stdout, "SimpleDate::getMonthAbr Fail\n");
      success = false;
   }

   if((SimpleDate::getDayName(1) != "SUNDAY") ||
      (SimpleDate::getDayName(7) != "SATURDAY"))
   {
      fprintf(stdout, "SimpleDate::getDayName Fail\n");
      success = false;
   }

   if((SimpleDate::getMonth("JANUARY") != 1) ||
      (SimpleDate::getMonth("DEC") != 12))
   {
      fprintf(stdout, "SimpleDate::getMonth Fail\n");
      success = false;
   }

   SimpleDate sDate1(2016,5,12);
   SimpleDate sDate2(2015,1,15);
   SimpleDate sDate3(1970,1,1);

   if((sDate1.getDayDifference(&sDate2) != -483) ||
      (sDate3.getDayDifference(&sDate1) != 16933) ||
      (sDate3.getDayDifference(&sDate2) != 16450))
   {
      fprintf(stdout, "SimpleDate::getDayDifference Fail\n");
      success = false;
   }

   return success;
}

int main(){

   if(SimpleDate_Test()) {
      fprintf(stdout, "SimpleDate_Test: PASS\n");
   } else {
      fprintf(stdout, "SimpleDate_Test: FAIL\n");
   }


   return 0;
}
