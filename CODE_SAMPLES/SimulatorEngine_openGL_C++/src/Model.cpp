//Dependencies======================================================================================
//==================================================================================================
#include "Model.hpp"

//Method Definitions================================================================================
//==================================================================================================
Model::Model(const char* _id, const char* _filePathIBO)
   :Element(_id)
{
   if ( _filePathIBO == NULL)
   {
      fprintf(stderr, "Model file path cannot be NULL\n");
      exit(MODEL_FAIL);
   }
   strncpy(this->file.file, _filePathIBO, MAX_FILE_LEN - 1);

   // Load indexed vertices from .ibo file
   std::vector<UINT32> indices;              // Indexing used to save space
   std::vector<glm::vec3> indexed_vertices;  // Vertices in indexed form
   std::vector<glm::vec2> indexed_texture;   // Texture in indexed form
   std::vector<glm::vec3> indexed_normals;   // Normal vertices in indexed form

   if (!this->loadIBO(_filePathIBO, &indices, &indexed_vertices, &indexed_texture, &indexed_normals))
   {
      fprintf(stderr, "Failed to load Model %s", this->getID().id);
      exit(MODEL_FAIL);
   }

   // Save the number of indices -- Used for drawing later
   this->indexCount = indices.size();

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

   // Load texture vertices into a VBO
   glGenBuffers(1, &(this->vto));
   glBindBuffer(GL_ARRAY_BUFFER, this->vto);
   glBufferData(GL_ARRAY_BUFFER, indexed_texture.size() * sizeof(glm::vec2), &(indexed_texture[0]), GL_STATIC_DRAW);

   // Load indices into an element buffer
   glGenBuffers(1, &(this->veo));
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->veo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(UINT32), &(indices[0]), GL_STATIC_DRAW);

   // Unbind VAO
   glBindVertexArray(0);
}

//**************************************************************************************************

Model::~Model(void)
{
   // Bind the VAO
   glBindVertexArray(this->vao);

   // Unload the vertex buffers
   glDeleteBuffers(1, &(this->vbo));
   glDeleteBuffers(1, &(this->vno));
   glDeleteBuffers(1, &(this->vto));
   glDeleteBuffers(1, &(this->veo));

   // Unbind the VAO and delete it
   glBindVertexArray(0);
   glDeleteVertexArrays(1, &(this->vao));
}

//**************************************************************************************************

GLuint Model::getVAO(void)
{
   return this->vao;
}

//**************************************************************************************************

GLuint Model::getVBO(void)
{
   return this->vbo;
}

//**************************************************************************************************

GLuint Model::getVTO(void)
{
   return this->vto;
}

//**************************************************************************************************

GLuint Model::getVNO(void)
{
   return this->vno;
}

//**************************************************************************************************

GLuint Model::getVEO(void)
{
   return this->veo;
}

//**************************************************************************************************

UINT32 Model::getIndexCount(void)
{
   return this->indexCount;
}

//**************************************************************************************************

bool Model::loadIBO(const char* fileName, std::vector<unsigned int>* indices,
                   std::vector<glm::vec3>* v, std::vector<glm::vec2>* uv, std::vector<glm::vec3>* n)
{
   // Attempt to open the file
   FILE *fp = fopen(fileName, "rb+");

   // Read the number of indices
   unsigned int size;
   if (!fread(&size, sizeof(unsigned int), 1, fp))
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Read the indices
   unsigned int* intPtr = new unsigned int[size];
   if (fread(intPtr, sizeof(unsigned int), size, fp) != size)
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Assign these values to the vector
   for (unsigned int i = 0; i < size; i++)
   {
      indices->push_back(intPtr[i]);
   }
   delete intPtr;
   intPtr = NULL;

   // Read the number of vertices
   if (!fread(&size, sizeof(unsigned int), 1, fp))
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Read the indexed vertices
   float* floatPtr = new float[size * 3];
   if (fread(floatPtr, sizeof(float), size * 3, fp) != size * 3)
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Assign these values to the vector
   for (unsigned int i = 0; i < size * 3; i += 3)
   {
      v->push_back(glm::vec3(floatPtr[i], floatPtr[i + 1], floatPtr[i + 2]));
   }

   // Read the indexed UV's
   if (fread(floatPtr, sizeof(float), size * 2, fp) != size * 2)
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Assign these values to the vector
   for (unsigned int i = 0; i < size * 2; i += 2)
   {
      uv->push_back(glm::vec2(floatPtr[i], floatPtr[i + 1]));
   }

   // Read the indexed normals
   if (fread(floatPtr, sizeof(float), size * 3, fp) != size * 3)
   {
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }

   // Assign these values to the vector
   for (unsigned int i = 0; i < size * 3; i += 3)
   {
      n->push_back(glm::vec3(floatPtr[i], floatPtr[i + 1], floatPtr[i + 2]));
   }
   delete floatPtr;
   floatPtr = NULL;

   fclose(fp);

   return true;
}
