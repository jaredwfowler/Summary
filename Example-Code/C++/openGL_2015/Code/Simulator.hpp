/*Created By: Jared Fowler
**June 8, 2015
**
**Simulator is the encapsulating class for all openGL
**operations. It contains collections of models,
**textures, and other variables, along with the
**functions needed to manipulate such.  Handles
**the loading, updating, drawing, etc, with calls
**from main.
**
**Made to run of MS Windows. Tested on Windows 8.1
**
*/



//DEFINES*****************************************
//************************************************
#define NA                 -1
#define MAX_FILE_LEN       256
#define MAX_ID_LEN         64
#define MAX_LIGHTS         64
#define MAX_RAND_FLOATS    64
#define MAX_TEXTURES       64
#define MAX_MESSAGE_LEN    1024
#define PI                 360.0f  //(3.14159f)
#define PI2                720.0f  //(6.28318f)
#define TEXTURE            "Texture_%u"
#define DEPTH_TEXTURE      "DepthTexture_%u"
#define LIGHT              "Light_%u"
#define DEPTH_BIAS_MVP     "DBMVP_%u"
#define RANDOM_FLOAT       "Rand_%u"
#define M_MATRIX           "M"
#define V_MATRIX           "V"
#define MV_MATRIX          "MV"
#define MVP_MATRIX         "MVP"
#define FOV                45.0f
#define MOVE_SPEED         0.1f
#define MOUSE_SPEED        0.1f
#define NEAR_CLIP          0.1f
#define FAR_CLIP           10000.0f
#define VOLUME_LVL         0.5f

//Error Codes
#define NULL_ID            -11
#define NULL_FILE          -12
#define MODEL_FAIL         -13
#define AUDIO_FAIL         -14
#define SHADER_FAIL        -15
#define TEXTURE_FAIL       -16
#define TEXT2D_FAIL        -17
#define CONTROL_FAIL       -18
#define CAMERA_FAIL        -19
#define OBJECT_FAIL        -20
#define LIGHT_FAIL         -21

#ifndef Simulator_HPP
#define Simulator_HPP

//TYPEDEFS****************************************
//************************************************
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef short               INT16;
typedef int                 INT32;
typedef long long           INT64;

typedef struct{
   char file[MAX_FILE_LEN];
} FILE_GL;

typedef struct{
   char id[MAX_ID_LEN];
} ID_GL;

//INCLUDES****************************************
//************************************************
//#pragma comment(lib, "../Resources/Libraries/irrKlang.lib")
//#pragma comment(lib, "../Resources/Libraries/glfw3.lib")
//#pragma comment(lib, "../Resources/Libraries/glew32.lib")

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

#include <Windows.h>
#include <wtypes.h>
#include <Core/GL/glew.h>
#include <Core/glm/glm.hpp>
#include <Core/glm/gtc/constants.hpp>
#include <Core/glm/gtc/matrix_transform.hpp>
#include <Core/glm/gtc/type_ptr.hpp>
#include <Core/GLFW/glfw3.h>

//Audio Library
#include <Core/irrKlang/irrKlang.h>
#include <conio.h>

//Core Classes
#include "Core/JWFs_openGL/Element.hpp"
#include "Core/JWFs_openGL/Shader.hpp"
#include "Core/JWFs_openGL/Texture.hpp"
#include "Core/JWFs_openGL/Model.hpp"
#include "Core/JWFs_openGL/Audio.hpp"
#include "Core/JWFs_openGL/ControlBind.hpp"
#include "Core/JWFs_openGL/Text2D.hpp"
#include "Core/JWFs_openGL/Light.hpp"
#include "Core/JWFs_openGL/Object3D.hpp"
#include "Core/JWFs_openGL/Camera.hpp"
#include "Core/JWFs_openGL/Background.hpp"

//CLASS :: Simulator *******************************************************
//**************************************************************************
class Simulator
{
public:
   // Enums #########################################################
   //################################################################
   static enum ControlFlags{
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

   static enum Collections{
      C_INSTANCE = 0x0001,
      C_SHADER = 0x0002,
      C_MODEL = 0x0004,
      C_TEXTURE = 0x0008,
      C_LIGHT = 0x0010,
      C_AUDIO = 0x0020,
      C_CAMERA = 0x0040,
      C_CONTROL = 0x0080
   };

   typedef std::unordered_map<std::string, Element*>::const_iterator ITERATOR;

   // Structure Defs ################################################
   //################################################################
   
   struct ControlInputs{
      std::vector<UINT16> systemKeyInputs;
      std::vector<ControlBind::KeyPressInfo> generalKeyInputs;
      std::vector<UINT16> simulatorKeys;
   };

   // Constructor(s) / Destructor ###################################
   //################################################################
   Simulator(const char* WindowName, 
      UINT16 WindowX, 
      UINT16 WindowY, 
      UINT32 flags);
   ~Simulator(void);

   // Main Functions ################################################
   //################################################################
   //****************************************************************
   //Large loop which handles control input, updates, draws, etc.
   //****************************************************************
   void runSimulation(void);
   
