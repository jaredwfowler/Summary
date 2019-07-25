//Dependencies======================================================================================
//==================================================================================================
#include "ControlBind.hpp"

//Method Definitions================================================================================
//==================================================================================================
ControlBind::ControlBind(const char* _id, KeyPressInfo _k, bool(*_callBack)(UINT16, UINT8))
   :Element(_id)
{
   if (_callBack == NULL)
   {
      fprintf(stderr, "Callback cannot be NULL\n");
      exit(CONTROL_FAIL);
   }

   this->kpInfo = _k;
   this->callBack = _callBack;
}

//**************************************************************************

ControlBind::~ControlBind(void)
{
   this->callBack = NULL;
}

//**************************************************************************

KeyPressInfo ControlBind::getKeyPressInfo(void)
{
   return this->kpInfo;
}

//**************************************************************************

bool ControlBind::runCallBack(void)
{
   return this->callBack(this->kpInfo.keyPress, this->kpInfo.modifiers);
}

//**************************************************************************

ID_GL ControlBind::getStringHash(KeyPressInfo _kpi)
{
   if (sizeof(KeyPressInfo) > MAX_ID_LEN - 1)
   {
      fprintf(stderr, "ControlBind Hash Fail. Need to increase MAX_ID_LEN\n");
      exit(CONTROL_FAIL);
   }

   ID_GL str;
   UINT8* ptr = (UINT8*)(&_kpi);
   sprintf_s(str.id, "%X::%X", _kpi.keyPress, _kpi.modifiers);
   return str;
}
