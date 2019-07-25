//Dependencies======================================================================================
//==================================================================================================
#include "OptionStock_Fidelity.hpp"
#include "HttpDownloader_Fidelity.hpp"
#include <algorithm>

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// OptionStock_Fidelity
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
OptionStock_Fidelity::OptionStock_Fidelity(std::string _ticker, std::string _name)
   : OptionStock(_ticker, _name)
{
   // Void
}

//--------------------------------------------------------------------------------------------------
//Name:
// addOptions_Fidelity
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
bool OptionStock_Fidelity::addOptions_Fidelity(const HttpDownloader_Fidelity* _httpRequest) 
{
   // Attempt to get this stocks options info
   std::vector<StockOption>* options = OptionStock_Fidelity::getThisWeeksOptions_Fidelity(this->getTicker(), _httpRequest);
   if (options == NULL)
   {

      // Fidelity adds a period in front of index symbols... let's try again with that
      fprintf(stderr, "OptionStock_Fidelity::addOptions_Fidelity failed to get options for %s. Trying index fund style.\n", this->getTicker().c_str());
      options = OptionStock_Fidelity::getThisWeeksOptions_Fidelity(std::string("." + this->getTicker()), _httpRequest);

      if (options == NULL)
      {
         fprintf(stderr, "OptionStock_Fidelity::addOptions_Fidelity failed to get options for %s\n", this->getTicker().c_str());
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
// getThisWeeksOptions_Fidelity
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
std::vector<StockOption>* OptionStock_Fidelity::getThisWeeksOptions_Fidelity(std::string _ticker, const HttpDownloader_Fidelity* _httpRequest) {
   std::vector <StockOption>* stk = NULL;

   std::string ticker = getAlternativeName(_ticker, SOURCE_FIDELITY);
   stk = getFidelityWeekOptions(ticker, _httpRequest);

   if (stk == NULL) 
   {
      fprintf(stderr, "Failed to get options for %s with Fidelity.\n", _ticker.c_str());
   }

   return stk;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getFidelityWeekOptions
//
//Arguments:
// _ticker -> stock symbol to look up in query
// _httpRequest -> HTTPS request handler.
//                 Must be non-NULL and already initialized with a cookie
//
//Output:
// void
//
//Returns:
// Vector of StockOptions. Returns NULL on fail.
//
//About:
// Given a ticker value and an initialized logged in HttpDownloader_Fidelity, pull and parse the
// current weeks option values at current time.
//
// String Identifier           Notes
//
// id="resultsBlock"           :: Start of table
// <tbody ... > ... <tr ... id="Mmm DD YYYY ... "  :: Options Expiration Date
// id="N_0"                    :: id of first row in each table (shared id)
// id="N_1"                    ::
//   ...                       ::
// id="N_<N>"                  :: id of Nth row
// id="N_0"                    :: id of first row in next table (next week's options)
//
// Inside each N_<N> table row example:
//
// <td ... >4.24</td>                     :: Last Price CALL
// <td ... >-0.51</td>                    :: Change CALL
// <td ... > ... <a ... >4.10</a>         :: Bid CALL
// <td ... > ... <a ... >4.45</a>         :: Ask CALL
// <td ... > ... <span ... >623</span>    :: Volume CALL
// <td ... > ... <span ... >1,998</span>  :: Open Int CALL
// <td ... >???</td>                      :: Implicit Volume .. Skip
// <td ... >???</td>                      :: Delta CALL .. Skip
// <td ... >???</td>                      :: Action .. Skip
// <td ... >115</td>                      :: Strike CALL & PUT
// <td ... >???</td>                      :: Action .. Skip
// <td ... >0.02</td>                     :: Last Price PUT
// <td ... >0.00</td>                     :: Change PUT
// <td ... > ... <a ... >0.01</a>         :: Bid PUT
// <td ... > ... <a ... >0.03</a>         :: Ask PUT
// <td ... > ... <span ... >1,050</span>  :: Volume PUT
// <td ... > ... <span ... >1,998</span>  :: Open Int PUT
// <td ... >???</td>                      :: Implicit Volume .. Skip
// <td ... >???</td>                      :: Delta PUT .. Skip
//
//
// This method is static to allow greater modular usage, and to ensure that the downloaded data
// is current. Errors in the download will result in no values being added to the class vectors,
// as this function, even if successful, returns a temporary vector to the results.
//--------------------------------------------------------------------------------------------------
std::vector<StockOption>* OptionStock_Fidelity::getFidelityWeekOptions(std::string _ticker, const HttpDownloader_Fidelity* _httpRequest) 
{
   // Fidelity has an input for number of options to display. This number can be over the max number
   // and fidelity is smart enough to just give us the max. The lower this number, the faster the
   // request will be...
   const unsigned int REQUEST_THIS_MANY_STRIKES_FROM_FIDELITY = 80;

   // Local variables
   std::vector<StockOption>* stk = NULL;
   StockOption* so = NULL;

   std::string returnStr;
   std::string optionTableStr;
   std::string tempStr;

   // Prepare https request
   char query[512] = { 0 };
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "https://researchtools.fidelity.com/ftgw/mloptions/goto/optionChain?symbols=%s&strikeSelected=%u",
             _ticker.c_str(), REQUEST_THIS_MANY_STRIKES_FROM_FIDELITY);
   #else
   snprintf(query, 512, "https://researchtools.fidelity.com/ftgw/mloptions/goto/optionChain?symbols=%s&strikeSelected=%u",
            _ticker.c_str(), REQUEST_THIS_MANY_STRIKES_FROM_FIDELITY);
   #endif

   // NOTE:
   // This is a little bit of a hack... it turns out that the biggest bottle neck is the time it
   // takes for fidelity to return with the options file. We are, therefore, going to multi-thread
   // this. We still need the right userName and Password, so we'll just extract that from the passed
   // in HttpDownloader_Fidelity object.

   HttpDownloader_Fidelity fid(_httpRequest->getUserName(), _httpRequest->getPassword(), std::string(_httpRequest->getCookie() + _ticker));
   fid.loginToFidelityAndSetCookie();

   // Request option chain
   returnStr = fid.download(query);
   if (returnStr == "")
   { 
      // Query failed
      return NULL;
   }

   // Extract expiration date
   int s = 0;
   int e = 0;

   unsigned int year = 1;
   unsigned int day = 1;
   unsigned int month = 1;

   s = returnStr.find("id=\"resultsBlock\"");
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1a\r\n");
      goto StockOption_fidelity_end;
   }

   s = returnStr.find("<tbody", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1b\r\n");
      goto StockOption_fidelity_end;
   }
   s = returnStr.find(">", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1c\r\n");
      goto StockOption_fidelity_end;
   }
   s = returnStr.find("<tr", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1d\r\n");
      goto StockOption_fidelity_end;
   }
   s = returnStr.find("id=", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1e\r\n");
      goto StockOption_fidelity_end;
   }

   // Skip over the id start portion
   s += 4;

   // Next 3 characters is the month
   tempStr = returnStr.substr(s, 3);
   month = SimpleDate::getMonth(tempStr);

   // Skip over the month and the space
   s += 4;

   // Day could be 1 or 2 characters...
   e = returnStr.find(" ", s);
   if (e == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1\r\n");
      goto StockOption_fidelity_end;
   }
   tempStr = returnStr.substr(s, e - s);
   try
   {
      day = std::stoi (tempStr.c_str());
   }
   catch(std::exception)
   {
      fprintf(stderr, "stoi fail 51a: %s\n", tempStr.c_str());
   }

   // Year is 4 characters
   tempStr = returnStr.substr(e + 1, 4);
   try
   {
      year = std::stoi (tempStr.c_str());
   }
   catch(std::exception)
   {
      fprintf(stderr, "stoi fail 51b: %s\n", tempStr.c_str());
   }


   // Extract a substring which will only include this week's options

   s = returnStr.find(">", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.1\r\n");
      goto StockOption_fidelity_end;
   }

   s = returnStr.find("id=\"N_0\"", s + 1);
   if (s == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.2\r\n");
      goto StockOption_fidelity_end;
   }

   e = returnStr.find("id=\"N_0\"", s + 1);
   if (e == std::string::npos)
   {
      fprintf(stderr, "fidelityOptions: Bad return string 1.3\r\n");
      goto StockOption_fidelity_end;
   }

   optionTableStr = returnStr.substr(s, e - s - 1);


   // Allocate a new vector to hold the options
   stk = new std::vector<StockOption>();

   s = e = 0;

   // Parse options and store in vector
   for (unsigned int index = 1; index <= REQUEST_THIS_MANY_STRIKES_FROM_FIDELITY; index++)
   {
      // Extract Call Price
      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 2.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 2.2\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 2.3\r\n");
         goto StockOption_fidelity_end;
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
         fprintf(stderr, "fidelityOptions: Bad return string 3.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 3.2\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 3.3\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Call Bid
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 4.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<a", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 4.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 4.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</a>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 4.4\r\n");
         goto StockOption_fidelity_end;
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
         fprintf(stderr, "fidelityOptions: Bad return string 5.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<a", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 5.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 5.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</a>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 5.4\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Call Volume
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 6.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<span", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 6.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 6.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</span>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 6.4\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Call Open Interest
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 7.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<span", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 7.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 7.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</span>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 7.4\r\n");
         goto StockOption_fidelity_end;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float callOI = 0.0f;
      try
      {
         callOI = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 56: %s\n", tempStr.c_str());
      }

      // Skip the next three fields
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 8.1\r\n");
         goto StockOption_fidelity_end;
      }
      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 8.2\r\n");
         goto StockOption_fidelity_end;
      }
      s = optionTableStr.find("<td", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 8.3\r\n");
         goto StockOption_fidelity_end;
      }
      e = optionTableStr.find("</td>", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 8.4\r\n");
         goto StockOption_fidelity_end;
      }

      // Extract the Strike (Both Call/Put)
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 9.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 9.2\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 9.3\r\n");
         goto StockOption_fidelity_end;
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

      // Skip 1 field
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 10.1\r\n");
         goto StockOption_fidelity_end;
      }
      e = optionTableStr.find("</td>", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 10.2\r\n");
         goto StockOption_fidelity_end;
      }

      // Extract Put Price
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 11.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 11.2\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 11.3\r\n");
         goto StockOption_fidelity_end;
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
         fprintf(stderr, "fidelityOptions: Bad return string 12.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 12.2\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</td>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 12.3\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Put Bid
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 13.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<a", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 13.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 13.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</a>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 13.4\r\n");
         goto StockOption_fidelity_end;
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
         fprintf(stderr, "fidelityOptions: Bad return string 14.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<a", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 14.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 14.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</a>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 14.4\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Put Volume
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 15.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<span", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 15.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 15.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</span>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 15.4\r\n");
         goto StockOption_fidelity_end;
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

      // Extract Put Open Interest
      s = optionTableStr.find("<td", e);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 16.1\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find("<span", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 16.2\r\n");
         goto StockOption_fidelity_end;
      }

      s = optionTableStr.find(">", s + 1);
      if (s == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 16.3\r\n");
         goto StockOption_fidelity_end;
      }

      e = optionTableStr.find("</span>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "fidelityOptions: Bad return string 16.4\r\n");
         goto StockOption_fidelity_end;
      }

      tempStr = optionTableStr.substr(s + 1, e - s - 1);
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), ','), tempStr.end());

      float putOI = 0.0f;
      try
      {
         putOI = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 63: %s\n", tempStr.c_str());
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


      // Find out if there is another row to parse
      char tempBuffer[20] = { 0 };

      #if defined(_MSC_VER) || defined(__WINDOWS__)
      sprintf_s(tempBuffer, "id=\"N_%u\"", index);
      #else
      snprintf(tempBuffer, 20, "id=\"N_%u\"", index);
      #endif

      s = optionTableStr.find(tempBuffer, e);
      if (s == std::string::npos)
      {
         // We are done
         break;
      }


      // fprintf(stdout, "Price:%f, CallChange:%f, strike:%f, PutOI:%f\r\n", callLastPrice, callChange, strike, putOI);

   }

   return stk;

StockOption_fidelity_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }

   return NULL;
}