   // Gets/Sets #####################################################
   //################################################################
   //****************************************************************
   //Sets the background object3D pointer to the passed in argument.
   //Will delete the space used by the previous background and 
   //re-assign itself to the new background
   //****************************************************************
   void setBackground(Object3D* background);
   //****************************************************************
   //Returns a pointer to the current background Object3D
   //****************************************************************
   inline Object3D* getBackground(void);
   //****************************************************************
   //Sets the stage object3D pointer to the passed in argument. This
   //object will take control of the dynamic memory.
   //****************************************************************
   void setStage(Object3D* stage);
   //****************************************************************
   //Returns a pointer to the current stage Object3D
   //****************************************************************
   inline Object3D* getStage(void);
   //****************************************************************
   //View and Projection Matrix Handles
   //****************************************************************
   inline glm::mat4 getViewMatrix(void);
   inline glm::mat4 getProjectionMatrix(void);
   inline glm::mat4 getDepthViewMatrix(void);
   inline glm::mat4 getDepthProjectionMatrix(void);
   //****************************************************************
   //Camera Handlers - Set/Get current camera id
   //****************************************************************
   inline ID_GL getCurrentCameraID(void);
   inline bool setCurrentCameraID(const char* id);
   //****************************************************************
   //Control Flags Get/Set
   //****************************************************************
   inline UINT32 getControlFlags(void);
   inline void setControlFlags(UINT32);

   // Tools #########################################################
   //################################################################
   //****************************************************************
   //Returns a scaled version of the input vector according to the 
   //passed in distance argument. Does not modify vector input.
   //****************************************************************
   static glm::vec3 get3D_ScaledVector(glm::vec3 directionalVector, float distance);
   //****************************************************************
   //Adds the coresponding structure to a collection held within
   //a vector. Returns false if the id is already taken, else, adds
   //the element and returns true if successful.
   //****************************************************************
   bool addShader(Shader* _ptr);
   bool addTexture(Texture* _ptr);
   bool addModel(Model* _ptr);
   bool addInstance(Object3D* _ptr);
   bool addLight(Light* _ptr);
   bool addAudio(Audio* _ptr);
   bool addCamera(Camera* _ptr);
   bool addInputControl(ControlBind* _ptr);
   bool addText2D(const char* _id, int _x, int _y, UINT8 _size, const char* _message);
   //****************************************************************
   //If the corresponding element with 'id' exists, it is removed
   //from the collection and true is returned, else, returns false.
   //****************************************************************
   bool removeShader(const char* _id);
   bool removeTexture(const char* _id);
   bool removeModel(const char* _id);
   bool removeInstance(const char* _id);
   bool removeLight(const char* _id);
   bool removeAudio(const char* _id);
   bool removeCamera(const char* _id);
   bool removeInputControl(ControlBind::KeyPressInfo _kpi);
   bool removeText2D(const char* _id);
   //****************************************************************
   //Returns a pointer to a specified element found by matching ID
   //If not found, returns NULL.
   //****************************************************************
   Object3D* getInstance(const char* _id);
   Shader* getShader(const char* _id);
   Model* getModel(const char* _id);
   Texture* getTexture(const char* _id);
   Light* getLight(const char* _id);
   Audio* getAudio(const char* _id);
   Camera* getCamera(const char* _id);
   ControlBind* getInputControl(ControlBind::KeyPressInfo _kpi);
   std::unordered_map<std::string, Element*>* getCollection(Collections collection);
   //****************************************************************
   //Removes information from the GPU and removes all elements in
   //the corresponding collection.
   //****************************************************************
   void clearShaders(void);
   void clearTextures(void);
   void clearModels(void);
   void clearInstance(void);
   void clearLights(void);
   void clearAudio(void);
   void clearCameras(void);
   void clearInputControls(void);
   void clearText2D(void);

private:
   // Variables #####################################################
   //################################################################
   Object3D* background = NULL;
   Object3D* stage = NULL;
   std::unordered_map<std::string, Element*> instanceCollection;
   std::unordered_map<std::string, Element*> modelCollection;
   std::unordered_map<std::string, Element*> shaderCollection;
   std::unordered_map<std::string, Element*> lightCollection;
   std::unordered_map<std::string, Element*> audioCollection;
   std::unordered_map<std::string, Element*> cameraCollection;
   std::unordered_map<std::string, Element*> textureCollection;
   std::unordered_map<std::string, Element*> inputControlCollection;
   Text2D* text2DCollection;
   
   ID_GL currentCameraID;
   UINT16 fps;

   //Shared Matrices
   static glm::mat4 ViewMatrix;
   static glm::mat4 ProjectionMatrix;

   static GLFWwindow* window;
   static UINT16 windowX, windowY;
   static UINT32 controlFlags;
   static ControlInputs controlInputs;

   // GLFW Callbacks ################################################
   //################################################################
   //****************************************************************
   //Called on windows resize event
   //****************************************************************
   static void window_size_callback(GLFWwindow* window, int width, int height);
   static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

   // Helpers #######################################################
   //################################################################
   //****************************************************************
   //Group of functions which handles key input events, both system
   //and user defined.
   //****************************************************************
   void handleKeyEvents(void);
   //****************************************************************
   //Calls the update function for each of the updatable elements
   //****************************************************************
   void update(void);
   //****************************************************************
   //Draws the elements in a specific order. Background, stage, 
   //instances, Text2D.
   //****************************************************************
   void draw(void);
   
};

#endif