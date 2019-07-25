//Dependencies======================================================================================
//==================================================================================================
#include "Simulator.hpp"

//Method Definitions================================================================================
//==================================================================================================
GLFWwindow* Simulator::window = NULL;
UINT16 Simulator::windowX = 1024;
UINT16 Simulator::windowY = 768;
Simulator::ControlInputs Simulator::controlInputs
{
   std::vector<UINT16>(),
   std::vector<KeyPressInfo>(),
   std::vector<UINT16>({ GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_RIGHT,
   GLFW_KEY_LEFT, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3,
   GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6, GLFW_KEY_F7,
   GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10, GLFW_KEY_F11, GLFW_KEY_F12 })
};

//**************************************************************************************************

Simulator::Simulator(const char* WindowName, UINT16 WindowX, UINT16 WindowY, UINT32 flags)
{
   // Initialize GLFW
   if (!glfwInit())
   {
      fprintf(stderr, "Failed to initialize GLFW\n");
      exit(1);
   }

   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   this->windowX = WindowX;
   this->windowY = WindowY;

   // Open a window and create its OpenGL context
   if ((flags & SimulatorEngineCollections::FullScreen) == SimulatorEngineCollections::FullScreen)
   {
      const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      this->windowX = mode->width;
      this->windowY = mode->height;
      window = glfwCreateWindow(windowX, windowY, WindowName, glfwGetPrimaryMonitor(), NULL);
   }
   else
   {
      window = glfwCreateWindow(windowX, windowY, WindowName, NULL, NULL);
   }
   if (window == NULL)
   {
      fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
      glfwTerminate();
      exit(1);
   }
   glfwMakeContextCurrent(window);

   // Initialize GLEW
   glewExperimental = true; // Needed for core profile
   if (glewInit() != GLEW_OK)
   {
      fprintf(stderr, "Failed to initialize GLEW\n");
      exit(1);
   }

   // GLFW callbacks
   glfwSetWindowSizeCallback(Simulator::window, Simulator::window_size_callback);
   glfwSetKeyCallback(Simulator::window, Simulator::key_callback);

   // Ensure we can capture the escape key being pressed below
   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
   glfwSetCursorPos(window, windowX / 2, windowY / 2);

   // Enable depth test
   glEnable(GL_DEPTH_TEST);
   // Accept fragment if it closer to the camera than the former one
   glDepthFunc(GL_LESS);
   // Cull triangles which normal is not towards the camera
   glEnable(GL_CULL_FACE);

   // Background clear color
   glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

   // Initialize Control FLags
   seCollections.setControlFlags(flags);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

   // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
   seCollections.setProjectionMatrix(glm::perspective(FOV, ((float)windowX / (float)windowY), NEAR_CLIP, FAR_CLIP));

   this->setCurrentCameraID("");
}

//**************************************************************************************************

Simulator::~Simulator(void)
{
   // Delete background and stage components
   this->setBackground(NULL);
   this->setStage(NULL);

   // Close OpenGL window and terminate GLFW
   glfwTerminate();
}

//**************************************************************************************************

void Simulator::window_size_callback(GLFWwindow* window, int width, int height)
{
   Simulator::windowX = width;
   Simulator::windowY = height;
   // Set the projection matrix
   seCollections.setProjectionMatrix(glm::perspective(FOV, (float)width / (float)height, NEAR_CLIP, FAR_CLIP));
}

//**************************************************************************************************

void Simulator::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   // Is this a key_press event?
   if (action != GLFW_PRESS)
   {
      return;
   }

   // Is this key a special simulator key?
   for (UINT16 i = 0; i < Simulator::controlInputs.simulatorKeys.size(); i++)
   {
      if (key == Simulator::controlInputs.simulatorKeys.at(i))
      {
         // Does the key already exist in the vector?
         for (UINT16 j = 0; j < Simulator::controlInputs.systemKeyInputs.size(); j++)
         {
            if (key == Simulator::controlInputs.systemKeyInputs.at(j))
            {
               // already exists... we are done.
               return;
            }
         }
         //Key hasn't been added to vector, so add it
         Simulator::controlInputs.systemKeyInputs.push_back((UINT16)key);
         return;
      }
   }
   //Key is not special, but possibly user defined
   for (UINT16 j = 0; j < Simulator::controlInputs.generalKeyInputs.size(); j++)
   {
      if (key == Simulator::controlInputs.generalKeyInputs.at(j).keyPress)
      {
         //already exists... we are done.
         return;
      }
   }
   //Key hasn't been added to vector, so add it
   KeyPressInfo kpi;
   kpi.keyPress = key;
   kpi.modifiers = mods;
   Simulator::controlInputs.generalKeyInputs.push_back(kpi);

   return;
}

