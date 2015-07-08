/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Object3D will reference which model,
** textures, and shader it will use via id, which is a special attribute
** defined by the user when adding elements to Simulators collections.
** Object3D is the base class to which several other children class should
** be written. The idea is to produce a polymorphic effect.
*/

#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;
class Light;

//CLASS :: Object3D*********************************************************
//**************************************************************************
class Object3D : public Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Object3D(const char* _id,
      Simulator* _simPtr,
      std::vector<char*> _textureIDs,
      std::vector<char*> _modelIDs,
      std::vector<char*> _lightIDs,
      const char* _colorShaderID,
      bool _isTransparent,
      glm::mat4 _scaleMatrix,
      glm::mat4 _translationMatrix, 
      glm::mat4 _rotationMatrix);
   Object3D(Object3D& obj);
   ~Object3D();

   // gets and sets #################################################
   //################################################################
   inline bool isTransparent(void);
   inline void setTransparent(bool b);

   inline glm::mat4 getRotationMatrix(void);
   inline bool setRotationMatrix(glm::mat4 rm);

   inline glm::mat4 getScaleMatrix(void);
   inline bool setScaleMatrix(glm::mat4 rm);

   inline glm::mat4 getTranslationMatrix(void);
   inline bool setTranslationMatrix(glm::mat4 rm);

   inline glm::mat4 getLastModelMatrix(void);
   inline glm::vec3 getLMM_Translation(void);
   inline glm::vec3 getLMM_Right(void);
   inline glm::vec3 getLMM_Up(void);
   inline glm::vec3 getLMM_At(void);

   inline std::vector<char*> getModelIDs(void);
   inline std::vector<char*> getTextureIDs(void);
   inline std::vector<char*> getLightIDs(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual glm::mat4 getModelMatrix(void);
   virtual bool update(void);
   virtual bool draw(void);
   virtual Light::LightInfoBasic updateLight(Light::LightInfoBasic _li);

protected:
   inline void setLastModelMatrix(glm::mat4 _m);
   inline ID_GL getColorShaderID(void);

private:
   glm::mat4 rotationMatrix;
   glm::mat4 scaleMatrix;
   glm::mat4 translationMatrix;
   glm::mat4 lastModelMatrix;
   std::vector<char*> textureIDs;
   std::vector<char*> modelIDs;
   std::vector<char*> lightIDs;
   ID_GL colorShaderID;
   bool transparent;
};

#endif