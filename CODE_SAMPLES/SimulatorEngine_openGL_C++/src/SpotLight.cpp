//Dependencies======================================================================================
//==================================================================================================
#include "SpotLight.hpp"
#include "Shader.hpp"
#include "Object3D.hpp"
#include "Model.hpp"

//Method Definitions================================================================================
//==================================================================================================
SpotLight::SpotLight(const char* _id, LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID)
   :Light(_id, _li, _sourceID)
{
   // Back up the ID of the shader to use
   if (_depthShaderID == NULL)
   {
      fprintf(stderr, "DepthShader cannot be NULL\n");
      exit(LIGHT_FAIL);
   }
   strncpy(this->depthShaderID.id, _depthShaderID, MAX_ID_LEN);

   // Set up the Projection Matrix
   if (_pb.nearClip <= 0 || _pb.farClip <= 0)
   {
      fprintf(stderr, "Warning, possible error with Spotlight projection bounds\n");
   }
   this->projection = glm::perspective(_pb.fov, _pb.aspectRatio, _pb.nearClip, _pb.farClip);

   // Initialize Shadow Map Buffers
   // Allocate a new frame buffer
   glGenFramebuffers(1, &(this->depthFrameBuffer));
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);

   // Allocate space for depth texture and set properties
   glGenTextures(1, &(this->depthTexture));
   glBindTexture(GL_TEXTURE_2D, this->depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

   // Attach depth texture to the frame buffer
   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthTexture, 0);
   // No color output in the bound framebuffer, only depth.
   glDrawBuffer(GL_NONE);

   // Did everything initialize good?
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      fprintf(stderr, "Failed to initialize depth framebuffer\n");
      exit(1);
   }

   // Unbind this frame buffer -- VERY IMPORTANT!
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//**************************************************************************************************

SpotLight::~SpotLight(void)
{
   // Delete the frameBuffers and textures
   glDeleteTextures(1, &(this->depthTexture));
   glDeleteFramebuffers(1, &(this->depthFrameBuffer));
}

//**************************************************************************************************

ID_GL SpotLight::getDepthShaderID(void)
{
   return this->depthShaderID;
}

//**************************************************************************************************

std::vector<glm::mat4> SpotLight::getDepthBiasVPMatrices(void)
{
   return std::vector<glm::mat4>({ this->depthBias_vpMatrix });
}

//**************************************************************************************************

std::vector<GLuint> SpotLight::getShadowTextureIDs(void)
{
   std::vector<GLuint> temp;
   temp.push_back(this->depthTexture);

   return temp;
}

//**************************************************************************************************

bool SpotLight::update(void)
{
   // Call base class version
   if (!Light::update())
   {
      return false;
   }

   // Is shadow rendering disabled?
   if ((seCollections.getControlFlags() & SimulatorEngineCollections::ControlFlags::DisableShadows) == SimulatorEngineCollections::ControlFlags::DisableShadows)
   {
      return true;
   }

   // Get a copy of the light structure
   LightInfoBasic lightInfo = this->getLightStruct();

   // Only proceed if the light is turned on
   if (!lightInfo.turnedOn)
   {
      return true;
   }

   // Get the ViewMatrix
   glm::mat4 depth_ViewMatrix = glm::lookAt(lightInfo.position, lightInfo.position + lightInfo.direction, glm::cross(lightInfo.position + lightInfo.direction, lightInfo.position + lightInfo.direction + glm::vec3(1, 2, 3)));
   // Get the ViewProjectionMatrix
   glm::mat4 depth_vpMatrix = this->projection * depth_ViewMatrix;

   // Update shadow framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);
   glViewport(0, 0, 1024, 1024);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Link the shader
   Shader* shader = seCollections.getShader(this->depthShaderID.id);
   if (shader == NULL)
   {
      fprintf(stderr, "Warning, failed to link shader in light ops.\n");
      glBindFramebuffer(0, this->depthFrameBuffer);
      return false;
   }
   glUseProgram(shader->getShaderID());

   // Get a handle to the instance collection
   std::unordered_map<std::string, Element*>* ptr = seCollections.getCollection(SimulatorEngineCollections::Collections::C_INSTANCE);

   for (auto kv : *ptr)
   {
      // Attempt to convert the element pointer into an Object3D*
      Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
      if (objPtr == NULL)
      {
         fprintf(stderr, "Warning, corruption in instance collection\n");
         continue;
      }

      // If the object is transparent, don't draw it
      if (objPtr->isTransparent())
      {
         continue;
      }

      // Attempt to get and then convert the corresponding model
      Model* model = seCollections.getModel(objPtr->getModelIDs().at(0));
      if (model == NULL)
      {
         fprintf(stderr, "Warning, corruption in model/instance collection\n");
         continue;
      }

      // Is this object the light source? Don't render it if it is
      if (strncmp(objPtr->getID().id, this->getSourceID().id, MAX_ID_LEN) == 0)
      {
         continue;
      }

      // Calculate the MVP
      glm::mat4 depth_mvpMatrix = depth_vpMatrix * objPtr->getLastModelMatrix();

      // Load this value into the depth shader
      if (shader->getmvpMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(depth_mvpMatrix[0][0]));
      }
      if (shader->getmMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(objPtr->getLastModelMatrix()[0][0]));
      }
      if (shader->getvMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &(depth_ViewMatrix[0][0]));
      }
      if (shader->getmvMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &((depth_ViewMatrix * objPtr->getLastModelMatrix())[0][0]));
      }

      // Bind VAO and then draw values
      glBindVertexArray(model->getVAO());

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

      // Bind the index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

      // Draw!
      glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

      // Disable linked vertex arrays
      glDisableVertexAttribArray(0);

   }

   // Return the frameBuffer to the normal one
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // Update the depthBias matrix
   // Takes the current vp matrix and multiplies it by a depthBias matrix.
   // The purpose of depth bias is to change texture coords from -1,1 to 0,1
   const glm::mat4 biasMatrix = glm::mat4(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
      );
   this->depthBias_vpMatrix = (biasMatrix * depth_vpMatrix);

   return true;
}
