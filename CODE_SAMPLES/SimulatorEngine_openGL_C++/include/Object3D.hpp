#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  March 14, 2017
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: March 14, 2017
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Built to be used by the class "Simulator". Simulator is a collection of models, textures, cameras,
etc.. Object3D will reference which model, textures, and shader it will use via id, which is a
special attribute defined by the user when adding elements to Simulators collections. Object3D is
the base class to which several other children class should be written.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"
#include "Light.hpp"

//Definition========================================================================================
//==================================================================================================
class Object3D : public Element
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   Object3D(const char* _id,
      std::vector<char*> _textureIDs,
      std::vector<char*> _modelIDs,
      std::vector<char*> _lightIDs,
      const char* _colorShaderID,
      bool _isTransparent,
      glm::mat4 _scaleMatrix,
      glm::mat4 _translationMatrix,
      glm::mat4 _rotationMatrix);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Object3D(void);

   //Gets===========================================================================================
   //===============================================================================================
   bool isTransparent(void);
   glm::mat4 getRotationMatrix(void);
   glm::mat4 getScaleMatrix(void);
   glm::mat4 getTranslationMatrix(void);
   glm::mat4 getLastModelMatrix(void);
   glm::vec3 getLMM_Translation(void);
   glm::vec3 getLMM_Right(void);
   glm::vec3 getLMM_Up(void);
   glm::vec3 getLMM_At(void);
   std::vector<char*> getModelIDs(void);
   std::vector<char*> getTextureIDs(void);
   std::vector<char*> getLightIDs(void);

   virtual glm::mat4 getModelMatrix(void);

   //Sets===========================================================================================
   //===============================================================================================
   void setTransparent(bool b);
   bool setRotationMatrix(glm::mat4 rm);
   bool setScaleMatrix(glm::mat4 rm);
   bool setTranslationMatrix(glm::mat4 rm);

   //Do=============================================================================================
   //===============================================================================================
   virtual bool update(void);
   virtual bool draw(void);
   virtual Light::LightInfoBasic updateLight(Light::LightInfoBasic _li);

protected: //#######################################################################################
   //Gets===========================================================================================
   //===============================================================================================
   ID_GL getColorShaderID(void);

   //Sets===========================================================================================
   //===============================================================================================
   void setLastModelMatrix(glm::mat4 _m);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   glm::mat4 rotationMatrix;
   glm::mat4 scaleMatrix;
   glm::mat4 translationMatrix;
   glm::mat4 lastModelMatrix;
   std::vector<char*> textureIDs;
   std::vector<char*> modelIDs;
   std::vector<char*> lightIDs;
   ID_GL colorShaderID;
   bool transparent;


   // There should only need be one instance of each object.

   void operator= (const Object3D& m) {}
   Object3D(const Object3D& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
