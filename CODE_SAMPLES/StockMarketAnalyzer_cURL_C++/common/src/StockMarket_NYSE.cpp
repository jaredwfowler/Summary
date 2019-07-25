//Dependencies======================================================================================
//==================================================================================================
#include "StockMarket_NYSE.hpp"

//Method Definitions================================================================================
//==================================================================================================

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
// Returns the time in New York City (NYSE) in Unix TimeStamp Format.
// This should be used with gmtime function to get the right time struct
//
//--------------------------------------------------------------------------------------------------
time_t StockMarket_NYSE::getNewYorkTime(void)
{
   HttpDownloader dwnld;
   std::string retStr = 
    dwnld.download("http://api.timezonedb.com/?zone=America/New_York&format=json&key=89JZJOKSD0OP");
   // Get the status of the download
   unsigned int i = retStr.find("\"status\":");
   if (i == std::string::npos)
   {
      fprintf(stderr, "StockMarket_NYSE::getNewYorkTime Download Failure \r\n");
      return 0;
   }
   i += 10;
   unsigned int j = retStr.find("\"", i);
   std::string status = retStr.substr(i, j - i);
   if (status != "OK")
   {
      fprintf(stderr, "StockMarket_NYSE::getNewYorkTime Status Failed \r\n");
      return 0;
   }
   i = retStr.find("\"timestamp\":");
   i += 12;
   j = retStr.find("}", i);
   std::string timeStamp = retStr.substr(i, j - i);
   unsigned int ts = std::stoi(timeStamp);
   return (time_t)ts;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
