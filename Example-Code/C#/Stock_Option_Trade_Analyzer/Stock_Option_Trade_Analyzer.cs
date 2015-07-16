using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Windows.Forms;
using System.Collections.Specialized;

namespace Stock_Option_Trade_Analyzer
{
   //Class Stock************************************************
   //Holds basic information about a stock. Does not preform
   //its own operations such as data validation, rather, relies
   //upon its containing class.
   //***********************************************************
   class Stock : IComparable<Stock>
   {
      //Structs/Enums/EmbeddedClasses-------------------
      //------------------------------------------------
      //Class StockHistory******************************
      //Container class. Holds info about stock's history.
      //************************************************
      public class StockHistory : IComparable<StockHistory>
      {
         //Constructor(s)/Destructor-----------------------
         //------------------------------------------------
         public StockHistory(DateTime _date, float _open, float _high,
            float _low, float _close, float _volume){
            date = _date;
            open = _open;
            high = _high;
            low = _low;
            close = _close;
            volume = _volume;
         }

         //External Auxilary-------------------------------
         //------------------------------------------------
         public int CompareTo(StockHistory stkH) {
            return this.Date.CompareTo(stkH.Date);
         }
         public override string ToString() {
            return String.Format("{0},{1},{2},{3},{4},{5}",
               this.Date, this.Open, this.High, this.Low, this.Close, this.Volume);
         }
         public override bool Equals(object obj) {
            StockHistory stkH = obj as StockHistory;
            return stkH.Date.Equals(this.Date);
         }
         public override int GetHashCode() {
            return this.ToString().GetHashCode();
         }

         //Properties--------------------------------------
         //------------------------------------------------
         public DateTime Date {
            get { return new DateTime(this.date.Ticks); }
         }
         public float Open {
            get { return this.open; }
         }
         public float Close {
            get { return this.close; }
         }
         public float High {
            get { return this.high; }
         }
         public float Low {
            get { return this.low; }
         }
         public float Volume {
            get { return this.volume; }
         }

         //Variables---------------------------------------
         //------------------------------------------------
         private DateTime date;
         private float open;
         private float high;
         private float low;
         private float close;
         private float volume;
      }
      public class StockOptionChain : IComparable<StockOptionChain>
      {
         //Constructor(s)/Destructor-----------------------
         //------------------------------------------------
         public StockOptionChain(DateTime _expiration, float _strike, float _price, float _bid, float _ask){
            this.strike = _strike;
            this.expiration = _expiration;
            this.price = _price;
            this.bid = _bid;
            this.ask = _ask;
         }

         //External Auxilary-------------------------------
         //------------------------------------------------
         public int CompareTo(StockOptionChain stkO) {
            //Compare date first, then strike price
            int value = this.Expiration.CompareTo(stkO.Expiration) * 10;
            int value2 = this.Strike.CompareTo(stkO.Strike);

            if ((value > 0) || ((value == 0) && (value2 > 0)))
               return 1;
            else if ((value < 0) || ((value == 0) && (value2 < 0)))
               return -1;
            else
               return 0;
         }
         public override int GetHashCode() {
            return this.ToString().GetHashCode();
         }

         //Properties--------------------------------------
         //------------------------------------------------
         public DateTime Expiration {
            get { return this.expiration; }
            set { this.expiration = value; }
         }
         public float Strike {
            get { return this.strike; }
            set { this.strike = value; }
         }
         public float Price {
            get { return this.price; }
            set { this.price = value; }
         }
         public float Bid {
            get { return this.bid; }
            set { this.bid = value; }
         }
         public float Ask {
            get { return this.ask; }
            set { this.ask = value; }
         }

         //Variables---------------------------------------
         //------------------------------------------------
         private DateTime expiration;
         private float strike, price, bid, ask;
      }
      public struct Rating
      {
         public DateTime date;
         public float rating;
         public string rater;
         public Rating(float _rating, string _rater, DateTime _date) {
            rating = _rating; rater = _rater; date = _date;
         }
      }

      //Constructor(s)/Destructor-----------------------
      //------------------------------------------------

      public Stock(string _tickerSymbol, string _nameFull, List<StockHistory> _history, Rating _rating, List<StockOptionChain> _calls, List<StockOptionChain> _puts) {
         this.tickerSymbol = _tickerSymbol;
         this.nameFull = _nameFull;
         this.history = _history;
         this.rating = _rating;
         this.calls = _calls;
         this.puts = _puts;
      }
      public Stock(string _tickerSymbol, string _nameFull, List<StockHistory> _history, Rating _rating)
         : this(_tickerSymbol, _nameFull, _history, _rating, new List<StockOptionChain>(), new List<StockOptionChain>())
      {
         //Void
      }
      public Stock(string _tickerSymbol, string _nameFull, List<StockHistory> _history)
         :this(_tickerSymbol, _nameFull, _history, new Rating(0, "", new DateTime()))
      {
         //Void
      }
      public Stock(string _tickerSymbol, string _nameFull)
         :this(_tickerSymbol, _nameFull, new List<StockHistory>())
      {
         //Void
      }
      public Stock(string _tickerSymbol)
         : this(_tickerSymbol, "") {
         //Void
      }
      public Stock()
         : this("") {
         //Void
      }
      public Stock(Stock stk) 
         : this(stk.TickerSymbol, stk.NameFull, stk.History, stk.Rate, stk.calls, stk.puts)
      {
         //Void
      }

