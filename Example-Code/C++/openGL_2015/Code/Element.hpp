/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Element will act as the ancestor class,
** for all other classes used by Simulator. Every element will be given an
** identifier. Simulator will make sure that the id is unique for every
** separate collection. Each element will also keep a count of how many other
** objects are referencing it. The accuracy of this count is up to the programmer
** to update appropriately. If done correctly, it could help with the management
** of object deletion.
*/

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: Element *********************************************************
//**************************************************************************
class Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Element(const char* _id, Simulator* _simPtr);
   Element(Element& e);
   ~Element(void);

   // gets and sets #################################################
   //################################################################
   virtual UINT32 getReferneceCount(void);
   ID_GL getID(void); //Virtual is needed to make polymorphic

   // Auxiliary Functions ###########################################
   //################################################################
   bool incReferenceCount(void);
   bool decReferenceCount(void);

protected:
   Simulator* simPtr;

private:
   ID_GL id;
   UINT32 refCount;
};

#endif