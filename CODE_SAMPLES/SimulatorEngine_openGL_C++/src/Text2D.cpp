//Dependencies======================================================================================
//==================================================================================================
#include "Text2D.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

//Method Definitions================================================================================
//==================================================================================================
Text2D::Text2D(const char* _id, const char* _filePathDDS, const char* _vertex, const char* _fragment)
   :Element(_id)
{
   if (_filePathDDS == NULL || _vertex == NULL || _fragment == NULL)
   {
      fprintf(stderr, "FilePath cannot be NULL\n");
      exit(TEXT2D_FAIL);
   }
   strncpy(this->texture.file, _filePathDDS, MAX_FILE_LEN - 1);
   strncpy(this->vertex.file, _vertex, MAX_FILE_LEN - 1);
   strncpy(this->fragment.file, _fragment, MAX_FILE_LEN - 1);

   // Initialize texture
   Text2DTextureID = Texture::loadDDS(_filePathDDS);

   // Generate VAO
   glGenVertexArrays(1, &(Text2DVAO));
   glBindVertexArray(Text2DVAO);

   // Initialize VBO
   glGenBuffers(1, &Text2DVertexBufferID);
   glGenBuffers(1, &Text2DUVBufferID);

   // Initialize Shader
   Text2DShaderID = Shader::LoadShaders(_vertex, _fragment);

   // Initialize uniforms' IDs
   char temp[64] = { 0 };
   sprintf_s(temp, TEXTURE, 0);
   Text2DUniformID = glGetUniformLocation(Text2DShaderID, temp);

   // Unbind VAO
   glBindVertexArray(0);
}

//**************************************************************************************************

Text2D::~Text2D(void)
{
   // Bind VAO
   glBindVertexArray(Text2DVAO);

   // Delete buffers
   glDeleteBuffers(1, &Text2DVertexBufferID);
   glDeleteBuffers(1, &Text2DUVBufferID);

   // Unbind VAO and delete
   glBindVertexArray(0);
   glDeleteVertexArrays(1, &(Text2DVAO));

   // Delete texture
   glDeleteTextures(1, &Text2DTextureID);

   // Delete shader
   glDeleteProgram(Text2DShaderID);

   // Clear vector
   this->text.clear();
}

//**************************************************************************************************

bool Text2D::addText2D(const char* _id, UINT16 _x, UINT16 _y, UINT8 _size, const char* _message)
{
   // Input validation
   if (_id == NULL || _message == NULL)
   {
      fprintf(stderr, "Arguments cannot be NULL.\n");
      return false;
   }

   //Unique ID?
   for (UINT16 i = 0; i < this->text.size(); i++)
   {
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0)
      {
         fprintf(stderr, "TextID already in use.\n");
         return false;
      }
   }

   // Create and add to vector
   Text2DInfo t;
   strncpy(t.id, _id, MAX_ID_LEN - 1);
   strncpy(t.message, _message, MAX_MESSAGE_LEN - 1);
   t.x = _x;
   t.y = _y;
   t.size = _size;
   this->text.push_back(t);

   return true;
}

//**************************************************************************************************

bool Text2D::removeText2D(const char* _id)
{
   // Does the id exist?
   if (_id == NULL)
   {
      return false;
   }

   for (UINT16 i = 0; i < this->text.size(); i++)
   {
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0)
      {
         this->text.erase(this->text.begin() + i);
         return true;
      }
   }

   return false;
}

//**************************************************************************************************

bool Text2D::removeAll(void)
{
   this->text.clear();
   return true;
}

//**************************************************************************************************

bool Text2D::printText2D(const char* _id)
{
   // Does the id exist?
   if (_id == NULL)
   {
      return false;
   }

   for (UINT16 i = 0; i < this->text.size(); i++)
   {
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0)
      {
         this->printText2D(this->text.at(i).message, this->text.at(i).x, this->text.at(i).y, this->text.at(i).size);
         return true;
      }
   }

   return false;
}

//**************************************************************************************************

bool Text2D::printAll(void)
{
   for (UINT16 i = 0; i < this->text.size(); i++)
   {
      this->printText2D(this->text.at(i).message, this->text.at(i).x, this->text.at(i).y, this->text.at(i).size);
   }
   return true;
}

//**************************************************************************************************

void Text2D::printText2D(const char * text, int x, int y, int size)
{
   unsigned int length = strlen(text);

   // Bind VAO
   glBindVertexArray(this->Text2DVAO);

   // Fill buffers
   std::vector<glm::vec2> vertices;
   std::vector<glm::vec2> UVs;
   for (unsigned int i = 0; i<length; i++)
   {
      glm::vec2 vertex_up_left = glm::vec2(x + i*size, y + size);
      glm::vec2 vertex_up_right = glm::vec2(x + i*size + size, y + size);
      glm::vec2 vertex_down_right = glm::vec2(x + i*size + size, y);
      glm::vec2 vertex_down_left = glm::vec2(x + i*size, y);

      vertices.push_back(vertex_up_left);
      vertices.push_back(vertex_down_left);
      vertices.push_back(vertex_up_right);

      vertices.push_back(vertex_down_right);
      vertices.push_back(vertex_up_right);
      vertices.push_back(vertex_down_left);

      char character = text[i];
      float uv_x = (character % 16) / 16.0f;
      float uv_y = (character / 16) / 16.0f;

      glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
      glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
      glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
      glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
      UVs.push_back(uv_up_left);
      UVs.push_back(uv_down_left);
      UVs.push_back(uv_up_right);

      UVs.push_back(uv_down_right);
      UVs.push_back(uv_up_right);
      UVs.push_back(uv_down_left);
   }

   glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
   glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

   // Bind shader
   glUseProgram(Text2DShaderID);

   // Bind texture
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, Text2DTextureID);

   // Set our "myTextureSampler" sampler to user Texture Unit 0
   glUniform1i(Text2DUniformID, 0);

   // 1rst attribute buffer : vertices
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

   // 2nd attribute buffer : UVs
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Draw call
   glDrawArrays(GL_TRIANGLES, 0, vertices.size());

   glDisable(GL_BLEND);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   // Unbind
   glBindVertexArray(0);
}
