//Dependencies======================================================================================
//==================================================================================================
#include "OptionStock.hpp"
#include <algorithm>
#include "HttpDownloader.hpp"

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// OptionStock
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
OptionStock::OptionStock(std::string _ticker, std::string _name)
   : Stock(_ticker, _name)
{
   // Void
}

//--------------------------------------------------------------------------------------------------
//Name:
// clear[calls/puts]
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// true if successful
//
//About:
// Clears this classes vectors of either calls or puts
//
//--------------------------------------------------------------------------------------------------
bool OptionStock::clearCalls(void)
{
   this->calls.clear();
   return true;
}

bool OptionStock::clearPuts(void)
{
   this->puts.clear();
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
//
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
bool OptionStock::addOptions(void)
{
   // Attempt to get this stocks options info
   std::vector<StockOption>* options = OptionStock::getThisWeeksOptions(this->getTicker());
   if (options == NULL)
   {
       fprintf(stderr, "OptionStock::addOptions failed to get options for %s\n",
                                                                         this->getTicker().c_str());
       return false;
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
// insertCall
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
// If the option doesn't already exist, inserts a copy of it into our vector.
//
//--------------------------------------------------------------------------------------------------
bool OptionStock::insertCall(const StockOption* stk)
{
   // Valid input object?
   if (stk == NULL)
   {
      return false;
   }

   // Does the option already exist?
   for (size_t i = 0; i < this->calls.size(); i++)
   {
      if (this->calls[i] == *stk)
      {
         return false;
      }
   }

   // Else, insert new
   this->calls.push_back(StockOption(*stk));
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// insertPut
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// true upon success
//
//About:
// If the option doesn't already exist, inserts a copy of it into our vector.
//
//--------------------------------------------------------------------------------------------------
bool OptionStock::insertPut(const StockOption* stk)
{
   // Valid input object?
   if (stk == NULL)
   {
      return false;
   }

   // Does the option already exist?
   for (size_t i = 0; i < this->puts.size(); i++)
   {
      if (this->puts[i] == *stk)
      {
         return false;
      }
   }

   // Else, insert new
   this->puts.push_back(StockOption(*stk));
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// sort[Calls/Puts]
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
// ...
//
//--------------------------------------------------------------------------------------------------
void OptionStock::sortCalls(void)
{
   std::sort(this->calls.begin(), this->calls.end());
}

void OptionStock::sortPuts(void)
{
   std::sort(this->puts.begin(), this->puts.end());
}

//--------------------------------------------------------------------------------------------------
//Name:
// options*Output
//
//Arguments:
// _stream -> output FILE stream
// *_vec   -> List of StockOptions to be printed out
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Outputs option information to stream
//
//--------------------------------------------------------------------------------------------------
void OptionStock::optionsTableOutput(const FILE* _stream, const std::vector<StockOption>* _vec)
{
   // Output the key
   fprintf(const_cast<FILE*>(_stream), "    Expiration |   Strike |    Price |      Ask |      Bid |        Volume |       oi |   change | change %% |     Type \n");
   fprintf(const_cast<FILE*>(_stream), "-----------------------------------------------------------------------------------------------------------------------\n");

   // Output the data
   for (unsigned int i = 0; i < _vec->size(); i++){
      fprintf(const_cast<FILE*>(_stream), "%15s %10.2f %10.2f %10.2f %10.2f %15lu %10.2f %10.2f %10.2f %10u\n",
         _vec->at(i).getExpiration().toString2().c_str(), _vec->at(i).getStrike(),
         _vec->at(i).getPrice(), _vec->at(i).getAsk(), _vec->at(i).getBid(), _vec->at(i).getVolume(),
         _vec->at(i).getOpenInterest(), _vec->at(i).getChange(), _vec->at(i).getChangePercent(), (int)_vec->at(i).getType());
   }
}

void OptionStock::optionsCSVOutput(const FILE* _stream, const std::vector<StockOption>* _vec)
{
   // Output the key
   fprintf(const_cast<FILE*>(_stream), "     Expiration,    Strike,     Price,       Ask,       Bid,         Volume,        oi,    change,  change %%\n");

   // Output the data
   for (unsigned int i = 0; i < _vec->size(); i++)
   {
      fprintf(const_cast<FILE*>(_stream), "%15s,%10.2f,%10.2f,%10.2f,%10.2f,%15lu,%10.2f,%10.2f,%10.2f\n",
         _vec->at(i).getExpiration().toString2().c_str(), _vec->at(i).getStrike(),
         _vec->at(i).getPrice(), _vec->at(i).getAsk(), _vec->at(i).getBid(), _vec->at(i).getVolume(),
         _vec->at(i).getOpenInterest(), _vec->at(i).getChange(), _vec->at(i).getChangePercent());
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// getThisWeeksOptions
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
// Attempt to get options data for the current week via google finance. If fails, try yahoo finance.
// We'll want to use google finance first b/c it actually provides csv tables. We have to page scrape
// for yahoo. Yahoo, unfortunately, has some stocks that google doesn't.
//
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock::getThisWeeksOptions(std::string _ticker)
{
   std::vector <StockOption>* stk = NULL;
   stk = getGoogleFinanceWeekOptions(_ticker);
   if(stk == NULL)
   {
      fprintf(stderr, "Failed to get options for %s with GoogleFinance. Trying Yahoo.\n", _ticker.c_str());
      //stk = getYahooFinanceWeekOptions(_ticker);
   }

   return stk;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getGoogleFinanceWeekOptions
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// Vector of StockOptions. Returns NULL on fail.
//
//About:
// Given a ticker value, pull the latest option values from google finances.
// Request format: http://www.google.com/finance/option_chain?q=<symbol>&output=json
// Return format: json object
// ....puts:[{...},{...}, ... ],calls:[{...},{...}, ... ]
// s:"..." (contract name)
// p:"..." (price)
// c:"..." (change)
// cp:"..." (change percent)
// b:"..." (bid)
// a:"..." (ask)
// oi:"..." (open interest)
// vol:"..." (volume)
// strike:"..." (strike)
// expiry:"..." (expiration :: MMM D[D], YYYY
//
// This method is static to allow greater modular usage, and to ensure that the downloaded data
// is corrent. Errors in the download will result in no values being added to the class vectors,
// as this function, even if successful, returns a temporary vector to the results.
//
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock::getGoogleFinanceWeekOptions(std::string _ticker)
{
   // Local variables
   std::vector<StockOption>* stk = NULL;
   StockOption* so = NULL;

   HttpDownloader request;
   std::string returnStr;
   std::string optionInfo;
   std::string tempStr;
   std::string tempStr2;

   // Prepare http request
   char query[512] = { 0 };
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://www.google.com/finance/option_chain?q=%s&output=json",
             _ticker.c_str());
   #else
   snprintf(query, 512, "http://www.google.com/finance/option_chain?q=%s&output=json",
            _ticker.c_str());
   #endif

   // Request option chain
   returnStr = request.download(query);
   if (returnStr == "")
   { 
      // Query failed
      return NULL;
   }

   stk = new std::vector<StockOption>();  // Allocate a new vector to hold the options

   // Parse options and store in vector
   for (unsigned int index = 0; index < 2; index++)
   {
      StockOption::OPTION_TYPE optionType;
      int i = 0;
      if (index == 0)
      {
         // PUTS
         optionType = StockOption::PUT;
         // Find the start of the puts list
         i = returnStr.find("puts:");
         if (i == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 1\r\n");
            goto StockOption_google_end;
         }
         i += 6; // Skip the next 6 chars : 'p' 'u' 't' 's' ':' '['
      }
      else
      {
         // CALLS
         optionType = StockOption::CALL;
         // Find the start of the calls list
         i = returnStr.find("calls:");
         if (i == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 1.1\r\n");
            goto StockOption_google_end;
         }
         i += 7; // Skip the next 6 chars : 'c' 'a' 'l' 'l' 's' ':' '['
      }

      // Parse through the rest of the list until we hit the ']' character
      unsigned int end = returnStr.find(']', i);
      if (end == std::string::npos)
      {
         fprintf(stderr, "googleOptions: Bad return string 2\r\n");
         goto StockOption_google_end;
      }
      while ((unsigned int)i < end)
      {
         // Get the next optionInfo
         unsigned int s = returnStr.find('{', i);
         unsigned int e = returnStr.find('}', i);
         if ((s == std::string::npos) || (e == std::string::npos))
         {
            fprintf(stderr, "googleOptions: Bad return string 3\r\n");
            goto StockOption_google_end;
         }
         i = e + 1; // Update i
         optionInfo = returnStr.substr(s + 1, e - s - 1);
         unsigned int s2, e2;
         // contractName
         s2 = optionInfo.find("s:");
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 4\r\n");
            goto StockOption_google_end;
         }
         s2 += 3; // Skip the 's' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 4.2\r\n");
            goto StockOption_google_end;
         }
         std::string contractName = optionInfo.substr(s2, e2 - s2);
         // price
         s2 = optionInfo.find("p:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 5\r\n");
            goto StockOption_google_end;
         }
         s2 += 3; // Skip the 'p' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 5.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float price = 0.0;
         if (tempStr != "-")
         {
            try
            {
               price = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 20: %s\n", tempStr.c_str());
               getchar();
            }
         }
         // change and changePercent
         s2 = optionInfo.find("c:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 6\r\n");
            goto StockOption_google_end;
         }
         s2 += 3; // Skip the 'c' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 6.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float change = 0.0;
         float changePercent = 0.0;
         if (tempStr != "-")
         {
            try
            {
               change = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 21: %s\n", tempStr.c_str());
            }
            // change percent -- only shows up if change has a value
            s2 = optionInfo.find("cp:", e2);
            if (s2 == std::string::npos)
            {
               fprintf(stderr, "googleOptions: Bad return string 7\r\n");
               goto StockOption_google_end;
            }
            s2 += 4; // Skip the 'c' 'p' ':' '"'
            e2 = optionInfo.find('\"', s2);
            if (s2 == std::string::npos)
            {
               fprintf(stderr, "googleOptions: Bad return string 7.2\r\n");
               goto StockOption_google_end;
            }
            tempStr = optionInfo.substr(s2, e2 - s2);
            if (tempStr != "-")
            {
               try
               {
                  changePercent = std::stof (tempStr.c_str());
               }
               catch(std::exception)
               {
                  fprintf(stderr, "stof fail 22: %s\n", tempStr.c_str());
               }
            }
         }
         // bid
         s2 = optionInfo.find("b:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 8\r\n");
            goto StockOption_google_end;
         }
         s2 += 3; // Skip the 'b' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 8.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float bid = 0.0;
         if (tempStr != "-")
         {
            try
            {
               bid = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 23: %s\n", tempStr.c_str());
            }
         }
         // ask
         s2 = optionInfo.find("a:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 9\r\n");
            goto StockOption_google_end;
         }
         s2 += 3; // Skip the 'a' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 9.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float ask = 0.0;
         if (tempStr != "-")
         {
            try
            {
               ask = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 24: %s\n", tempStr.c_str());
            }
         }
         // open interest
         s2 = optionInfo.find("oi:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 10\r\n");
            goto StockOption_google_end;
         }
         s2 += 4; // Skip the 'o' 'i' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 10.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float openInterest = 0.0;
         if (tempStr != "-")
         {
            try
            {
               openInterest = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 25: %s\n", tempStr.c_str());
            }
         }
         // volume
         s2 = optionInfo.find("vol:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 11\r\n");
            goto StockOption_google_end;
         }
         s2 += 5; // Skip the 'v' 'o' 'l' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 11.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         unsigned long volume = 0;
         if (tempStr != "-")
         {
            try
            {
               volume = std::stoul (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 26: %s\n", tempStr.c_str());
            }
         }
         // strike
         s2 = optionInfo.find("strike:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 12\r\n");
            goto StockOption_google_end;
         }
         s2 += 8; // Skip the 's' 't' 'r' 'i' 'k' 'e' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 12.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         float strike = 0.0;
         if (tempStr != "-")
         {
            try
            {
               strike = std::stof (tempStr.c_str());
            }
            catch(std::exception)
            {
               fprintf(stderr, "stof fail 27: %s\n", tempStr.c_str());
            }
         }
         // Expiration
         s2 = optionInfo.find("expiry:", e2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 13\r\n");
            goto StockOption_google_end;
         }
         s2 += 8; // Skip the 'e' 'x' 'p' 'i' 'r' 'y' ':' '"'
         e2 = optionInfo.find('\"', s2);
         if (s2 == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 13.2\r\n");
            goto StockOption_google_end;
         }
         tempStr = optionInfo.substr(s2, e2 - s2);
         // We'll have to break the date down into MMM, D[D], YYYY
         tempStr2 = tempStr.substr(0, 3);
         unsigned int month = SimpleDate::getMonth(tempStr2);
         unsigned int dateComma = tempStr.find(',');
         if (dateComma == std::string::npos)
         {
            fprintf(stderr, "googleOptions: Bad return string 13.3\r\n");
            goto StockOption_google_end;
         }
         tempStr2 = tempStr.substr(4, dateComma - 4); // 4 skips the 3 letter abbreviation and space
         unsigned int day = atoi(tempStr2.c_str());
         tempStr2 = tempStr.substr(tempStr.length() - 4, 4);
         unsigned int year = atoi(tempStr2.c_str());

         // Create a new option and add it to the vector, then clean up.
         so = new StockOption(optionType, contractName, price, change,
                              changePercent, bid, ask, openInterest, volume, strike,
                              SimpleDate(year, month, day), SimpleDate::today());
         stk->push_back(*so);
         delete so;
      }
   }

   return stk;

