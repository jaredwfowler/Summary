//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"

//Method Definitions================================================================================
//==================================================================================================
Element::Element(const char* _id)
{
   if (_id == NULL)
   {
      fprintf(stderr, "ID cannot be NULL\n");
      exit(NULL_ID);

   }

   strncpy(this->id.id, _id, MAX_ID_LEN - 1);
   refCount = 0;
}

//**************************************************************************************************

Element::~Element(void)
{
   //Void
}

//**************************************************************************************************

UINT32 Element::getReferneceCount(void)
{
   return this->refCount;
}

//**************************************************************************

ID_GL Element::getID(void)
{
   return this->id;
}

//**************************************************************************

bool Element::incReferenceCount(void)
{
   bool withinBounds = true;

   this->refCount++;

   if (this->refCount == 0)
   {
      withinBounds = false;
   }

   return withinBounds;
}

//**************************************************************************

bool Element::decReferenceCount(void)
{
   bool withinBounds = true;

   this->refCount--;

   if (this->refCount == -1)
   {
      withinBounds = false;
   }

   return withinBounds;
}
