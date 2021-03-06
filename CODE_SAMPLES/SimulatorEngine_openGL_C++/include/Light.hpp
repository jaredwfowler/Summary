#ifndef LIGHT_HPP
#define LIGHT_HPP

/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
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

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"

//Definition========================================================================================
//==================================================================================================
class Light : public Element
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   typedef struct
   {
      glm::vec3 position;
      float intensity;
      glm::vec3 color;
      bool turnedOn;
      glm::vec3 direction;

   }
   LightInfoBasic;

   typedef struct
   {
      float minX;
      float maxX;
      float minY;
      float maxY;
      float nearClip;
      float farClip;
   }
   OrthoBounds;

   typedef struct
   {
      float fov;
      float aspectRatio;
      float nearClip;
      float farClip;
   }
   ProjectionBounds;

   //Constructor(s)=================================================================================
   //===============================================================================================
   Light(const char* _id, Light::LightInfoBasic _li, const char* _sourceID);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Light(void);

   //Gets===========================================================================================
   //===============================================================================================
   LightInfoBasic getLightStruct(void);
   glm::mat4 getLightShaderInfo(void);
   ID_GL getSourceID(void);

   virtual std::vector<GLuint> getShadowTextureIDs(void);
   virtual std::vector<glm::mat4> getDepthBiasVPMatrices(void);

   //Do=============================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // update
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // If this light is linked to an Object3D, then that Object may update this light's values. This
   // will call the generic function in Object3D which will return the new position of the light,
   // along with other indicators. It will be up to the user to provide a pointer to the object,
   // based upon the sourceID held by the light
   //
   //-----------------------------------------------------------------------------------------------
   virtual bool update(void);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   LightInfoBasic info;                      // Collection of light info
   ID_GL sourceID;                           // ID of object which updates the light

   //Sets===========================================================================================
   //===============================================================================================
   void setSourceID(const char* _source);

   //Do=============================================================================================
   //===============================================================================================
   glm::vec3 verifyColor(glm::vec3 _v);
   float verifyIntensity(float _i);


   // There should only need be one instance of each object.

   void operator= (const Light& m) {}
   Light(const Light& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
