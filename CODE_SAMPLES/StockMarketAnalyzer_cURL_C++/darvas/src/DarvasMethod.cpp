//Dependencies======================================================================================
//==================================================================================================
#include "DarvasMethod.hpp"
#include <iostream>
#include <stdlib.h>
#include <algorithm>

//Method Definitions================================================================================
//==================================================================================================
std::mutex DarvasMethod::darvasMutex;
unsigned int DarvasMethod::totalJobs = 0;
unsigned int DarvasMethod::jobsDone = 0;
unsigned int DarvasMethod::printJobActive = 0;
std::mutex DarvasMethod::darvasFailureStrMutex;
std::string DarvasMethod::failureString = "";

//--------------------------------------------------------------------------------------------------
//Name:
// DarvasMethod
//
//Arguments:
// _inputFile - List of stock symbols, 1 per line, which are to be processed.
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Reads the input file, determines common needed info, and processes
//
//--------------------------------------------------------------------------------------------------
DarvasMethod::DarvasMethod(const std::vector<std::string>* _tickerList)
   : t(NULL), printThread(NULL)
{
   // Copy the list of stocks
   this->stockSymbols = *(_tickerList);

   // Start date will be one year ago, at the start of the week.
   // Start by getting todays date.
   this->endDate = SimpleDate::today();
   this->startDate = SimpleDate(this->endDate.getYear() - 1, this->endDate.getMonth(), this->endDate.getDayM());
   this->startDate = this->startDate.addDays(2 - this->startDate.getDayW());
}

//--------------------------------------------------------------------------------------------------
//Name:
// writeCSV[data,results]
//
//Arguments:
//
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Print methods to be called by the user of this object. Write data will output both history
// and Darvas tables to a specified directory. (We create the files) Write results will print
// the results of the Darvas method to the provided stream.
//
//--------------------------------------------------------------------------------------------------
void DarvasMethod::writeCSVdata(const char* _dir) const 
{
   for (unsigned int i = 0; i < this->darvasStockList.size(); i++)
   {
      this->darvasStockList.at(i).printDataCSV(_dir);
   }
}

