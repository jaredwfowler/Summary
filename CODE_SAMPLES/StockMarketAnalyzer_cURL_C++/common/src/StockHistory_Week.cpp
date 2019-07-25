//Dependencies======================================================================================
//==================================================================================================
#include "StockHistory_Week.hpp"
#include <exception>
#include <float.h>

#include "HttpDownloader.hpp"  //TODO Remove this

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// StockHistory_Week
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
// Default Constructor. Values are set default b/c the class will take care of updating the 
// member values based on the day objects passed in.
//
//--------------------------------------------------------------------------------------------------
StockHistory_Week::StockHistory_Week()
{
   this->setDate(SimpleDate(1,1,1));
   this->setCloseDate(SimpleDate(1, 1, 1));
   this->setOpen(-1);
   this->setClose(-1);
   this->setHigh(-1);
   this->setLow(-1);
   this->setAvgVolume(-1);
   this->setTotalVolume(-1);
   this->setStockDays(0);
   this->setPercentChange(-1);
   for (int i = 0; i < 5; i++)
   {
      this->dayHistory[i] = NULL;
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// StockHistory_Week
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
// Copy Constructor. Utilizes the assignment overload.
//
//--------------------------------------------------------------------------------------------------
StockHistory_Week::StockHistory_Week(const StockHistory_Week& _stk)
{
   StockHistory_Week::copyThis(this, &_stk, false);
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~StockHistory_Week
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
// Destructor.
//
//--------------------------------------------------------------------------------------------------
StockHistory_Week::~StockHistory_Week()
{
   for (int i = 0; i < 5; i++)
   {
      if (this->dayHistory[i] != NULL)
      {
         delete this->dayHistory[i];
         this->dayHistory[i] = NULL;
      }
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// getDayHistory
//
//Arguments:
// _i -> Array offset (0-4), where 0 is Monday and 4 is Friday
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Returns a copy, stored in the heap, of the StockHistory_Day object.
// If the object doesn't exist, Null is returned.
//
//--------------------------------------------------------------------------------------------------
StockHistory_Day* StockHistory_Week::getDayHistory(unsigned int _i)
{
   if ((_i > 4) || (dayHistory[_i] == NULL))
   {
      return NULL;
   }

   return (new StockHistory_Day(*dayHistory[_i]));
}

//--------------------------------------------------------------------------------------------------
//Name:
// addDayHistory
//
//Arguments:
// *_stk -> Pointer to a StockHistory_Day object
//
//Output:
// ...
//
//Returns:
// bool - true if successfully added, else false.
//
//About:
// Takes in a StockHistory_Day object, creates a copy of it in the heap and points to it. If a 
// previous Day already exists with the same date, the old will be deleted and the new will take 
// its place. We will make sure that there are no duplicated dates, and also verify that the dates 
// make sense. Upon every entry, we will find the new weekly values.
//
//--------------------------------------------------------------------------------------------------
bool StockHistory_Week::addDayHistory(const StockHistory_Day* _stk)
{
   // NULL Check
   if (_stk == NULL)
   {
      fprintf(stderr, "StockHistory_Week::addDayHistory null input\n");
      return false;
   }

   // Check Sanity
   if (!dayHistorySanityCheck(_stk))
   {
      fprintf(stderr, "StockHistory_Week::addDayHistory sanity check fail\n");
      return false;
   }

   // Get offset
   int offset = getDayOffset(_stk);
   if (offset == -1)
   {
      fprintf(stderr, "StockHistory_Week::addDayHistory day offset fail\n");
      return false;
   }

   // Set new value
   if (this->dayHistory[offset] != NULL)
   {
      // Erase old content
      delete this->dayHistory[offset];
   }
   this->dayHistory[offset] = new StockHistory_Day(*_stk);

   // Update the week's values
   this->updateWeekValues();

   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// addDayHistory
//
//Arguments:
// std::vector<StockHistory_Day>* -> Pointer to a vector of StockHistory_Day's
//
//Output:
// ...
//
//Returns:
// bool - Returns true... should this return false?
//
//About:
// Same idea as other addDayHistory, but this one is a bit more efficient in that we don't determine
// weekly information until we finish adding all the days passed in.
//
//--------------------------------------------------------------------------------------------------
bool StockHistory_Week::addDayHistory(const std::vector<StockHistory_Day>* _stk) 
{
   // NULL Check
   if (_stk == NULL)
   {
      fprintf(stderr, "StockHistory_Week::addDayHistory null input\n");
      return false;
   }

   // Loop through each of the elements. 
   for(unsigned int i = 0; i < _stk->size(); i++)
   {

      StockHistory_Day shd(_stk->at(i));

      // Check Sanity
      if (!dayHistorySanityCheck(&shd))
      {
         fprintf(stderr, "StockHistory_Week::addDayHistory sanity check fail\n");
         continue;
      }

      // Get offset
      int offset = getDayOffset(&shd);
      if (offset == -1)
      {
         fprintf(stderr, "StockHistory_Week::addDayHistory day offset fail\n");
         continue;
      }

      // Set new value
      if (this->dayHistory[offset] != NULL)
      {
         // Erase old content
         delete this->dayHistory[offset];
      }
      this->dayHistory[offset] = new StockHistory_Day(shd);

   }

   // Update the week's values
   this->updateWeekValues();

   return true;
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
// Overloaded operators
//
//--------------------------------------------------------------------------------------------------
bool StockHistory_Week::operator< (const StockHistory_Week& _stk) const 
{
   SimpleDate a = _stk.getDate();
   if (this->getDate() < a) return true;
   return false;
}

void StockHistory_Week::operator= (const StockHistory_Week& _stk)
{
   StockHistory_Week::copyThis(this, &_stk, true);
}

//--------------------------------------------------------------------------------------------------
//Name:
// copyThis
//
//Arguments:
// _dest -> destination to write to
// _src  -> source to read from
// _init -> has the _dest already been initialized before? (Copy Constructor vs Assignment)
//
//Output:
// void
//
//Returns:
// ...
//
//About:
// Used by xopy constructor and assignment operator
//
//--------------------------------------------------------------------------------------------------
void StockHistory_Week::copyThis(StockHistory_Week* _dest, const StockHistory_Week* _src, bool _init)
{
   _dest->date = _src->date;
   _dest->open = _src->open;
   _dest->close = _src->close;
   _dest->high = _src->high;
   _dest->low = _src->low;

   // This loop needs to be done SEPARATELY b/c of sanity checker.
   for (int i = 0; i < 5; i++)
   { 
      // If not NULL, delete first
      if(_init && (_dest->dayHistory[i] != NULL))
      {
         delete _dest->dayHistory[i];
      }
      _dest->dayHistory[i] = NULL;
   }

   for (int i = 0; i < 5; i++)
   {
      if (_src->dayHistory[i] != NULL)
      {
         _dest->addDayHistory(_src->dayHistory[i]);
      }
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// sortByOCpercentChange
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
// Used by the STL to sort this object when it exists in a vector.
//
//--------------------------------------------------------------------------------------------------
bool StockHistory_Week::sortByOCpercentChange(const StockHistory_Week& _s1, 
                                              const StockHistory_Week& _s2)
{
   return (_s1.getPercentChange() < _s2.getPercentChange());
}

//--------------------------------------------------------------------------------------------------
//Name:
// getDayOffset
//
//Arguments:
// *_stk -> stock day object we want to find the offset for
//
//Output:
// void
//
//Returns:
// Offset in our member array of 5 elements. Returns -1 if not Mon-Fri
//
//About:
// Determines which day of the week this history's date falls on. (1-7). We expect days 2-6, which 
// are possible open days in the market.
//
//--------------------------------------------------------------------------------------------------
int StockHistory_Week::getDayOffset(const StockHistory_Day* _stk) const 
{
   if (_stk == NULL)
      return 1;
   unsigned int dayW = (_stk->getDate()).getDayW();
   if (dayW <= 1 || dayW >= 7)
      return -1;
   else
      return dayW - 2;
}

//--------------------------------------------------------------------------------------------------
//Name:
// dayHistorySanityCheck
//
//Arguments:
// *_stk -> StockHistory_Day object to compare to current member vector
//
//Output:
// void
//
//Returns:
// bool - true if 
//
//About:
// Preforms a sanity check to determine if a StockHistory_Day object is sequential with the already 
// inserted objects. If the argument is null, a sanity check wil be preformed for all the objects 
// available. If sane, returns true, else, returns false.
//
//--------------------------------------------------------------------------------------------------
bool StockHistory_Week::dayHistorySanityCheck(const StockHistory_Day* _stk) const 
{
   int i, j;
   // Run through the entire list
   if (_stk == NULL)
   {
      // Get the first available object
      for (i = 0; i < 5; i++)
      {
         if (this->dayHistory[i] != NULL)
         {
            _stk = this->dayHistory[i];
            j = i;
            i++;
            break;
         }
      }
      // Determine if the days are sequential
      for (; i < 5; i++)
      {
         if (this->dayHistory[i] != NULL)
         {
            SimpleDate sdTemp = _stk->getDate();
            int delta = this->dayHistory[i]->getDate().getDayDifference(&sdTemp);
            if ((j - i) != delta)
            {
               fprintf(stderr, "Sanity check 1 failed. Delta=%d\n", delta);
               return false;
            }
            else
            {
               _stk = this->dayHistory[i];
               j = i;
            }
         }
      }
   }
   // Only compare the new insert
   else
   {
      // Get the offset of the object
      int offset = getDayOffset(_stk);
      if (offset == -1) return false;
      for (i = 0; i < 5; i++)
      {
         // Skip the same offset
         if (i == offset) 
            continue;
         if (this->dayHistory[i] != NULL)
         {
            SimpleDate sdTemp = _stk->getDate();
            int delta = this->dayHistory[i]->getDate().getDayDifference(&sdTemp);
            if ((offset - i) != delta)
            {
               fprintf(stderr, "Sanity check 2 failed. Delta=%d\n", delta);
               return false;
            }
         }
      }
   }
   return true;
}

//--------------------------------------------------------------------------------------------------
//Name:
// update*
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
// Analyzing the individual day histories, we get values for the week.
//
//--------------------------------------------------------------------------------------------------
void StockHistory_Week::updateWeekValues(void)
{
   this->updateClose();
   this->updateOpen();
   this->updateHigh();
   this->updateLow();
   this->updateDate();
   this->updateCloseDate();
   this->updateVolume();
   this->updatePercentChange();
}
void StockHistory_Week::updateDate(void)
{
   for (int i = 0; i < 5; i++)
   {
      if (this->dayHistory[i] != NULL)
      {
         this->setDate(this->dayHistory[i]->getDate());
         break;
      }
   }
   return;
}
void StockHistory_Week::updateCloseDate(void)
{
   for (int i = 4; i >= 0; i--)
   {
      if (this->dayHistory[i] != NULL)
      {
         this->setCloseDate(this->dayHistory[i]->getDate());
         break;
      }
   }
   return;
   }
   void StockHistory_Week::updateOpen(void)
   {
   for (int i = 0; i < 5; i++)
   {
      if (this->dayHistory[i] != NULL)
      {
         this->setOpen(this->dayHistory[i]->getOpen());
         break;
      }
   }
   return;
   }
   void StockHistory_Week::updateClose(void)
   {
   for (int i = 4; i >= 0; i--)
   {
      if (this->dayHistory[i] != NULL)
      {
         this->setClose(this->dayHistory[i]->getClose());
         break;
      }
   }
   return;
}
void StockHistory_Week::updateHigh(void)
{
   float a = this->getHigh();
   for (int i = 0; i < 5; i++)
   {
      if (this->dayHistory[i] != NULL)
      {
         float b = this->dayHistory[i]->getHigh();
         if (b > a)
            a = b;
      }
   }
   this->setHigh(a);
   return;
}
void StockHistory_Week::updateLow(void)
{
   float a = FLT_MAX;
   for (int i = 0; i < 5; i++)
   {
      if (this->dayHistory[i] != NULL)
      {
         float b = this->dayHistory[i]->getLow();
         if (b < a)
            a = b;
      }
   }
   if (a == FLT_MAX)
      a = -1;
   this->setLow(a);
   return;
}
void StockHistory_Week::updateVolume(void)
{
   unsigned long a = 0;
   unsigned char count = 0;
   for (int i = 0; i < 5; i++)
   {
       if (this->dayHistory[i] != NULL)
       {
            count++;
            a += this->dayHistory[i]->getVolume();
       }
   }
   this->setTotalVolume(a);
   this->setStockDays(count);
   a /= count;
   this->setAvgVolume(a);
   return;
}
void StockHistory_Week::updatePercentChange(void)
{
   this->setPercentChange((this->close - this->open) / this->open);
   return;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************
April 28, 2016 - JFowler
Added variables to keep track of total volume and number of days occupied in day history array.
This was done in response to a need to compare avg. volumes over a course of many weeks by 
client objects.
****************************************************************************************************
***************************************************************************************************/
