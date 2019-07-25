//Dependencies======================================================================================
//==================================================================================================
#include "StockOption.hpp"

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// StockOption
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
StockOption::StockOption(OPTION_TYPE _ot, std::string _contractName, float _price, float _change, 
   float _changePercent, float _bid, float _ask, float _openInterest, unsigned long _volume, 
   float _strike, SimpleDate _expDate, SimpleDate _todaysDate)
{
  this->setType(_ot);
  this->setExpiration(_expDate);
  this->setDate(_todaysDate);
  this->setStrike(_strike);
  this->setPrice(_price);
  this->setBid(_bid);
  this->setAsk(_ask);
  this->setChange(_change);
  this->setChangePercent(_changePercent);
  this->setopenInterest(_openInterest);
  this->setVolume(_volume);
  this->setContractName(_contractName);
}

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
// Operator Overloads
//
//--------------------------------------------------------------------------------------------------
bool StockOption::operator< (const StockOption& _stk) const 
{
   // Date Compare
   SimpleDate a = _stk.getExpiration();

   if (this->getExpiration() == a)
   {
      // Strike Price Compare
      return (this->getStrike() < _stk.getStrike());
   }

   if (this->getExpiration() < a)
   {
      return true;
   }

   return false;
}

bool StockOption::operator== (const StockOption& _stk) const 
{
   return (this->getContractName() == _stk.getContractName());
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
