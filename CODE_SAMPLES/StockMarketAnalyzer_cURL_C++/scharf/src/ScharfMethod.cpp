//Dependencies======================================================================================
//==================================================================================================
#include "ScharfMethod.hpp"
#include "StockMarket_NYSE.hpp"
#include "HttpDownloader_Fidelity.hpp"
#include <iostream>
#include <algorithm>

//Method Definitions================================================================================
//==================================================================================================
std::mutex ScharfMethod::scharfMutex;  // Static mutex init
unsigned int ScharfMethod::totalJobs = 0;
unsigned int ScharfMethod::jobsDone = 0;
unsigned int ScharfMethod::printJobActive = 0;
std::mutex ScharfMethod::scharfFailureStrMutex;
std::string ScharfMethod::failureString = "";

//--------------------------------------------------------------------------------------------------
//Name:
// ScharfMethod
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
ScharfMethod::ScharfMethod(const std::vector<std::string>* _tickerList)
   : t(NULL), printThread(NULL)
{
   // Copy the stock symbol list
   this->optionSymbols = *(_tickerList);

   // Prepare an http request to pass down
   this->authenticatedHttpRequest = new HttpDownloader();

   // Start date will be one year ago, at the start of the week.
   // Start by getting todays date.
   this->endDate = SimpleDate::today();
   this->startDate = SimpleDate(this->endDate.getYear() - 1, this->endDate.getMonth(), this->endDate.getDayM());
   this->startDate = this->startDate.addDays(2 - this->startDate.getDayW());

   // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
   // Get last weeks closing... this is a bit tricky, and I'm not going to do this
   // right, for now. TODO.  Sometimes the market will be closed on Friday, and
   // sometimes the market will close early on Friday. A future solutions might be
   // to download some option info and determine these factors based on the
   // available dates. A better method would be reading a file which is formatted to
   // include special dates and closed dates and half-day info. This file would have
   // to be updated on a yearly basis.
   // For now, let's assume that the market closed on Friday, 4pm NY time.

   // Days are numbered 1-7 (Sunday - Saturday)
   // Is it before Friday?
   if (this->endDate.getDayW() < 6) 
   {
      this->lastWeekCloseDate = this->endDate.addDays(-1 * (int)(this->endDate.getDayW() + 1));
   }
   // Is today Friday?
   else if (this->endDate.getDayW() == 6)
   {
      // Get the current time in New York City. (NYSE)
      time_t t = StockMarket_NYSE::getNewYorkTime();
      if (t == 0) 
      { 
         // If failed to get time....
         fprintf(stderr, "Check INTERNET connection and try again.\n");
         exit(1);
      }
      struct tm * timeNY = gmtime(&t);
      if ((timeNY->tm_hour >= 16) || (timeNY->tm_wday == 6)) 
      {
         this->lastWeekCloseDate = this->endDate;
      }
      else 
      {
         this->lastWeekCloseDate = this->endDate.addDays(-7);
      }
   }
   // It's Saturday
   else 
   {
      this->lastWeekCloseDate = this->endDate.addDays(-1);
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~ScharfMethod
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
// Destructor
//
//--------------------------------------------------------------------------------------------------
ScharfMethod::~ScharfMethod() 
{
   delete this->authenticatedHttpRequest;
   this->authenticatedHttpRequest = NULL;
}

//--------------------------------------------------------------------------------------------------
//Name:
// processOptions
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
void ScharfMethod::processOptions() 
{
   // Set the number of jobs to do
   totalJobs = optionSymbols.size();
   jobsDone = 0;

   // Start the print thread
   printJobActive = 1;
   printThread = new std::thread(ScharfMethod::printPercentDone_thread);

   // Iterate through, getting the options data and analyzing
   t = new std::thread[ScharfMethod::SCHARF_THREADS];
   for (unsigned int i = 0; i < optionSymbols.size(); i += ScharfMethod::SCHARF_THREADS) 
   {

      int jobs = optionSymbols.size() - i - ScharfMethod::SCHARF_THREADS;

      if (jobs >= 0) 
      {
         jobs = ScharfMethod::SCHARF_THREADS;
      } 
      else 
      {
         jobs = ScharfMethod::SCHARF_THREADS + jobs;
      }

      for (int j = 0; j < jobs; j++) 
      {
         t[j] = std::thread(ScharfMethod::downloadParseStoreStockInfo_thread,
               this->optionSymbols.at(i + j), &(this->optionsList), this->startDate,
               this->endDate, this->lastWeekCloseDate, this->authenticatedHttpRequest);
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
   // delete[] t;

   fprintf(stdout, "\nDownload/Parse DONE.\n");

   // Sort the list by symbols.
   std::sort(this->optionsList.begin(), this->optionsList.end(), ScharfOptionStock::compareSymbols);
   fprintf(stdout, "Sorting DONE.\n");
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
// and options tables to a specified directory. (We create the files) Write results will print
// the results of the Scharf method to the provided stream.
//
//--------------------------------------------------------------------------------------------------
void ScharfMethod::writeCSVdata(const char* _dir) const 
{
   for (unsigned int i = 0; i < this->optionsList.size(); i++) 
   {
      this->optionsList.at(i).printDataCSV(_dir);
   }
}

void ScharfMethod::writeCSVresultsSimpleFiltered(const FILE* _stream) const 
{
   // Print out common values for every stock
   fprintf(const_cast<FILE*>(_stream), "  History Start,%15s\n", this->startDate.toString2().c_str());
   fprintf(const_cast<FILE*>(_stream), "    History End,%15s\n", this->endDate.toString2().c_str());
   fprintf(const_cast<FILE*>(_stream), "Last Close Date,%15s\n", this->lastWeekCloseDate.toString2().c_str());

   // Print out the key value row
   ScharfOptionStock::writeCSVkeySimple(_stream);

   for (unsigned int i = 0; i < this->optionsList.size(); i++) 
   {
      if (outputFilterSimple(i)) 
      {
         ScharfOptionStock::writeCSVrowSimple(_stream, &(this->optionsList.at(i)));
      }
   }
}

void ScharfMethod::writeCSVresults(const FILE* _stream) const 
{
   // Print out common values for every stock
   fprintf(const_cast<FILE*>(_stream), "  History Start,%15s\n", this->startDate.toString2().c_str());
   fprintf(const_cast<FILE*>(_stream), "    History End,%15s\n", this->endDate.toString2().c_str());
   fprintf(const_cast<FILE*>(_stream), "Last Close Date,%15s\n", this->lastWeekCloseDate.toString2().c_str());

   // Print out the key value row
   ScharfOptionStock::writeCSVkey(_stream);

   for (unsigned int i = 0; i < this->optionsList.size(); i++) 
   {
      if (outputFilter(i)) 
      {
         ScharfOptionStock::writeCSVrow(_stream, &(this->optionsList.at(i)));
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
void ScharfMethod::downloadParseStoreStockInfo_thread(std::string _str,
   std::vector<ScharfOptionStock>* _ptr, SimpleDate _start, SimpleDate _end,
   SimpleDate _lastClosing, const HttpDownloader* _request)
{
   ScharfOptionStock a(_str, _str);
   int errorCode = a.processAnalysis(_start, _end, _lastClosing, _request);

   if (ScharfOptionStock::ERR_NONE == errorCode) 
   {
      // B/C this is multi-threaded, and we are using a ptr, we need to use a mutex
      ScharfMethod::scharfMutex.lock();
      _ptr->push_back(a);
      ScharfMethod::scharfMutex.unlock();
   } 
   else 
   {
      ScharfMethod::scharfFailureStrMutex.lock();
      ScharfMethod::failureString += a.getTicker() + "," + std::to_string(errorCode) + "\n";
      ScharfMethod::scharfFailureStrMutex.unlock();
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
// Thread function which collaborates with ScharfMethod Class to determine how many jobs
// are left and done, and determines/prints out a percent done.
//
//--------------------------------------------------------------------------------------------------
void ScharfMethod::printPercentDone_thread()
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
bool ScharfMethod::outputFilter(int _i) const 
{
   bool allGood = true;

   if ((this->optionsList.at(_i).getPutRatio() < 0.0f) ||
       (this->optionsList.at(_i).getCallRatio() < 0.0f))
   { // Filter 1: If ratio < 0, error in dwnld
      return false;
   }

   return allGood;
}

bool ScharfMethod::outputFilterSimple(int _i) const 
{

   const float GUY_FACTOR = 20.0f;
   bool allGood = true;

   if ((this->optionsList.at(_i).getPutRatio() < 0.0f) ||
      (this->optionsList.at(_i).getCallRatio() < 0.0f)) 
   { // Filter 1: If ratio < 0, error in dwnld
      allGood = false;
   }
   else if (this->optionsList.at(_i).getClosestStrikePut().getStrike() > (this->optionsList.at(_i).getClosestStrikePut().getBid() * 100 * GUY_FACTOR)) 
   {
      allGood = false;
   }

   return allGood;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
