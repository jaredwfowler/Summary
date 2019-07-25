//Dependencies======================================================================================
//==================================================================================================
#include "Stock.hpp"
#include "HttpDownloader.hpp"
#include <algorithm>

//Method Definitions================================================================================
//==================================================================================================

const Stock::alternative_symbol_names_t Stock::alternative_symbol_names_table[] =
{
   // Checked Stocks. No Alternates:
   // DIA, GDX, IWM, QQQ, SPY,

   // List order:
   // NORMAL GOOGLE YAHOO FIDELITY MARKETWATCH

   { // S&P 500
      "SPX", "SPX", "^SPXPM", ".SPX", "SPX"
   },
   { // RUT
      "RUT", "RUT", "^RUT", ".RUT", "RUT"
   },
   { //Always have this one last so we know if we are at end of list.
      "_NULL", "_NULL", "_NULL", "_NULL", "_NULL"
   }
};

//--------------------------------------------------------------------------------------------------
//Name:
// Stock
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
Stock::Stock(std::string _ticker, std::string _name)
{
   this->setName(_name);
   this->setTicker(_ticker);
   this->history = std::vector<StockHistory_Week>();
}

//--------------------------------------------------------------------------------------------------
//Name:
// getStockRating
//
//Arguments:
// _rater -> Assigned name of the rater
//
//Output:
// void
//
//Returns:
// StockRating obj
//
//About:
// Looks through our list of ratings and returns a copy of the StockRating object which matches
// based of rater name.
//
//--------------------------------------------------------------------------------------------------
StockRating Stock::getStockRating(std::string _rater) const 
{
   for (size_t i = 0; i < this->rating.size(); i++)
   {
      if (_rater == this->rating[i].getRater())
      {
         return StockRating(this->rating[i]);
      }
   }
   return StockRating(SimpleDate(1,1,1), -1, "DNE");
}

//--------------------------------------------------------------------------------------------------
//Name:
// getHistory*Date
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// Date
//
//About:
// Analyze our array of history, which should be sorted by date, and get the first/last entries.
//
//--------------------------------------------------------------------------------------------------
SimpleDate Stock::getHistoryStartDate(void) const
{
   if (this->history.size() >= 1)
   {
      return (this->history[0].getDate());
   }
   else
   {
      return SimpleDate(1, 1, 1);
   }
}

