//Dependencies======================================================================================
//==================================================================================================
#include "SimpleDate.hpp"

//Method Definitions================================================================================
//==================================================================================================
const char* SimpleDate::WEEK_DAYS[] = { "NULL", "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", 
                                        "THURSDAY", "FRIDAY", "SATURDAY" };
const char* SimpleDate::MONTHS[] = { "NULL", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
                                     "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER",
                                     "DECEMBER" };
const char* SimpleDate::MONTHS_ABR[] = { "NULL", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", 
                                         "AUG", "SEP", "OCT", "NOV", "DEC" };
const int SimpleDate::MONTH_DAYS[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//--------------------------------------------------------------------------------------------------
//Name:
// SimpleDate
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
SimpleDate::SimpleDate(unsigned int _year, unsigned int _month, unsigned int _day)
{
   setDate(_year, _month, _day);
}

SimpleDate::SimpleDate(time_t _t)
{
   struct tm * time = gmtime(&_t);
   setDate(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
}

//--------------------------------------------------------------------------------------------------
//Name:
// set*
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// true if successful
//
//About:
// Basic set methods. Each time we set the date we re-evaluate what day of the 
// week it falls on.
//
//--------------------------------------------------------------------------------------------------
bool SimpleDate::setDate(unsigned int _year, unsigned int _month, unsigned int _day)
{
   this->year = _year;
   this->month = _month;
   this->dayM = _day;
   this->dayW = this->dayofweek(this->dayM, this->month, this->year);
   return true;
}
bool SimpleDate::setYear(unsigned int _year)
{
   this->year = _year;
   this->dayW = this->dayofweek(this->dayM, this->month, this->year);
   return true;
}
bool SimpleDate::setMonth(unsigned int _month)
{
   this->month = _month;
   this->dayW = this->dayofweek(this->dayM, this->month, this->year);
   return true;
}
bool SimpleDate::setDay(unsigned int _day)
{
   this->dayM = _day;
   this->dayW = this->dayofweek(this->dayM, this->month, this->year);
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// static get*
//
//Arguments:
// _x -> offset starting at 1
//
//Output:
// void
//
//Returns:
// std::string. Returns "DNE" upon failure. 
//
//About:
// These functions simply look into our defined arrays and returns the requested information.
//
//--------------------------------------------------------------------------------------------------
std::string SimpleDate::getMonthName(unsigned int _x)
{
   if (_x <= 0 || _x >= 13)
      return std::string("DNE");
   return std::string(MONTHS[_x]);
}
std::string SimpleDate::getMonthAbr(unsigned int _x)
{
   if (_x <= 0 || _x >= 13)
      return std::string("DNE");
   return std::string(MONTHS_ABR[_x]);
}
std::string SimpleDate::getDayName(unsigned int _x)
{
   if (_x <= 0 || _x >= 8)
      return std::string("DNE");
   return std::string(WEEK_DAYS[_x]);
}

//--------------------------------------------------------------------------------------------------
//Name:
// static get*
//
//Arguments:
// _str -> name of month, either full name or abbreviation
//
//Output:
// void
//
//Returns:
// int. Returns -1 upon failure, else, returns 1-12
//
//About:
// Compares the string with the values in our predefined arrays. Returns the index of the matching 
// string.
//
//--------------------------------------------------------------------------------------------------
int SimpleDate::getMonth(std::string _str)
{
   // Change the input str to all uppercase
   std::string a = _str;
   for (unsigned int i = 0; i < _str.length(); i++)
   {
      a[i] = toupper(_str[i]);
   }
   // Compare the string to both month abr and name. If not found, return -1
   for (unsigned int i = 1; i <= 12; i++)
   {
      std::string b = MONTHS[i];
      std::string c = MONTHS_ABR[i];
      if ((a.compare(b) == 0) || (a.compare(c) == 0))
      {
         return i;
      }
   }
   return -1;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getUnixTimeStamp
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Returns the unix timestamp of the current simple date object
//
//--------------------------------------------------------------------------------------------------
time_t SimpleDate::getUnixTimeStamp(void)
{
   struct tm time = { 0 };

   time.tm_mday = this->dayM;
   time.tm_mon = (this->month - 1);
   time.tm_year = (this->year - 1900);

   return mktime(&time);
}

//--------------------------------------------------------------------------------------------------
//Name:
// today
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// today's date
//
//About:
// Returns today's date, taken from the system, as a SimpleDate object.
//
//--------------------------------------------------------------------------------------------------
SimpleDate SimpleDate::today(void)
{
   time_t t = time(0);   // get time now
   struct tm * now = localtime(&t);
   return SimpleDate(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
}

//--------------------------------------------------------------------------------------------------
//Name:
// addDays
//
//Arguments:
// _delta -> Number of days to add to the current date. Use a negative value for subtraction.
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Returns a new SimpleDate object which repesents the difference in delta days of the object that 
// called it.
//
//--------------------------------------------------------------------------------------------------
SimpleDate SimpleDate::addDays(int _delta) const 
{
   //Create a ctime date object
   struct tm date = { 0, 0, 12 };  // nominal time midday (arbitrary).
   date.tm_year = this->year - 1900;
   date.tm_mon = this->month - 1;  // note: zero indexed
   date.tm_mday = this->dayM;       // note: not zero indexed

   // Seconds since start of epoch
   time_t date_seconds = mktime(&date) + (_delta * 86400); // Where 86400 is sec/day

   // Update caller's date
   // Use localtime because mktime converts to UTC so may change date
   date = *localtime(&date_seconds);;

   return SimpleDate(date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
}

//--------------------------------------------------------------------------------------------------
//Name:
// dayofweek
//
//Arguments:
// _d -> day 1-31
// _m -> month 1-12
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Determines the day of the week. Code taken from: geeksforgeeks
//
//--------------------------------------------------------------------------------------------------
unsigned int SimpleDate::dayofweek(int _d, int _m, int _y) 
{
   static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
   _y -= _m < 3;
   return ((_y + _y / 4 - _y / 100 + _y / 400 + t[_m - 1] + _d) % 7) + 1;
}

//--------------------------------------------------------------------------------------------------
//Name:
// getDayDifference
//
//Arguments:
// _d2 -> object to compare to this
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Determines the number of days between this and input dates. Slightly Modified from geeksforgeeks.
//
//--------------------------------------------------------------------------------------------------
int SimpleDate::getDayDifference(const SimpleDate* _d2) const 
{

   if(_d2 == NULL) return -1;

   // COUNT TOTAL NUMBER OF DAYS BEFORE FIRST DATE 'dt1'

   // initialize count using years and day
   long int n1 = this->year * 365 + this->dayM;

   // Add days for months in given date
   for (size_t i = 0; i < this->month - 1; i++)
      n1 += MONTH_DAYS[i];

   // Since every leap year is of 366 days,
   // Add a day for every leap year
   n1 += countLeapYears(this->year, this->month);

   // SIMILARLY, COUNT TOTAL NUMBER OF DAYS BEFORE 'dt2'

   long int n2 = _d2->year * 365 + _d2->dayM;
   for (size_t i = 0; i<_d2->month - 1; i++)
      n2 += MONTH_DAYS[i];
   n2 += countLeapYears(_d2->year, _d2->month);

   // return difference between two counts
   return (n2 - n1);
}

//--------------------------------------------------------------------------------------------------
//Name:
// countLeapYears
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
// Determines the number of leap years from a given date
//
//--------------------------------------------------------------------------------------------------
int SimpleDate::countLeapYears(int _years, int _month) const 
{
   // Check if the current year needs to be considered
   // for the count of leap years or not
   if (_month <= 2)
      _years--;

   // An year is a leap year if it is a multiple of 4,
   // multiple of 400 and not a multiple of 100.
   return _years / 4 - _years / 100 + _years / 400;
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
// ...
//
//About:
// Operator overloads
//
//--------------------------------------------------------------------------------------------------
bool SimpleDate::operator< (const SimpleDate& _date) const 
{
   int _x = this->getDayDifference(&_date);
   if (_x <= 0) return false;
   else return true;
}
bool SimpleDate::operator> (const SimpleDate& _date) const
{
   int _x = this->getDayDifference(&_date);
   if (_x >= 0) return false;
   else return true;
}
bool SimpleDate::operator== (const SimpleDate& _date) const
{
   if ((this->year == _date.year) && (this->month == _date.month) && (this->dayM == _date.dayM))
      return true;
   else
      return false;
}
bool SimpleDate::operator!= (const SimpleDate& _date) const 
{
   return !(*this == _date);
}
bool SimpleDate::operator<= (const SimpleDate& _date) const 
{
   return ((*this < _date) || (*this == _date));
}
bool SimpleDate::operator>= (const SimpleDate& _date) const
{
   return ((*this > _date) || (*this == _date));
}

//--------------------------------------------------------------------------------------------------
//Name:
// toString*
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Translates this as a string and returns it.
//
//--------------------------------------------------------------------------------------------------
std::string SimpleDate::toString1(void) const
{
   char buffer[64] = { 0 };
   sprintf(buffer, "%5s %3u, %4u", MONTHS_ABR[this->getMonth()], this->getDayM(), this->getYear());
   return std::string(buffer);
}
std::string SimpleDate::toString2(void) const
{
   char buffer[64] = { 0 };
   sprintf(buffer, "%7s-%02u-%4u", MONTHS_ABR[this->getMonth()], this->getDayM(), this->getYear());
   return std::string(buffer);
}


/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************
March  3, 2015 - JFowler
Redid much of the file to include better commentaries and structure.

****************************************************************************************************
***************************************************************************************************/
