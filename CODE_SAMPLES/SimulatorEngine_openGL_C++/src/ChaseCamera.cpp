//Dependencies======================================================================================
//==================================================================================================
#include "ChaseCamera.hpp"
#include "Object3D.hpp"

//Method Definitions================================================================================
//==================================================================================================
ChaseCamera::ChaseCamera(const char* _id, glm::vec3 _eyeOffset, glm::vec3 _atOffset, const char* _objID)
   :Camera(_id, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))
{
   if (_objID == NULL)
   {
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }
   strncpy(this->objID.id, _objID, MAX_ID_LEN - 1);

   this->eyeOffset = _eyeOffset;
   this->atOffset = _atOffset;
}

//**************************************************************************************************

ChaseCamera::~ChaseCamera(void)
{
   // Void
}

//**************************************************************************************************

glm::mat4 ChaseCamera::getViewMatrix(void)
{
   // Uses parent function. Relys upon the update function being called
   return Camera::getViewMatrix();
}

//**************************************************************************************************

ID_GL ChaseCamera::getObjID(void)
{
   return this->objID;
}

//**************************************************************************************************

glm::vec3 ChaseCamera::getEyeOffset(void)
{
   return this->eyeOffset;
}

//**************************************************************************************************

glm::vec3 ChaseCamera::getAtOffset(void)
{
   return this->atOffset;
}

//**************************************************************************************************

bool ChaseCamera::update(void)
{
   // Attempt to get object
   Object3D* objPtr = seCollections.getInstance(this->getObjID().id);
   if (objPtr == NULL)
   {
      return false;
   }

   // UP is the exact same as obj's up
   this->setUp(objPtr->getLMM_Up());

   // We need to translate the offsets to match the model's point of view
   glm::vec3 offX = get3D_ScaledVector(objPtr->getLMM_Right(), this->eyeOffset.x);
   glm::vec3 offY = get3D_ScaledVector(objPtr->getLMM_Up(), this->eyeOffset.y);
   glm::vec3 offZ = get3D_ScaledVector(objPtr->getLMM_At(), this->eyeOffset.z);
   glm::vec3 offset = offX + offY + offZ;
   this->setEye(offset + objPtr->getLMM_Translation());

   offX = get3D_ScaledVector(objPtr->getLMM_Right(), this->atOffset.x);
   offY = get3D_ScaledVector(objPtr->getLMM_Up(), this->atOffset.y);
   offZ = get3D_ScaledVector(objPtr->getLMM_At(), this->atOffset.z);
   offset = offX + offY + offZ;
   this->setAt(offset + objPtr->getLMM_Translation());

   return true;
}