void DarvasMethod::writeCSVresults(const FILE* _stream) const 
{
   // Print out common values for every stock
   fprintf(const_cast<FILE*>(_stream), "Relative Start Date:\t%15s\n", this->startDate.toString2().c_str());

   // Print out the key value row
   DarvasStock::writeCSVkey(_stream);

   for (unsigned int i = 0; i < this->darvasStockList.size(); i++)
   {
      if (outputFilter(i)) 
      {
         DarvasStock::writeCSVrow(_stream, &(this->darvasStockList.at(i)));
      }
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// downloadParseStoreStockInfo_thread
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
// Threaded function. Gets stock info, and adds it to this vector
//
//--------------------------------------------------------------------------------------------------
void DarvasMethod::downloadParseStoreStockInfo_thread(std::string _str,
   std::vector<DarvasStock>* _ptr, SimpleDate _start, SimpleDate _end) 
{

   DarvasStock a(_str, _str);
   int errorCode = a.processAnalysis(_start, _end);

   if (DarvasStock::ERR_NONE == errorCode) 
   {
      // B/C this is multi-threaded, and we are using a ptr, we need to use a mutex
      DarvasMethod::darvasMutex.lock();
      _ptr->push_back(a);
      DarvasMethod::darvasMutex.unlock();
   } 
   else 
   {
      DarvasMethod::darvasFailureStrMutex.lock();
      DarvasMethod::failureString += a.getTicker() + "," + std::to_string(errorCode) + "\n";
      DarvasMethod::darvasFailureStrMutex.unlock();
   }

   jobsDone++;
}

//--------------------------------------------------------------------------------------------------
//Name:
// printPercentDone_thread
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
// Thread function which collaborates with DarvasMethod Class to determine how many jobs
// are left and done, and determines/prints out a percent done.
//
//--------------------------------------------------------------------------------------------------
void DarvasMethod::printPercentDone_thread()
{
   while (printJobActive) 
   {
      fprintf(stdout, "Working: %5.2f%%\r", ((float)jobsDone * 100 / totalJobs));
      std::this_thread::sleep_for (std::chrono::milliseconds(25));
   }
   fprintf(stdout, "Working: %5.2f%%\r", 100.00f);
}

//--------------------------------------------------------------------------------------------------
//Name:
// outputFilter
//
//Arguments:
// _i -> offset in this vector
//
//Output:
// void
//
//Returns:
// true if filter doesn't catch anything
//
//About:
// We can set up a series of filters here.
//
//--------------------------------------------------------------------------------------------------
bool DarvasMethod::outputFilter(int _i) const 
{
   bool passesFilter = true;

   // Volume Filter
   const float VOLUME_FLUX = 2.5f;
   std::vector<DarvasStock::Darvas_Volume>* volumeVec =
      this->darvasStockList.at(_i).getCopyOfVolumeSamples();
   if (volumeVec->at(0).ratio < 2.5 && volumeVec->at(1).ratio < 2.5) 
   {
      passesFilter = false;
   }
   delete volumeVec;
   volumeVec = NULL;

   // Price Filter
   std::vector<DarvasStock::Darvas_Price>* priceVec =
      this->darvasStockList.at(_i).getCopyOfPriceSamples();
   if (priceVec->at(0).slope < 0 && priceVec->at(1).slope < 0)
   {
      passesFilter = false;
   }
   delete priceVec;
   priceVec = NULL;

   // 52 Week High Filter
   const int DAYS_WITHIN_52_WEEK_HIGH = 15;
   DarvasStock::Darvas_52WeekHigh yearHigh = this->darvasStockList.at(_i).getHigh_52Weeks();
   SimpleDate yearHighDate = yearHigh.copyOfDayHistory.getDate();
   SimpleDate todaysDate = SimpleDate::today();
   int daysSince52WeekHigh = yearHighDate.getDayDifference(&todaysDate);
   daysSince52WeekHigh = std::abs(daysSince52WeekHigh);
   if (daysSince52WeekHigh > DAYS_WITHIN_52_WEEK_HIGH)
   {
      passesFilter = false;
   }

   return passesFilter;
}

//--------------------------------------------------------------------------------------------------
//Name:
// processStocks
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Creates a series of threads to handle the downloading of stock information and parsing of it.
// We use multiple threads b/c the time it takes to download is our biggest bottleneck.
//
//--------------------------------------------------------------------------------------------------
void DarvasMethod::processDarvasStocks()
{
   // Set the number of jobs to do
   totalJobs = stockSymbols.size();
   jobsDone = 0;

   // Start the print thread
   printJobActive = 1;
   printThread = new std::thread(DarvasMethod::printPercentDone_thread);

   // Iterate through, getting the options data and analyzing
   t = new std::thread[DarvasMethod::DARVAS_THREADS];
   for (unsigned int i = 0; i < stockSymbols.size(); i += DarvasMethod::DARVAS_THREADS)
   {
      int jobs = stockSymbols.size() - i - DarvasMethod::DARVAS_THREADS;

      if (jobs >= 0)
      {
         jobs = DarvasMethod::DARVAS_THREADS;
      }
      else
      {
         jobs = DarvasMethod::DARVAS_THREADS + jobs;
      }

      for (int j = 0; j < jobs; j++)
      {
         t[j] = std::thread(DarvasMethod::downloadParseStoreStockInfo_thread,
               this->stockSymbols.at(i + j), &(this->darvasStockList), this->startDate,
               this->endDate);
      }

      for (int j = 0; j < jobs; j++)
      {
         t[j].join();

      }
   }

   // Stop the print thread
   printJobActive = 0;
   printThread->join();

   // Clean up
   delete printThread;
   delete[] t;

   fprintf(stdout, "\nDownload/Parse DONE.\n");

   // Sort the list by symbols.
   std::sort(this->darvasStockList.begin(), this->darvasStockList.end(), DarvasStock::compareSymbols);
   fprintf(stdout, "Sorting DONE.\n");
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
