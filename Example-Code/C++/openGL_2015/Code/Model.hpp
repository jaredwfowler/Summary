/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Model, once constructed, contains buffer
** links to the graphics card, such as the vao, vbo, etc. This information 
** is generated via specific input files (version of this file will determine 
** what types of files are supported).  Inherits from class Element.
**
** This version only supports IBO file types. This is a special format I 
** created. It's just pre-found ibo's loaded into a file. Decreases load
** time significantly.
*/

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: Model ***********************************************************
//**************************************************************************
class Model : public Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Model(const char* _id, Simulator* _simPtr, const char* _filePathIBO);
   Model(Model& m);
   ~Model(void);

   // gets and sets #################################################
   //################################################################
   inline GLuint getVAO(void);
   inline GLuint getVBO(void);
   inline GLuint getVTO(void);
   inline GLuint getVNO(void);
   inline GLuint getVEO(void);
   inline UINT32 getIndexCount(void);

private:
   GLuint vao;
   GLuint vbo;
   GLuint vno;
   GLuint vto;
   GLuint veo;
   UINT32 indexCount;
   FILE_GL file;

   // Helper Functions ##############################################
   //################################################################
   //****************************************************************
   //A helpful function I created which reads a binary file which 
   //contains the ready-to-go indexed vertices information. This
   //saves considerable time versus loading the .obj model at run
   //time, however, it does require the user to use the IBO creator
   //on all .obj files before-hand. The input vectors should be
   //empty, and exist! Do not pass in NULL, or a partially filled
   //vector, that would probably break it.
   //****************************************************************
   bool loadIBO(const char* fileName,
      std::vector<unsigned int>* indices, 
      std::vector<glm::vec3>* v,
      std::vector<glm::vec2>* uv,
      std::vector<glm::vec3>* n);
};

#endif