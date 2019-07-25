//Dependencies======================================================================================
//==================================================================================================
#include "Object3D.hpp"
#include "Shader.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Texture.hpp"

#define MIN(a,b)  ((((a) < (b))) ? (a) : (b))

//Method Definitions================================================================================
//==================================================================================================
Object3D::Object3D(const char* _id,
   std::vector<char*> _textureIDs,
   std::vector<char*> _modelIDs,
   std::vector<char*> _lightIDs,
   const char* _colorShaderID,
   bool _isTransparent,
   glm::mat4 _scaleMatrix = glm::mat4(),
   glm::mat4 _translationMatrix = glm::mat4(),
   glm::mat4 _rotationMatrix = glm::mat4())

   :Element(_id)
{
   this->setRotationMatrix(_rotationMatrix);
   this->setScaleMatrix(_scaleMatrix);
   this->setTranslationMatrix(_translationMatrix);
   this->lastModelMatrix = getModelMatrix();

   this->setTransparent(_isTransparent);

   // Do the textures exist in Simulator?
   for (size_t i = 0; i < _textureIDs.size(); i++)
   {
      if (seCollections.getTexture(_textureIDs.at(i)) == NULL)
      {
         fprintf(stderr, "Texture with id: %s doesn't exist\n", _textureIDs.at(i));
         exit(OBJECT_FAIL);
      }
   }
   this->textureIDs = _textureIDs;

   // Does the Shader exist in Simulator?
   if (_colorShaderID == NULL || seCollections.getShader(_colorShaderID) == NULL)
   {
      fprintf(stderr, "Shader with id: %s doesn't exist\n", _colorShaderID);
      exit(OBJECT_FAIL);
   }
   strncpy(colorShaderID.id, _colorShaderID, MAX_ID_LEN - 1);

   // Do the models exist in Simulator?
   for (size_t i = 0; i < _modelIDs.size(); i++)
   {
      if (seCollections.getModel(_modelIDs.at(i)) == NULL)
      {
         fprintf(stderr, "Model with id: %s doesn't exist\n", _modelIDs.at(i));
         exit(OBJECT_FAIL);
      }
   }
   this->modelIDs = _modelIDs;

   // Do the lights exist in Simulator?
   for (size_t i = 0; i < _lightIDs.size(); i++)
   {
      if (seCollections.getLight(_lightIDs.at(i)) == NULL)
      {
         fprintf(stderr, "Light with id: %s doesn't exist\n", _lightIDs.at(i));
         // This will not fail, but may have strange results
      }
   }
   this->lightIDs = _lightIDs;
}
//**************************************************************************************************

Object3D::~Object3D()
{
   // Void
}

//**************************************************************************************************

bool Object3D::isTransparent(void)
{
   return this->transparent;
}

//**************************************************************************************************

glm::mat4 Object3D::getRotationMatrix(void)
{
   return this->rotationMatrix;
}

//**************************************************************************************************

glm::mat4 Object3D::getScaleMatrix(void)
{
   return this->scaleMatrix;
}

//**************************************************************************************************

glm::mat4 Object3D::getTranslationMatrix(void)
{
   return this->translationMatrix;
}

//**************************************************************************************************

glm::mat4 Object3D::getLastModelMatrix(void)
{
   return this->lastModelMatrix;
}

//**************************************************************************************************

glm::vec3 Object3D::getLMM_Translation(void)
{
   return glm::vec3(this->lastModelMatrix[3][0], this->lastModelMatrix[3][1], this->lastModelMatrix[3][2]);
}

//**************************************************************************************************

glm::vec3 Object3D::getLMM_Right(void)
{
   return glm::vec3(this->lastModelMatrix[0][0], this->lastModelMatrix[0][1], this->lastModelMatrix[0][2]);
}

//**************************************************************************************************

glm::vec3 Object3D::getLMM_Up(void)
{
   return glm::vec3(this->lastModelMatrix[1][0], this->lastModelMatrix[1][1], this->lastModelMatrix[1][2]);
}

//**************************************************************************************************

glm::vec3 Object3D::getLMM_At(void)
{
   return glm::vec3(this->lastModelMatrix[2][0], this->lastModelMatrix[2][1], this->lastModelMatrix[2][2]);
}

//**************************************************************************************************

std::vector<char*> Object3D::getModelIDs(void)
{
   return this->modelIDs;
}

//**************************************************************************************************

std::vector<char*> Object3D::getTextureIDs(void)
{
   return this->textureIDs;
}

//**************************************************************************************************

std::vector<char*> Object3D::getLightIDs(void)
{
   return this->lightIDs;
}

//**************************************************************************************************

glm::mat4 Object3D::getModelMatrix(void)
{
   this->lastModelMatrix = (this->getTranslationMatrix() * this->getRotationMatrix() * this->getScaleMatrix());
   return this->lastModelMatrix;
}

//**************************************************************************************************

void Object3D::setTransparent(bool b)
{
   this->transparent = b;
}

//**************************************************************************************************

bool Object3D::setRotationMatrix(glm::mat4 rm)
{
   this->rotationMatrix = rm;
   return true;
}

//**************************************************************************************************

bool Object3D::setScaleMatrix(glm::mat4 rm)
{
   this->scaleMatrix = rm;
   return true;
}

//**************************************************************************************************

bool Object3D::setTranslationMatrix(glm::mat4 rm)
{
   this->translationMatrix = rm;
   return true;
}

//**************************************************************************************************

bool Object3D::update(void)
{
   return true;
}

//**************************************************************************************************

