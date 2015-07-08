/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. ControlBind refers to user defined 
** controls. This basic class will store input information along with 
** the corresponding function call_back.
*/

#ifndef CONTROLBIND_HPP
#define CONTROLBIND_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: ControlBind *****************************************************
//**************************************************************************
class ControlBind : public Element
{
public:
   //STRUCTS ########################################################
   //################################################################
   typedef struct{
      UINT16 keyPress;
      UINT8 modifiers;
   } KeyPressInfo;

   // Constructor(s) / Destructor ###################################
   //################################################################
   ControlBind(const char* _id, Simulator* _simPtr, ControlBind::KeyPressInfo _k, bool(*_callBack)(UINT16, UINT8));
   ControlBind(ControlBind& c);
   ~ControlBind(void);

   // gets and sets #################################################
   //################################################################
   inline ControlBind::KeyPressInfo getKeyPressInfo(void);

   // Auxiliary Functions ###########################################
   //################################################################
   bool runCallBack(void);
   static ID_GL getStringHash(ControlBind::KeyPressInfo _kpi);

private:
   ControlBind::KeyPressInfo kpInfo;
   bool(*callBack)(UINT16, UINT8); //Pointer to function callback
};

#endif