      //External Auxilary-------------------------------
      //------------------------------------------------
      public int CompareTo(Stock stk) {
         return this.TickerSymbol.CompareTo(stk.TickerSymbol);
      }
      public override string ToString() {
         return this.TickerSymbol;
      }
      public override bool Equals(object obj) {
         Stock stk = obj as Stock;
         return stk.TickerSymbol.Equals(this.TickerSymbol);
      }
      public override int GetHashCode() {
         return this.TickerSymbol.GetHashCode();
      }

      //Interface---------------------------------------
      //------------------------------------------------


      //Properties--------------------------------------
      //------------------------------------------------
      public string TickerSymbol {
         get { return this.tickerSymbol; }
      }
      public string NameFull {
         get { return this.nameFull; }
      }
      public List<StockHistory> History {
         get { return this.history; }
         set { this.history = value; }
      }
      public List<StockOptionChain> Calls {
         get { return this.calls; }
         set { this.calls = value; }
      }
      public List<StockOptionChain> Puts {
         get { return this.puts; }
         set { this.puts = value; }
      }
      public Rating  Rate{
         get { return this.rating; }
         set { this.rating = value; }
      }

      //Variables---------------------------------------
      //------------------------------------------------
      private string tickerSymbol;
      private string nameFull;
      private List<StockHistory> history;
      private List<StockOptionChain> calls;
      private List<StockOptionChain> puts;
      private Rating rating;
   }

   //Class Stock_Option_Trade_Analyzer**************************
   //Manages a collection of Stock objects, including data
   //validation checks, queries, etc. Has been specificially 
   //taylored to work with specific data tables from specific 
   //sources as found in the class definition.
   //***********************************************************
   class Stock_Option_Trade_Analyzer
   {
      //Constants---------------------------------------
      //------------------------------------------------
      //Format: <TickerSymbol>|<Security Name>|<..more stuff...>\n
      private const string TICKER_SYMBOL_LIST_1_URI = "ftp://ftp.nasdaqtrader.com/SymbolDirectory/otherlisted.txt";
      private const string TICKER_SYMBOL_LIST_2_URI = "ftp://ftp.nasdaqtrader.com/SymbolDirectory/nasdaqlisted.txt";
      private const string TICKER_SYMBOL_LIST_1_NAME = "otherlisted.txt";
      private const string TICKER_SYMBOL_LIST_2_NAME = "nasdaqlisted.txt";
      //Format: HISTORY_QUERY_BASE<TickerSymbol>&startdate=<yyyy-mm-dd>&enddate=<yyyy-mm-dd>&output=csv
      private const string HISTORY_QUERY_BASE = "https://www.google.com/finance/historical?q=";
      //Format: html... string Equity Summary Score: <div class="bullish"> , followed soon by (<score>)
      private const string FIDELITY_SCORE = "https://eresearch.fidelity.com/eresearch/goto/evaluate/snapshot.jhtml?symbols=";
      private const string FIDELITY_LOGIN = "https://login.fidelity.com/ftgw/Fas/Fidelity/RtlCust/Login/Response";
      //Options, puts and calls, request
      private const string OPTIONS_QUERY_BASE = "http://www.google.com/finance/option_chain?q="; //<symbol>&output=json

      private const UInt16 NYSM_UPPER_LIMIT = 10000;  //Upper limit to NewYorkStockMarket stock listing count
      private const UInt32 MAX_FILE_LENGTH = 5000000; //No file read/write should be over 5 MB
      private const UInt16 OLDEST_STOCK_YEAR = 1990;  //Limit lookups to 1990 and later