SimpleDate Stock::getHistoryEndDate(void) const 
{
   if (this->history.size() >= 1)
   {
      return (this->history[this->history.size() - 1].getCloseDate());
   }
   else
   {
      return SimpleDate(1, 1, 1);
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// getAlternativeName
//
//Arguments:
// _name -> symbol name
// _source -> which agencies version do we want?
//
//Output:
// void
//
//Returns:
// Alternative symbol name. If not found, returns passed in string
//
//About:
// Symbols are different for yahoo, google, and fidelity
//
//--------------------------------------------------------------------------------------------------
std::string Stock::getAlternativeName(std::string _name, ticker_source_t _source)
{

   int index = 0;

   while (true)
   {
      if (alternative_symbol_names_table[index].scharf == "_NULL")
      {
         return _name;
      }
      else if (alternative_symbol_names_table[index].scharf == _name)
      {
         switch (_source)
         {
            case SOURCE_GOOGLE:
               return std::string(alternative_symbol_names_table[index].google);
               break;

            case SOURCE_YAHOO:
               return std::string(alternative_symbol_names_table[index].yahoo);
               break;

            case SOURCE_FIDELITY:
               return std::string(alternative_symbol_names_table[index].fidelity);
               break;

            case SOURCE_MARKETWATCH:
               return std::string(alternative_symbol_names_table[index].marketWatch);
               break;

            default:
               break;
         }
      }

      index += 1;
   }

}

//--------------------------------------------------------------------------------------------------
//Name:
// clearHistory
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// true on success
//
//About:
// Clears the contents of the history vector
//
//--------------------------------------------------------------------------------------------------
bool Stock::clearHistory(void)
{
   this->history.clear();
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// addHistory
//
//Arguments:
// Start and End dates
//
//Output:
// void
//
//Returns:
// true on success, else false
//
//About:
// This acts as the clients gateway to start filling up the stock info objects. A connection will
// be made to the internet, information gathered, parsed, sorted, etc.
//
//--------------------------------------------------------------------------------------------------
bool Stock::addHistory(SimpleDate _start, SimpleDate _end)
{
   // Attempt to get stock's history
   std::vector<StockHistory_Week>* hist =
                                 Stock::downloadParseReturnHistory(this->getTicker(), _start, _end);
   if (hist == NULL)
   {
      fprintf(stderr, "Stock::addHistory failed to download history for %s\n",
                                                                         this->getTicker().c_str());
      return false;
   }

   // Insert weekly values into this vector
   for (unsigned int i = 0; i < hist->size(); i++)
   {
      this->insertHistory(&(hist->at(i)));
   }

   // Clean up
   delete hist;
   hist = NULL;

   // Sort
   this->sortHistory();

   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// addRating
//
//Arguments:
// Rating objects
//
//Output:
// void
//
//Returns:
// true on success, else false
//
//About:
// Clients gateway to add ratings to this classes vector
//
//--------------------------------------------------------------------------------------------------
bool Stock::addRating(const StockRating* _stkRating)
{


   // Sort
   this->sortRating();

   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// sort*
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
// Functions used to sort the rating and history vectors of this class.
//
//--------------------------------------------------------------------------------------------------
void Stock::sortRating(void)
{
   std::sort(this->rating.begin(), this->rating.end());
}
void Stock::sortHistory(void)
{
   std::sort(this->history.begin(), this->history.end());
}

//--------------------------------------------------------------------------------------------------
//Name:
// addRating
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// true if inserted, else false
//
//About:
// Add a rating object to our vector. If a rating already exists with this rater, replace it.
//
//--------------------------------------------------------------------------------------------------
bool Stock::insertRating(const StockRating* _r)
{
   // Valid input object?
   if (_r == NULL) return false;
   // Does the rater already exist?
   for (size_t i = 0; i < this->rating.size(); i++)
   {
      if (_r->getRater() == this->rating[i].getRater())
      {
         // Replace older version
         this->rating[i] = *_r;
         return true;
      }
   }
   // Else, insert new
   this->rating.push_back(StockRating(*_r));
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// addWeekHistory
//
//Arguments:
// A week of stock history to be inserted.
//
//Output:
// void
//
//Returns:
// true if inserted, else false
//
//About:
// Add a StockHistory_Week object to our vector. If the week already exists, replace it.
//
//--------------------------------------------------------------------------------------------------
bool Stock::insertHistory(const StockHistory_Week* _stk)
{
   // Valid input object?
   if (_stk == NULL) return false;
   // Does this week already exist?
   for (size_t i = 0; i < this->history.size(); i++)
   {
      SimpleDate a = this->history[i].getDate();
      if (_stk->getDate() == a)
      {
         // Replace older version
         this->history[i] = *_stk;
         return true;
      }
   }
   // Else, insert new
   this->history.push_back(*_stk);
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// history*Output
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
// Print methods to export gathered information on stock's history. These are static b/c the
// downloadParseReturnHistory is static as well.
//
//--------------------------------------------------------------------------------------------------
void Stock::historyCSVOutput(const FILE* _stream, const std::vector<StockHistory_Week>* _vec)
{
   // Output the key
   fprintf(const_cast<FILE*>(_stream), "    Week's Date,      Open,     Close,      High,       Low,      AvgVolume,  %% Change\n");
   // Output the data
   for (unsigned int i = 0; i < _vec->size(); i++)
   {
      fprintf(const_cast<FILE*>(_stream), "%15s,%10.2f,%10.2f,%10.2f,%10.2f,%15lu,%10.2f\n",
         _vec->at(i).getDate().toString2().c_str(), _vec->at(i).getOpen(), _vec->at(i).getClose(),
         _vec->at(i).getHigh(), _vec->at(i).getLow(), _vec->at(i).getAvgVolume(),
         _vec->at(i).getPercentChange() * 100);
   }
}

void Stock::historyTableOutput(const FILE* _stream, const std::vector<StockHistory_Week>* _vec)
{
   // Output the key
   fprintf(const_cast<FILE*>(_stream), "   Week's Date |     Open |    Close |     High |      Low | %% Change \n");
   fprintf(const_cast<FILE*>(_stream), "----------------------------------------------------------------------\n");
   // Output the data
   for (unsigned int i = 0; i < _vec->size(); i++)
   {
      fprintf(const_cast<FILE*>(_stream), "%15s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
         _vec->at(i).getDate().toString2().c_str(), _vec->at(i).getOpen(), _vec->at(i).getClose()
         , _vec->at(i).getHigh(), _vec->at(i).getLow(), _vec->at(i).getPercentChange() * 100);
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// downloadParseReturnHistory
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// A pointer to a vector of Stock_Historys. It will be up to the receiving end to free up the
// dynamic memory when done. If failure occurrs, NULL is returned.
//
//About:
// Connects to google finance and requests the history of the provided ticker between the provided
// dates. After much thought, I've made this method static b/c I want to enforce the rule that we
// only add information to the class objects vector if the downloaded info is all correct. If at
// any point the downloaded file is found to be corrupt, the temporary vector is discarded and the
// object vector is not affected. It's static property also allows it to be called without the need
// to encapsulate it in a Stock object.
//
//--------------------------------------------------------------------------------------------------
std::vector<StockHistory_Week>* Stock::downloadParseReturnHistory(std::string _ticker,
                                                                  SimpleDate _start,
                                                                  SimpleDate _end)
{
   std::vector<StockHistory_Week>* stk = NULL;

   // Argument validity checks
   if (_start < _end)
   {

#if GOOGLE_YAHOO_HISTORY_API
       // Attempt to get the history via google finance
      std::string ticker = Stock::getAlternativeName(_ticker, SOURCE_GOOGLE);
      stk = Stock::getGoogleFinanceHistory(ticker, _start, _end);
      if(stk == NULL)
      {
         fprintf(stderr, "Failed to get history for %s with GoogleFinance. Trying Yahoo.\n", _ticker.c_str());
         // Attempt to get the history via yahoo finance
         ticker = Stock::getAlternativeName(_ticker, SOURCE_YAHOO);
         stk = Stock::getYahooFinanceHistory(ticker, _start, _end);
      }
#elif YAHOO_HISTORY_PURE_SCRAPE
       // Attempt to get the history via yahoo page scraping
       std::string ticker = Stock::getAlternativeName(_ticker, SOURCE_YAHOO);
       stk = Stock::getYahooFinanceHistory_PurePageScrape(ticker);
#else
       // Attempt to get the history via yahoo page scraping
       std::string ticker = Stock::getAlternativeName(_ticker, SOURCE_YAHOO);
       stk = Stock::getYahooFinanceHistory_PageScrape(ticker, _start, _end);
#endif

   }
   else
   {
      fprintf(stderr, "getHistory: StartDate >= EndDate\r\n");
   }

   return stk;
}

//--------------------------------------------------------------------------------------------------
//Name:
// get*FinanceRequestHistory
//
//Arguments:
//  ...
//
//Output:
// void
//
//Returns:
// null if unsuccesful, else, returns a pointer to a dynamic vector of StockHistory_Week objs.
//
//About:
// These functions do the same thing via different api's.  I've found that Yahoo's api is pretty nice
// compared to googles when it comes to stock history.  However, Yahoo doesn't offer a good solution
// to getting stock options, besides just page scraping. Thus, I will be using google's api by default
// and Yahoo will be the backup if google fails.
//
//--------------------------------------------------------------------------------------------------

#if GOOGLE_YAHOO_HISTORY_API
std::vector<StockHistory_Week>* Stock::getGoogleFinanceHistory(std::string _ticker, SimpleDate _start, SimpleDate _end)
{
   std::vector<StockHistory_Week>* stk = NULL;
   StockHistory_Week* shw = NULL;
   std::vector<StockHistory_Day> shdVec;

   HttpDownloader request;
   std::string returnStr;
   std::string dayInfo;
   std::string tempStr;
   int stkDayW = 9;  // This will help us determine when we are starting a new week object
   char query[256] = { 0 };
   int i = 0;

   // Format: http://www.google.com/finance/historical?q=<TickerSymbol>&startdate=<yyyy-mm-dd>&enddate=<yyyy-mm-dd>&output=csv
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://www.google.com/finance/historical?q=%s&startdate=%04u-%02u-%02u&enddate=%04u-%02u-%02u&output=csv",
      _ticker.c_str(), _start.getYear(), _start.getMonth(), _start.getDayM(), _end.getYear(), _end.getMonth(), _end.getDayM());
   #else
   snprintf(query, 256, "http://www.google.com/finance/historical?q=%s&startdate=%04u-%02u-%02u&enddate=%04u-%02u-%02u&output=csv",
      _ticker.c_str(), _start.getYear(), _start.getMonth(), _start.getDayM(), _end.getYear(), _end.getMonth(), _end.getDayM());
   #endif

   // Request history
   returnStr = request.download(query);

   // Did we get valid feedback? Should be a csv... no html tag.
   i = returnStr.find("html");
   if(i != std::string::npos)
   {
      fprintf(stderr, "googleHistory : html file returned... %s\r\n", _ticker.c_str());
      goto StockHistory_google_end;
   }

   stk = new std::vector<StockHistory_Week>();  // Allocate a new vector to hold the week history
   shw = new StockHistory_Week();  // Allocate a new stk week object

   // Parse history and store in vector
   // We can ignore the first line b/c it's the table keys
   i = returnStr.find('\n');
   if (i == std::string::npos)
   {
      fprintf(stderr, "googleHistory : Bad return string \r\n");
      goto StockHistory_google_end;
   }
   i += 1;

   if ((unsigned int)i >= returnStr.length())
   {
      fprintf(stderr, "googleHistory : Bad return string 1 \r\n");
      goto StockHistory_google_end;
   }

   while ((unsigned int)i < returnStr.length())
   {
      unsigned int j = returnStr.find('\n', i);  // Find the next occurrence of endl
      if (j == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 2 \r\n");
         goto StockHistory_google_end;
      }
      dayInfo = returnStr.substr(i, j - i + 1);  // Get the single day info and '\n'
      i = j + 1;  // Increment i for the next iteration

      // Attemp to parse out the data... format: Date(DD-MMM-YY),Open(float),High(float),Low(float),Close(float),Volume(uint)
      // We ASSUME that the data will be listed from youngest to oldest date.
      // Day,Month,Year
      // Day
      j = 0;
      unsigned int k = 0;
      k = dayInfo.find('-', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 3 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int day = atoi(tempStr.c_str());
      j = k + 1;
      // Month
      k = dayInfo.find('-', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 4 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int month = SimpleDate::getMonth(tempStr);
      if (month == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 4.2 \r\n");
         goto StockHistory_google_end;
      }
      j = k + 1;
      // Year
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 5 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int year = atoi(tempStr.c_str());
      // Translate the year to 4 digits
      if (year >= 50)  // TODO - ONLY GOOD UNTIL 2050
         year += 1900;
      else
         year += 2000;
      j = k + 1;

      // Open,High,Low,Close,Volume
      // The only guaranteed fields we have are the date, close, and volume.
      // If any of the other fields have '-', then we will copy the close into the other fields.
      bool copyClose = false;
      // Open
      float open = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 6 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      if (tempStr == "-")
         copyClose = true;
      else{
         try
         {
            open = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 01: %s\n", tempStr.c_str());
            getchar();
         }
      }
      j = k + 1;
      // High
      float high = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 7 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      if (tempStr == "-")
      {
         copyClose = true;
      }
      else
      {
         try
         {
            high = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 02: %s\n", tempStr.c_str());
         }
      }
      j = k + 1;
      // Low
      float low = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 8 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      if (tempStr == "-")
         copyClose = true;
      else
      {
         try
         {
            low = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 03: %s\n", tempStr.c_str());
         }
      }
      j = k + 1;
      // Close
      float close = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 9 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      if (tempStr == "-")
      {
         fprintf(stderr, "googleHistory : Bad return string 9.1 \r\n");
         goto StockHistory_google_end;
      }
      else
      {
         try
         {
            close = std::stof (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stof fail 04: %s\n", tempStr.c_str());
         }
      }

      // If incomplete fields, just copy the close
      if (copyClose)
      {
         open = close;
         high = close;
         low = close;
      }
      j = k + 1;
      // Volume
      unsigned long volume = 0;
      k = dayInfo.find('\n', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "googleHistory : Bad return string 10 \r\n");
         goto StockHistory_google_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      if (tempStr != "-")
      {
         try
         {
            volume = std::stoul (tempStr.c_str());
         }
         catch(std::exception)
         {
            fprintf(stderr, "stoul fail 05: %s\n", tempStr.c_str());
         }
      }

      // Determine if this stock day is part of the current week, or if we need to add a week.
      int dayW = SimpleDate::dayofweek(day, month, year);
      if (dayW > stkDayW)
      { // Are we at the 'start' (really end) of a new week?
         // Add vector of days to the week, then clean vector
         shw->addDayHistory(&shdVec);
         shdVec.clear();
         // Push the week object into the vector
         stk->push_back(*shw);
         delete shw;
         shw = new StockHistory_Week();
      }
      // Update our day of week checker
      stkDayW = dayW;

      // Create a new stock day object, and add it to the week vector, then clean up
      shdVec.push_back(StockHistory_Day(SimpleDate(year, month, day), open, close, high, low, volume));
   }

   // Add vector of days to the week, then clean vector
   shw->addDayHistory(&shdVec);
   shdVec.clear();
   // We'll need to push the week object onto the vector
   stk->push_back(*shw);
   delete shw;
   shw = NULL;

   // Return vector pointer
   return stk;

   // Error.. Delete dynamic memory if allocated
   StockHistory_google_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }
   if (shw != NULL)
   {
      delete shw;
      shw = NULL;
   }
   return NULL;
}

std::vector<StockHistory_Week>* Stock::getYahooFinanceHistory(std::string _ticker, SimpleDate _start, SimpleDate _end)
{
   std::vector<StockHistory_Week>* stk = NULL;
   StockHistory_Week* shw = NULL;
   std::vector<StockHistory_Day> shdVec;

   HttpDownloader request;
   std::string returnStr;
   std::string dayInfo;
   std::string tempStr;
   int stkDayW = 9;  // This will help us determine when we are starting a new week object
   char query[256] = { 0 };
   int i = 0;

   // Format: http://ichart.finance.yahoo.com/table.csv?s=<symbol>&a=<mm>&b=<dd>&c=<yyyy>&d=<mm>&e=<dd>&f=<yyyy>&g=d&ignore=.csv
   #if defined(_MSC_VER) || defined(__WINDOWS__)
   sprintf_s(query, "http://ichart.finance.yahoo.com/table.csv?s=%s&a=%02u&b=%02u&c=%04u&d=%02u&e=%02u&f=%04u&g=d&ignore=.csv",
      _ticker.c_str(), _start.getMonth(), _start.getDayM(), _start.getYear(), _end.getMonth(), _end.getDayM(), _end.getYear());
   #else
   snprintf(query, 256, "http://ichart.finance.yahoo.com/table.csv?s=%s&a=%02u&b=%02u&c=%04u&d=%02u&e=%02u&f=%04u&g=d&ignore=.csv",
      _ticker.c_str(), _start.getMonth(), _start.getDayM(), _start.getYear(), _end.getMonth(), _end.getDayM(), _end.getYear());
   #endif

   // Request history
   returnStr = request.download(query);

   // Did we get valid feedback? Should be a csv... no html tag.
   i = returnStr.find("html");
   if(i != std::string::npos)
   {
      fprintf(stderr, "yahooHistory : html file returned... %s\r\n", _ticker.c_str());
      goto StockHistory_yahoo_end;
   }

   stk = new std::vector<StockHistory_Week>();  // Allocate a new vector to hold the week history
   shw = new StockHistory_Week();  // Allocate a new stk week object

   // Parse history and store in vector
   // We can ignore the first line b/c it's the table keys
   i = returnStr.find('\n');
   if (i == std::string::npos)
   {
      fprintf(stderr, "yahooHistory : Bad return string \r\n");
      goto StockHistory_yahoo_end;
   }
   i += 1;

   if ((unsigned int)i >= returnStr.length())
   {
      fprintf(stderr, "yahooHistory : Bad return string 1 \r\n");
      goto StockHistory_yahoo_end;
   }

   while ((unsigned int)i < returnStr.length())
   {
      unsigned int j = returnStr.find('\n', i);  // Find the next occurrence of endl
      if (j == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 2 \r\n");
         goto StockHistory_yahoo_end;
      }
      dayInfo = returnStr.substr(i, j - i + 1);  // Get the single day info and '\n'
      i = j + 1;  // Increment i for the next iteration

      // Attempt to parse out the data... format: Date(YYYY-MM-DD),Open(float),High(float),Low(float),Close(float),Volume(uint),AdjClose(float)
      // We ASSUME that the data will be listed from youngest to oldest date.
      // Year,Month,Day
      // Year
      j = 0;
      unsigned int k = 0;
      k = dayInfo.find('-', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 3 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int year = atoi(tempStr.c_str());
      j = k + 1;
      // Month
      k = dayInfo.find('-', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 4 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int month = atoi(tempStr.c_str());
      if (month == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 4.2 \r\n");
         goto StockHistory_yahoo_end;
      }
      j = k + 1;
      // Day
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 5 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      unsigned int day = atoi(tempStr.c_str());
      j = k + 1;

      // Open,High,Low,Close,Volume
      // Open
      float open = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 6 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      try
      {
         open = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 01: %s\n", tempStr.c_str());
         getchar();
      }
      j = k + 1;
      // High
      float high = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 7 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      try
      {
         high = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 02: %s\n", tempStr.c_str());
      }
      j = k + 1;
      // Low
      float low = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 8 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      try
      {
         low = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 03: %s\n", tempStr.c_str());
      }
      j = k + 1;
      // Close
      float close = 0.0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 9 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      try
      {
         close = std::stof (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stof fail 04: %s\n", tempStr.c_str());
      }
      j = k + 1;
      // Volume
      unsigned long volume = 0;
      k = dayInfo.find(',', j);
      if (k == std::string::npos)
      { // Error Check
         fprintf(stderr, "yahooHistory : Bad return string 10 \r\n");
         goto StockHistory_yahoo_end;
      }
      tempStr = dayInfo.substr(j, k - j);
      try
      {
         volume = std::stoul (tempStr.c_str());
      }
      catch(std::exception)
      {
         fprintf(stderr, "stoul fail 05: %s\n", tempStr.c_str());
      }
      j = k + 1;
      // Skip This...
      k = dayInfo.find('\n', j);
      j = k + 1;

      // Determine if this stock day is part of the current week, or if we need to add a week.
      int dayW = SimpleDate::dayofweek(day, month, year);
      if (dayW > stkDayW)
      { 
         // Are we at the 'start' (really end) of a new week?
         // Add vector of days to the week, then clean vector
         shw->addDayHistory(&shdVec);
         shdVec.clear();
         // Push the week object into the vector
         stk->push_back(*shw);
         delete shw;
         shw = new StockHistory_Week();
      }
      // Update our day of week checker
      stkDayW = dayW;

      // Create a new stock day object, and add it to the week vector, then clean up
      shdVec.push_back(StockHistory_Day(SimpleDate(year, month, day), open, close, high, low, volume));
   }

   // Add vector of days to the week, then clean vector
   shw->addDayHistory(&shdVec);
   shdVec.clear();
   // We'll need to push the week object onto the vector
   stk->push_back(*shw);
   delete shw;
   shw = NULL;

   // Return vector pointer
   return stk;

   // Error.. Delete dynamic memory if allocated
   StockHistory_yahoo_end:
   if (stk != NULL)
   {
      delete stk;
      stk = NULL;
   }
   if (shw != NULL)
   {
      delete shw;
      shw = NULL;
   }
   return NULL;
}

#elif YAHOO_HISTORY_PURE_SCRAPE

std::vector<StockHistory_Week>* Stock::getYahooFinanceHistory_PurePageScrape(std::string _ticker)
{
   const unsigned int ROW_VALIDITY_TD_CHECKS = 4;
   bool validityCheckPass;

   std::vector<StockHistory_Week>* stk = NULL;
   StockHistory_Week* shw = NULL;
   std::vector<StockHistory_Day> shdVec;

   HttpDownloader request;
   std::string returnStr;
   std::string tempStr;
   int stkDayW = 9;  // This will help us determine when we are starting a new week object
   char query[256] = { 0 };
   int i = 0;

   int s = 0;
   int s2 = 0;
   int e = 0;
   int e2 = 0;

   // Format:https://finance.yahoo.com/quote/<ticker>/history?p=<ticker>
   #if defined(_MSC_VER) || defined(__WINDOWS__)
      sprintf_s(query, "http://finance.yahoo.com/quote/%s/history?p=%s",
      _ticker.c_str(), _ticker.c_str());
   #else
      snprintf(query, 256, "http://finance.yahoo.com/quote/%s/history?p=%s",
      _ticker.c_str(), _ticker.c_str());
   #endif

   // Request history
   returnStr = request.download(query);

   // Extract the relevant table information

   s = returnStr.find("historical-prices", 0);  // This is the start table class
   if (s == std::string::npos)
   {
      fprintf(stderr, "Yahoo_Scrape: Bad return string 1.0\r\n");
      goto StockHistory_yahoo_full_scrape_end;
   }
   e = returnStr.find("</table>", s + 1);
   if (e == std::string::npos)
   {
      fprintf(stderr, "Yahoo_Scrape: Bad return string 1.1\r\n");
      goto StockHistory_yahoo_full_scrape_end;
   }
   returnStr = returnStr.substr(s, e - s);

   // Loop through table information until we extract each stock day

   stk = new std::vector<StockHistory_Week>();  // Allocate a new vector to hold the week history
   shw = new StockHistory_Week();  // Allocate a new stk week object

   e = s = e2 = s2 = 0;

   while (true)
   {
      // Find and extract the next row of data, or determine if we are finished.

      std::string rowData;

      s = returnStr.find("<tr", e);
      if (s == std::string::npos)
      {
         break;
      }
      e = returnStr.find("</tr>", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 2.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }

      rowData = returnStr.substr(s, e - s);

      // Every so often, a row is going to contain dividend information, or som other
      // information which I'm not looking for. Check for at least 4 </td>'s to ensure
      // validity of the row, else, skip this row.

      s2 = 0;

      validityCheckPass = true;
      for (i = 0; i < ROW_VALIDITY_TD_CHECKS; i++)
      {
         s2 = rowData.find("</td>", s2 + 1);
         if (s2 == std::string::npos)
         {
            validityCheckPass = false;
            break;
         }
      }
      if (!validityCheckPass)
      {
         continue;
      }

      // Extract each of the row items

      std::string rowItem;
      s2 = e2 = 0;

      // Extract the date - Format: MMM DD, YYYY

      unsigned int year = 1;
      unsigned int day = 1;
      unsigned int month = 1;

      SimpleDate tempDate;

      s2 = rowData.find("<td", 0);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      // Month
      tempStr = rowItem.substr(0, 3);
      month = SimpleDate::getMonth(tempStr);

      // Day
      s2 = rowItem.find(",", 0);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.4\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      tempStr = rowItem.substr(3, s2 - 3);
      day = atoi(tempStr.c_str());

      // Year
      tempStr = rowItem.substr(s2 + 1, 8);
      year = atoi(tempStr.c_str());

      tempDate.setDate(year, month, day);

      // Extract Open

      float open = 0.0;

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         open = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 01: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Extract High

      float high = 0.0;

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         high = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 02: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Extract Low

      float low = 0.0;

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         low = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 03: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Extract Close

      float close = 0.0;

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         close = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 04: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Skip Adj Close

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 9.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</td>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 9.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Extract Volume

      float volume = 0.0;

      s2 = rowData.find("<td", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.0\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find("<span", s2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.1\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      s2 = rowData.find(">", s2 + 1);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.2\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      e2 = rowData.find("</span>", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.3\r\n");
         goto StockHistory_yahoo_full_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      // Remove commas
      rowItem.erase(std::remove(rowItem.begin(), rowItem.end(), ','), rowItem.end());

      try
      {
         volume = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 05: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_full_scrape_end;
      }

      // Determine if this stock day is part of the current week, or if we need to add a week.
      int dayW = SimpleDate::dayofweek(day, month, year);
      if (dayW > stkDayW)
      {
         // Are we at the 'start' (really end) of a new week?
         // Add vector of days to the week, then clean vector
         shw->addDayHistory(&shdVec);
         shdVec.clear();
         // Push the week object into the vector
         stk->push_back(*shw);
         delete shw;
         shw = new StockHistory_Week();
      }
      // Update our day of week checker
      stkDayW = dayW;

      // Create a new stock day object, and add it to the week vector, then clean up
      shdVec.push_back(StockHistory_Day(SimpleDate(year, month, day), open, close, high, low, volume));
   }

   // Add vector of days to the week, then clean vector
   shw->addDayHistory(&shdVec);
   shdVec.clear();
   // We'll need to push the week object onto the vector
   stk->push_back(*shw);
   delete shw;
   shw = NULL;

   // Return vector pointer
   return stk;

   // Error.. Delete dynamic memory if allocated
   StockHistory_yahoo_full_scrape_end:
   if (stk != NULL)
   {
   delete stk;
   stk = NULL;
   }
   if (shw != NULL)
   {
   delete shw;
   shw = NULL;
   }

   return NULL;
}

#else

// Turns out that yahoo provides a JSON object with the page load which comes with the historical 
// data. The "shown" table only goes for about 4 months, and as the user scrolls down the rest of
// the table is filled from this JSON information.

std::vector<StockHistory_Week>* Stock::getYahooFinanceHistory_PageScrape(std::string _ticker, SimpleDate _start, SimpleDate _end)
{
   std::vector<StockHistory_Week>* stk = NULL;
   StockHistory_Week* shw = NULL;
   std::vector<StockHistory_Day> shdVec;

   HttpDownloader request;
   std::string returnStr;
   std::string tempStr;
   int stkDayW = 9;  // This will help us determine when we are starting a new week object
   char query[256] = { 0 };
   int i = 0;

   int s = 0;
   int s2 = 0;
   int e = 0;
   int e2 = 0;

   // Format:
   // https://finance.yahoo.com/quote/<ticker>/history?period1=<unix_timestamp>&period2=<unix_timestamp>&interval=1d&filter=history&frequency=1d

   #if defined(_MSC_VER) || defined(__WINDOWS__)
      sprintf_s(query, "https://finance.yahoo.com/quote/%s/history?period1=%llu&period2=%llu&interval=1d&filter=history&frequency=1d",
      _ticker.c_str(), _start.getUnixTimeStamp(), _end.getUnixTimeStamp());
   #else
      snprintf(query, 256, "https://finance.yahoo.com/quote/%s/history?period1=%llu&period2=%llu&interval=1d&filter=history&frequency=1d",
      _ticker.c_str(), _start.getUnixTimeStamp(), _end.getUnixTimeStamp());
   #endif

   // Request history
   returnStr = request.download(query);

   // Extract the relevant table information

   s = returnStr.find("HistoricalPriceStore", 0); // JSON Object Identifier
   if (s == std::string::npos)
   {
      fprintf(stderr, "Yahoo_Scrape: Bad return string 1.0\r\n");
      goto StockHistory_yahoo_scrape_end;
   }
   s = returnStr.find("[", s + 1);  // This is the start of JSON array
   if (s == std::string::npos)
   {
      fprintf(stderr, "Yahoo_Scrape: Bad return string 1.1\r\n");
      goto StockHistory_yahoo_scrape_end;
   }
   e = returnStr.find("]", s + 1);  // This is the end of JSON array
   if (e == std::string::npos)
   {
      fprintf(stderr, "Yahoo_Scrape: Bad return string 1.2\r\n");
      goto StockHistory_yahoo_scrape_end;
   }
   returnStr = returnStr.substr(s, e - s);

   // Loop through table information until we extract each stock day

   stk = new std::vector<StockHistory_Week>();  // Allocate a new vector to hold the week history
   shw = new StockHistory_Week();  // Allocate a new stk week object

   e = s = e2 = s2 = 0;

   while (true)
   {
      // Find and extract the next group of data, or determine if we are finished.

      std::string rowData;

      s = returnStr.find("{", e);
      if (s == std::string::npos)
      {
         break;
      }
      e = returnStr.find("}", s + 1);
      if (e == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 2.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }

      rowData = returnStr.substr(s, e - s);

      // Every so often, a row is going to contain dividend information. Skip it.
      // Just verify that the row of data has some of the various tags:
      //   'open', 'high', 'close', etc..

      s2 = e2 = 0;
      s2 = rowData.find("open", s2 + 1);
      e2 = rowData.find("volume", e2 + 1);
      if ((s2 == std::string::npos) || (e2 == std::string::npos))
      {
         continue;
      }

      // Extract each of the row items

      std::string rowItem;
      s2 = e2 = 0;

      // Extract the date - Format: UNIX TIME STAMP

      unsigned int year = 1;
      unsigned int day = 1;
      unsigned int month = 1;

      SimpleDate tempDate;
      time_t timeStamp = 0;

      s2 = rowData.find(":", 0);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 3.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      timeStamp = (time_t)(atoi(rowItem.c_str()));
      struct tm * time = gmtime(&timeStamp);

      day = time->tm_mday;
      month = time->tm_mon + 1;
      year = time->tm_year + 1900;
      tempDate.setDate(year, month, day);

      // Extract Open

      float open = 0.0;

      s2 = rowData.find(":", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 4.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         open = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 01: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_scrape_end;
      }

      // Extract High

      float high = 0.0;

      s2 = rowData.find(":", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 5.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         high = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 02: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_scrape_end;
      }

      // Extract Low

      float low = 0.0;

      s2 = rowData.find(":", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 6.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         low = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 03: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_scrape_end;
      }

      // Extract Close

      float close = 0.0;

      s2 = rowData.find(":", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 7.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         close = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 04: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_scrape_end;
      }

      // Extract Volume

      float volume = 0.0;

      s2 = rowData.find(":", e2);
      if (s2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.0\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      e2 = rowData.find(",", s2 + 1);
      if (e2 == std::string::npos)
      {
         fprintf(stderr, "Yahoo_Scrape: Bad return string 8.1\r\n");
         goto StockHistory_yahoo_scrape_end;
      }
      rowItem = rowData.substr(s2 + 1, e2 - s2 - 1);

      try
      {
         volume = std::stof(rowItem.c_str());
      }
      catch (std::exception)
      {
         fprintf(stderr, "stof fail 05: %s\n", rowItem.c_str());
         goto StockHistory_yahoo_scrape_end;
      }

      // Skip Adj Close
      // No Code Needed...

      // Determine if this stock day is part of the current week, or if we need to add a week.
      int dayW = SimpleDate::dayofweek(day, month, year);
      if (dayW > stkDayW)
      {
         // Are we at the 'start' (really end) of a new week?
         // Add vector of days to the week, then clean vector
         shw->addDayHistory(&shdVec);
         shdVec.clear();
         // Push the week object into the vector
         stk->push_back(*shw);
         delete shw;
         shw = new StockHistory_Week();
      }
      // Update our day of week checker
      stkDayW = dayW;

      // Create a new stock day object, and add it to the week vector, then clean up
      shdVec.push_back(StockHistory_Day(SimpleDate(year, month, day), open, close, high, low, (unsigned long)volume));
   }

   // Add vector of days to the week, then clean vector
   shw->addDayHistory(&shdVec);
   shdVec.clear();
   // We'll need to push the week object onto the vector
   stk->push_back(*shw);
   delete shw;
   shw = NULL;

   // Return vector pointer
   return stk;

   // Error.. Delete dynamic memory if allocated
   StockHistory_yahoo_scrape_end:
   if (stk != NULL)
   {
   delete stk;
   stk = NULL;
   }
   if (shw != NULL)
   {
   delete shw;
   shw = NULL;
   }

   return NULL;
}

#endif 

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************
March 22, 2016 - Jared Fowler
Moved from getting stock history from google finance to yahoo finance.
****************************************************************************************************
***************************************************************************************************/
