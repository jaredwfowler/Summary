/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Light is a bit more complicated than 
** most other classes. Lights come in all different forms, such as directional,
** spot, and point. Because of the wide range, I've split these up into sub-
** classes.
*/

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;
class Object3D;

//CLASS :: Light ***********************************************************
//**************************************************************************
class Light : public Element
{
public:
   // Struct Defs ###################################################
   //################################################################
   typedef struct{
      glm::vec3 position;
      float intensity;
      glm::vec3 color;
      bool turnedOn;
      glm::vec3 direction;

   } LightInfoBasic;

   typedef struct{
      float minX;
      float maxX;
      float minY;
      float maxY;
      float nearClip;
      float farClip;
   } OrthoBounds;

   typedef struct{
      float fov;
      float aspectRatio;
      float nearClip;
      float farClip;
   } ProjectionBounds;

   // Constructor(s) / Destructor ###################################
   //################################################################
   Light(const char* _id, Simulator* _simPtr, Light::LightInfoBasic _li, const char* _sourceID);
   Light(Light& l);
   ~Light(void);

   // gets and sets #################################################
   //################################################################
   Light::LightInfoBasic getLightStruct(void);
   glm::mat4 getLightShaderInfo(void);
   ID_GL getSourceID(void);
   virtual std::vector<GLuint> getShadowTextureIDs(void);
   virtual std::vector<glm::mat4> getDepthBiasVPMatrices(void);

   // Auxilary Functions ############################################
   //################################################################
   //****************************************************************
   //If this light is linked to an Object3D, then that Object may
   //update this light's values. This will call the generic function
   //in Object3D which will return the new position of the light, along
   //with other indicators. It will be up to the user to provide
   //a pointer to the object, based upon the sourceID held by the light
   //****************************************************************
   virtual bool update(void);

private:
   LightInfoBasic info;        //Collection of light info
   ID_GL sourceID;  //ID of object which updates the light

   // gets and sets #################################################
   //################################################################
   glm::vec3 verifyColor(glm::vec3 _v);
   float verifyIntensity(float _i);
   void setSourceID(const char* _source);
};

//CLASS :: DirectionalLight ************************************************
//**************************************************************************
class DirectionalLight : public Light
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   DirectionalLight(const char* _id, Simulator* _simPtr, LightInfoBasic _li, OrthoBounds _ob, const char* _sourceID, const char* _depthShader);
   DirectionalLight(DirectionalLight& t);
   ~DirectionalLight(void);

   // gets and sets #################################################
   //################################################################
   ID_GL getDepthShaderID(void);
   virtual std::vector<GLuint> getShadowTextureIDs(void);
   virtual std::vector<glm::mat4> getDepthBiasVPMatrices(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   ID_GL depthShaderID;
   GLuint depthFrameBuffer;
   GLuint depthTexture;
   glm::mat4 depthBias_vpMatrix;
   glm::mat4 orthoProjection;
};

//CLASS :: SpotLight *******************************************************
//**************************************************************************
class SpotLight : public Light
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   SpotLight(const char* _id, Simulator* _simPtr, Light::LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID);
   SpotLight(SpotLight& t);
   ~SpotLight(void);

   // gets and sets #################################################
   //################################################################
   ID_GL getDepthShaderID(void);
   virtual std::vector<GLuint> getShadowTextureIDs(void);
   virtual std::vector<glm::mat4> getDepthBiasVPMatrices(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   ID_GL depthShaderID;
   GLuint depthFrameBuffer;
   GLuint depthTexture;
   glm::mat4 depthBias_vpMatrix;
   glm::mat4 projection;
};

//CLASS :: PointLight ******************************************************
//**************************************************************************
class PointLight : public Light
{
public:
   static struct CameraDirection
   {
      GLenum CubemapFace;
      glm::vec3 Target;
      glm::vec3 Up;
   };

   static const CameraDirection gCameraDirections[];

   // Constructor(s) / Destructor ###################################
   //################################################################
   PointLight(const char* _id, Simulator* _simPtr, Light::LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID);
   PointLight(PointLight& t);
   ~PointLight(void);

   // gets and sets #################################################
   //################################################################
   ID_GL getDepthShaderID(void);
   virtual std::vector<GLuint> getShadowTextureIDs(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   ID_GL depthShaderID;
   GLuint depthFrameBuffer;
   GLuint depthShadowMap;
   GLuint depthTexture;
   glm::mat4 projection;
};

#endif