      private static readonly List<String> MONTHS_Mmm = new List<String>{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

      //Structs/Enums/Embedded Classes------------------
      //------------------------------------------------
      //************************************************
      //Helpful class I found on stack overflow by 
      //Darin Dimitrov. Helps with site login with
      //a cookie assignment.
      //************************************************
      public class WebClientEx : WebClient
      {
         public CookieContainer CookieContainer { get; private set; }

         public WebClientEx() {
            CookieContainer = new CookieContainer();
         }

         protected override WebRequest GetWebRequest(Uri address) {
            var request = base.GetWebRequest(address);
            if (request is HttpWebRequest) {
               (request as HttpWebRequest).CookieContainer = CookieContainer;
            }
            return request;
         }
      }
      public struct StockName : IComparable<StockName>
      {
         private string tickerSymbol;
         private string securityName;
         public StockName(string _tickerSymbol, string _securityName) {
            tickerSymbol = _tickerSymbol;
            securityName = _securityName;
         }
         public int CompareTo(StockName stk) {
            return this.TickerSymbol.CompareTo(stk.TickerSymbol);
         }
         public bool Equals(StockName stk) {
            if (this.TickerSymbol == stk.TickerSymbol &&
               this.SecurityName == stk.SecurityName)
               return true;
            return false;
         }
         public override string ToString() {
            return this.tickerSymbol;
         }
         public string TickerSymbol {
            get { return this.tickerSymbol; }
         }
         public string SecurityName {
            get { return this.securityName; }
         }
      }

      public enum Status_Codes
      {
         sCode_ALL_GOOD = 0x0000,
         sCode_DOES_NOT_EXIST = 0x0001,
         sCode_ALREADY_EXISTS = 0x0002,
         sCode_TICKER_LOOKUP_FAIL = 0x0004,
         sCode_HISTORY_LOOKUP_FAIL = 0x0008,
         sCode_INTERNET_CONNECT_FAIL = 0x0010,
         sCode_INVALID_DATE_RANGE = 0x0020,
         sCode_FILE_OPEN_ERROR = 0x0040,
         sCode_FILE_READ_ERROR = 0x0080,
         sCode_CORRUPT_FORMAT = 0x0100,
         sCode_FILE_WRITE_ERROR = 0x0200,
         sCode_FILE_CLOSE_ERROR = 0x0400,
         sCode_NULL_ARG_ERROR = 0x0800,
         sCode_FIDELITY_FAIL = 0x1000,
      }

      //Constructor(s)/Destructor-----------------------
      //------------------------------------------------
      public Stock_Option_Trade_Analyzer() {
         this.stockList = new List<Stock>();
         this.stockNameList = new List<StockName>(NYSM_UPPER_LIMIT);

         //Check for stock list input file. If Exists, set data
         string str = null;
         string str2 = null;
         UInt16 val = this.readFile(TICKER_SYMBOL_LIST_1_NAME, ref str);
         UInt16 val2 = this.readFile(TICKER_SYMBOL_LIST_2_NAME, ref str2);
         val |= this.parseSymbolList(str, ref this.stockNameList);
         val2 |= this.parseSymbolList(str2, ref this.stockNameList);
         this.stockNameList.Sort();

         //Failed?
         if ((val != (UInt16)Status_Codes.sCode_ALL_GOOD) || (val2 != (UInt16)Status_Codes.sCode_ALL_GOOD)) {
            //Attempt to update
            val = updateStockNameList();
            if (val != (UInt16)Status_Codes.sCode_ALL_GOOD)
               Console.WriteLine("StockName initialization failed.");
         }
      }

      //Interface---------------------------------------
      //------------------------------------------------
      //addStock****************************************
      //Adds a new Stock object to the local collection,
      //if it's a valid stock and doesn't already exist
      //in the local collection. Will add the stock's 
      //full name. Will not grab information online.
      //************************************************
      public UInt16 addStock(string _tickerSymbol) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Does this ticker symbol exist?
         _tickerSymbol = _tickerSymbol.ToUpper();
         Int32 index = this.stockNameList.BinarySearch(new StockName(_tickerSymbol, ""));
         if (index < 0) {
            returnStatus |= (UInt16)Status_Codes.sCode_TICKER_LOOKUP_FAIL;
            return returnStatus;
         }

         //Was the ticker symbol previously declared?
         if (this.stockList.Contains(new Stock(_tickerSymbol))) {
            returnStatus |= (UInt16)Status_Codes.sCode_ALREADY_EXISTS;
            return returnStatus;
         }

         //Everything's valid, so add it to the list and sort it
         this.stockList.Add(new Stock(this.stockNameList[index].TickerSymbol, this.stockNameList[index].SecurityName));
         this.stockList.Sort();

         return returnStatus;
      }
      //removeStock*************************************
      //Removes a single stock by name from the local
      //collection. Ensures that the list is ordered.
      //************************************************
      public UInt16 removeStock(string _tickerSymbol) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Attempt to remove value
         if (!this.stockList.Remove(new Stock(_tickerSymbol.ToUpper())))
            returnStatus |= (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
         else
            this.stockList.Sort();

         return returnStatus;
      }

