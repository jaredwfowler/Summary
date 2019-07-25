//Dependencies======================================================================================
//==================================================================================================
#include "OptionStock_MarketWatch.hpp"
#include "HttpDownloader.hpp"
#include "StockMarket_NYSE.hpp"
#include <algorithm>

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// OptionStock_MarketWatch
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
// Default Constructor
//
//--------------------------------------------------------------------------------------------------
OptionStock_MarketWatch::OptionStock_MarketWatch(std::string _ticker, std::string _name)
   : OptionStock(_ticker, _name)
{
   // Void
}

//--------------------------------------------------------------------------------------------------
//Name:
// addOptions_MarketWatch
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
//
//
//--------------------------------------------------------------------------------------------------
bool OptionStock_MarketWatch::addOptions_MarketWatch(const HttpDownloader* _httpRequest)
{
   // Attempt to get this stocks options info
   std::vector<StockOption>* options = OptionStock_MarketWatch::getThisWeeksOptions_MarketWatch(this->getTicker(), _httpRequest);
   if (options == NULL)
   {
      options = OptionStock_MarketWatch::getThisWeeksOptions_MarketWatch(std::string(this->getTicker()), _httpRequest);

      if (options == NULL)
      {
         fprintf(stderr, "OptionStock_MarketWatch::addOptions_MarketWatch failed to get options for %s\n", this->getTicker().c_str());
         return false;
      }
   }

   // Sort values into vectors
   for (unsigned int i = 0; i < options->size(); i++)
   {
       if (options->at(i).getType() == StockOption::PUT)
       {
            this->insertPut(&options->at(i));
       }
       else
       {
            this->insertCall(&options->at(i));
       }
   }

   // Clean up
   delete options;
   options = NULL;

   // Sort
   this->sortCalls();
   this->sortPuts();

   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getThisWeeksOptions_MarketWatch
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// vector of StockOptions
//
//About:
// Attempt to get options data for the current week via fidelity.com.
//
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock_MarketWatch::getThisWeeksOptions_MarketWatch(std::string _ticker, const HttpDownloader* _httpRequest)
{
   std::vector <StockOption>* stk = NULL;

   std::string ticker = getAlternativeName(_ticker, SOURCE_FIDELITY);
   stk = getMarketWatchWeekOptions(ticker, _httpRequest);

   if (stk == NULL)
   {
      fprintf(stderr, "Failed to get options for %s with MarketWatch.\n", _ticker.c_str());
   }

   return stk;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getMarketWatchWeekOptions
//
//Arguments:
// _ticker -> stock symbol to look up in query
// _httpRequest -> HTTPS request handler.
//
//Output:
// void
//
//Returns:
// Vector of StockOptions. Returns NULL on fail.
//
//About:
// Given a ticker value and an initialized HttpDownloader, pull and parse the
// current weeks option values at current time.
//
// String Identifier           Notes
//
// id="options"                :: Start of table
// <tbody> ...
// CALLS ... PUTS
// <tr ...> ...
// <td ...>
// Expires June 30, 2017</td>  :: Expiration date of all options on page
// <tr ... >                   :: Skip this row. Is table columnn header
//
// <tr ... >                   :: Each row will start with this
// <td ... quotes ... </td>    :: Ensure that the word quotes exists in this row
//                                if it doesnt, this is a row separator
// <tr class="optiontoggle"    :: End of table (What we are concerned with.
// NOTE: Back up to previous </tr> and set this to end of table.
//
// Inside each table row example: (After the first td quote as explained above)
//
// <td ... >4.24</td>                     :: Last Price CALL
// <td ... >-0.51</td>                    :: Change CALL
// <td ... >4.00</td>                     :: Volume CALL
// <td ... >4.10</td>                     :: Bid CALL
// <td ... >4.45</td>                     :: Ask CALL
// <td ... >1,998</td>                    :: Open Int CALL
// <td ... >115</td>                      :: Strike CALL & PUT
// <td ... >???</td>                      :: quote .. Skip
// <td ... >0.02</td>                     :: Last Price PUT
// <td ... >0.00</td>                     :: Change PUT
// <td ... >4.00</td>                     :: Volume PUT
// <td ... >4.10</td>                     :: Bid PUT
// <td ... >4.45</td>                     :: Ask PUT
// <td ... >1,998</td>                    :: Open Int PUT
//
// NOTE: Open Init will sometimes just be left blank. <td ... ></td>
//
//
// This method is static to allow greater modular usage, and to ensure that the downloaded data
// is current. Errors in the download will result in no values being added to the class vectors,
// as this function, even if successful, returns a temporary vector to the results.
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock_MarketWatch::getMarketWatchWeekOptions(std::string _ticker, const HttpDownloader* _httpRequest)
{
   //Local variables
   std::vector<StockOption>* stk = NULL;
   StockOption* so = NULL;

   std::string returnStr;
   std::string optionTableStr;
   std::string tempStr;

   // Prepare https request
   char query[512] = { 0 };
#if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://www.marketwatch.com/investing/fund/%s/options",
      _ticker.c_str());
#else
   snprintf(query, 512, "http://www.marketwatch.com/investing/fund/%s/options",
      _ticker.c_str());
#endif

   // TODO
   // Unlike fidelity, we don't have the need to use the passed in httpRequest... Is there
   // some way to make this cleaner?
   // We don't have a need to use the passed in downloader...
   HttpDownloader fid;

   // Request option chain
   returnStr = fid.download(query);
   if (returnStr == "")
   { //Query failed
      return NULL;
   }

   // We are only interested in options closing at the end of this week. We have to deal with 
   // options which expire half way through the week, and weeks in which the market closes 
   // before Friday. Determine which day is friday, and find the first available date less than
   // or equal to that date.

   SimpleDate endOfWeekDay = SimpleDate::today();
   if (endOfWeekDay.getDayW() <= 6)
   {
      endOfWeekDay.addDays(6 - endOfWeekDay.getDayW());
   }
   else  // It's Saturday, so set it for next week friday
   {
      endOfWeekDay.addDays(6);
   }

   // Extract expiration date
   int s = 0;
   int s2 = 0;
   int e = 0;
   int e2 = 0;

   unsigned int year = 1;
   unsigned int day = 1;
   unsigned int month = 1;

   SimpleDate tempDate;
   unsigned int checked_for_midweek_expiration = 0;

   //fprintf(stderr, "%s\r\n", returnStr.c_str());

StockOPtion_FindStart:

   s = returnStr.find("id=\"options\"");
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1a\r\n");
      goto StockOption_MarketWatch_End;
   }

   s = returnStr.find("<table", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1b\r\n");
      goto StockOption_MarketWatch_End;
   }

StockOPtion_FindPUT:

   s = returnStr.find("PUTS", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1c\r\n");
      goto StockOption_MarketWatch_End;
   }
   s = returnStr.find("<tr", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1d\r\n");
      goto StockOption_MarketWatch_End;
   }
   s = returnStr.find("<td", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1e\r\n");
      goto StockOption_MarketWatch_End;
   }
   s = returnStr.find(">", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1f\r\n");
      goto StockOption_MarketWatch_End;
   }

   // Skip over the "Expires " string
   s += 9;

   // Month is fully written out...go until we hit a space
   e = returnStr.find(" ", s);
   if (e == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1g\r\n");
      goto StockOption_MarketWatch_End;
   }

   // Extract the month
   tempStr = returnStr.substr(s, e - s);
   month = SimpleDate::getMonth(tempStr);

   // Skip over the month and the space
   s = e + 1;

   // Day could be 1 or 2 characters...
   // Be sure to get rid of comma
   e = returnStr.find(" ", s);
   if (e == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1h\r\n");
      goto StockOption_MarketWatch_End;
   }
   tempStr = returnStr.substr(s, e - s - 1);
   try
   {
      day = std::stoi(tempStr.c_str());
   }
   catch (std::exception)
   {
      fprintf(stderr, "stoi fail 51a: %s\n", tempStr.c_str());
   }

   // Skip over the day, comma, and the space
   s = e + 1;

   // Year is 4 characters
   tempStr = returnStr.substr(s, 4);
   try
   {
      year = std::stoi(tempStr.c_str());
   }
   catch (std::exception)
   {
      fprintf(stderr, "stoi fail 51b: %s\n", tempStr.c_str());
   }

   // Compare this date to what we would expect for a normal week (close on Friday)
   // If the date is not Friday, attempt to look for another group of calls and puts
   // in the table. If there is one, than explore that possibility. If the date is
   // greater than Friday, we need to revert back to our first date..., so just 
   // restart the find process with a flag set.

   tempDate = SimpleDate(year, month, day);
   if (endOfWeekDay.getDayDifference(&tempDate) == 0)
   {
      // We are right on the money! No need to do any further work
   }
   else if (endOfWeekDay.getDayDifference(&tempDate) > 0 && checked_for_midweek_expiration)
   {
      // We have surpassed the end of week date and we need to back up. 
      goto StockOPtion_FindStart;
   }
   else if (endOfWeekDay.getDayDifference(&tempDate) > 0 && !checked_for_midweek_expiration)
   {
      // This is the first available option date, so we'll take it. It's very possible that 
      // today is Friday and there are no longer options available for today or this week!
   }
   else if (endOfWeekDay.getDayDifference(&tempDate) < 0 && !checked_for_midweek_expiration)
   {
      // It is possible that this is a mid-week closing option... 
     checked_for_midweek_expiration = 1;
     goto StockOPtion_FindPUT;
   }
   else if (endOfWeekDay.getDayDifference(&tempDate) < 0 && checked_for_midweek_expiration)
   {
      // We have already tried evaluating a mid-week option expiration.. so just continue
      // This could handle an edge case where expiration dates are on both Wed. and Thurs.
   }
   else
   {
      // This should never happen...
      fprintf(stderr, "MarketWatchOptions: Error 1.1y\r\n");
      goto StockOption_MarketWatch_End;
   }

   // Locate and skip the table label row
   s = returnStr.find("<tr", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1j\r\n");
      goto StockOption_MarketWatch_End;
   }
   s = returnStr.find("</tr>", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1k\r\n");
      goto StockOption_MarketWatch_End;
   }

   //Extract a substring which will only include this week's options

   s = returnStr.find("<tr", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.1m\r\n");
      goto StockOption_MarketWatch_End;
   }

   // END will be either at next set of options per next week or at end of list..
   // find out which and take whichever comes first

   e = returnStr.find("optiontoggle", s + 1);
   if (e == std::string::npos)
   {
      fprintf(stderr, "MarketWatchOptions: Bad return string 1.2\r\n");
      goto StockOption_MarketWatch_End;
   }
   e2 = returnStr.find("CALLS", s + 1);
   if ((e2 != std::string::npos) && (e2 < e))
   {
      e = e2;
   }

   // Now, find the last </tr> up to this end point
   s2 = s + 1;
   while (true)
   {
      s2 = returnStr.find("</tr>", s2 + 1);
      if ((s2 != std::string::npos) && (s2 < e))
      {
         e2 = s2;
      }
      else
      {
         break;
      }
   } 
   // e2 should now contain the end bound, so assign it to e
   e = e2;

   // Extract a substring from the downloaded file of the information we are 
   // going to be parsing.
   optionTableStr = returnStr.substr(s, e - s);

   // Allocate a new vector to hold the options
   stk = new std::vector<StockOption>();

   // Reset index holders
   s = e = 0;

   //fprintf(stdout, "%s\r\n", optionTableStr.c_str());

   // Parse options and store in vector
   for (unsigned int index = 1; index <= 128; index++)
   {
      // Look for the next <tr tag. If we can't find one, we are done.
      s = optionTableStr.find("<tr", s);
      if (s == std::string::npos)
      {
         break;
      }

      // Ensure that the "quote" link exists, but skip over this row once we 
      // determine that it does exist. MarketWatch will contain 1 row in the middle
      // which does not have this value... 

      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 2.01\r\n");
         goto StockOption_MarketWatch_End;
      }
      e = optionTableStr.find("</td>", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 2.02\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);

      if (tempStr.find("quote", 0) == std::string::npos)
      {
         // Skip this row
         continue;
      }

      // Extract Call Price
      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 2.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 2.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 2.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callLastPrice = 0.0f;
      try
      {
         callLastPrice = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 51: %s\n", tempStr.c_str());
      }

      // Extract Call Change
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 3.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 3.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 3.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callChange = 0.0f;
      try
      {
         callChange = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 52: %s\n", tempStr.c_str());
      }

      // Calculate Call % Change
      float callPercentChange = 0.0f;
      if ((callLastPrice - callChange) != 0) 
      {
         callPercentChange = (callLastPrice - (callLastPrice - callChange)) / (callLastPrice - callChange);
      }

      // Extract Call Volume
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos){
         fprintf(stderr, "MarketWatchOptions: Bad return string 6.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 6.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 6.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callVolume = 0.0f;
      try
      {
         callVolume = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 55: %s\n", tempStr.c_str());
      }

      // Extract Call Bid
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 4.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 4.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 4.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callBid = 0.0f;
      try
      {
         callBid = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 53: %s\n", tempStr.c_str());
      }

      // Extract Call Ask
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 5.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 5.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 5.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callAsk = 0.0f;
      try
      {
         callAsk = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 54: %s\n", tempStr.c_str());
      }

      // Extract Call Open Interest
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 7.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 7.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 7.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callOI = 0.0f;

      if (tempStr != "")  // Market watch will keep this empty if DNE
      {
         try
         {
            callOI = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 56: %s\n", tempStr.c_str());
         }
      }

      // Extract the Strike (Both Call/Put)
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 9.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 9.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 9.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float strike = 0.0f;
      try
      {
         strike = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 57: %s\n", tempStr.c_str());
      }

      // Skip quote link field
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 10.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      // Extract Put Price
      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 11.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 11.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 11.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putLastPrice = 0.0f;
      try
      {
         putLastPrice = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 58: %s\n", tempStr.c_str());
      }

      // Extract Put Change
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 12.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 12.2\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 12.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putChange = 0.0f;
      try
      {
         putChange = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 59: %s\n", tempStr.c_str());
      }

      // Calculate Put % Change
      float putPercentChange = 0.0f;
      if ((putLastPrice - putChange) != 0) 
      {
         putPercentChange = (putLastPrice - (putLastPrice - putChange)) / (putLastPrice - putChange);
      }

      //Extract Put Volume
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 15.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 15.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 15.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putVolume = 0.0f;
      try
      {
         putVolume = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 62: %s\n", tempStr.c_str());
      }

      // Extract Put Bid
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 13.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 13.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 13.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putBid = 0.0f;
      try
      {
         putBid = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 60: %s\n", tempStr.c_str());
      }

      // Extract Put Ask
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 14.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 14.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 14.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putAsk = 0.0f;
      try
      {
         putAsk = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 61: %s\n", tempStr.c_str());
      }

      // Extract Put Open Interest
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 16.1\r\n");
         goto StockOption_MarketWatch_End;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 16.3\r\n");
         goto StockOption_MarketWatch_End;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "MarketWatchOptions: Bad return string 16.4\r\n");
         goto StockOption_MarketWatch_End;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putOI = 0.0f;

      if (tempStr != "")  // Market watch will keep this empty if DNE
      {
         try
         {
            putOI = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 63: %s\n", tempStr.c_str());
         }
      }

      // Add this information to the vector
      so = new StockOption(StockOption::CALL, std::to_string(index), callLastPrice, callChange,
                              callPercentChange, callBid, callAsk, callOI, (unsigned long)callVolume, strike,
                              SimpleDate(year, month, day), SimpleDate::today());
      stk->push_back(*so);
      delete so;
      so = new StockOption(StockOption::PUT, std::to_string(index), putLastPrice, putChange,
                              putPercentChange, putBid, putAsk, putOI, (unsigned long)putVolume, strike,
                              SimpleDate(year, month, day), SimpleDate::today());
      stk->push_back(*so);
      delete so;

      // fprintf(stdout, "%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f STRIKE:%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\r\n", 
      //    callLastPrice, callChange, callVolume, callBid, callAsk, callOI, strike, 
      //    putLastPrice, putChange, putVolume, putBid, putAsk, putOI);
   }

   return stk;

StockOption_MarketWatch_End:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }

   return NULL;
}
