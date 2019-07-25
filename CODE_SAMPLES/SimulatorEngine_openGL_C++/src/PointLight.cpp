//Dependencies======================================================================================
//==================================================================================================
#include "PointLight.hpp"
#include "Shader.hpp"
#include "Object3D.hpp"
#include "Model.hpp"

//Method Definitions================================================================================
//==================================================================================================
const PointLight::CameraDirection PointLight::gCameraDirections[] = {
   { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
   { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
};

//**************************************************************************************************

PointLight::PointLight(const char* _id, Light::LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID)
   :Light(_id, _li, _sourceID)
{
   // Back up the ID of the shader to use
   if (_depthShaderID == NULL)
   {
      fprintf(stderr, "DepthShader cannot be NULL\n");
      exit(LIGHT_FAIL);
   }
   strncpy(this->depthShaderID.id, _depthShaderID, MAX_ID_LEN);

   // Set up projection matrix
   if (_pb.farClip <= 0 || _pb.nearClip <= 0 || _pb.farClip < _pb.nearClip)
   {
      fprintf(stderr, "Warning, probable error with depth projection.\n");
   }
   if (_pb.fov != 90.0f)
   {
      fprintf(stderr, "Warning, point light fov should normally be 90.0f\n");
   }
   this->projection = glm::perspective<float>(_pb.fov, _pb.aspectRatio, _pb.nearClip, _pb.farClip);

#if(0)
   // Setup CameraDirection Array
   gCameraDirections[0] = CameraDirection{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
   gCameraDirections[1] = CameraDirection{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
   gCameraDirections[2] = CameraDirection{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) };
   gCameraDirections[3] = CameraDirection{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
   gCameraDirections[4] = CameraDirection{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
   gCameraDirections[5] = CameraDirection{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
#endif

   // Create the FBO
   glGenFramebuffers(1, &(this->depthFrameBuffer));
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);

   // Create the depth buffer
   glGenTextures(1, &(this->depthTexture));
   glBindTexture(GL_TEXTURE_2D, this->depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBindTexture(GL_TEXTURE_2D, 0);

   // Create the cube map
   glGenTextures(1, &(this->depthShadowMap));
   glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthShadowMap);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   for (UINT8 i = 0; i < 6; i++)
   {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R16F, 1024, 1024, 0, GL_RED, GL_FLOAT, NULL);
   }

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

   // Disable writes to the color buffer
   glDrawBuffer(GL_NONE);
   // Disable reads from the color buffer
   glReadBuffer(GL_NONE);

   GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (Status != GL_FRAMEBUFFER_COMPLETE)
   {
      fprintf(stderr, "Failed to initialize depth framebuffer\n");
      exit(1);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//**************************************************************************************************

PointLight::~PointLight(void)
{
   // Delete the frameBuffers and textures
   glDeleteTextures(1, &(this->depthTexture));
   glDeleteTextures(1, &(this->depthShadowMap));
   glDeleteFramebuffers(1, &(this->depthFrameBuffer));
}

//**************************************************************************************************

ID_GL PointLight::getDepthShaderID(void)
{
   return this->depthShaderID;
}

//**************************************************************************************************

std::vector<GLuint> PointLight::getShadowTextureIDs(void)
{
   std::vector<GLuint> temp;
   temp.push_back(this->depthShadowMap);

   return temp;
}

//**************************************************************************************************

bool PointLight::update(void)
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

   // Link the shader
   Shader* shader = seCollections.getShader(this->depthShaderID.id);
   if (shader == NULL)
   {
      fprintf(stderr, "Warning, failed to link shader in light ops.\n");
      glBindFramebuffer(0, this->depthFrameBuffer);
      return false;
   }
   glUseProgram(shader->getShaderID());

   // Load in shader values that are the same for every cube face
   // Light Position
   std::vector<GLuint> lightIDs = shader->getLightID();
   if (lightIDs.size() >= 1 && lightIDs[0] != NA)
   {
      glUniformMatrix4fv(lightIDs[0], 1, GL_FALSE, &(this->getLightShaderInfo()[0][0]));
   }

   // Get a handle to the instance collection
   std::unordered_map<std::string, Element*>* ptr = seCollections.getCollection(SimulatorEngineCollections::Collections::C_INSTANCE);

   glCullFace(GL_FRONT);
   glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

   // Bind the FrameBuffer
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->depthFrameBuffer);
   glViewport(0, 0, 1024, 1024);

   for (UINT8 i = 0; i < 6; i++)
   {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gCameraDirections[i].CubemapFace, this->depthShadowMap, 0);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

      // Get the ViewMatrix
      glm::mat4 depth_ViewMatrix = glm::lookAt(lightInfo.position, this->gCameraDirections[i].Target, this->gCameraDirections[i].Up);
      // Get the ViewProjectionMatrix
      glm::mat4 depth_vpMatrix = this->projection * depth_ViewMatrix;

      // Load in shader values that are same for all models
      if (shader->getvMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &(depth_ViewMatrix[0][0]));
      }

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
   }

   return true;
}