      public void clearStock() {
         //Removes all the stocks from our collection
         this.stockList.Clear();
      }
      //getStock****************************************
      //Returns a given stock if it's in our stock
      //list. Returns a copy of the data, as to avoid
      //any form of data corruption.
      //************************************************
      public Stock getStock(string _tickerSymbol) {
         //Does that stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         Stock stk = this.stockList.Find(x => x.TickerSymbol == _tickerSymbol);
         if (stk == null)
            return stk;
         else
            return new Stock(stk);
      }
      //getStock****************************************
      //Returns a given stock if it's in our stock
      //list. Returns a copy of the data, as to avoid
      //any form of data corruption.
      //************************************************
      public UInt16 getStockName(string _tickerSymbol, ref StockName _stkN) {
         //Does that stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         _stkN = this.stockNameList.Find(x => x.TickerSymbol == _tickerSymbol);
         if (!_stkN.Equals(new StockName()))
            return (UInt16)Status_Codes.sCode_ALL_GOOD;
         else
            return (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
      }
      //updateStockHistory******************************
      //Requests stock history information from google
      //finance. The request is composed of the ticker-
      //symbol, and start and end dates. This information
      //is then parsed and placed into the appropriate obj
      //The actual dates aquired might vary, as the stock
      //market is closed on weekends and holidays.
      //************************************************
      public UInt16 updateStockHistory(string _tickerSymbol, DateTime _startDate, DateTime _endDate) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Does this Stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         if (!this.stockList.Contains(new Stock(_tickerSymbol))) { //DNE
            returnStatus |= (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
            goto updateStockHistory_END;
         }

         //Do the start and end dates make sense?
         if ((_startDate > _endDate) || (_endDate > DateTime.Now) || (_startDate.Year < OLDEST_STOCK_YEAR)) {
            returnStatus |= (UInt16)Status_Codes.sCode_INVALID_DATE_RANGE;
            goto updateStockHistory_END;
         }

         //Construct a web request string
         Stock stk = this.stockList.Find(x => x.TickerSymbol == _tickerSymbol);
         string requestStr = String.Format("{0}{1}&startdate={2:D4}-{3:D2}-{4:D2}&enddate={5:D4}-{6:D2}-{7:D2}&output=csv",
            HISTORY_QUERY_BASE, stk.TickerSymbol, _startDate.Year, _startDate.Month, _startDate.Day,
            _endDate.Year, _endDate.Month, _endDate.Day);

         //Set up webclient and make request
         byte[] buffer = null;
         WebClient request = new WebClient();
         try {
            buffer = request.DownloadData(requestStr);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_HISTORY_LOOKUP_FAIL;
            goto updateStockHistory_END;
         }
         string historyRaw = System.Text.Encoding.UTF8.GetString(buffer);

         //Clear the previous contents of stock's hitory, and parse in new info
         if (stk.History != null) stk.History.Clear();
         returnStatus |= parseStockHistory(historyRaw, ref stk);
         stk.History.Sort();

         updateStockHistory_END:
         return returnStatus;
      }
      //updateOptionInfo********************************
      //Requests the option chain information, puts and
      //calls which expire the coming friday.
      //************************************************
      public UInt16 updateOptionInfo(string _tickerSymbol) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Does this Stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         if (!this.stockList.Contains(new Stock(_tickerSymbol))) { //DNE
            returnStatus |= (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
            goto updateOptionInfo_END;
         }

         //Construct a web request string
         Stock stk = this.stockList.Find(x => x.TickerSymbol == _tickerSymbol);
         string requestStr = String.Format("{0}{1}&output=json", OPTIONS_QUERY_BASE, stk.TickerSymbol);

         //Set up webclient and make request
         byte[] buffer = null;
         WebClient request = new WebClient();
         try {
            buffer = request.DownloadData(requestStr);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_HISTORY_LOOKUP_FAIL;
            goto updateOptionInfo_END;
         }
         string optionsRaw = System.Text.Encoding.UTF8.GetString(buffer);

         //Clear the previous contents of stock's options, and parse in new info
         if (stk.Calls != null) stk.Calls.Clear();
         else stk.Puts = new List<Stock.StockOptionChain>();
         if (stk.Puts != null) stk.Puts.Clear();
         else stk.Calls = new List<Stock.StockOptionChain>();
         returnStatus |= parseOptions(optionsRaw, ref stk);
         stk.Calls.Sort();
         stk.Puts.Sort();

         updateOptionInfo_END:
         return returnStatus;
      }
      //updateFidelityInfo******************************
      //Grabs information specific to Fidelity.com,
      //including stock rating.
      //************************************************
      public UInt16 updateFidelityInfo(string _tickerSymbol, string _userName, string _password) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Does this Stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         if (!this.stockList.Contains(new Stock(_tickerSymbol))) { //DNE
            returnStatus |= (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
            goto updateFidelityInfo_END;
         }
         //Get a pointer to the stock we are to modify
         Stock stk = this.stockList.Find(x => x.TickerSymbol == _tickerSymbol);


         //Set up the request string
         string requestStr = String.Format("{0}{1}&type=sq-NavBar", FIDELITY_SCORE, stk.TickerSymbol);

         //Set up webclient and make request
         string resultRaw;
         using (var client = new WebClientEx()) {
            var values = new NameValueCollection
            {
                { "SSN", _userName },
                { "PIN", _password },
            };
            try {
               // Authenticate
               client.UploadValues(FIDELITY_LOGIN, values);
               // Download desired page
               resultRaw = client.DownloadString(requestStr);
            }
            catch {
               returnStatus |= (UInt16)Status_Codes.sCode_FIDELITY_FAIL;
               goto updateFidelityInfo_END;
            }
         }

         //Search for the keyline: <div class=\"bullish\"> ... (<rating>)
         //Where the rating will be between first set of ()
         Int32 index = resultRaw.IndexOf("<div class=\"bullish\">");
         if (index == -1) {
            returnStatus |= (UInt16)Status_Codes.sCode_FIDELITY_FAIL;
            goto updateFidelityInfo_END;
         }
         Int32 index2 = resultRaw.IndexOf('(', index);
         Int32 index3 = resultRaw.IndexOf(')', index);
         if ((index2 == -1) || (index3 == -1) || (index3 - index2 > 4) || (index2 > index + 50)) {
            returnStatus |= (UInt16)Status_Codes.sCode_FIDELITY_FAIL;
            goto updateFidelityInfo_END;
         }
         string ratingStr = resultRaw.Substring(index2 + 1, index3 - index2 - 1);
         float rating = (float)Convert.ToDouble(ratingStr);
         if ((rating <= 0) || (rating > 10)) {
            returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
            goto updateFidelityInfo_END;
         }
         //Search for the keyline: <span class="date-time block"> AS OF 
         //First occurance after the summary score
         string searchStr = "<span class=\"date-time block\"> AS OF ";
         index = resultRaw.IndexOf(searchStr, index);
         if (index == -1) {
            goto updateFidelityInfo_TIMEFAIL;
         }
         //Format is: MM/DD/YYYY
         index2 = index + searchStr.Length;
         int month = Convert.ToInt16(resultRaw.Substring(index2, 2));
         int day = Convert.ToInt16(resultRaw.Substring(index2 + 3, 2));
         int year = Convert.ToInt16(resultRaw.Substring(index2 + 6, 4));

         //Finally, update the stocks rating
         stk.Rate = new Stock.Rating(rating, "Fidelity", new DateTime(year, month, day));
         goto updateFidelityInfo_END;

         updateFidelityInfo_TIMEFAIL:
         stk.Rate = new Stock.Rating(rating, "Fidelity", new DateTime());

         updateFidelityInfo_END:
         return returnStatus;
      }
      //getDailyPercentChange***************************
      //For a given stock, calculates the percent change
      //from open to closing price for each day. Returns
      //a list of these values, not ordered. Will not 
      //delete the current contents of the list, only 
      //adds. If null, will create a new list.
      //************************************************
      public UInt16 getDailyPercentChange(string _tickerSymbol, ref List<float> _percentChange) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Does this Stock exist in our list?
         _tickerSymbol = _tickerSymbol.ToUpper();
         if (!this.stockList.Contains(new Stock(_tickerSymbol))) { //DNE
            returnStatus |= (UInt16)Status_Codes.sCode_DOES_NOT_EXIST;
            goto getDailyPercentChange_END;
         }
         Stock stk = this.stockList.Find(x => x.TickerSymbol == _tickerSymbol);

         //Null List?
         if (_percentChange == null) {
            _percentChange = new List<float>(stk.History.Count);
         }

         //For every day in the stock's history, find the percent difference
         for (UInt16 i = 0; i < stk.History.Count; i++) {
            //Avoid divide by zero error
            if (stk.History[i].Open == 0)
               continue;
            float percentChange = ((stk.History[i].Close - stk.History[i].Open) / stk.History[i].Open) * 100.0f;
            _percentChange.Add(percentChange);
         }

         getDailyPercentChange_END:
         return returnStatus;
      }