//**************************************************************************************************

Object3D* Simulator::getStage(void)
{
   return this->stage;
}

//**************************************************************************************************

void Simulator::setStage(Object3D* stage)
{
   if (this->stage != NULL)
   {
      delete this->stage;
   }
   this->stage = stage;
}

//**************************************************************************************************

Object3D* Simulator::getBackground(void)
{
   return this->background;
}

//**************************************************************************************************

void Simulator::setBackground(Object3D* background)
{
   if (this->background != NULL)
   {
      delete this->background;
   }
   this->background = background;
}

//**************************************************************************************************

ID_GL Simulator::getCurrentCameraID(void)
{
   return this->currentCameraID;
}

//**************************************************************************************************

bool Simulator::setCurrentCameraID(const char* _id)
{
   // Does the id exist?
   Camera* cam = seCollections.getCamera(_id);
   if (cam == NULL)
   {
      // Turn on free camera mode...
      strncpy(this->currentCameraID.id, "", MAX_ID_LEN - 1);
      return false;
   }
   strncpy(this->currentCameraID.id, _id, MAX_ID_LEN - 1);

   return true;
}

//**************************************************************************************************

void Simulator::handleKeyEvents()
{
   // glfwGetTime is called only once, the first time this function is called
   static double lastTime = glfwGetTime();

   // Compute time difference between current and last frame
   double currentTime = glfwGetTime();
   float deltaTime = float(currentTime - lastTime);
   float hAngle = 0;
   float vAngle = 0;
   glm::mat4 rotation;
   glm::mat4 translation;

   // Get a copy of the current control flags
   UINT16 ctrlFlags = seCollections.getControlFlags();

   // Handle the simulator special inputs
   for (UINT16 i = 0; i < this->controlInputs.systemKeyInputs.size(); i++)
   {
      UINT16 key = this->controlInputs.systemKeyInputs.at(i);
      // Enter/Exit FreeMouse
      if (key == GLFW_KEY_F1)
      {
         ctrlFlags = ctrlFlags ^ SimulatorEngineCollections::FreeMouse;
         if ((ctrlFlags & SimulatorEngineCollections::FreeMouse) != SimulatorEngineCollections::FreeMouse)
         {
            glfwSetCursorPos(window, this->windowX / 2, this->windowY / 2);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
         }
         else
         {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
         }
      }
      // Enter/Exit FreeCamera
      else if (key == GLFW_KEY_F2)
      {
         ctrlFlags = ctrlFlags ^ SimulatorEngineCollections::FreeCamera;
      }
      // Next Camera
      else if (key == GLFW_KEY_F3)
      {
         // Does there exist at least one camera?
         if (seCollections.getCollection(SimulatorEngineCollections::C_CAMERA)->size() > 0)
         {
            // Turn off free camera mode if it is on
            if ((ctrlFlags & SimulatorEngineCollections::FreeCamera) == SimulatorEngineCollections::FreeCamera)
            {
               ctrlFlags = ctrlFlags ^ SimulatorEngineCollections::FreeCamera;
            }

            // Get a list of the available cameras
            std::vector<std::string> vals;
            vals.reserve(seCollections.getCollection(SimulatorEngineCollections::C_CAMERA)->size());

            for (auto kv : *(seCollections.getCollection(SimulatorEngineCollections::C_CAMERA)))
            {
               vals.push_back(kv.first);
            }

            // Find the matching string to ID, then update
            UINT8 nextIndex = -1;
            for (UINT16 k = 0; k < vals.size(); k++)
            {
               if (strncmp(this->currentCameraID.id, &(vals.at(k).at(0)), MAX_ID_LEN) == 0)
               {
                  nextIndex = k + 1;
                  if (nextIndex >= vals.size())
                  {
                     nextIndex = 0;
                  }
                  break;
               }
            }

            if (nextIndex == (UINT8)-1)
            {
               fprintf(stderr, "Warning, current camera ID was not found in collection\n");
               nextIndex = 0;
            }

            // Set the current camera
            this->setCurrentCameraID(&(vals.at(nextIndex).at(0)));
         }
      }
      // Hide/Show Text2D
      else if (key == GLFW_KEY_F4)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::HideText2D);
      }
      // Mute/UnMute Audio
      else if (key == GLFW_KEY_F5)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::MuteAudio);
         // TODO - Mute all audios?
      }
      // On/Off max UPS and FPS
      else if (key == GLFW_KEY_F6)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::MaxUPS_FPS);
      }
      // On/Off shadow rendering
      else if (key == GLFW_KEY_F7)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::DisableShadows);
      }
      // Show/Hide FPS
      else if (key == GLFW_KEY_F11)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::ShowFPS);
      }
      // Show/Hide Text2D about author
      else if (key == GLFW_KEY_F12)
      {
         ctrlFlags = (ctrlFlags ^ SimulatorEngineCollections::ShowAbout);
      }
   }

   // Free Camera Mode?
   SimulatorEngineCollections::ITERATOR iter = seCollections.getCollection(SimulatorEngineCollections::C_CAMERA)->find(this->getCurrentCameraID().id);
   if (((ctrlFlags & SimulatorEngineCollections::FreeCamera) == SimulatorEngineCollections::FreeCamera) || (iter == seCollections.getCollection(SimulatorEngineCollections::C_CAMERA)->end()))
   {
      // Rotations due to mouse?
      if ((ctrlFlags & SimulatorEngineCollections::FreeMouse) != SimulatorEngineCollections::FreeMouse)
      {
         // Get mouse position
         double xpos, ypos;
         glfwGetCursorPos(window, &xpos, &ypos);

         // Reset mouse position for next frame
         glfwSetCursorPos(window, this->windowX / 2, this->windowY / 2);

         // Compute new orientation
         hAngle += MOUSE_SPEED * float(this->windowX / 2 - xpos);
         vAngle += MOUSE_SPEED * float(this->windowY / 2 - ypos);
      }

      // Direction : Spherical coordinates to Cartesian coordinates conversion
      rotation = glm::rotate(glm::mat4(), -hAngle, glm::vec3(0, 1, 0)) *
         glm::rotate(glm::mat4(), -vAngle, glm::vec3(1, 0, 0)) * rotation;

      // Move forward
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      {
         translation = glm::translate(glm::mat4(), glm::vec3(0, 0, MOVE_SPEED)) * translation;
      }
      // Move backward
      else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      {
         translation = glm::translate(glm::mat4(), glm::vec3(0, 0, -MOVE_SPEED)) * translation;
      }
      // Strafe right
      else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      {
         translation = glm::translate(glm::mat4(), glm::vec3(-MOVE_SPEED, 0, 0)) * translation;
      }
      // Strafe left
      else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      {
         translation = glm::translate(glm::mat4(), glm::vec3(MOVE_SPEED, 0, 0)) * translation;
      }

      // Update the ViewMatrix
      seCollections.setViewMatrix(translation * rotation * seCollections.getViewMatrix());
   }
   // Camera Mode
   else
   {
      // Get the updated viewMatrix
      Camera* camT = dynamic_cast<Camera*>(iter->second);
      if (camT == NULL)
      {
         fprintf(stderr, "Warning! Corruption in the Camera Collection\n");
         //Turn on freeCam mode
         ctrlFlags = ctrlFlags ^ SimulatorEngineCollections::FreeCamera;
      }

      // Update Camera
      camT->update();
      // Get ViewMatrix
      seCollections.setViewMatrix(camT->getViewMatrix());
   }

   // Go through user-defined controls
   for (UINT16 i = 0; i < this->controlInputs.generalKeyInputs.size(); i++)
   {
      UINT16 key = this->controlInputs.generalKeyInputs.at(i).keyPress;
      UINT8 mods = this->controlInputs.generalKeyInputs.at(i).modifiers;

      // Is this key binded?
      SimulatorEngineCollections::ITERATOR iter = seCollections.getCollection(SimulatorEngineCollections::C_CONTROL)->find(ControlBind::getStringHash(this->controlInputs.generalKeyInputs.at(i)).id);
      if (iter != seCollections.getCollection(SimulatorEngineCollections::C_CONTROL)->end())
      {
         // Attempt to dynamic_cast
         ControlBind* cbT = dynamic_cast<ControlBind*>(iter->second);
         if (cbT == NULL)
         {
            fprintf(stderr, "Warning! Corruption in ControlBind Collection\n");
         }
         else
         {
            cbT->runCallBack();
         }
      }
   }

   // Clear the input vectors
   this->controlInputs.generalKeyInputs.clear();
   this->controlInputs.systemKeyInputs.clear();

   // For the next frame, the "last time" will be "now"
   lastTime = currentTime;

   // Set the control flags
   seCollections.setControlFlags(ctrlFlags);
}

