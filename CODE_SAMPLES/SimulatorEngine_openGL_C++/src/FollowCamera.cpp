//Dependencies======================================================================================
//==================================================================================================
#include "FollowCamera.hpp"
#include "Object3D.hpp"

//Method Definitions================================================================================
//==================================================================================================
FollowCamera::FollowCamera(const char* _id, glm::vec3 _eyeOffset, const char* _objID)
   :Camera(_id, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))
{
   if (_objID == NULL)
   {
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }
   strncpy(this->objID.id, _objID, MAX_ID_LEN - 1);

   this->eyeOffset = _eyeOffset;
}

//**************************************************************************************************

FollowCamera::~FollowCamera(void)
{
   //Void
}

//**************************************************************************************************

glm::mat4 FollowCamera::getViewMatrix(void)
{
   // Uses parent function. Relies upon the update function being called
   return Camera::getViewMatrix();
}

//**************************************************************************************************

ID_GL FollowCamera::getObjID(void)
{
   return this->objID;
}

//**************************************************************************************************

glm::vec3 FollowCamera::getEyeOffset(void)
{
   return this->eyeOffset;
}

//**************************************************************************************************

bool FollowCamera::update(void)
{
   // Attempt to get object
   Object3D* objPtr = seCollections.getInstance(this->getObjID().id);
   if (objPtr == NULL)
   {
      return false;
   }

   // UP is the exact same as obj's up
   this->setUp(objPtr->getLMM_Up());

   // Eye position found in world_view terms, not models
   this->setEye(this->eyeOffset + objPtr->getLMM_Translation());

   // Camera will be directed at the model.
   this->setAt(objPtr->getLMM_Translation());

   return true;
}
