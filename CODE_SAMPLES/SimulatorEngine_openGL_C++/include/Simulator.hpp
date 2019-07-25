#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  March 14, 2017
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: March 14, 2017
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Simulator is the encapsulating class for all openGL operations. It contains collections of models,
textures, and other variables, along with the functions needed to manipulate such. Handles the
loading, updating, drawing, etc, with calls from main.
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include "Defines.hpp"

// Core Classes
#include "Element.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "Audio.hpp"
#include "ControlBind.hpp"
#include "Text2D.hpp"
#include "Light.hpp"
#include "Object3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"


//Definition========================================================================================
//==================================================================================================
class Simulator
{
public: //##########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   struct ControlInputs
   {
      std::vector<UINT16> systemKeyInputs;
      std::vector<KeyPressInfo> generalKeyInputs;
      std::vector<UINT16> simulatorKeys;
   };

   //Constructor(s)=================================================================================
   //===============================================================================================
   Simulator(const char* WindowName, UINT16 WindowX, UINT16 WindowY, UINT32 flags);

   //Destructor=====================================================================================
   //===============================================================================================
   ~Simulator(void);

   //Gets===========================================================================================
   //===============================================================================================
   Object3D* getBackground(void);
   Object3D* getStage(void);

   ID_GL getCurrentCameraID(void);

   //Sets===========================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // setBackground
   //
   //Arguments:
   // background              - Object3D to use as the background
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Sets the background object3D pointer to the passed in argument. Will delete the space used by
   // the previous background and re-assign itself to the new background
   //
   //-----------------------------------------------------------------------------------------------
   void setBackground(Object3D* background);

   //-----------------------------------------------------------------------------------------------
   //Name:
   // setStage
   //
   //Arguments:
   // setStage                - Object3D to use as the stage
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Sets the stage object3D pointer to the passed in argument
   //
   //-----------------------------------------------------------------------------------------------
   void setStage(Object3D* stage);

   //Do=============================================================================================
   //===============================================================================================
   //-----------------------------------------------------------------------------------------------
   //Name:
   // runSimulation
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Large loop which handles control input, updates, draws, etc.
   //
   //-----------------------------------------------------------------------------------------------
   void runSimulation(void);

   bool setCurrentCameraID(const char* id);

   private: //#########################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   Object3D* background = NULL;
   Object3D* stage = NULL;

   ID_GL currentCameraID;
   UINT16 fps;

   static GLFWwindow* window;
   static UINT16 windowX, windowY;

   static ControlInputs controlInputs;

   //Do=============================================================================================
   //===============================================================================================
   // GLFW Callbacks. Called on windows resize event.
   static void window_size_callback(GLFWwindow* window, int width, int height);
   static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

   // Group of functions which handles key input events, both system and user defined.
   void handleKeyEvents(void);

   // Calls the update function for each of the update-able elements
   void update(void);

   // Draws the elements in a specific order. Background, stage, instances, Text2D.
   void draw(void);


   // There should only need be one instance of each object.

   void operator= (const Simulator& m) {}
   Simulator(const Simulator& m);

};
//==================================================================================================
//End Definition====================================================================================

#endif
