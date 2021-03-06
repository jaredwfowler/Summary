#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

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
#include "Light.hpp"

//Definition========================================================================================
//==================================================================================================
class SpotLight : public Light
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   SpotLight(const char* _id, Light::LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID);

   //Destructor=====================================================================================
   //===============================================================================================
   ~SpotLight(void);

   //Gets===========================================================================================
   //===============================================================================================
   ID_GL getDepthShaderID(void);

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
   ID_GL depthShaderID;
   GLuint depthFrameBuffer;
   GLuint depthTexture;
   glm::mat4 depthBias_vpMatrix;
   glm::mat4 projection;


   // There should only need be one instance of each object.

   void operator= (const SpotLight& m) {}
   SpotLight(const SpotLight& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