      //Auxilary----------------------------------------
      //------------------------------------------------
      //updateStockNameList*****************************
      //Requires internet connection. Grabs the latest
      //stock ticker name listings. Extracts all of the
      //ticker symbols and security names and puts them
      //into our local StockName list. This list is then
      //sorted. Will clear current content of list.
      //************************************************
      private UInt16 updateStockNameList() {
         //StatusReport
         UInt16 returnStatus = 0;

         byte[] buffer1 = null;
         byte[] buffer2 = null;
         FileStream file1 = null;
         FileStream file2 = null;

         //Set up the web requests and download files
         WebClient request1 = new WebClient();
         WebClient request2 = new WebClient();
         try {
            buffer1 = request1.DownloadData(TICKER_SYMBOL_LIST_1_URI);
            buffer2 = request2.DownloadData(TICKER_SYMBOL_LIST_2_URI);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_TICKER_LOOKUP_FAIL;
            goto updateStockNameList_END;
         }

         //Back-up information into files
         try {
            //Delete old files
            File.Delete(TICKER_SYMBOL_LIST_1_NAME);
            File.Delete(TICKER_SYMBOL_LIST_2_NAME);
            //Create files for writing
            file1 = new FileStream(TICKER_SYMBOL_LIST_1_NAME, FileMode.Create, FileAccess.Write);
            file2 = new FileStream(TICKER_SYMBOL_LIST_2_NAME, FileMode.Create, FileAccess.Write);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_OPEN_ERROR;
            goto updateStockNameList_END;
         }
         try {
            file1.Write(buffer1, 0, buffer1.Length);
            file2.Write(buffer2, 0, buffer2.Length);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_WRITE_ERROR;
            goto updateStockNameList_END;
         }
         try {
            file1.Close();
            file2.Close();
            file1 = null;
            file2 = null;
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_CLOSE_ERROR;
            goto updateStockNameList_END;
         }

         //Convert byte arrays to strings and update local list
         string str1 = System.Text.Encoding.UTF8.GetString(buffer1);
         string str2 = System.Text.Encoding.UTF8.GetString(buffer2);
         this.stockNameList.Clear();
         returnStatus |= this.parseSymbolList(str1, ref this.stockNameList);
         returnStatus |= this.parseSymbolList(str2, ref this.stockNameList);
         this.stockNameList.Sort();

         updateStockNameList_END:
         //Close the files and return
         try {
            if(file1 != null)
               file1.Close();
            if(file2 != null)
               file2.Close();
         }
         catch {
            //Void
         }

         return returnStatus;
      }
      //parseSymbolListFile*****************************
      //Takes in the character byte data which contains
      //ticker symbols and security names in the format:
      //<TickerSymbol>|<Security Name>|<..more stuff...>\n
      //and ends with File Creation Time: <val>|||||\n
      //The first line of the file is disregarded.
      //Parses it into StockName struct format. These
      //are then added to the passed in list object.
      //************************************************
      private UInt16 parseSymbolList(string _data, ref List<StockName> _list) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Instantiated list?
         if (_list == null)
            _list = new List<StockName>(NYSM_UPPER_LIMIT);

