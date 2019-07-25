#ifndef SHADER_HPP
#define SHADER_HPP

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

****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"

//Definition========================================================================================
//==================================================================================================
class Shader : public Element
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   Shader(const char* _id, const char* _vertex, const char* _fragment);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Shader(void);

   //Gets===========================================================================================
   //===============================================================================================
   GLuint getShaderID(void);
   GLuint getmMatrixID(void);
   GLuint getvMatrixID(void);
   GLuint getmvMatrixID(void);
   GLuint getmvpMatrixID(void);
   GLuint getRenderTextureID(void);
   std::vector<GLuint> getDepthMVPID(void);
   std::vector<GLuint> getDepthTextureID(void);
   std::vector<GLuint> getLightID(void);
   std::vector<GLuint> getTextureID(void);
   std::vector<GLuint> getRandomFloatID(void);

   //Do=============================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // LoadShaders
   //
   //Arguments:
   // ...
   //
   //Output:
   // void
   //
   //Returns:
   // GLuint      - Handle to GLSL Shader
   //
   //About:
   // Loads and compiles the passed in shader files. Returns a handle to the registered shader
   // program. This function was provided by the openGL tutorials : http://www.opengl-tutorial.org/
   //
   //-----------------------------------------------------------------------------------------------
   static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
   static GLuint LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   GLuint shaderID;
   GLuint mMatrixID;
   GLuint vMatrixID;
   GLuint mvMatrixID;
   GLuint mvpMatrixID;
   std::vector<GLuint> lightID;
   std::vector<GLuint> dbmvpID;
   std::vector<GLuint> depthTextureID;
   std::vector<GLuint> textureID;
   std::vector<GLuint> randomFloatID;
   GLuint renderTextureID;
   FILE_GL vertexShader;
   FILE_GL fragmentShader;


   // There should only need be one instance of each object.

   void operator= (const Shader& m) {}
   Shader(const Shader& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
