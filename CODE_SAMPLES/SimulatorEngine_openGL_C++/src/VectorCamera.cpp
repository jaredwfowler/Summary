//Dependencies======================================================================================
//==================================================================================================
#include "VectorCamera.hpp"
#include "Object3D.hpp"

//Method Definitions================================================================================
//==================================================================================================
VectorCamera::VectorCamera(const char* _id, float _distZ, float _distY, const char* _obj1, const char* _obj2)
   :Camera(_id, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0,1,0))
{
   this->distZ = _distZ;
   this->distY = _distY;

   if (_obj1 == NULL || _obj2 == NULL)
   {
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }

   strncpy(this->objA.id, _obj1, MAX_ID_LEN - 1);
   strncpy(this->objB.id, _obj2, MAX_ID_LEN - 1);
}

//**************************************************************************************************

VectorCamera::~VectorCamera(void)
{
   // Void
}

//**************************************************************************************************

glm::mat4 VectorCamera::getViewMatrix(void)
{
   // Uses parent function. Relies upon the update function being called
   return Camera::getViewMatrix();
}

//**************************************************************************************************

ID_GL VectorCamera::getObjA(void)
{
   return this->objA;
}

//**************************************************************************************************

ID_GL VectorCamera::getObjB(void)
{
      return this->objB;
}

//**************************************************************************************************

bool VectorCamera::update(void)
{
   // Attempt to get object_1
   Object3D* _obj1 = seCollections.getInstance(this->getObjA().id);
   if (_obj1 == NULL)
   {
      return false;
   }

   // Attempt to get object_2
   Object3D* _obj2 = seCollections.getInstance(this->getObjB().id);
   if (_obj2 == NULL)
   {
      return false;
   }

   // UP is the exact same as obj1's up
   this->setUp(_obj1->getLMM_Up());

   // AT is the far objects position
   this->setAt(_obj2->getLMM_Translation());

   // EYE is the combination of _obj1's position, mixed with the distZ and distY offsets
   glm::vec3 yOffset = get3D_ScaledVector(_obj1->getLMM_Up(), this->distY);
   glm::vec3 zOffset = get3D_ScaledVector((_obj1->getLMM_Translation() - _obj2->getLMM_Translation()), this->distZ);
   this->setEye(_obj1->getLMM_Translation() + yOffset + zOffset);

   return true;
}