         //Non-null string?
         if (_data == null) {
            returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
            goto parseSymbolList_END;
         }

         UInt32 index = 0;
         UInt32 indexLineStart = index;
         UInt32[] separater = new UInt32[2];

         //Throw  out the first line of data
         while (index < _data.Length) {
            if (_data[(int)index] == '\n') {
               index++;
               break;
            }
            index++;
         }
         if (index >= _data.Length) {
            returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
            goto parseSymbolList_END;
         }

         //Get line, get | locations, extract data, repeat
         do {
            //Backup start of line index
            indexLineStart = index;

            //Find separaters
            for (UInt16 i = 0; i < 2; i++) {
               while (index < _data.Length) {
                  if (_data[(int)index] == '|') {
                     separater[i] = index;
                     index++;
                     break;
                  }
                  index++;
               }
            }

            //Find the end of line
            while (index < _data.Length) {
               if (_data[(int)index] == '\n') {
                  index++;
                  break;
               }
               index++;
            }

            //Have we reached the end of the string, or corrupted file?
            if(((separater[0] - indexLineStart) == 0) || (index > _data.Length)){ //Corrupt String
               returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
               break;
            }
            else if (separater[1] - separater[0] == 1){ //Successful End
               break;
            }

            //Pull out data and add it to the list
            StockName stockN = new StockName(_data.Substring((int)indexLineStart, (int)(separater[0] - indexLineStart)),
               _data.Substring((int)(separater[0] + 1), (int)(separater[1] - (separater[0] + 1))));
            _list.Add(stockN);

         } while (true);

         parseSymbolList_END:
         return returnStatus;
      }
      //parseOptions************************************
      //Format corresponds to a json string taken from 
      //google finances. 
      //p:"..." (price)
      //b:"..." (bid)
      //a:"..." (ask)
      //strike:"..." (strike)
      //expiry:"..." (expiration :: MMM D[D], YYYY
      //************************************************
      private UInt16 parseOptions(string _data, ref Stock _stk) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Puts are ordered before calls in the json.. so do them first.
         int index = _data.IndexOf("puts:");
         int indexCalls = _data.IndexOf("calls:");
         if (index == -1 || indexCalls == -1) {
            returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
            goto parseOptions_END;
         }

