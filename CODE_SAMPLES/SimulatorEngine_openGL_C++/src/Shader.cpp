//Dependencies======================================================================================
//==================================================================================================
#include "Shader.hpp"

//Method Definitions================================================================================
//==================================================================================================
Shader::Shader(const char* _id, const char* _vertex, const char* _fragment)
   :Element(_id)
{
   if (_vertex == NULL)
   {
      fprintf(stderr, "vShaderPath cannot be NULL\n");
      exit(SHADER_FAIL);
   }
   if (_fragment == NULL)
   {
      fprintf(stderr, "fShaderPath cannot be NULL\n");
      exit(SHADER_FAIL);
   }

   strncpy(this->vertexShader.file, _vertex, MAX_FILE_LEN - 1);
   strncpy(this->fragmentShader.file, _fragment, MAX_FILE_LEN - 1);

   // Load Shaders and get handle
   this->shaderID = this->LoadShaders(_vertex, _fragment);
   glUseProgram(this->shaderID);

   // Attempt to get shader uniform handles. If not exist, write NA
   // Note: glGetUniformLocation returns -1 if value isn't found
   if ((this->mMatrixID = glGetUniformLocation(this->shaderID, M_MATRIX)) == -1)
   {
      this->mMatrixID = NA;
   }
   if ((this->vMatrixID = glGetUniformLocation(this->shaderID, V_MATRIX)) == -1)
   {
      this->vMatrixID = NA;
   }
   if ((this->mvMatrixID = glGetUniformLocation(this->shaderID, MV_MATRIX)) == -1)
   {
      this->mvMatrixID = NA;
   }
   if ((this->mvpMatrixID = glGetUniformLocation(this->shaderID, MVP_MATRIX)) == -1)
   {
      this->mvpMatrixID = NA;
   }

   // Get handles for the textures
   char uniformName[50] = { 0 };
   GLuint texTemp = NA;

   // Regular textures
   for (UINT8 i = 0; i < MAX_TEXTURES; i++)
   {
      sprintf(uniformName, TEXTURE, i);
      texTemp = glGetUniformLocation(this->shaderID, uniformName);
      if (texTemp == -1)
      {
         texTemp = NA;
         break;
      }
      this->textureID.push_back(texTemp);
   }

   // Get handles for light uniforms
   for (UINT8 j = 0; j < MAX_LIGHTS; j++)
   {
      sprintf(uniformName, LIGHT, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1)
      {
         texTemp = NA;
         break;
      }
      this->lightID.push_back(texTemp);
   }

   // Get handles for Shadow Textures
   for (UINT8 j = 0; j < MAX_LIGHTS; j++)
   {
      sprintf(uniformName, DEPTH_TEXTURE, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1)
      {
         texTemp = NA;
         break;
      }
      this->depthTextureID.push_back(texTemp);
   }

   // Get handles for DepthBiasMVPmatrices
   for (UINT8 j = 0; j < MAX_LIGHTS; j++)
   {
      sprintf(uniformName, DEPTH_BIAS_MVP, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1)
      {
         texTemp = NA;
         break;
      }
      this->dbmvpID.push_back(texTemp);
   }

   // Get handles for RandomFloats
   for (UINT8 j = 0; j < MAX_RAND_FLOATS; j++)
   {
      sprintf(uniformName, RANDOM_FLOAT, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1)
      {
         texTemp = NA;
         break;
      }
      this->randomFloatID.push_back(texTemp);
   }
}

//**************************************************************************************************

Shader::~Shader(void)
{
   // Unload shader
   glDeleteProgram(this->shaderID);
   this->lightID.clear();
   this->textureID.clear();
}

//**************************************************************************************************

GLuint Shader::getShaderID(void)
{
   return this->shaderID;
}

//**************************************************************************************************

GLuint Shader::getmMatrixID(void)
{
   return this->mMatrixID;
}

//**************************************************************************************************

GLuint Shader::getvMatrixID(void)
{
   return this->vMatrixID;
}

//**************************************************************************************************

GLuint Shader::getmvMatrixID(void)
{
   return this->mvMatrixID;
}

//**************************************************************************************************

GLuint Shader::getmvpMatrixID(void)
{
   return this->mvpMatrixID;
}

//**************************************************************************************************

GLuint Shader::getRenderTextureID(void)
{
   return this->renderTextureID;
}

//**************************************************************************************************

std::vector<GLuint> Shader::getDepthTextureID(void)
{
   return this->depthTextureID;
}

//**************************************************************************************************

std::vector<GLuint> Shader::getDepthMVPID(void)
{
   return this->dbmvpID;
}

//**************************************************************************************************

std::vector<GLuint> Shader::getLightID(void)
{
   return this->lightID;
}

//**************************************************************************************************

std::vector<GLuint> Shader::getTextureID(void)
{
   return this->textureID;
}

//**************************************************************************************************

std::vector<GLuint> Shader::getRandomFloatID(void)
{
   return this->randomFloatID;
}

//**************************************************************************************************

GLuint Shader::LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{
   // Read the Vertex Shader code from the file
   std::string VertexShaderCode;
   std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
   if (VertexShaderStream.is_open())
   {
      std::string Line = "";
      while (getline(VertexShaderStream, Line))
      {
         VertexShaderCode += "\n" + Line;
      }
      VertexShaderStream.close();
   }
   else
   {
      printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
      getchar();
      return 0;
   }

   // Read the Fragment Shader code from the file
   std::string FragmentShaderCode;
   std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
   if (FragmentShaderStream.is_open())
   {
      std::string Line = "";
      while (getline(FragmentShaderStream, Line))
      {
         FragmentShaderCode += "\n" + Line;
      }

      FragmentShaderStream.close();
   }

   return Shader::LoadShaders(VertexShaderCode, FragmentShaderCode);
}

//**************************************************************************************************

GLuint Shader::LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode)
{
   // Create the shaders
   GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
   GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

   GLint Result = GL_FALSE;
   int InfoLogLength;

   // Compile Vertex Shader
   char const * VertexSourcePointer = VertexShaderCode.c_str();
   glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
   glCompileShader(VertexShaderID);

   // Check Vertex Shader
   glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
   glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

   if (InfoLogLength > 0)
   {
      std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
      glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
      printf("%s", &VertexShaderErrorMessage[0]);
   }

   // Compile Fragment Shader
   char const * FragmentSourcePointer = FragmentShaderCode.c_str();
   glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
   glCompileShader(FragmentShaderID);

   // Check Fragment Shader
   glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
   glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

   if (InfoLogLength > 0)
   {
      std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
      glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
      printf("%s", &FragmentShaderErrorMessage[0]);
   }

   // Link the program
   GLuint ProgramID = glCreateProgram();
   glAttachShader(ProgramID, VertexShaderID);
   glAttachShader(ProgramID, FragmentShaderID);
   glLinkProgram(ProgramID);

   // Check the program
   glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
   glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

   if (InfoLogLength > 0)
   {
      std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
      glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
      printf("%s", &ProgramErrorMessage[0]);
   }

   glDeleteShader(VertexShaderID);
   glDeleteShader(FragmentShaderID);

   return ProgramID;
}