//**************************************************************************************************

void Simulator::update(void)
{
   // Update the background
   if (this->background != NULL)
   {
      this->background->update();
   }

   // Update the stage
   if (this->stage != NULL)
   {
      this->stage->update();
   }

   // Update the instances
   for (UINT16 i = 0; i < seCollections.getCollection(SimulatorEngineCollections::C_INSTANCE)->size(); i++)
   {
      for (auto kv : *(seCollections.getCollection(SimulatorEngineCollections::C_INSTANCE)))
      {
         Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
         if (objPtr != NULL)
         {
            objPtr->update();
         }
         else
         {
            fprintf(stderr, "Warning, corruption in Instance Collection.\n");
         }
      }
   }

   // Updates from inputs
   this->handleKeyEvents();

   // Update the lights
   for (UINT16 i = 0; i < seCollections.getCollection(SimulatorEngineCollections::C_LIGHT)->size(); i++)
   {
      for (auto kv : *(seCollections.getCollection(SimulatorEngineCollections::C_LIGHT)))
      {
         Light* liPtr = dynamic_cast<Light*>(kv.second);
         if (liPtr != NULL)
         {
            liPtr->update();
         }
         else
         {
            fprintf(stderr, "Warning, corruption in Light Collection.\n");
         }
      }
   }
}

