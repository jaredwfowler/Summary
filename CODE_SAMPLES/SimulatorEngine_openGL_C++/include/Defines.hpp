#ifndef DEFINES_HPP
#define DEFINES_HPP

//Defines===========================================================================================
//==================================================================================================

#define MAJOR_VERSION       2
#define MINOR_VERSION       1

#define NA                  -1
#define MAX_FILE_LEN        256
#define MAX_ID_LEN          64
#define MAX_LIGHTS          64
#define MAX_RAND_FLOATS     64
#define MAX_TEXTURES        64
#define MAX_MESSAGE_LEN     1024
#define PI                  360.0f
#define PI2                 720.0f
#define TEXTURE             "Texture_%u"
#define DEPTH_TEXTURE       "DepthTexture_%u"
#define LIGHT               "Light_%u"
#define DEPTH_BIAS_MVP      "DBMVP_%u"
#define RANDOM_FLOAT        "Rand_%u"
#define M_MATRIX            "M"
#define V_MATRIX            "V"
#define MV_MATRIX           "MV"
#define MVP_MATRIX          "MVP"
#define FOV                 45.0f
#define MOVE_SPEED          0.1f
#define MOUSE_SPEED         0.1f
#define NEAR_CLIP           0.1f
#define FAR_CLIP            10000.0f
#define VOLUME_LVL          0.5f

#define NULL_ID             -11
#define NULL_FILE           -12
#define MODEL_FAIL          -13
#define AUDIO_FAIL          -14
#define SHADER_FAIL         -15
#define TEXTURE_FAIL        -16
#define TEXT2D_FAIL         -17
#define CONTROL_FAIL        -18
#define CAMERA_FAIL         -19
#define OBJECT_FAIL         -20
#define LIGHT_FAIL          -21

//TypeDefs==========================================================================================
//==================================================================================================

typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef short               INT16;
typedef int                 INT32;
typedef long long           INT64;

typedef struct
{
   char file[MAX_FILE_LEN];
}
FILE_GL;

typedef struct
{
   char id[MAX_ID_LEN];
}
ID_GL;

typedef struct
{
   UINT16 keyPress;
   UINT8 modifiers;
}
KeyPressInfo;

//Includes==========================================================================================
//==================================================================================================

// GL / GLM / GLFW

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

// Simulator Engine

class Element;
class Shader;
class Texture;
class Model;
class Audio;
class ControlBind;
class Text2D;
class Light;
class Object3D;
class Camera;
class SkyBox;

// C++

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>


//#include <Windows.h>
//#include <wtypes.h>

//**************************************************************************************************

