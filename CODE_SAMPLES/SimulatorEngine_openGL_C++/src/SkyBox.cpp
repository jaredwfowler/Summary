//Dependencies======================================================================================
//==================================================================================================
#include "SkyBox.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Defines.hpp"

//Method Definitions================================================================================
//==================================================================================================

SkyBox::SkyBox(const char* _id,
   const char* _colorShaderID,
   float _planeLength,
   const char* _texPosY,
   const char* _texNegY,
   const char* _texPosX,
   const char* _texNegX,
   const char* _texPosZ,
   const char* _texNegZ)

   // Call the parent constructor
   : Object3D(_id, std::vector<char*>(), std::vector<char*>(), std::vector<char*>(),
     _colorShaderID, false, glm::mat4(), glm::mat4(), glm::mat4())
{
   float length = (_planeLength / 2);

   // Manually create vertices that make up a plane of length <planeLength> on the x,z plane
   std::vector<glm::vec3> indexed_vertices = std::vector<glm::vec3>({
      glm::vec3(-length, 0.0, -length),
      glm::vec3(-length, 0.0, length),
      glm::vec3(length, 0.0, -length),
      glm::vec3(length, 0.0, length) });

   // Manually map the textures to the plane
   std::vector<glm::vec2> indexed_texture = std::vector<glm::vec2>({
      glm::vec2(0.0, -1.0),
      glm::vec2(0.0, 0.0),
      glm::vec2(1.0, -1.0),
      glm::vec2(1.0, 1.0) });

   // Manually set the normals to the planes
   std::vector<glm::vec3> indexed_normals = std::vector<glm::vec3>({
      glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 1.0, 0.0) });

   // Manually set the indices which use the vertices to create a square plane
   std::vector<UINT32> indices = std::vector<UINT32>({ 0, 1, 2, 3, 2, 1 });

   // Generate and Bind a new Vertex Array Object
   glGenVertexArrays(1, &(this->vao));
   glBindVertexArray(this->vao);

   // Generate GPU vertex buffers, get handles, and upload values
   // Load vertex info into a VBO
   glGenBuffers(1, &(this->vbo));
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &(indexed_vertices[0]), GL_STATIC_DRAW);

   // Load normal info into a VBO
   glGenBuffers(1, &(this->vno));
   glBindBuffer(GL_ARRAY_BUFFER, this->vno);
   glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &(indexed_normals[0]), GL_STATIC_DRAW);

   //Load texture vertices into a VBO
   glGenBuffers(1, &(this->vto));
   glBindBuffer(GL_ARRAY_BUFFER, this->vto);
   glBufferData(GL_ARRAY_BUFFER, indexed_texture.size() * sizeof(glm::vec2), &(indexed_texture[0]), GL_STATIC_DRAW);

   //Load indices into an element buffer
   glGenBuffers(1, &(this->veo));
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->veo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(UINT32), &(indices[0]), GL_STATIC_DRAW);

   //Unbind VAO
   glBindVertexArray(0);

   strncpy(this->skyPlane[0].textureID.id, _texPosY, MAX_ID_LEN - 1);
   strncpy(this->skyPlane[1].textureID.id, _texNegY, MAX_ID_LEN - 1);
   strncpy(this->skyPlane[2].textureID.id, _texPosX, MAX_ID_LEN - 1);
   strncpy(this->skyPlane[3].textureID.id, _texNegX, MAX_ID_LEN - 1);
   strncpy(this->skyPlane[4].textureID.id, _texPosZ, MAX_ID_LEN - 1);
   strncpy(this->skyPlane[5].textureID.id, _texNegZ, MAX_ID_LEN - 1);

   // Manually setup translations and rotations to form a box
   this->skyPlane[0].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, length, 0)) *
                                   glm::rotate(glm::mat4(), 180.0f, glm::vec3(1, 0, 0));
   this->skyPlane[1].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, -length, 0));
   this->skyPlane[2].modelMatrix = glm::translate(glm::mat4(), glm::vec3(length, 0, 0)) *
                                   glm::rotate(glm::mat4(), 90.0f, glm::vec3(0, 0, 1));
   this->skyPlane[3].modelMatrix = glm::translate(glm::mat4(), glm::vec3(-length, 0, 0)) *
                                   glm::rotate(glm::mat4(), 90.0f, glm::vec3(0, 0, -1));
   this->skyPlane[4].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, length)) *
                                   glm::rotate(glm::mat4(), 90.0f, glm::vec3(-1, 0, 0));
   this->skyPlane[5].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -length)) *
                                   glm::rotate(glm::mat4(), 90.0f, glm::vec3(1, 0, 0));
}

//**************************************************************************************************

SkyBox::~SkyBox(void)
{
   //void
}

//**************************************************************************************************

bool SkyBox::draw(void)
{
   // Local Vars
   Shader* shader = seCollections.getShader(this->getColorShaderID().id);

   // Valid values?
   if (shader == NULL)
   {
      return false;
   }

   // Get a copy of the current viewMatrix
   glm::mat4 viewMatrix = seCollections.getViewMatrix();

   // Eliminate the translation aspect of the view matrix
   viewMatrix[3][0] = viewMatrix[3][1] = viewMatrix[3][2] = 0;
   viewMatrix[3][3] = 1;

   viewMatrix = seCollections.getProjectionMatrix() * viewMatrix;

   // Bind VAO and Shaders
   glBindVertexArray(this->vao);
   glUseProgram(shader->getShaderID());

   // Disable Depth testing
   glDisable(GL_DEPTH_TEST);

   // Draw each of the planes
   for (UINT8 i = 0; i < 6; i++)
   {
      // Get the modelView Matrix
      glm::mat4 mvpMatrix = viewMatrix * this->skyPlane[i].modelMatrix;

      // Set the shader uniform
      if (shader->getmvpMatrixID() != NA)
      {
         glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(mvpMatrix[0][0]));
      }

      // Bind the texture
      glActiveTexture(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, seCollections.getTexture(this->skyPlane[i].textureID.id)->getTextureID());

      // Link texture to the shader
      glUniform1i(shader->getTextureID().at(0), 0);

      // Link vertices to shader
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
      glVertexAttribPointer(
         0,                               // attribute
         3,                               // size
         GL_FLOAT,                        // type
         GL_FALSE,                        // normalized?
         0,                               // stride
         (void*)0                         // array buffer offset
         );

      // Link texture coords to shader
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, this->vto);
      glVertexAttribPointer(
         1,                               // attribute
         2,                               // size
         GL_FLOAT,                        // type
         GL_FALSE,                        // normalized?
         0,                               // stride
         (void*)0                         // array buffer offset
         );

      // Link normals to shader
      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, this->vno);
      glVertexAttribPointer(
         2,                               // attribute
         3,                               // size
         GL_FLOAT,                        // type
         GL_FALSE,                        // normalized?
         0,                               // stride
         (void*)0                         // array buffer offset
         );

      // Bind the index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->veo);

      // Draw!
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

      // Disable linked vertex arrays
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);

   }

   // Unbind the vertex array
   glBindVertexArray(0);

   return true;
}
