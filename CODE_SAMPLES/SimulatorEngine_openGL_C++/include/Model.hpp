#ifndef MODEL_HPP
#define MODEL_HPP

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
etc.. Model, once constructed, contains buffer links to the graphics card, such as the vao, vbo,
etc. This information  is generated via specific input files (version of this file will determine
what types of files are supported).  Inherits from class Element. This version only supports IBO
file types. This is a special format I created. It's just pre-found ibo's loaded into a file.
Decreases load time significantly.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"

//Definition========================================================================================
//==================================================================================================
class Model : public Element
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   Model(const char* _id, const char* _filePathIBO);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Model(void);

   //Gets===========================================================================================
   //===============================================================================================
   GLuint getVAO(void);
   GLuint getVBO(void);
   GLuint getVTO(void);
   GLuint getVNO(void);
   GLuint getVEO(void);
   UINT32 getIndexCount(void);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   GLuint vao;
   GLuint vbo;
   GLuint vno;
   GLuint vto;
   GLuint veo;
   UINT32 indexCount;
   FILE_GL file;

   //Do=============================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // loadIBO
   //
   //Arguments:
   // fileName       - Name of the file to open and read
   // indices        - Vector to hold the read indices. Should be instantiated and empty.
   // v              - Vector to hold th read vertices. Should be instantiated and empty.
   // uv             - Vector to hold the read texture vertices. Should be instantiated and empty.
   // n              - Vector to hold the read normal vertices. Should be instantiated and empty.
   //
   //Output:
   // void
   //
   //Returns:
   // bool - true if draw success, else, false
   //
   //About:
   // A helpful function which reads a binary file which contains the ready-to-go indexed
   // vertices information. This saves considerable time versus loading the .obj model at run time,
   // however, it does require the user to use the IBO creator on all .obj files before-hand. The
   // input vectors should be empty, and exist! Do not pass in NULL, or a partially filled vector,
   // that would probably break it.
   //
   //-----------------------------------------------------------------------------------------------
   bool loadIBO(const char* fileName, std::vector<unsigned int>* indices, std::vector<glm::vec3>* v,
                std::vector<glm::vec2>* uv, std::vector<glm::vec3>* n);


   // There should only need be one instance of each object.

   void operator= (const Model& m) {}
   Model(const Model& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
