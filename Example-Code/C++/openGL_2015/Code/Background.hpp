/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. These background classes are derived
** from Object3D. Provide an easy way for the user to add a background 
** texture without having to derive the code for themselves.
*/

#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: SkyBox***********************************************************
//**************************************************************************
class SkyBox : public Object3D
{
public:
   struct SkyPlane{
      glm::mat4 modelMatrix;   //Fixed model matrix
      ID_GL textureID;         //ID of texture
   };

   // Constructor(s) / Destructor ###################################
   //################################################################
   SkyBox(const char* _id,
      Simulator* _simPtr,
      const char* _colorShaderID,
      float _planeLength,
      const char* _texPosY,
      const char* _texNegY,
      const char* _texPosX,
      const char* _texNegX,
      const char* _texPosZ,
      const char* _texNegZ);
   ~SkyBox(void);

   //#######################################################################
   //Utilizes the current information (matrices, textures, etc.) and draws
   //the model.  Before calling this function all updates to lights, matrices
   //etc, should be taken care of.
   //This draw method is special in that we want our final location
   //to always be the center of this object. We will not take into account
   //the translation of the viewMatrix, only its rotations. 
   //#######################################################################
   virtual bool draw(void);

private:
   SkyPlane skyPlane[6];
   GLuint vao;
   GLuint vbo;
   GLuint vto;
   GLuint vno;
   GLuint veo;
};

#endif