bool Object3D::draw(void)
{
   // Local Vars
   Model* model = seCollections.getModel(this->modelIDs.at(0));
   Shader* shader = seCollections.getShader(this->colorShaderID.id);

   // Valid values?
   if (model == NULL || shader == NULL)
   {
      return false;
   }

   // Bind VAO and Shaders
   glBindVertexArray(model->getVAO());
   glUseProgram(shader->getShaderID());
   glEnable(GL_DEPTH_TEST);

   // Is this object transparent?
   if (this->isTransparent())
   {
      // Enable blending
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else
   {
      glDisable(GL_BLEND);
   }


   // Create temporary matrices and bind them to the shaders
   glm::mat4 mvMatrix = seCollections.getViewMatrix() * this->getLastModelMatrix();
   glm::mat4 mvpMatrix = seCollections.getProjectionMatrix() * mvMatrix;

   // Set matrix shader uniforms. NA indicates that they don't exist
   // Matrices
   if (shader->getmvpMatrixID() != NA)
   {
      glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(mvpMatrix[0][0]));
   }
   if (shader->getmMatrixID() != NA)
   {
      glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(getLastModelMatrix()[0][0]));
   }
   if (shader->getvMatrixID() != NA)
   {
      glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &((seCollections.getViewMatrix())[0][0]));
   }
   if (shader->getmvMatrixID() != NA)
   {
      glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &(mvMatrix[0][0]));
   }

   // Light Positions
   std::vector<GLuint> tempVec = shader->getLightID();
   UINT32 maxIts = MIN(this->lightIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++)
   {
      if (tempVec.at(j) != NA)
      {
         Light* t = seCollections.getLight(this->lightIDs.at(j));
         if (t == NULL)
         {
            continue;
         }

         glm::mat4 lsi = t->getLightShaderInfo();
         glUniformMatrix4fv(tempVec.at(j), 1, GL_FALSE, &(lsi[0][0]));
      }
      else
      {
         break;
      }
   }

   // Shadow Textures
   tempVec = shader->getDepthTextureID();
   maxIts = MIN(this->lightIDs.size(), tempVec.size());
   UINT16 textureOffset = 0;
   for (UINT16 j = 0; j < maxIts; j++)
   {
      if (tempVec.at(j) != NA)
      {
         Light* t = seCollections.getLight(this->lightIDs.at(j));
         if (t == NULL)
         {
            continue;
         }

         // Get shadow texture list
         std::vector<GLuint> shadowList = t->getShadowTextureIDs();
         for (UINT16 k = 0; k < shadowList.size(); k++)
         {
            glActiveTexture(GL_TEXTURE0 + textureOffset);
            glBindTexture(GL_TEXTURE_2D, shadowList.at(k));

            // Link this texture to the shader
            glUniform1i(tempVec.at(j), textureOffset);

            // Update the texture offset
            textureOffset++;
         }
      }
      else
      {
         break;
      }
   }

   // Shadow Matrices
   tempVec = shader->getDepthMVPID();
   maxIts = MIN(this->lightIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++)
   {
      if (tempVec.at(j) != NA)
      {
         Light* t = seCollections.getLight(this->lightIDs.at(j));
         if (t == NULL)
         {
            continue;
         }

         // Get shadow matrix list
         std::vector<glm::mat4> shadowList = t->getDepthBiasVPMatrices();
         for (UINT16 k = 0; k < shadowList.size(); k++)
         {
            glm::mat4 dbmvpTemp = shadowList.at(k) * this->getLastModelMatrix();
            glUniformMatrix4fv(tempVec.at(j), 1, GL_FALSE, &(dbmvpTemp[0][0]));
         }
      }
      else
      {
         break;
      }
   }

   // Bind Textures
   tempVec = shader->getTextureID();
   maxIts = MIN(this->textureIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++)
   {
      if (tempVec.at(j) != NA)
      {
         Texture* t = seCollections.getTexture(this->textureIDs.at(j));
         if (t == NULL) continue;
         glActiveTexture(GL_TEXTURE0 + textureOffset);
         glBindTexture(GL_TEXTURE_2D, t->getTextureID());

         // Link this texture to the shader
         glUniform1i(tempVec.at(j), textureOffset);

         // Update the texture offset
         textureOffset++;
      }
   }

   // Bind random floats if they exist
   tempVec = shader->getRandomFloatID();
   for (UINT16 j = 0; j < tempVec.size(); j++)
   {
      // Generate random numbers
      float randVal = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
      glUniform1f(tempVec.at(j), randVal);
   }

   // Link vertices to shader
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, model->getVBO());
   glVertexAttribPointer(
      0,                  // attribute
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
      );

   // Link texture coords to shader
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, model->getVTO());
   glVertexAttribPointer(
      1,                                // attribute
      2,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
      );

   // Link normals to shader
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, model->getVNO());
   glVertexAttribPointer(
      2,                                // attribute
      3,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
      );

   // Bind the index buffer
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

   // Draw!
   glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

   // Disable linked vertex arrays
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);

   // Unbind the vertex array
   glBindVertexArray(0);

   return true;
}

//**************************************************************************************************

Light::LightInfoBasic Object3D::updateLight(Light::LightInfoBasic _li)
{
   //Congratulations, you've been classified as a light.. just return
   //your current position...and arbitrary values
   _li.position = this->getLMM_Translation();
   _li.direction = this->getLMM_At();
   return _li;
}

//**************************************************************************************************

ID_GL Object3D::getColorShaderID(void)
{
   return this->colorShaderID;
}

//**************************************************************************************************

void Object3D::setLastModelMatrix(glm::mat4 _m)
{
   this->lastModelMatrix = _m;
}
