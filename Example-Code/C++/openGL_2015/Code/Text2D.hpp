/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Text2D mainly acts as an encapsulating
** shell for the functions provided by the openGL tutorial.
*/

#ifndef TEXT2D_HPP
#define TEXT2D_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: Text2D **********************************************************
//**************************************************************************
class Text2D : public Element
{
public:
   typedef struct{
      char id[MAX_ID_LEN];           //User defined ID
      UINT16 x = 0;                  //x start position on screen
      UINT16 y = 0;                  //y start position on screen
      UINT8 size = 5;                //size of message text
      char message[MAX_MESSAGE_LEN]; //message
   } Text2DInfo;

   // Constructor(s) / Destructor ###################################
   //################################################################
   Text2D(const char* _id, Simulator* _simPtr, const char* _filePathDDS, const char* _vertex, const char* _fragment);
   Text2D(Text2D& t);
   ~Text2D(void);

   // Auxiliary Functions ###########################################
   //################################################################
   bool addText2D(const char* _id, UINT16 _x, UINT16 _y, UINT8 _size, const char* _message);
   bool removeText2D(const char* _id);
   bool removeAll(void);
   bool printText2D(const char* _id);
   bool printAll(void);
   //****************************************************************
   //The following functions are provided by the openGL tutorials:
   //http://www.opengl-tutorial.org/, and are used to manage Text2D.
   //Initialization takes place in the constructor, cleanUP in the
   //destructor, and printText2D will draw the Text Collection.
   //****************************************************************
   void printText2D(const char * text, int x, int y, int size);

private:
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
};

#endif