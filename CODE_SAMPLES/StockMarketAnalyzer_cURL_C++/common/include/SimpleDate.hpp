
#ifndef SIMPLEDATE_HPP
#define SIMPLEDATE_HPP

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
****  Date:  February 15, 2016
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
SimpleDate was my response to dumbing down the time struct provided by c. SimpleDate only keeps 
track of day of month, day of week, month, and year. It has other static functions which can be 
used to determine what day of the week a date is on, the date if we add or subtract delta days 
from a date ,etc.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <string>
#include <ctime>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//Definition========================================================================================
//==================================================================================================
class SimpleDate
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   bool operator< (const SimpleDate& _date) const;
   bool operator> (const SimpleDate& _date) const;
   bool operator== (const SimpleDate& _date) const;
   bool operator!= (const SimpleDate& _date) const;
   bool operator<= (const SimpleDate& _date) const;
   bool operator>= (const SimpleDate& _date) const;

   //Constructor(s)=================================================================================
   //===============================================================================================
   SimpleDate(unsigned int _year = 1, unsigned int _month = 1, unsigned int _day = 1);
   SimpleDate(time_t _t);

   //Gets===========================================================================================
   //===============================================================================================
   inline unsigned int getYear(void) const { return this->year; }
   inline unsigned int getMonth(void) const { return this->month; }
   inline unsigned int getDayM(void) const { return this->dayM; }
   inline unsigned int getDayW(void) const { return this->dayW; }

   time_t getUnixTimeStamp(void);

   static std::string getMonthName(unsigned int _x);
   static std::string getMonthAbr(unsigned int _x);
   static std::string getDayName(unsigned int _x);
   static int getMonth(std::string _str);
   static SimpleDate today(void);

   //Sets===========================================================================================
   //===============================================================================================
   bool setDate(unsigned int _year, unsigned int _month, unsigned int _day);
   bool setYear(unsigned int _year);
   bool setMonth(unsigned int _month);
   bool setDay(unsigned int _day);

   //Do=============================================================================================
   //===============================================================================================
   std::string toString1(void) const;
   std::string toString2(void) const;

   SimpleDate addDays(int _delta) const;
   int getDayDifference(const SimpleDate* _d2) const;

   static unsigned int dayofweek(int _d, int _m, int _y);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   static const char* WEEK_DAYS[];
   static const char* MONTHS[];
   static const char* MONTHS_ABR[];
   static const int MONTH_DAYS[];

   unsigned int year;  //yyyy
   unsigned int month; //1-12
   unsigned int dayM;  //1-31
   unsigned int dayW;  //1-7

   //Do=============================================================================================
   //===============================================================================================
   int countLeapYears(int _years, int _month) const;
};
//==================================================================================================
//End Definition====================================================================================

#endif