         //Iterate through...
         int endPuts = _data.IndexOf("]", index);
         while (index < endPuts) {
            int index2, index3;
            //Get the price
            index2 = _data.IndexOf("p:", index);
            if (index2 >= endPuts || index2 == -1)
               break;
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string price = _data.Substring(index2, index3 - index2);
            if (price.Equals("-")) price = "999999999999";
            //Get the bid
            index2 = _data.IndexOf("b:", index);
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string bid = _data.Substring(index2, index3 - index2);
            if (bid.Equals("-")) bid = "999999999999";
            //Get the ask
            index2 = _data.IndexOf("a:", index);
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string ask = _data.Substring(index2, index3 - index2);
            if (ask.Equals("-")) ask = "999999999999";
            //Get the strike
            index2 = _data.IndexOf("strike:", index);
            index2 += 8;
            index3 = _data.IndexOf("\"", index2);
            string strike = _data.Substring(index2, index3 - index2);
            if (strike.Equals("-")) strike = "999999999999";
            //Get the expiration date
            index2 = _data.IndexOf("expiry:", index);
            index2 += 8;
            index3 = _data.IndexOf("\"", index2);
            string expiration = _data.Substring(index2, index3 - index2);
            //Extract individual date components
            string monthMMM = expiration.Substring(0, 3);
            int month = MONTHS_Mmm.IndexOf(monthMMM) + 1;
            index2 = expiration.IndexOf(",");
            int day = Convert.ToInt32(expiration.Substring(4, index2 - 4));
            int year = Convert.ToInt32(expiration.Substring(index2 + 2, 4));

            //Add this to our puts list
            _stk.Puts.Add(new Stock.StockOptionChain(new DateTime(year, month, day), (float)Convert.ToDouble(strike), 
               (float)Convert.ToDouble(price), (float)Convert.ToDouble(bid), (float)Convert.ToDouble(ask)));

            //update the index
            index = index3;
         }
         index = indexCalls;
         int endCalls = _data.IndexOf("]", index);
         while (index < endCalls) {
            int index2, index3;
            //Get the price
            index2 = _data.IndexOf("p:", index);
            if (index2 >= endCalls || index2 == -1)
               break;
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string price = _data.Substring(index2, index3 - index2);
            if (price.Equals("-")) price = "999999999999";
            //Get the bid
            index2 = _data.IndexOf("b:", index);
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string bid = _data.Substring(index2, index3 - index2);
            if (bid.Equals("-")) bid = "999999999999";
            //Get the ask
            index2 = _data.IndexOf("a:", index);
            index2 += 3;
            index3 = _data.IndexOf("\"", index2);
            string ask = _data.Substring(index2, index3 - index2);
            if (ask.Equals("-")) ask = "999999999999";
            //Get the strike
            index2 = _data.IndexOf("strike:", index);
            index2 += 8;
            index3 = _data.IndexOf("\"", index2);
            string strike = _data.Substring(index2, index3 - index2);
            if (strike.Equals("-")) strike = "999999999999";
            //Get the expiration date
            index2 = _data.IndexOf("expiry:", index);
            index2 += 8;
            index3 = _data.IndexOf("\"", index2);
            string expiration = _data.Substring(index2, index3 - index2);
            //Extract individual date components
            string monthMMM = expiration.Substring(0, 3);
            int month = MONTHS_Mmm.IndexOf(monthMMM) + 1;
            index2 = expiration.IndexOf(",");
            int day = Convert.ToInt32(expiration.Substring(4, index2 - 4));
            int year = Convert.ToInt32(expiration.Substring(index2 + 2, 4));

            //Add this to our calls list
            _stk.Calls.Add(new Stock.StockOptionChain(new DateTime(year, month, day), (float)Convert.ToDouble(strike),
               (float)Convert.ToDouble(price), (float)Convert.ToDouble(bid), (float)Convert.ToDouble(ask)));

            //update the index
            index = index3;
         }