//--------------------------------------------------------------------------------------------------
//Name:
// SimulatorEngineCollections
//
//About:
// In a long struggle to decide where and how to store the common collection of data which i needed
// for the various classes in Simulator Engine, I have decided to make a global monostate class 
// which will be in charge of storing the collections of objects. The earlier approach was to have
// these collections reside in Simulator, however, this caused dependency looping head-aches. 
//
//--------------------------------------------------------------------------------------------------
class SimulatorEngineCollections
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   typedef std::unordered_map<std::string, Element*>::const_iterator ITERATOR;

   enum Collections
   {
      C_INSTANCE = 0x0001,
      C_SHADER = 0x0002,
      C_MODEL = 0x0004,
      C_TEXTURE = 0x0008,
      C_LIGHT = 0x0010,
      C_AUDIO = 0x0020,
      C_CAMERA = 0x0040,
      C_CONTROL = 0x0080
   };

   enum ControlFlags
   {
      FreeMouse = 0x0001,
      DisableText = 0x0002,
      FreeCamera = 0x0004,
      HideText2D = 0x0008,
      MuteAudio = 0x0010,
      MaxUPS_FPS = 0x0020,
      FullScreen = 0x0040,
      ShowAbout = 0x0080,
      ShowFPS = 0x0100,
      DisableShadows = 0x0200,
      NONE = 0x0000
   };

   //Constructor(s)=================================================================================
   //===============================================================================================
   SimulatorEngineCollections(void);

   //Destructor=====================================================================================
   //===============================================================================================
   ~SimulatorEngineCollections(void);

   //Gets===========================================================================================
   //===============================================================================================
   UINT32 getControlFlags(void);
   glm::mat4 getViewMatrix(void);
   glm::mat4 getProjectionMatrix(void);
   glm::mat4 getDepthViewMatrix(void) {}
   glm::mat4 getDepthProjectionMatrix(void) {}

   Text2D* getText2DCollection(void);

   // Returns a pointer to a specified element found by matching ID. If not found, returns NULL.

   Object3D* getInstance(const char* _id);
   Shader* getShader(const char* _id);
   Model* getModel(const char* _id);
   Texture* getTexture(const char* _id);
   Light* getLight(const char* _id);
   Audio* getAudio(const char* _id);
   Camera* getCamera(const char* _id);
   ControlBind* getInputControl(KeyPressInfo _kpi);
   std::unordered_map<std::string, Element*>* getCollection(Collections collection);

   //Sets===========================================================================================
   //===============================================================================================
   void setProjectionMatrix(glm::mat4);
   void setViewMatrix(glm::mat4);
   void setControlFlags(UINT32);

   //Do=============================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // initText2D
   //
   //Arguments:
   // _id                     - Name of the text 2d collection
   // _filePathDDS            - Path to DDS file
   // _vertex                 - Vertex Shader file path
   // _fragment               - Fragment Shader file path
   //
   //Output:
   // void
   //
   //Returns:
   // bool                    - Success
   //
   //About:
   // Text2D has its own vertex and fragment shaders. This must be called before adding any text to
   // the screen. If this has already been called, the current values are discarded and the new one
   // takes its place. This will in-turn, also clear all the current text.
   //
   //-----------------------------------------------------------------------------------------------
   bool initText2D(const char* _id, const char* _filePathDDS, const char* _vertex, const char* _fragment);

   // Adds the corresponding structure to a collection held within a vector. Returns false if the
   // id is already taken, else, adds the element and returns true if successful.

   bool addShader(Shader* _ptr);
   bool addTexture(Texture* _ptr);
   bool addModel(Model* _ptr);
   bool addInstance(Object3D* _ptr);
   bool addLight(Light* _ptr);
   bool addAudio(Audio* _ptr);
   bool addCamera(Camera* _ptr);
   bool addInputControl(ControlBind* _ptr);
   bool addText2D(const char* _id, int _x, int _y, UINT8 _size, const char* _message);

   // If the corresponding element with 'id' exists, it is removed from the collection and true is
   // returned, else, returns false.

   bool removeShader(const char* _id);
   bool removeTexture(const char* _id);
   bool removeModel(const char* _id);
   bool removeInstance(const char* _id);
   bool removeLight(const char* _id);
   bool removeAudio(const char* _id);
   bool removeCamera(const char* _id);
   bool removeInputControl(KeyPressInfo _kpi);
   bool removeText2D(const char* _id);

   // Removes information from the GPU and removes all elements in the corresponding collection.

   void clearShaders(void);
   void clearTextures(void);
   void clearModels(void);
   void clearInstance(void);
   void clearLights(void);
   void clearAudio(void);
   void clearCameras(void);
   void clearInputControls(void);
   void clearText2D(void);

private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   static std::unordered_map<std::string, Element*> instanceCollection;
   static std::unordered_map<std::string, Element*> modelCollection;
   static std::unordered_map<std::string, Element*> shaderCollection;
   static std::unordered_map<std::string, Element*> lightCollection;
   static std::unordered_map<std::string, Element*> audioCollection;
   static std::unordered_map<std::string, Element*> cameraCollection;
   static std::unordered_map<std::string, Element*> textureCollection;
   static std::unordered_map<std::string, Element*> inputControlCollection;
   static Text2D* text2DCollection;

   // Shared Matrices
   static glm::mat4 ViewMatrix;
   static glm::mat4 ProjectionMatrix;

   static UINT32 controlFlags;

   // There should only need be one instance of each object.

   void operator= (const SimulatorEngineCollections& m) {}
   SimulatorEngineCollections(const SimulatorEngineCollections& m);
};

extern SimulatorEngineCollections seCollections;

//**************************************************************************************************

//--------------------------------------------------------------------------------------------------
//Name:
// get3D_ScaledVector
//
//Arguments:
// directionalVector       - Input vector
// distance                - Length to scale input vector to
//
//Output:
// void
//
//Returns:
// glm::vec3               - Scaled Vector
//
//About:
// Returns a scaled version of the input vector according to the passed in distance argument.
// Does not modify vector input.
//
//--------------------------------------------------------------------------------------------------
extern glm::vec3 get3D_ScaledVector(glm::vec3 directionalVector, float distance);

#endif
