//Dependencies======================================================================================
//==================================================================================================
#include "DarvasStock.hpp"
#include "math.h"

const unsigned int DarvasStock::DARVAS_PRICE_SAMPLES_WEEKS[DARVAS_PRICE_SAMPLES] =
   {1, 2, 3, 4, 8, 12, 26, 52};

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// DarvasStock
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
// Constructor(s)
//
//--------------------------------------------------------------------------------------------------
DarvasStock::DarvasStock(std::string _ticker, std::string _name)
   : Stock(_ticker, _name), volumeSamples(NULL), priceSamples(NULL)
{
   // Allocate arrays
   this->volumeSamples = new Darvas_Volume[DARVAS_VOLUME_SAMPLES];
   this->priceSamples = new Darvas_Price[DARVAS_PRICE_SAMPLES];
}

DarvasStock::DarvasStock(const DarvasStock& _ds)
   : Stock(_ds)
{
   // Bug Fix - Apparently C++ will not initialize * to NULL
   this->volumeSamples = NULL;
   this->priceSamples = NULL;

   // Copy function
   DarvasStock::copyThis(this, &_ds);
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~DarvasStock
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
DarvasStock::~DarvasStock(void)
{
   // Free dynamic memory
   delete [] this->volumeSamples;
   this->volumeSamples = NULL;
   delete [] this->priceSamples;
   this->priceSamples = NULL;
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
void DarvasStock::operator= (const DarvasStock& _ds)
{
   // Call base assignment operator
   Stock::operator=(_ds);
   // Copy function
   DarvasStock::copyThis(this, &_ds);
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
// Handles the operation of copying a DarvasStock object. Called by copy constructor and
// overloaded assignment operator.
//
//--------------------------------------------------------------------------------------------------
void DarvasStock::copyThis(DarvasStock* _dest, const DarvasStock* _src)
{
   _dest->high_52week = _src->high_52week;

   // De-allocate destinations dynamic memory
   delete [] _dest->volumeSamples;
   _dest->volumeSamples = NULL;
   delete [] _dest->priceSamples;
   _dest->priceSamples = NULL;

   if (NULL != _src->volumeSamples)
   {
      _dest->volumeSamples = new Darvas_Volume[DARVAS_VOLUME_SAMPLES];
      for(int i = 0; i < DARVAS_VOLUME_SAMPLES; i++)
      {
         _dest->volumeSamples[i] = _src->volumeSamples[i];
      }
   }

   if (NULL != _src->priceSamples)
   {
      _dest->priceSamples = new Darvas_Price[DARVAS_PRICE_SAMPLES];
      for (int i = 0; i < DARVAS_PRICE_SAMPLES; i++)
      {
         _dest->priceSamples[i] = _src->priceSamples[i];
      }
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// getCopyOf*Samples
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// Returns a vector containing the collection of * samples
//
//About:
// It will be up to the caller to properly free up dynamic memory
//
//--------------------------------------------------------------------------------------------------
std::vector<DarvasStock::Darvas_Volume>* DarvasStock::getCopyOfVolumeSamples() const
{
   std::vector<DarvasStock::Darvas_Volume>* vec = new std::vector<Darvas_Volume>(DARVAS_VOLUME_SAMPLES);
   for (int i = 0; i < DARVAS_VOLUME_SAMPLES; i++)
   {
      vec->at(i) = this->volumeSamples[i];
   }

   return vec;
}

std::vector<DarvasStock::Darvas_Price>* DarvasStock::getCopyOfPriceSamples() const
{
   std::vector<DarvasStock::Darvas_Price>* vec = new std::vector<Darvas_Price>(DARVAS_PRICE_SAMPLES);
   for (int i = 0; i < DARVAS_PRICE_SAMPLES; i++)
   {
      vec->at(i) = this->priceSamples[i];
   }

   return vec;
}

//--------------------------------------------------------------------------------------------------
//Name:
// processAnalysis
//
//Arguments:
// Start and End SimpleDates
//
//Output:
// void
//
//Returns:
// DarvasAnalysis Error Code
//
//About:
// This is a wrapper for getting the history and doing the analysis
//
//--------------------------------------------------------------------------------------------------
int DarvasStock::processAnalysis(SimpleDate _start, SimpleDate _end)
{
   int errors = ERR_NONE;

   // Add stock history - Call to ancestor Stock
   if (this->addHistory(_start, _end) == false)
   {
      fprintf(stderr, "AddHistory Error with: %s \n", this->getTicker().c_str());
      errors |= ERR_HISTORY;
   } 
   else
   {
      if (!analyze52WeekHigh())
      {
         errors |= ERR_52WEEK;
      }
      if (!analyzeVolume())
      {
         errors |= ERR_VOLUME;
      }
      if (!analyzePrice())
      {
         errors |= ERR_PRICE;
      }
   }

   return errors;
}

//--------------------------------------------------------------------------------------------------
//Name:
// compareSymbols
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// bool if _d1 comes before _d2
//
//About:
// ...
//
//--------------------------------------------------------------------------------------------------
bool DarvasStock::compareSymbols(const DarvasStock& _d1, const DarvasStock& _d2)
{
   return (_d1.getTicker() < _d2.getTicker());
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
void DarvasStock::writeCSVkey(const FILE* _stream)
{
   // Symbol and 52 week high info
   fprintf(const_cast<FILE*>(_stream), "%10s\t%20s\t%15s", "symbol", "52_HI Price", "52_HI Date");

   char buffStr[128] = {0};

   // Volume
   for (int i = 1; i <= DarvasStock::DARVAS_VOLUME_SAMPLES; i++)
   {
      #if defined(_MSC_VER) || defined(__WINDOWS__)
      sprintf_s(buffStr, "Vol. Ratio %d", i);
      #else
      snprintf(buffStr, 128, "Vol. Ratio %d", i);
      #endif
      fprintf(const_cast<FILE*>(_stream), "\t%20s", buffStr);
   }

   // Price
   for (int i = 0; i < DarvasStock::DARVAS_PRICE_SAMPLES; i++)
   {
      #if defined(_MSC_VER) || defined(__WINDOWS__)
      sprintf_s(buffStr, "Normal BFL %d/52", DarvasStock::DARVAS_PRICE_SAMPLES_WEEKS[i]);
      #else
      snprintf(buffStr, 128, "Normal BFL %d/52", DarvasStock::DARVAS_PRICE_SAMPLES_WEEKS[i]);
      #endif
      fprintf(const_cast<FILE*>(_stream), "\t%20s", buffStr);
   }

   // Add a newline char
   fprintf(const_cast<FILE*>(_stream), "\n");
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
// Write a single line of output which contains this stocks Darvas information.
//
//--------------------------------------------------------------------------------------------------
void DarvasStock::writeCSVrow(const FILE* _stream, const DarvasStock* _ds)
{
   if (_ds == NULL)
   {
      return;
   }

   // Symbol and 52 week high info
   const std::vector<StockHistory_Week>* stkHist =  _ds->getHistoryPtr();
   Darvas_52WeekHigh hi = _ds->getHigh_52Weeks();

   StockHistory_Week stkWeek = stkHist->at(hi.weekOffset);
   StockHistory_Day* stkDay = stkWeek.getDayHistory(hi.dayOffset);

   std::string date52 = "N/A";
   float price52 = 0.0f;

   if (NULL != stkDay)
   {
      date52 = stkDay->getDate().toString2();
      price52 = stkDay->getHigh();
      delete stkDay;
      stkDay = NULL;
   }

   fprintf(const_cast<FILE*>(_stream), "%10s\t%20f\t%15s", _ds->getTicker().c_str(), price52, date52.c_str());

   // Volume
   for (int i = 0; i < DarvasStock::DARVAS_VOLUME_SAMPLES; i++)
   {
      fprintf(const_cast<FILE*>(_stream), "\t%20f", _ds->volumeSamples[i].ratio);
   }

   // Price
   for (int i = 0; i < DarvasStock::DARVAS_PRICE_SAMPLES; i++)
   {
      float ratio = _ds->priceSamples[i].slope / _ds->priceSamples[i].meanY;
      fprintf(const_cast<FILE*>(_stream), "\t%20f", ratio);
   }

   // Yahoo Finance Link
   fprintf(const_cast<FILE*>(_stream), "\thttp://finance.yahoo.com/echarts?s=%s+Interactive#{\"showArea\":false,\"showLine\":false,\"showOhlc\":true,\"lineType\":\"bar\",\"range\":\"3mo\",\"allowChartStacking\":true}", _ds->getTicker().c_str());

   // Add a newline char
   fprintf(const_cast<FILE*>(_stream), "\n");
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
void DarvasStock::printDataCSV(const char* _dir) const
{
   char file[512] = {0};
   FILE* fd = NULL;

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
// analyze52WeekHigh
//
//Arguments:
// void
//
//Output:
// Error message if failure
//
//Returns:
// bool - true on success, else false
//
//About:
// To be called after stock history is gathered. Steps through each week of stock history and
// determines when the 52 week high was. If more than 52 weeks are saved, will take the last,
// most current 52 weeks. Stores an offset to the StockHistory_Week and an offset to the day
// within that week.
//
//--------------------------------------------------------------------------------------------------
bool DarvasStock::analyze52WeekHigh(void)
{
   bool success = true;

   // Get a pointer to this stocks history
   const std::vector<StockHistory_Week>* stk = this->getHistoryPtr();

   if ((NULL != stk) || (0 > stk->size()))
   {
      // Determine starting offset. (We only want to analyze the last 52 weeks or less)
      unsigned int i = 0;
      if (52 < stk->size())
      {
         i = stk->size() - 52;
      }

      // Step through the history and determine the highest week
      float hi = 0.0f;
      int hiWeekOffset = 0;
      for ( ; i < stk->size(); i++)
      {
         float weekHi = stk->at(i).getHigh();
         if (weekHi >= hi)
         {
            hi = weekHi;
            hiWeekOffset = i;
         }
      }

      // Find the hi day offset of the hi week
      int hiDayOffset = 0;
      bool allDone = false;
      StockHistory_Week stkWeek = stk->at(hiWeekOffset);
      for (i = 0; i < 5; i++)
      {
         StockHistory_Day* stkDay = stkWeek.getDayHistory(i);
         if (NULL != stkDay)
         {
            if (stkDay->getHigh() == hi)
            {
               hiDayOffset = i;

               // Set the structure values
               this->high_52week.weekOffset = hiWeekOffset;
               this->high_52week.dayOffset = hiDayOffset;
               this->high_52week.copyOfDayHistory = *stkDay;

               allDone = true;
            }
            delete stkDay;
            stkDay = NULL;
         }
         if (allDone)
         {
            break;
         }
      }

   } else
   {
      fprintf(stderr, "DarvasStock::analyze52WeekHigh error. %s\n", this->getTicker().c_str());
      success = false;
   }

   return success;
}

//--------------------------------------------------------------------------------------------------
//Name:
// analyzeVolume
//
//Arguments:
// void
//
//Output:
// Error message if failure
//
//Returns:
// bool - true on success, else false
//
//About:
// To be called after stock history is gathered. Compares volume of 4 most recent weeks to the
// previous 48 weeks. This is done individually per week as to analyze any build up of volume over
// the course of a month.
//
//--------------------------------------------------------------------------------------------------
bool DarvasStock::analyzeVolume(void)
{
   bool success = true;

   // Get a pointer to this stocks history
   const std::vector<StockHistory_Week>* stk = this->getHistoryPtr();

   if ((NULL != stk) && (DARVAS_VOLUME_MIN_WEEKS / 2 < stk->size()))
   {
      unsigned long sumSampleA = 0;
      unsigned long sumSampleB = 0;
      unsigned int countA = 0;
      unsigned int countB = 0;
      float avgA = 0;
      float avgB = 0;

      // Get total day count and volume for up to 48 previous weeks.
      unsigned int i = 0;
      if (52 < stk->size())
      {
         i = stk->size() - 52;
      }
      for ( ; i < stk->size() - DARVAS_VOLUME_SAMPLES; i++)
      {
         sumSampleA += stk->at(i).getTotalVolume();
         countA += stk->at(i).getStockDays();
      }
      avgA = (float)sumSampleA / countA;

      // Now, for the comparison weeks, take the avg per the week, and divide it by
      // the average for the previous value found for the 48 weeks.
      for (int j = 0; j < DARVAS_VOLUME_SAMPLES; j++)
      {
         int stkOffset = stk->size() - j - 1;

         countB = stk->at(stkOffset).getStockDays();
         sumSampleB = stk->at(stkOffset).getTotalVolume();
         avgB = (float)sumSampleB / countB;

         volumeSamples[j].ratio = avgB / avgA;
      }
   } 
   else
   { // Assign worst possible values for stock scoring purpose.
      fprintf(stderr, "DarvasStock::analyzeVolume error. %s\n", this->getTicker().c_str());
      success = false;
   }

   return success;
}

//--------------------------------------------------------------------------------------------------
//Name:
// analyzePrice
//
//Arguments:
// void
//
//Output:
// Error message if failure
//
//Returns:
// bool - true on success, else false
//
//About:
//
//
//--------------------------------------------------------------------------------------------------
bool DarvasStock::analyzePrice(void)
{
   bool success = true;

   for (int i = 0; i < DARVAS_PRICE_SAMPLES; i++)
   {
      if (!(this->calculateBestFitLine(DARVAS_PRICE_SAMPLES_WEEKS[i], &(this->priceSamples[i]))))
      {
         success = false;
         break;
      }
   }

   return success;
}

//--------------------------------------------------------------------------------------------------
//Name:
// calculateBestFitLineSlope
//
//Arguments:
// _weeks - Number of weeks (taken from this stocks history, present to past)
// _dp    - Structure to hold the calculated results
//
//Output:
// Error message if failure
//
//Returns:
// boolean true if successful, else false
//
//About:
// Uses inherited stock history data to determine the best fit line given a certain number of weeks.
// The results are then stored in the passed in referenced Darvas_price struct.
//
// Linear Regression:
//
// X :: mean of x values
// Y :: mean of y values
//
// m = ( SUM( (x - X)(y - Y) ) / ( SUM( pow(x-X, 2) ) ) )
//
//--------------------------------------------------------------------------------------------------
bool DarvasStock::calculateBestFitLine(int _weeks, Darvas_Price* _dp)
{
   bool success = true;

   // Get a pointer to this stocks history
   const std::vector<StockHistory_Week>* stk =  this->getHistoryPtr();

   // Valid data?
   if ((NULL != _dp) && (NULL != stk) && (0 < stk->size()) && (0 < _weeks))
   {
      // Determine index boundaries we are working with
      int eIndex = stk->size() - 1;
      int sIndex = stk->size() - _weeks;
      if (sIndex < 0)
      {
         sIndex = 0;
      }

      // Allocate a buffer to hold the x,y values
      struct Darvas_PricePoint
      {
         int daysSince1970; //  x
         float closeValue; //  y
      };
      Darvas_PricePoint* buffer = new Darvas_PricePoint[(eIndex - sIndex + 1) * 5];

      double X = 0.0;
      double Y = 0.0;
      float m = 0.0f;

      SimpleDate January_01_1970(1970, 1, 1);
      SimpleDate tempDate; // Needed to pass in SimpleDate via pointer

      int count = 0;
      // Fill the buffer and calculate the means
      for (int i = sIndex; i <= eIndex; i++)
      {
         for (int j = 0; j < 5; j++)
         {
            StockHistory_Day* shd =
               (const_cast<std::vector<StockHistory_Week>*>(stk))->at(i).getDayHistory(j);
            if (NULL != shd)
            {
               buffer[count].closeValue = shd->getClose();
               tempDate = shd->getDate();
               buffer[count].daysSince1970 = January_01_1970.getDayDifference(&tempDate);
               X += buffer[count].daysSince1970;
               Y += buffer[count].closeValue;
               count++;
            }
            delete shd;
         }
      }
      X /= count;
      Y /= count;

      // Calculate the slope
      double a = 0.0;
      double b = 0.0;
      for (int i = 0; i < count; i++)
      {
         a += ( (buffer[i].daysSince1970 - X) * (buffer[i].closeValue - Y) );
         b += pow( (buffer[i].daysSince1970 - X) , 2);
      }

      // Write the values
      _dp->slope = static_cast<float>(a / b);
      _dp->meanY = static_cast<float>(Y);
      _dp->weeks = _weeks;

      // Cleanup
      delete [] buffer;
      buffer = NULL;

   }
   else
   {
      fprintf(stderr, "DarvasStock::calculateBestFitLine error. %s\n", this->getTicker().c_str());
      success = false;
   }

   return success;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