StockOption_google_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }

   return NULL;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getYahooFinanceWeekOptions
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// Vector of StockOptions. Retuns NULL on fail.
//
//About:
// Yahoo doesn't provide a nice way like google does to get stock option data, however, yahoo
// has a more extensive collection of stock info than google. We page scrape an html result from
// yahoo. There are no particular key indicators to what we should look for in the html, so the
// scrape is shaky at best. May change if yahoo developers change look of page.
//
// This method is static to allow greater modular usage, and to ensure that the downloaded data
// is current. Errors in the download will result in no values being added to the class vectors,
// as this function, even if successful, returns a temporary vector to the results.
//
// NOTE: Yahoo updated site to only download JS and data is then imported... Page scraping is much
// more difficult and no longer an option.. Should probably use a service like:
// http://www.activetick.com/activetick/contents/PersonalServicesDataAPIPricing.aspx
//
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock::getYahooFinanceWeekOptions(std::string _ticker) 
{

   // Local variables
   std::vector<StockOption>* stk = NULL;
   StockOption* so = NULL;

   HttpDownloader request;
   std::string returnStr;
   std::string optionInfo;
   std::string tempStr;
   std::string tempStr2;

   // Prepare http request
   char query[512] = { 0 };
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://finance.yahoo.com/q/op?s=%s+Options", _ticker.c_str());
   #else
   snprintf(query, 512, "http://finance.yahoo.com/q/op?s=%s+Options", _ticker.c_str());
   #endif

   // Request option chain
   returnStr = request.download(query);
   if (returnStr == "") 
   { 
      // Query failed
      return NULL;
   }

   stk = new std::vector<StockOption>();  // Allocate a new vector to hold the options
   int i, j;
   unsigned int day, month, year;
   int callStart, putStart, callEnd, putEnd;

   // Find the start and end of the calls and puts tables:
   callStart = returnStr.find("<tr class=\"data-row0");
   if (callStart == std::string::npos) 
   {
      fprintf(stderr, "yahooOptions : Bad return string 2\r\n");
      goto StockOption_yahoo_end;
   }
   callEnd = returnStr.find("</tbody>", callStart + 1);
   if (callEnd == std::string::npos) 
   {
      fprintf(stderr, "yahooOptions : Bad return string 2.1\r\n");
      goto StockOption_yahoo_end;
   }

   putStart = returnStr.find("<tr class=\"data-row0", callEnd + 1);
   if (putStart == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 2.2\r\n");
      goto StockOption_yahoo_end;
   }
   putEnd = returnStr.find("</tbody>", putStart + 1);
   if (putEnd == std::string::npos) 
   {
      fprintf(stderr, "yahooOptions : Bad return string 2.3\r\n");
      goto StockOption_yahoo_end;
   }

#if 0
   // Get the Expiration Date (Will be same for all options)
   i = returnStr.find("date=");
   if (i == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1\r\n");
      goto StockOption_yahoo_end;
   }
   i = returnStr.find('>', i);
   if (i == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.1\r\n");
      goto StockOption_yahoo_end;
   }
   j = returnStr.find('<', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.2\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr = returnStr.substr(i + 1, j - i - 1);
   // Extract the Month
   i = 0;
   j = tempStr.find(' ', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.3\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr2 = tempStr.substr(i, j - i);
   month = SimpleDate::getMonth(tempStr2);
   // Extract the Day
   i = j + 1;
   j = tempStr.find(',', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.4\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr2 = tempStr.substr(i, j - i);
   day = atoi(tempStr2.c_str());
   // Extract the Year
   i = j + 2;
   tempStr2 = tempStr.substr(i, tempStr.length() - i);
   year = atoi(tempStr2.c_str());

#endif

   // Parse options and store in vector
   for (unsigned int index = 0; index < 2; index++) 
   {
      StockOption::OPTION_TYPE optionType;
      std::string optionString;

      // Set the option type and get a substring which contains the table we want
      if (index == 0) 
      {
         optionType = StockOption::CALL;
         optionString = returnStr.substr(callStart, callEnd - callStart);
      } else 
      {
         optionType = StockOption::PUT;
         optionString = returnStr.substr(putStart, putEnd - putStart);
      }

      // Loop through the table, extracting information
      /*Format ::
      <tr class="data-row0 Bgc($extraLightBlue):h BdT Bdc($finLightGray) H(36px) in-the-money Bgc($inTheMoneyBlue)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0">
         <td class="data-col0 Ta(end) Pstart(10px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$0">
            <a href="/quote/AAPL/options?strike=70&amp;straddle=false" data-symbol="AAPL" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$0.0">
               70.00
            </a>
         </td>
         <td class="data-col1 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$1">
            <a class="Fz(s) Ell C($actionBlue)" href="/quote/AAPL161104C00070000?p=AAPL161104C00070000" title="AAPL161104C00070000" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$1.0">
               AAPL161104C00070000
            </a>
         </td>
         <td class="data-col2 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$2">
            41.65
         </td>
         <td class="data-col3 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$3">
            38.80
         </td>
         <td class="data-col4 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$4">
            38.90
         </td>
         <td class="data-col5 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$5">
            <span data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$5.0">
               0.00
            </span>
         </td>
         <td class="data-col6 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$6">
            <span data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$6.0">
               0.00%
            </span>
         </td>
         <td class="data-col7 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$7">
            6
         </td>
         <td class="data-col8 Ta(end) Pstart(20px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$8">
            0
         </td>
         <td class="data-col9 Ta(end) Pstart(20px) Pend(10px)" data-reactid=".178j2ghzrdm.0.$0.0.0.3.1.$main-0-Quote-Proxy.$main-0-Quote.2.0.0.0.2:$calls.$calls.1.$0.0:$9">
            300.00%
         </td>
      </tr>
      */

      // Store all extractions into an array of strings, for more generic code.
      bool continueToNextRow = true;
      i = j = 0;

      while (continueToNextRow) 
      {
         std::string dataExtract[10];

         for (int fieldCount = 0; fieldCount < 10; fieldCount++) 
         {
            /* As seen in the format above, each of the values we want to extract are within the inner
             * most element. We will iterate until we've found the inner-most value, extract it, then
             * move on to the next.
            */

            // Find the next table row
            i = optionString.find("<td", i);
            if (i == std::string::npos) 
            {
               continueToNextRow = false;
               break;
            }
            j = optionString.find("</td", i);
            if (j == std::string::npos) 
            {
               fprintf(stderr, "yahooOptions : Bad return string 3.1\r\n");
               goto StockOption_yahoo_end;
            }
            std::string optionRow = optionString.substr(i, j - i);

            // Find the inner most element
            while (true) 
            {
               j = i;
               i = optionRow.find("<", i + 1);
               if (i == std::string::npos) 
               { 
                  // We've reached the inner-most element
                  i = j; // Restore i to previously found position
                  break;
               }
            }

            // Extract the inner-most element
            i = optionRow.find(">", i);
            if (i == std::string::npos) 
            {
               fprintf(stderr, "yahooOptions : Bad return string 3.2\r\n");
               goto StockOption_yahoo_end;
            }
            j = optionRow.find("<", i);
            if (j == std::string::npos) 
            {
               fprintf(stderr, "yahooOptions : Bad return string 3.3\r\n");
               goto StockOption_yahoo_end;
            }

            dataExtract[fieldCount] = optionRow.substr(i + 1, j - 1);
         }

         // Exit while loop?
         if (!continueToNextRow) 
         {
            break;
         }

         // Convert and store the extracted strings
         // STRIKE
         float strike = 0.0f;
         try 
         {
            strike = std::stof (dataExtract[0].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 31: %s\n", dataExtract[0].c_str());
         }

         // CONTRACT NAME
         std::string contractName(dataExtract[1]);

         // PRICE
         float price = 0.0f;
         try 
         {
            price = std::stof (dataExtract[2].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 32: %s\n", dataExtract[2].c_str());
         }

         // BID
         float bid = 0.0f;
         try 
         {
            bid = std::stof (dataExtract[3].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 33: %s\n", dataExtract[3].c_str());
         }

         // ASK
         float ask = 0.0f;
         try 
         {
            ask = std::stof (dataExtract[4].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 34: %s\n", dataExtract[4].c_str());
         }

         // CHANGE
         float change = 0.0f;
         try 
         {
            change = std::stof (dataExtract[5].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 35: %s\n", dataExtract[5].c_str());
         }

         // CHANGE PERCENT
         float changePercent = 0.0f;
         try 
         {
            changePercent = std::stof (dataExtract[6].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 36: %s\n", dataExtract[6].c_str());
         }

         // VOLUME
         long unsigned int volume = 0;
         try 
         {
            volume = std::stoul (dataExtract[7].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stoul fail 37: %s\n", dataExtract[7].c_str());
         }

         // OPEN INTEREST
         float openInterest = 0.0f;
         try 
         {
            openInterest = std::stof (dataExtract[8].c_str());
         } 
         catch (std::exception) 
         {
            fprintf(stderr, "stof fail 38: %s\n", dataExtract[8].c_str());
         }

         // Create a new option and add it to the vector, then clean up.
         so = new StockOption(optionType, contractName, price, change,
                              changePercent, bid, ask, openInterest, volume, strike,
                              SimpleDate(year, month, day), SimpleDate::today());
         stk->push_back(*so);
         delete so;
         so = NULL;
      }
   }

   return stk;

StockOption_yahoo_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }

   return NULL;
}

#if 0  // Yahoo updated format...this is outdated.
//--------------------------------------------------------------------------------------------------
//Name:
// getYahooFinanceWeekOptions
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// Vector of StockOptions. Retuns NULL on fail.
//
//About:
// Yahoo doesn't provide a nice way like google does to get stock option data, however, yahoo
// has a more extensive collection of stock info than google. We page scrape an html result from
// yahoo. There are no particular key indicators to what we should look for in the html, so the
// scrape is shaky at best. May change if yahoo developers change look of page.
//
// This method is static to allow greater modular usage, and to ensure that the downloaded data
// is current. Errors in the download will result in no values being added to the class vectors,
// as this function, even if successful, returns a temporary vector to the results.
//
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock::getYahooFinanceWeekOptions(std::string _ticker)
{
   // Local variables
   std::vector<StockOption>* stk = NULL;
   StockOption* so = NULL;

   HttpDownloader request;
   std::string returnStr;
   std::string optionInfo;
   std::string tempStr;
   std::string tempStr2;

   // Prepare http request
   char query[512] = { 0 };
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://finance.yahoo.com/q/op?s=%s+Options", _ticker.c_str());
   #else
   snprintf(query, 512, "http://finance.yahoo.com/q/op?s=%s+Options", _ticker.c_str());
   #endif

   // Request option chain
   returnStr = request.download(query);
   if (returnStr == "")
   { 
      // Query failed
      return NULL;
   }

   stk = new std::vector<StockOption>();  // Allocate a new vector to hold the options
   int i, j;
   unsigned int day, month, year;
   int callStart, putStart;

   // Get the Expiration Date (Will be same for all options)
   // Looks like this: <option data-selectbox-link="/q/op?s=AAPL&amp;date=1459468800" value="1459468800">April 1, 2016</option>
   i = returnStr.find("date=");
   if (i == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1\r\n");
      goto StockOption_yahoo_end;
   }
   i = returnStr.find('>', i);
   if (i == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.1\r\n");
      goto StockOption_yahoo_end;
   }
   j = returnStr.find('<', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.2\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr = returnStr.substr(i + 1, j - i - 1);
   // Extract the Month
   i = 0;
   j = tempStr.find(' ', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.3\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr2 = tempStr.substr(i, j - i);
   month = SimpleDate::getMonth(tempStr2);
   // Extract the Day
   i = j + 1;
   j = tempStr.find(',', i);
   if (j == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 1.4\r\n");
      goto StockOption_yahoo_end;
   }
   tempStr2 = tempStr.substr(i, j - i);
   day = atoi(tempStr2.c_str());
   // Extract the Year
   i = j + 2;
   tempStr2 = tempStr.substr(i, tempStr.length() - i);
   year = atoi(tempStr2.c_str());

   // Find the start of the calls and puts tables:
   callStart = returnStr.find("<tr data-row=\"0\"");
   if(callStart == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 2\r\n");
      goto StockOption_yahoo_end;
   }
   putStart = returnStr.find("<tr data-row=\"0\"", callStart + 1);
   if(putStart == std::string::npos)
   {
      fprintf(stderr, "yahooOptions : Bad return string 2.1\r\n");
      goto StockOption_yahoo_end;
   }

   // Parse options and store in vector
   for (unsigned int index = 0; index < 2; index++)
   {
      StockOption::OPTION_TYPE optionType;
      std::string optionString;

      // Set the option type and get a substring which contains the table we want
      if(index == 0)
      {
         optionType = StockOption::CALL;
         optionString = returnStr.substr(callStart, putStart);
      } 
      else 
      {
         optionType = StockOption::PUT;
         optionString = returnStr.substr(putStart, returnStr.length() - putStart);
      }

      // Loop through the table, extracting information
      /*Format ::
      <tr data-row="0" data-row-quote="_" class="in-the-money even ">
        <td>
            <strong data-sq=":value" data-raw=""><a href="/q/op?s=AAPL&amp;strike=65.00">65.00</a></strong>
        </td>
        <td>
            <div class="option_entry Fz-m"><a href="/q?s=AAPL160401C00065000">AAPL160401C00065000</a></div>
        </td>
        <td>
            <div class="option_entry Fz-m">0.00</div>
        </td>
        <td>
            <div class="option_entry Fz-m">36.05</div>
        </td>
        <td>
            <div class="option_entry Fz-m">36.35</div>
        </td>
        <td>
            <div class="option_entry Fz-m">0.00</div>
        </td>
        <td>
           <div class="option_entry Fz-m">0.00%</div>
        </td>
        <td>
            <strong data-sq=":volume" data-raw="0">0</strong>
        </td>
        <td>
            <div class="option_entry Fz-m">0</div>
        </td>
        <td>
            <div class="option_entry Fz-m">0.00%</div>
        </td>
      </tr>
    */
      i = j = 0;
      while(true)
      {
         // Strike Price
         i = optionString.find("<a", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 3\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 3.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 3.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float strike = 0.0f;
         try 
         {
            strike = std::stof (tempStr.c_str());
         } catch(std::exception)
         {
            fprintf(stderr, "stof fail 31: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Contract Name
         i = optionString.find("<a", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 4\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 4.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 4.2\r\n");
            goto StockOption_yahoo_end;
         }
         std::string contractName(optionString.substr(i + 1, j - i - 1));
         i = j + 1;
         // Price
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 5\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 5.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 5.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float price = 0.0f;
         try
         {
            price = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 32: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Bid
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 6\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 6.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 6.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float bid = 0.0f;
         try
         {
            bid = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 33: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Ask
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 7\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 7.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 7.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float ask = 0.0f;
         try 
         {
            ask = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 34: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Change
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 8\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 8.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 8.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float change = 0.0f;
         try 
         {
            change = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 35: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Change Percent
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 9\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 9.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 9.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float changePercent = 0.0f;
         try 
         {
            changePercent = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 36: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Volume
         i = optionString.find("<strong", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 10\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 10.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 10.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         long unsigned int volume = 0;
         try 
         {
            volume = std::stoul (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stoul fail 37: %s\n", tempStr.c_str());
         }
         i = j + 1;
         // Open Interest
         i = optionString.find("<div", i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 11\r\n");
            goto StockOption_yahoo_end;
         }
         i = optionString.find('>', i);
         if (i == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 11.1\r\n");
            goto StockOption_yahoo_end;
         }
         j = optionString.find('<', i + 1);
         if (j == std::string::npos)
         {
            fprintf(stderr, "yahooOptions : Bad return string 11.2\r\n");
            goto StockOption_yahoo_end;
         }
         tempStr = optionString.substr(i + 1, j - i - 1);
         float openInterest = 0.0f;
         try 
         {
            openInterest = std::stof (tempStr.c_str());
         } 
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 38: %s\n", tempStr.c_str());
         }
         i = j + 1;

         // Create a new option and add it to the vector, then clean up.
         so = new StockOption(optionType, contractName, price, change,
                              changePercent, bid, ask, openInterest, volume, strike,
                              SimpleDate(year, month, day), SimpleDate::today());
         stk->push_back(*so);
         delete so;

         // Move to the next row
         i = optionString.find("<tr data-row=", i);
         if (i == std::string::npos)
         {
            // We are done with this loop iteration...
            break;
         } 
         else 
         {
            // Catch j up to i
            j = i + 1;
         }
      }
   }

   return stk;

StockOption_yahoo_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }

   return NULL;
}
#endif

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
