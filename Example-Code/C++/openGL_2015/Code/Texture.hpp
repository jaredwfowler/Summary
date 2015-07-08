/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Texture contains simple information
** about the fileName, and holds a openGL handle to the loaded texture.
**
** This class version will only support .DDS format. A nice extension for
** GIMP exists for exporting .DDS
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: Texture *********************************************************
//**************************************************************************
class Texture : public Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Texture(const char* _id, Simulator* _simPtr, const char* _filePathDDS);
   Texture(Texture& t);
   ~Texture(void);

   // gets and sets #################################################
   //################################################################
   inline GLuint getTextureID(void);

   // Helper Functions ##############################################
   //################################################################
   //****************************************************************
   //Loads a DDS compressed image into the GPU and returns an openGL
   //handler. This function was provided by the openGL tutorials:
   //http://www.opengl-tutorial.org/
   //****************************************************************
   static GLuint loadDDS(const char * imagepath);

private:
   GLuint textureID;
   FILE_GL file;
};

#endif