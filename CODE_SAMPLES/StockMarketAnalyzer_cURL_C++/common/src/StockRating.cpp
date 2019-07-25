//Dependencies======================================================================================
//==================================================================================================
#include "StockRating.hpp"

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// operator*
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Overloaded Operators
//
//--------------------------------------------------------------------------------------------------
bool StockRating::operator< (const StockRating& _r) const 
{
   SimpleDate a = _r.getDate();
   if(this->getDate() < a) return true;
   return false;
}

//--------------------------------------------------------------------------------------------------
//Name:
// StockRating
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Constructor
//
//--------------------------------------------------------------------------------------------------
StockRating::StockRating(SimpleDate _date, float _rating, std::string _rater)
{
   this->setDate(_date);
   this->setRating(_rating);
   this->setRater(_rater);
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
