//Dependencies======================================================================================
//==================================================================================================
#include "Light.hpp"
#include "Object3D.hpp"

//Method Definitions================================================================================
//==================================================================================================
Light::Light(const char* _id, LightInfoBasic _li, const char* _sourceID)
   :Element(_id)
{
   _li.color = this->verifyColor(_li.color);
   _li.intensity = this->verifyIntensity(_li.intensity);
   this->info = _li;
   this->setSourceID(_sourceID);
}

//**************************************************************************************************

Light::~Light(void)
{
   // Void
}

//**************************************************************************************************

glm::vec3 Light::verifyColor(glm::vec3 _v)
{
   if (_v.x < 0.0f) _v.x = 0.0f;
   if (_v.x > 1.0f) _v.x = 1.0f;
   if (_v.x < 0.0f) _v.y = 0.0f;
   if (_v.x > 1.0f) _v.y = 1.0f;
   if (_v.x < 0.0f) _v.z = 0.0f;
   if (_v.x > 1.0f) _v.z = 1.0f;

   return _v;
}

//**************************************************************************************************

float Light::verifyIntensity(float _i)
{
   if (_i < 0) _i = 0;

   return _i;
}

//**************************************************************************************************

void Light::setSourceID(const char* _source)
{
   if (_source == NULL) // AKA, this light is stationary, independent
   {
      strncpy(this->sourceID.id, "", MAX_ID_LEN - 1);
   }
   else
   {
      strncpy(this->sourceID.id, _source, MAX_ID_LEN - 1);
   }
}

//**************************************************************************************************

Light::LightInfoBasic Light::getLightStruct(void)
{
   return this->info;
}

//**************************************************************************************************

glm::mat4 Light::getLightShaderInfo(void)
{
   return glm::mat4(glm::vec4(this->info.position, this->info.intensity),
      glm::vec4(this->info.color, this->info.turnedOn),
      glm::vec4(this->info.direction, NA),
      glm::vec4(NA, NA, NA, NA));
}

//**************************************************************************************************

ID_GL Light::getSourceID(void)
{
   return this->sourceID;
}

//**************************************************************************************************

std::vector<GLuint> Light::getShadowTextureIDs(void)
{
   return std::vector<GLuint>();
}

//**************************************************************************************************

std::vector<glm::mat4> Light::getDepthBiasVPMatrices(void)
{
   return std::vector<glm::mat4>();
}

//**************************************************************************************************

bool Light::update(void)
{
   // Does this light belong to an object?
   if (strncmp(this->getSourceID().id, "", MAX_ID_LEN) == 0)
   {
      return false;
   }

   // Attempt to get object
   Object3D* objPtr = seCollections.getInstance(this->getSourceID().id);
   if (objPtr == NULL)
   {
      return false;
   }

   // Call the lightUpdate function
   LightInfoBasic _li = objPtr->updateLight(this->getLightStruct());
   _li.color = this->verifyColor(_li.color);
   _li.intensity = this->verifyIntensity(_li.intensity);
   this->info = _li;

   return true;
}