//**************************************************************************************************

void Simulator::draw(void)
{
   /* Rendering will be in 2 steps. The first step will take into account what the scene looks like
   from the point of view of the light sources, and the appropriate FrameBuffers will be filled.
   Only the objects that block light will be rendered in this first group. The second rendering
   will take into account all the objects. Each object can link to the appropriate FrameBuffer via
   its light collection. */

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   // Set the viewport
   glViewport(0, 0, this->windowX, this->windowY);

   // Reset some values
   glCullFace(GL_BACK);
   glClearColor(0,0,1.0,0);

   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Create a list of objects to draw later b/c transparent
   std::vector<Object3D*> transparentList;
   // Render all non-transparent objects
   {
      if (this->background != NULL)
      {
         if (this->background->isTransparent())
         {
            transparentList.push_back(this->background);
         }
         else
         {
            this->background->draw();
         }
      }

      if (this->stage != NULL)
      {
         if (this->stage->isTransparent())
         {
            transparentList.push_back(this->stage);
         }
         else
         {
            this->stage->draw();
         }
      }

      // Draw the instances
      for (auto kv : *(seCollections.getCollection(SimulatorEngineCollections::C_INSTANCE)))
      {
         // Attempt to cast to Object
         Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
         if (objPtr == NULL)
         {
            fprintf(stderr, "Warning! Corruption in Instance Collection\n");
            continue;
         }
         if (objPtr->isTransparent())
         {
            transparentList.push_back(objPtr);
         }
         else
         {
            objPtr->draw();
         }
      }
      // Draw transparent instances
      for (UINT16 i = 0; i < transparentList.size(); i++)
      {
         transparentList.at(i)->draw();
      }
   }

   // Draw the Text2D
   {
      if ((seCollections.getControlFlags() & SimulatorEngineCollections::HideText2D) != SimulatorEngineCollections::HideText2D)
      {
         // Draw Collection
         seCollections.getText2DCollection()->printAll();
         // Draw FPS
         if ((seCollections.getControlFlags() & SimulatorEngineCollections::ShowFPS) == SimulatorEngineCollections::ShowFPS)
         {
            char fpsT[10];
            sprintf_s(fpsT, "%u", (UINT16)this->fps);
            seCollections.getText2DCollection()->printText2D(fpsT, 2, 2, 15);
         }
         // Draw About
         if ((seCollections.getControlFlags() & SimulatorEngineCollections::ShowAbout) == SimulatorEngineCollections::ShowAbout)
         {
            char text[128] = {0};
            #if defined(_MSC_VER) || defined(__WINDOWS__)
            sprintf_s(text, "JFowler Simulator - Version: %d.%d", MAJOR_VERSION, MINOR_VERSION);
            #else
            snprintf(text, 128, "JFowler Simulator - Version: %d.%d", MAJOR_VERSION, MINOR_VERSION);
            #endif

            seCollections.getText2DCollection()->printText2D(text, 50, 2, 15);
         }
      }
   }

}

//**************************************************************************************************

void Simulator::runSimulation(void)
{
   double lastLoop = glfwGetTime() * 1000;
   double currentLoop = 0;
   double deltaTime = 0;
   this->fps = 0;

   do
   {
      currentLoop = glfwGetTime() * 1000;
      deltaTime = currentLoop - lastLoop;
      this->fps = (UINT16)(1000 / deltaTime);

      // If MAXUPS_FPS flag is set...
      if ((seCollections.getControlFlags() & SimulatorEngineCollections::MaxUPS_FPS) != SimulatorEngineCollections::MaxUPS_FPS)
      {
         if (deltaTime < 16.666)
         {
            continue;
         }
      }

      // Update
      this->update();

      // Draw
      this->draw();

      // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();

      lastLoop = currentLoop;

   } // Check if the ESC key was pressed or the window was closed
   while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}