         parseOptions_END:
         return returnStatus;
      }
      //parseStockHistory*******************************
      //Format: Date,Open,High,Low,Close,Volume\n
      //where the first line should be dissregarded, 
      //date is in format <day[0,1,..,10,etc]>-<month [
      //Jan,Feb,etc]>-<yy> , and the rest of the values
      //are of type float. These values are added to the
      //passed in stock's embedded history class. It is
      //up to the caller to clear the list before hand.
      //************************************************
      private UInt16 parseStockHistory(string _data, ref Stock _stk) {
         //StatusReport
         UInt16 returnStatus = 0;

         //Instantiated values?
         if (_data == null) {
            goto parseStockHistory_FormatError;
         }
         if (_stk == null) {
            returnStatus |= (UInt16)Status_Codes.sCode_NULL_ARG_ERROR;
            goto parseStockHistory_END;
         }
         if (_stk.History == null) {
            _stk.History = new List<Stock.StockHistory>();
         }

         UInt32 index = 0;
         //Throw  out the first line of data
         while (index < _data.Length) {
            if (_data[(int)index] == '\n') {
               index++;
               break;
            }
            index++;
         }
         if (index >= _data.Length) {
            goto parseStockHistory_FormatError;
         }

         UInt32 indexStart = index;
         string[] dataRaw = new string[6];
         //Iterate through...
         while (index < _data.Length) {
            //Get the individual substrings
            for (Byte i = 0; i < 6; i++) {
               indexStart = index;
               while (index < _data.Length) {
                  if ((_data[(int)index] == ',') && (i != 5))
                     break;
                  else if ((i == 5) && (_data[(int)index] == '\n'))
                     break;
                  index++;
               }
               if (index >= _data.Length) {
                  returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;
                  goto parseStockHistory_FormatError;
               }
               dataRaw[i] = _data.Substring((int)indexStart, (int)(index - indexStart));
               index++;
            }
            
            //Parse the date substring
            int j, k;
            j = dataRaw[0].IndexOf('-', 0);
            if ((j == -1) || (j > 2)) {
               goto parseStockHistory_FormatError;
            }
            k = dataRaw[0].IndexOf('-', j + 1);
            if ((k == -1) || (k > 6)) {
               goto parseStockHistory_FormatError;
            }
            if((k - j - 1) != 3){
               goto parseStockHistory_FormatError;
            }
            int year = Convert.ToInt32(dataRaw[0].Substring(k + 1, dataRaw[0].Length - k - 1));
            int month = MONTHS_Mmm.FindIndex(x => x == dataRaw[0].Substring(j+1, k-j-1)) + 1;
            int day = Convert.ToInt32(dataRaw[0].Substring(0, j));
            if((year == 0) || (month == 0) || (day == 0)) //Error Check
               goto parseStockHistory_FormatError;
            DateTime date = new DateTime(year, month, day);

            //Check data to prevent errors
            for (int i = 1; i < 6; i++) {
               if (dataRaw[i] == "-")
                  dataRaw[i] = "0";
            }

            double val1 = Convert.ToDouble(dataRaw[1]);
            double val2 = Convert.ToDouble(dataRaw[2]);
            double val3 = Convert.ToDouble(dataRaw[3]);
            double val4 = Convert.ToDouble(dataRaw[4]);
            double val5 = Convert.ToDouble(dataRaw[5]);

            //Add StockHistory object to the list
            _stk.History.Add(new Stock.StockHistory(date, (float)val1,
               (float)val2, (float)val3, (float)val4, (float)val5));
         }

         goto parseStockHistory_END;
         parseStockHistory_FormatError:
         returnStatus |= (UInt16)Status_Codes.sCode_CORRUPT_FORMAT;

         parseStockHistory_END:
         return returnStatus;
      }
      //readFile****************************************
      //Simply opens a local file and reads the contents
      //of that file into a string.
      //************************************************
      private UInt16 readFile(string _filePath, ref string _data) {
         //StatusReport
         UInt16 returnStatus = 0;
         FileStream file = null;

         //Attempt to open the file for reading
         try {
            file = new FileStream(_filePath, FileMode.Open, FileAccess.Read);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_OPEN_ERROR;
            goto readFile_END;
         }

         //Attempt to read the entire file
         UInt32 fileLength = (UInt32)file.Length;
         if (fileLength > MAX_FILE_LENGTH) { //Within File Size?
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_READ_ERROR;
            goto readFile_END;
         }
         UInt32 bytesRead = 0;
         byte[] array = new byte[fileLength];
         try {
            bytesRead = (UInt32)file.Read(array, 0, (int)fileLength);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_READ_ERROR;
            goto readFile_END;
         }
         
         //Verify read integrity and convert to string
         if (bytesRead != fileLength) {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_READ_ERROR;
            goto readFile_END;
         }
         _data = Encoding.UTF8.GetString(array, 0, array.Length);

         readFile_END:
         //Close the file and return
         try {
            if(file != null)
               file.Close();
         }
         catch {
            //Void
         }
         return returnStatus;
      }
      //writeFile***************************************
      //Simply creates a new file and writes to it.
      //************************************************
      private UInt16 writeFile(string _buffer, string _filePath) {
         //StatusReport
         UInt16 returnStatus = 0;
         FileStream file = null;

         //Valid inputs
         if (_buffer == null || _filePath == null) {
            returnStatus |= (UInt16)Status_Codes.sCode_NULL_ARG_ERROR;
         }

         //Attempt to open the file for writing, delete if file name exists
         try {
            //Delete old files
            File.Delete(_filePath);
            //Create files for writing
            file = new FileStream(_filePath, FileMode.Create, FileAccess.Write);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_OPEN_ERROR;
            goto writeFile_END;
         }
         try {
            byte[] buffer = Encoding.Unicode.GetBytes(_buffer);
            file.Write(buffer, 0, buffer.Length);
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_WRITE_ERROR;
            goto writeFile_END;
         }
         try {
            file.Close();
            file = null;
         }
         catch {
            returnStatus |= (UInt16)Status_Codes.sCode_FILE_CLOSE_ERROR;
            goto writeFile_END;
         }

         writeFile_END:
         return returnStatus;
      }

      //Properties--------------------------------------
      //------------------------------------------------
      public List<Stock> StockList { //Returns Copy
         get { return new List<Stock>(this.stockList); }
      }
      public List<StockName> StockNameList { //Returns Copy
         get { return new List<StockName>(this.stockNameList); }
      }

      //Variables---------------------------------------
      //------------------------------------------------
      private List<Stock> stockList;
      private List<StockName> stockNameList;
   }
}
