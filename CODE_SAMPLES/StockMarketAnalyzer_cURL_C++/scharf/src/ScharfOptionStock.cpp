//Dependencies======================================================================================
//==================================================================================================
#include "ScharfOptionStock.hpp"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <utility>
#include <cstring>

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// ScharfOptionStock
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
ScharfOptionStock::ScharfOptionStock(std::string _ticker, std::string _name)
   : OptionStock_MarketWatch(_ticker, _name)
{
   // Void
}

//--------------------------------------------------------------------------------------------------
//Name:
// ScharfOptionStock
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
// Copy Constructor
//
//--------------------------------------------------------------------------------------------------
ScharfOptionStock::ScharfOptionStock(const ScharfOptionStock& _sos)
   : OptionStock_MarketWatch(_sos)
{
   //Copy function
   ScharfOptionStock::copyThis(this, &_sos);
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~ScharfOptionStock
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
// Destructor
//
//--------------------------------------------------------------------------------------------------
ScharfOptionStock::~ScharfOptionStock(void)
{
   delete this->closestStrikeCall;
   delete this->closestStrikePut;
   this->closestStrikeCall = NULL;
   this->closestStrikePut = NULL;
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
// void
//
//About:
// Overloaded Operators
//
//--------------------------------------------------------------------------------------------------
void ScharfOptionStock::operator= (const ScharfOptionStock& _sos)
{
   // Call base assignment operator
   OptionStock::operator=(_sos);
   // Copy function
   ScharfOptionStock::copyThis(this, &_sos);
}

//--------------------------------------------------------------------------------------------------
//Name:
// copyThis
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
// Common function for both the copy constructor and assignment operator to call
//
//--------------------------------------------------------------------------------------------------
void ScharfOptionStock::copyThis(ScharfOptionStock* _dest, const ScharfOptionStock* _src)
{
   _dest->lastWeeksClosing = _src->lastWeeksClosing;
   _dest->minCallOptionStrikePrice = _src->minCallOptionStrikePrice;
   _dest->maxPutOptionStrikPrice = _src->maxPutOptionStrikPrice;
   _dest->putRatio = _src->putRatio;
   _dest->callRatio = _src->callRatio;
   _dest->rises = _src->rises;
   _dest->falls = _src->falls;
   _dest->fc = _src->fc;
   _dest->fp = _src->fp;
   _dest->openToClosePercentChangeOrdered = _src->openToClosePercentChangeOrdered;

   // De-allocate dynamic memory
   delete _dest->closestStrikeCall;
   _dest->closestStrikeCall = NULL;
   delete _dest->closestStrikePut;
   _dest->closestStrikePut = NULL;

   if (_src->closestStrikeCall != NULL) 
   {
      _dest->closestStrikeCall = new StockOption(*_src->closestStrikeCall);
   }

   if (_src->closestStrikePut != NULL)
   {
      _dest->closestStrikePut = new StockOption(*_src->closestStrikePut);
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// comparePutRatios, compareCallRatios, compareSymbols
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
// ...
//
//--------------------------------------------------------------------------------------------------
bool ScharfOptionStock::comparePutRatios(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2)
{
   return (_s1.getPutRatio() < _s2.getPutRatio());
}
bool ScharfOptionStock::compareCallRatios(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2)
{
   return (_s1.getCallRatio() < _s2.getCallRatio());
}
bool ScharfOptionStock::compareSymbols(const ScharfOptionStock& _s1, const ScharfOptionStock& _s2)
{
   return (_s1.getTicker() < _s2.getTicker());
}

//--------------------------------------------------------------------------------------------------
//Name:
// writeCSVkey
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
// Writes a single row which represents the column format of the CSV output
//
//--------------------------------------------------------------------------------------------------
void ScharfOptionStock::writeCSVkey(const FILE* _stream) 
{
   fprintf(const_cast<FILE*>(_stream), "%10s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s,%20s\n",
      "symbol",
      "avg%WeekHighRise",
      "avg%WeekHighDrop",
      "lastWeekClosePrice",
      "minCallStrikePrice",
      "callStrike",
      "callBid",
      "callAsk",
      "callVolume",
      "maxPutStrikPrice",
      "putStrike",
      "putBid",
      "putAsk",
      "putVolume",
      "callRatio",
      "putRatio");
}

void ScharfOptionStock::writeCSVkeySimple(const FILE* _stream) 
{
   fprintf(const_cast<FILE*>(_stream), "%10s,%20s,%20s,%20s,%20s\n",
      "symbol",
      "callStrike",
      "putStrike",
      "callRatio",
      "putRatio");
}

//--------------------------------------------------------------------------------------------------
//Name:
// writeCSVrow
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
// Write a single line of output which contains this stocks scharf information.
//
//--------------------------------------------------------------------------------------------------
void ScharfOptionStock::writeCSVrow(const FILE* _stream, const ScharfOptionStock* _sos)
{

   if (_sos == NULL) 
   {
      return;
   }

   // Check for null values
   float callStrike = 0.0f;
   float callBid = 0.0f;
   float callAsk = 0.0f;
   unsigned long callVolume = 0;
   float putStrike = 0.0f;
   float putBid = 0.0f;
   float putAsk = 0.0f;
   unsigned long putVolume = 0;
   if (_sos->closestStrikeCall != NULL)
   {
      callStrike = _sos->closestStrikeCall->getStrike();
      callBid = _sos->closestStrikeCall->getBid();
      callAsk = _sos->closestStrikeCall->getAsk();
      callVolume = _sos->closestStrikeCall->getVolume();
   }
   if (_sos->closestStrikePut != NULL) 
   {
      putStrike = _sos->closestStrikePut->getStrike();
      putBid = _sos->closestStrikePut->getBid();
      putAsk = _sos->closestStrikePut->getAsk();
      putVolume = _sos->closestStrikePut->getVolume();
   }

   fprintf(const_cast<FILE*>(_stream), "%10s,%20.2f,%20.2f,%20.2f,%20.2f,%20.2f,%20.2f,%20.2f,%20lu,%20.2f,%20.2f,%20.2f,%20.2f,%20lu,%20.2f,%20.2f\n",
      _sos->getName().c_str(),
      _sos->rises.avgSecondAndThird * 100,
      _sos->falls.avgSecondAndThird * 100,
      _sos->lastWeeksClosing.value,
      _sos->minCallOptionStrikePrice,
      callStrike,
      callBid,
      callAsk,
      callVolume,
      _sos->maxPutOptionStrikPrice,
      putStrike,
      putBid,
      putAsk,
      putVolume,
      _sos->getCallRatio(),
      _sos->getPutRatio());
}

void ScharfOptionStock::writeCSVrowSimple(const FILE* _stream, const ScharfOptionStock* _sos) 
{

   if (_sos == NULL)
   {
      return;
   }

   // Check for null values
   float callStrike = 0.0f;
   float putStrike = 0.0f;

   if (_sos->closestStrikeCall != NULL) 
   {
      callStrike = _sos->closestStrikeCall->getStrike();
   }
   if (_sos->closestStrikePut != NULL)
   {
      putStrike = _sos->closestStrikePut->getStrike();
   }

   fprintf(const_cast<FILE*>(_stream), "%10s,%20.2f,%20.2f,%20.2f,%20.2f\n",
      _sos->getName().c_str(),
      callStrike,
      putStrike,
      _sos->getCallRatio(),
      _sos->getPutRatio());
}

//--------------------------------------------------------------------------------------------------
//Name:
// printDataCSV
//
//Arguments:
// _dir -> directory where we will create the files and write the data
//
//Output:
// void
//
//Returns:
// void
//
//About:
// outputs data to individual files which this method creates in the provided directory
//
//--------------------------------------------------------------------------------------------------
void ScharfOptionStock::printDataCSV(const char* _dir) const 
{
   // Print Option Data
   char file[512] = {0};
   sprintf(file, "%s/%s_options.csv", _dir, this->getTicker().c_str());
   FILE* fd = NULL;
   if ((fd = std::fopen(file, "w")) == NULL) 
   {
      fprintf(stderr, "Failed to open file for writing.\n");
   }
   fprintf(fd,"Calls\n");
   OptionStock::optionsCSVOutput(fd, this->getCallsPtr());
   fprintf(fd,"\n\n");
   fprintf(fd,"Puts\n");
   OptionStock::optionsCSVOutput(fd, this->getPutsPtr());
   std::fclose(fd);

   // Print History Data
   sprintf(file, "%s/%s_history.csv", _dir, this->getTicker().c_str());
   if ((fd = std::fopen(file, "w")) == NULL) 
   {
      fprintf(stderr, "Failed to open file for writing.\n");
   }
   Stock::historyCSVOutput(fd, this->getHistoryPtr());
   std::fclose(fd);
}

//--------------------------------------------------------------------------------------------------
//Name:
// processAnalysis
//
//Arguments:
// _start - Start Date
// _end   - End Date
// _lastWeekCloseDate - As a convenience, please pass in the last weeks close date...
//
//Output:
// void
//
//Returns:
// void
//
//About:
// outputs data to individual files which this method creates in the provided directory
//
//--------------------------------------------------------------------------------------------------
int ScharfOptionStock::processAnalysis(SimpleDate _start, SimpleDate _end, SimpleDate _lastWeekCloseDate, const HttpDownloader* _request)
{
   int errors = ERR_NONE;

   // Add stock history - Call to ancestor Stock
   if (this->addHistory(_start, _end) == false) 
   {
      fprintf(stderr, "AddHistory Error with: %s \n", this->getTicker().c_str());
      errors |= ERR_HISTORY;
   }
   // Add stock calls/puts - Call to ancestor OptionStock
   if ((ERR_NONE == errors) && (this->addOptions_MarketWatch(_request) == false)) 
   {
      fprintf(stderr, "AddOptions Error with: %s \n", this->getTicker().c_str());
      errors |= ERR_OPTIONS;
   }

   if(ERR_NONE != errors) 
   {
      return errors;
   }

   // Sort the Stock Week Info by % change from open-close, but first get the date and price of last week's close
   this->openToClosePercentChangeOrdered = this->getAllStockHistory();
   // We need at least 3 weeks of data to get accurate results... and for the code to work properly.
   if (this->openToClosePercentChangeOrdered.size() < 3) 
   {
      return (errors | ERR_SAMPLES);
   }

   for (unsigned int i = this->openToClosePercentChangeOrdered.size() - 1; i >= 0; i--)
   {
      if (this->openToClosePercentChangeOrdered.at(i).getCloseDate() <= _lastWeekCloseDate) 
      {
         this->lastWeeksClosing.date = this->openToClosePercentChangeOrdered.at(i).getCloseDate();
         this->lastWeeksClosing.value = this->openToClosePercentChangeOrdered.at(i).getClose();
         break;
      }
   }
   // Now sort the list
   std::sort(this->openToClosePercentChangeOrdered.begin(), this->openToClosePercentChangeOrdered.end(), StockHistory_Week::sortByOCpercentChange);
   // Get the (1-3) largest rises, falls, and avg of 2,3
   this->rises.first = this->openToClosePercentChangeOrdered.at(this->openToClosePercentChangeOrdered.size() - 1).getPercentChange();
   this->rises.second = this->openToClosePercentChangeOrdered.at(this->openToClosePercentChangeOrdered.size() - 2).getPercentChange();
   this->rises.third = this->openToClosePercentChangeOrdered.at(this->openToClosePercentChangeOrdered.size() - 3).getPercentChange();
   this->rises.avgSecondAndThird = (this->rises.second + this->rises.third) / (float)2.0;
   this->falls.first = this->openToClosePercentChangeOrdered.at(0).getPercentChange();
   this->falls.second = this->openToClosePercentChangeOrdered.at(1).getPercentChange();
   this->falls.third = this->openToClosePercentChangeOrdered.at(2).getPercentChange();
   this->falls.avgSecondAndThird = (this->falls.second + this->falls.third) / (float)2.0;
   // Calculate Risk Factors
   // fc = 1 + Avg%WeekChange(2nd and 3rd largest rises)
   this->fc = (float)1.0 + this->rises.avgSecondAndThird;
   // fp = 1 + Avg%WeekChange(2nd and 3rd largest falls)
   this->fp = (float)1.0 + this->falls.avgSecondAndThird;
   // Get the minCall and maxPut Strike Prices
   this->minCallOptionStrikePrice = this->fc * this->lastWeeksClosing.value;
   this->maxPutOptionStrikPrice = this->fp * this->lastWeeksClosing.value;
   // Find the closest available strike prices for calls and puts
   std::vector<StockOption> calls = this->getCalls();
   std::vector<StockOption> puts = this->getPuts();
   int index = -1;
   float closestValue = FLT_MAX;
   for (unsigned int i = 0; i < calls.size(); i++)
   {
      float value = std::abs(calls.at(i).getStrike() - this->minCallOptionStrikePrice);
      if (value < closestValue) 
      {
         closestValue = value;
         index = i;
      }
   }
   if (index == -1)
   {
      fprintf(stderr, "SharfOptionStock::Constructor Error Occurred.\n");
      return (errors | ERR_INDEX);
   }
   this->closestStrikeCall = new StockOption(calls.at(index));
   index = -1;
   closestValue = FLT_MAX;
   for (unsigned int i = 0; i < puts.size(); i++) 
   {
      float value = std::abs(puts.at(i).getStrike() - this->maxPutOptionStrikPrice);
      if (value < closestValue) 
      {
         closestValue = value;
         index = i;
      }
   }
   if (index == -1)
   {
      fprintf(stderr, "SharfOptionStock::Constructor Error Occurred.\n");
      return (errors | ERR_INDEX);
   }
   this->closestStrikePut = new StockOption(puts.at(index));

   // Get normalized ratio... this is the comparing point
   // Ratio = (100 * BidPrice) / StrikePrice
   this->putRatio = (100.0f * this->closestStrikePut->getBid()) / this->closestStrikePut->getStrike();
   this->callRatio = (100.0f * this->closestStrikeCall->getBid()) / this->closestStrikeCall->getStrike();

   return errors;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************
March 9, 2016 - Jared Fowler
Added a function "copyThis" which handles the common copy traits of the assignment operator and
copy constructor. Fixed it to delete any allocated memory that is being overwritten, to fix
memory leaks.
****************************************************************************************************
***************************************************************************************************/
