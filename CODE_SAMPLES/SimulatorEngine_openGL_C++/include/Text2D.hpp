#ifndef TEXT2D_HPP
#define TEXT2D_HPP

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
etc.. Text2D acts as an encapsulating shell for the functions provided by the openGL tutorial.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Element.hpp"

//Definition========================================================================================
//==================================================================================================
class Text2D : public Element
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   typedef struct
   {
      char id[MAX_ID_LEN];           //User defined ID
      UINT16 x = 0;                  //x start position on screen
      UINT16 y = 0;                  //y start position on screen
      UINT8 size = 5;                //size of message text
      char message[MAX_MESSAGE_LEN]; //message
   }
   Text2DInfo;

   //Constructor(s)=================================================================================
   //===============================================================================================
   Text2D(const char* _id, const char* _filePathDDS, const char* _vertex, const char* _fragment);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Text2D(void);

   //Do=============================================================================================
   //===============================================================================================
   bool addText2D(const char* _id, UINT16 _x, UINT16 _y, UINT8 _size, const char* _message);
   bool removeText2D(const char* _id);
   bool removeAll(void);
   bool printText2D(const char* _id);
   bool printAll(void);

   //-----------------------------------------------------------------------------------------------
   //Name:
   // printText2D
   //
   //Arguments:
   // text           - String of text to write
   // x              - x coord to write to
   // y              - y coord to write to
   // size           - text size
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // The following functions are provided by the openGL tutorials: http://www.opengl-tutorial.org/,
   // and are used to manage Text2D. Initialization takes place in the constructor, cleanUP in the
   // destructor, and printText2D will draw the Text Collection.
   //
   //-----------------------------------------------------------------------------------------------
   void printText2D(const char * text, int x, int y, int size);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   GLuint Text2DVAO;
   GLuint Text2DTextureID;
   GLuint Text2DVertexBufferID;
   GLuint Text2DUVBufferID;
   GLuint Text2DShaderID;
   GLuint Text2DUniformID;
   std::vector<Text2D::Text2DInfo> text;
   FILE_GL texture;
   FILE_GL vertex;
   FILE_GL fragment;


   // There should only need be one instance of each object.

   void operator= (const Text2D& m) {}
   Text2D(const Text2D& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
