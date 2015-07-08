/*Created By: Jared Fowler , yrolg4tseuq@sbcglobal.net
**June 10, 2015
**
**ABOUT:
** Collection of class function implimentations.
*/

#include "Core/JWFs_openGL/Simulator.hpp"
#include "Core/JWFs_openGL/Object3D.hpp"
#include "Core/JWFs_openGL/Camera.hpp"
#include "Core/JWFs_openGL/Element.hpp"
#include "Core/JWFs_openGL/Audio.hpp"
#include "Core/JWFs_openGL/Shader.hpp"
#include "Core/JWFs_openGL/Texture.hpp"
#include "Core/JWFs_openGL/Text2D.hpp"
#include "Core/JWFs_openGL/Light.hpp"
#include "Core/JWFs_openGL/ControlBind.hpp"
#include "Core/JWFs_openGL/Model.hpp"
#include "Core/JWFs_openGL/Background.hpp"

//**************************************************************************
//**************************************************************************
//** SIMULATOR *************************************************************
//**************************************************************************
//Static Initializations
//**************************************************************************
glm::mat4 Simulator::ViewMatrix = glm::mat4();
glm::mat4 Simulator::ProjectionMatrix = glm::mat4();
GLFWwindow* Simulator::window = NULL;
UINT16 Simulator::windowX = 1024;
UINT16 Simulator::windowY = 768;
UINT32 Simulator::controlFlags = NONE;
Simulator::ControlInputs Simulator::controlInputs{
   std::vector<UINT16>(),
   std::vector<ControlBind::KeyPressInfo>(),
   std::vector<UINT16>({ GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_RIGHT,
   GLFW_KEY_LEFT, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3,
   GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6, GLFW_KEY_F7,
   GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10, GLFW_KEY_F11, GLFW_KEY_F12 })
};
//**************************************************************************
Simulator::Simulator(const char* WindowName,
   UINT16 WindowX,
   UINT16 WindowY,
   UINT32 flags)
{
   // Initialise GLFW
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
   if ((flags & FullScreen) == FullScreen){
      const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      this->windowX = mode->width;
      this->windowY = mode->height;
      window = glfwCreateWindow(windowX, windowY, WindowName, glfwGetPrimaryMonitor(), NULL);
   }
   else{
      window = glfwCreateWindow(windowX, windowY, WindowName, NULL, NULL);
   }
   if (window == NULL){
      fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
      glfwTerminate();
      exit(1);
   }
   glfwMakeContextCurrent(window);

   // Initialize GLEW
   glewExperimental = true; // Needed for core profile
   if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      exit(1);
   }

   //GLFW callbacks
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

   //Initialize Control FLags
   this->controlFlags = flags;
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

   // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
   ProjectionMatrix = glm::perspective(FOV, ((float)windowX / (float)windowY), NEAR_CLIP, FAR_CLIP);

   this->setCurrentCameraID("");

   //Initialize Text2D
   text2DCollection = new Text2D("text_v1", this, "../Resources/Content/Textures/Holstein.DDS", "../Resources/Content/Shaders/TextVertexShader.vertexshader", "../Resources/Content/Shaders/TextVertexShader.fragmentshader");
}
//**************************************************************************
Simulator::~Simulator(void){
   //Delete background and stage components
   this->setBackground(NULL);
   this->setStage(NULL);

   //Delete Collections
   this->clearModels();
   this->clearShaders();
   this->clearText2D();
   this->clearTextures();
   this->clearInstance();
   this->clearLights();
   this->clearAudio();
   this->clearCameras();
   this->clearInputControls();

   if (this->text2DCollection != NULL){
      delete this->text2DCollection;
      this->text2DCollection = NULL;
   }

   // Close OpenGL window and terminate GLFW
   glfwTerminate();
}
//**************************************************************************
void Simulator::window_size_callback(GLFWwindow* window, int width, int height)
{
   Simulator::windowX = width;
   Simulator::windowY = height;
   //Set the projection matrix
   ProjectionMatrix = glm::perspective(FOV, (float)width / (float)height, NEAR_CLIP, FAR_CLIP);
}
//**************************************************************************
void Simulator::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
   //Is this a key_press event?
   if (action != GLFW_PRESS)
      return;

   //Is this key a special simulator key?
   for (UINT16 i = 0; i < Simulator::controlInputs.simulatorKeys.size(); i++){
      if (key == Simulator::controlInputs.simulatorKeys.at(i)){
         //Does the key already exist in the vector?
         for (UINT16 j = 0; j < Simulator::controlInputs.systemKeyInputs.size(); j++){
            if (key == Simulator::controlInputs.systemKeyInputs.at(j)){
               //already exists... we are done. 
               return;
            }
         }
         //Key hasn't been added to vector, so add it
         Simulator::controlInputs.systemKeyInputs.push_back((UINT16)key);
         return;
      }
   }
   //Key is not special, but possibly user defined
   for (UINT16 j = 0; j < Simulator::controlInputs.generalKeyInputs.size(); j++){
      if (key == Simulator::controlInputs.generalKeyInputs.at(j).keyPress){
         //already exists... we are done. 
         return;
      }
   }
   //Key hasn't been added to vector, so add it
   ControlBind::KeyPressInfo kpi;
   kpi.keyPress = key;
   kpi.modifiers = mods;
   Simulator::controlInputs.generalKeyInputs.push_back(kpi);
   return;
}
//**************************************************************************
glm::vec3 Simulator::get3D_ScaledVector(glm::vec3 directionalVector, float distance){
   /*This function will take a 3D vector, along with a distance and return an equivalant vector which points
   in the same direction but is scaled to reflect the distance it would cover if the vector was treated as a point
   from the origin.

   Theory:

   d^2 = x^2 + y^2 + z^2

   Lets say that x is the smallest:

   d^2 = 1x^2 + (nx)^2 + (mx)^2
   = (x^2)(1 + n^2 + m^2)

   d / ( (1 + n^2 + m^2)^.5) = x

   Multiply x by original vector values to obtain proper magnitude.
   */

   //Make a copy of the above vector, but make all the values positive
   glm::vec3 allPositive;
   allPositive[0] = (directionalVector[0] < 0) ? -(directionalVector[0]) : directionalVector[0];
   allPositive[1] = (directionalVector[1] < 0) ? -(directionalVector[1]) : directionalVector[1];
   allPositive[2] = (directionalVector[2] < 0) ? -(directionalVector[2]) : directionalVector[2];

   /*We know the distance can be fitted into this equation:  d^2 = x^2 + y^2 + z^2
   We'll have to put two of the variables in perpective with one variable, and then solve.*/

   //Begin by finding out which of x,y,z on the directional vector is the smallest.
   //Warning: we want absolute values, not negative ones!
   //Warning..we don't care about zeros!,, in fact, if not done right, they could cause errors.

   //All values are zero?
   if (allPositive[0] == 0 && allPositive[1] == 0 && allPositive[2] == 0)
      return glm::vec3(0, 0, 0);

   //Find the smallest non-zero value
   float smallest = std::numeric_limits<float>::infinity();
   if (allPositive[0] < smallest  && allPositive[0] != 0)
      smallest = allPositive[0];
   if (allPositive[1] < smallest  && allPositive[1] != 0)
      smallest = allPositive[1];
   if (allPositive[2] < smallest && allPositive[2] != 0)
      smallest = allPositive[2];

   //Now divide the values by the smallest to get a reduced ratio
   allPositive[0] /= smallest;
   allPositive[1] /= smallest;
   allPositive[2] /= smallest;

   //We'll need to square each of these values now and store the sum
   float sum = (float)((pow(allPositive[0], 2)) + (pow(allPositive[1], 2)) + (pow(allPositive[2], 2)));

   //Solve for common variable 'a'
   float var = (float)(abs(distance) / (pow(sum, .5)));

   //We can now multiply this var to directional vector to get distance-directional vector
   allPositive[0] *= var;
   allPositive[1] *= var;
   allPositive[2] *= var;

   //Revert back to both positive and negative values
   if (directionalVector[0] < 0)
      allPositive[0] *= -1;
   if (directionalVector[1] < 0)
      allPositive[1] *= -1;
   if (directionalVector[2] < 0)
      allPositive[2] *= -1;

   //We now have our completed vector, so return it to the caller
   return allPositive;
}
//**************************************************************************
Object3D* Simulator::getStage(void){
   return this->stage;
}
//**************************************************************************
void Simulator::setStage(Object3D* stage){
   if (this->stage != NULL){
      delete this->stage;
   }
   this->stage = stage;
}
//**************************************************************************
Object3D* Simulator::getBackground(void){
   return this->background;
}
//**************************************************************************
void Simulator::setBackground(Object3D* background){
   if (this->background != NULL){
      delete this->background;
   }
   this->background = background;
}
//**************************************************************************
glm::mat4 Simulator::getViewMatrix(){
   return this->ViewMatrix;
}
//**************************************************************************
glm::mat4 Simulator::getProjectionMatrix(){
   return this->ProjectionMatrix;
}
//**************************************************************************
ID_GL Simulator::getCurrentCameraID(void){
   return this->currentCameraID;
}
//**************************************************************************
bool Simulator::setCurrentCameraID(const char* _id){
   //Does the id exist?
   Camera* cam = this->getCamera(_id);
   if (cam == NULL){
      //Turn on free camera mode...
      strncpy(this->currentCameraID.id, "", MAX_ID_LEN - 1);
      return false;
   }
   strncpy(this->currentCameraID.id, _id, MAX_ID_LEN - 1);
   return true;
}
//**************************************************************************
UINT32 Simulator::getControlFlags(void){
   return this->controlFlags;
}
//**************************************************************************
void Simulator::setControlFlags(UINT32 flags){
   this->controlFlags = flags;
}
//**************************************************************************
Object3D* Simulator::getInstance(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->instanceCollection.find(_id);
   if (iter == this->instanceCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Object3D* obj = dynamic_cast<Object3D*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Object3D* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Shader* Simulator::getShader(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->shaderCollection.find(_id);
   if (iter == this->shaderCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Shader* obj = dynamic_cast<Shader*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Shader* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Model* Simulator::getModel(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->modelCollection.find(_id);
   if (iter == this->modelCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Model* obj = dynamic_cast<Model*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Model* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Texture* Simulator::getTexture(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->textureCollection.find(_id);
   if (iter == this->textureCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Texture* obj = dynamic_cast<Texture*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Texture* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Light* Simulator::getLight(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->lightCollection.find(_id);
   if (iter == this->lightCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Light* obj = dynamic_cast<Light*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Light* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Audio* Simulator::getAudio(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->audioCollection.find(_id);
   if (iter == this->audioCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Audio* obj = dynamic_cast<Audio*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Light* cast failed\n");
   }
   return obj;
}
//**************************************************************************
Camera* Simulator::getCamera(const char* _id){
   //Does the id exist?
   ITERATOR iter = this->cameraCollection.find(_id);
   if (iter == this->cameraCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   Camera* obj = dynamic_cast<Camera*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "Camera* cast failed\n");
   }
   return obj;
}
//**************************************************************************
ControlBind* Simulator::getInputControl(ControlBind::KeyPressInfo _kpi){
   //Does the id exist?
   ITERATOR iter = this->inputControlCollection.find(ControlBind::getStringHash(_kpi).id);
   if (iter == this->inputControlCollection.end()){
      return NULL;
   }
   //Attempt to Dynamic Cast
   ControlBind* obj = dynamic_cast<ControlBind*>(iter->second);
   if (obj == NULL){ //Cast failed...
      fprintf(stderr, "ControlBind* cast failed\n");
   }
   return obj;
}
//**************************************************************************
std::unordered_map<std::string, Element*>* Simulator::getCollection(Collections collection){
   //Return a pointer to the collection based on enumerator
   if (collection == Collections::C_AUDIO)
      return &(this->audioCollection);
   else if (collection == Collections::C_CAMERA)
      return &(this->cameraCollection);
   else if (collection == Collections::C_CONTROL)
      return &(this->inputControlCollection);
   else if (collection == Collections::C_INSTANCE)
      return &(this->instanceCollection);
   else if (collection == Collections::C_LIGHT)
      return &(this->lightCollection);
   else if (collection == Collections::C_MODEL)
      return &(this->modelCollection);
   else if (collection == Collections::C_SHADER)
      return &(this->shaderCollection);
   else if (collection == Collections::C_TEXTURE)
      return &(this->textureCollection);
   else
      return NULL;
}
//**************************************************************************
bool Simulator::addShader(Shader* _ptr){
   //Is the id available?
   ITERATOR iter = this->shaderCollection.find(_ptr->getID().id);
   if (iter != this->shaderCollection.end()){
      fprintf(stderr, "ShaderID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->shaderCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addTexture(Texture* _ptr){
   //Is the id available?
   ITERATOR iter = this->textureCollection.find(_ptr->getID().id);
   if (iter != this->textureCollection.end()){
      fprintf(stderr, "TextureID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->textureCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addModel(Model* _ptr){
   //Is the id available?
   ITERATOR iter = this->modelCollection.find(_ptr->getID().id);
   if (iter != this->modelCollection.end()){
      fprintf(stderr, "ModelID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->modelCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addInstance(Object3D* _ptr){
   //Is the id available?
   ITERATOR iter = this->instanceCollection.find(_ptr->getID().id);
   if (iter != this->instanceCollection.end()){
      fprintf(stderr, "InstanceID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->instanceCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addLight(Light* _ptr){
   //Is the id available?
   ITERATOR iter = this->lightCollection.find(_ptr->getID().id);
   if (iter != this->lightCollection.end()){
      fprintf(stderr, "LightID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->lightCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addAudio(Audio* _ptr){
   //Is the id available?
   ITERATOR iter = this->audioCollection.find(_ptr->getID().id);
   if (iter != this->audioCollection.end()){
      fprintf(stderr, "AudioID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->audioCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addCamera(Camera* _ptr){
   //Is the id available?
   ITERATOR iter = this->cameraCollection.find(_ptr->getID().id);
   if (iter != this->cameraCollection.end()){
      fprintf(stderr, "CameraID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->cameraCollection.insert({ _ptr->getID().id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addInputControl(ControlBind* _ptr){
   //Is the id available?
   ITERATOR iter = this->inputControlCollection.find(ControlBind::getStringHash(_ptr->getKeyPressInfo()).id);
   if (iter != this->inputControlCollection.end()){
      fprintf(stderr, "InputControlID %s already exists.\n", iter->first);
      return false;
   }
   //Insert it
   this->inputControlCollection.insert({ ControlBind::getStringHash(_ptr->getKeyPressInfo()).id, _ptr });
   return true;
}
//**************************************************************************
bool Simulator::addText2D(const char* _id, int _x, int _y, UINT8 _size, const char* _message){
   this->text2DCollection->addText2D(_id, _x, _y, _size, _message);
   return true;
}
//**************************************************************************
bool Simulator::removeShader(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->shaderCollection.find(_id);
   if (iter == this->shaderCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->shaderCollection.erase(_id);
   if (ret == 0) 
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeTexture(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->textureCollection.find(_id);
   if (iter == this->textureCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->textureCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeModel(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->modelCollection.find(_id);
   if (iter == this->modelCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->modelCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeInstance(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->instanceCollection.find(_id);
   if (iter == this->instanceCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->instanceCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeLight(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->lightCollection.find(_id);
   if (iter == this->lightCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->lightCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeAudio(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->audioCollection.find(_id);
   if (iter == this->audioCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->audioCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeCamera(const char* _id){
   //Attempt to get element ptr
   ITERATOR iter = this->cameraCollection.find(_id);
   if (iter == this->cameraCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->cameraCollection.erase(_id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeInputControl(ControlBind::KeyPressInfo _kpi){
   //Attempt to get element ptr
   ITERATOR iter = this->inputControlCollection.find(ControlBind::getStringHash(_kpi).id);
   if (iter == this->inputControlCollection.end())
      return false;
   delete iter->second;

   UINT8 ret = this->inputControlCollection.erase(ControlBind::getStringHash(_kpi).id);
   if (ret == 0)
      return false;
   return true;
}
//**************************************************************************
bool Simulator::removeText2D(const char* _id){
   return this->text2DCollection->removeText2D(_id);
}
//**************************************************************************
void Simulator::clearShaders(void){
   for (auto kv : this->shaderCollection) {
      delete kv.second;
   }
   this->shaderCollection.clear();
}
//**************************************************************************
void Simulator::clearTextures(void){
   for (auto kv : this->textureCollection) {
      delete kv.second;
   }
   this->textureCollection.clear();
}
//**************************************************************************
void Simulator::clearModels(void){
   for (auto kv : this->modelCollection) {
      delete kv.second;
   }
   this->modelCollection.clear();
}
//**************************************************************************
void Simulator::clearInstance(void){
   for (auto kv : this->instanceCollection) {
      delete kv.second;
   }
   this->instanceCollection.clear();
}
//**************************************************************************
void Simulator::clearLights(void){
   for (auto kv : this->lightCollection) {
      delete kv.second;
   }
   this->lightCollection.clear();
}
//**************************************************************************
void Simulator::clearAudio(void){
   for (auto kv : this->audioCollection) {
      delete kv.second;
   }
   this->audioCollection.clear();
}
//**************************************************************************
void Simulator::clearCameras(void){
   for (auto kv : this->cameraCollection) {
      delete kv.second;
   }
   this->cameraCollection.clear();
}
//**************************************************************************
void Simulator::clearInputControls(void){
   for (auto kv : this->inputControlCollection) {
      delete kv.second;
   }
   this->inputControlCollection.clear();
}
//**************************************************************************
void Simulator::clearText2D(void){
   this->text2DCollection->removeAll();
}
//**************************************************************************
void Simulator::handleKeyEvents(){
   // glfwGetTime is called only once, the first time this function is called
   static double lastTime = glfwGetTime();

   // Compute time difference between current and last frame
   double currentTime = glfwGetTime();
   float deltaTime = float(currentTime - lastTime);
   float hAngle = 0;
   float vAngle = 0;
   glm::mat4 rotation;
   glm::mat4 translation;

   //Handle the simulator special inputs
   for (UINT16 i = 0; i < this->controlInputs.systemKeyInputs.size(); i++){
      UINT16 key = this->controlInputs.systemKeyInputs.at(i);
      //Enter/Exit FreeMouse
      if (key == GLFW_KEY_F1){
         this->controlFlags = (this->controlFlags ^ FreeMouse);
         if ((controlFlags & FreeMouse) != FreeMouse){
            glfwSetCursorPos(window, this->windowX / 2, this->windowY / 2);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
         }
         else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
         }
      }
      //Enter/Exit FreeCamera
      else if (key == GLFW_KEY_F2){
         this->controlFlags = (this->controlFlags ^ FreeCamera);
      }
      //Next Camera
      else if (key == GLFW_KEY_F3){
         //Does there exist at least one camera?
         if (this->cameraCollection.size() > 0){
            //Turn off free camera mode if it is on
            if ((this->controlFlags & FreeCamera) == FreeCamera)
               this->controlFlags = (this->controlFlags ^ FreeCamera);
            //Get a list of the available cameras
            std::vector<std::string> vals;
            vals.reserve(this->cameraCollection.size());
            for (auto kv : this->cameraCollection) {
               vals.push_back(kv.first);
            }
            //Find the matching string to ID, then update
            UINT8 nextIndex = -1;
            for (UINT16 k = 0; k < vals.size(); k++){
               if (strncmp(this->currentCameraID.id, &(vals.at(k).at(0)), MAX_ID_LEN) == 0){
                  nextIndex = k + 1;
                  if (nextIndex >= vals.size())
                     nextIndex = 0;
                  break;
               }
            }
            if (nextIndex == (UINT8)-1){
               fprintf(stderr, "Warning, current camera ID was not found in collection\n");
               nextIndex = 0;
            }
            //Set the current camera
            this->setCurrentCameraID(&(vals.at(nextIndex).at(0)));
         }
      }
      //Hide/Show Text2D
      else if (key == GLFW_KEY_F4){
         this->controlFlags = (this->controlFlags ^ HideText2D);
      }
      //Mute/UnMute Audio
      else if (key == GLFW_KEY_F5){
         this->controlFlags = (this->controlFlags ^ MuteAudio);
         //TODO - Mute all audios?
      }
      //On/Off max UPS and FPS
      else if (key == GLFW_KEY_F6){
         this->controlFlags = (this->controlFlags ^ MaxUPS_FPS);
      }
      //On/Off shadow rendering
      else if (key == GLFW_KEY_F7){
         this->controlFlags = (this->controlFlags ^ DisableShadows);
      }
      //Show/Hide FPS
      else if (key == GLFW_KEY_F11){
         this->controlFlags = (this->controlFlags ^ ShowFPS);
      }
      //Show/Hide Text2D about author
      else if (key == GLFW_KEY_F12){
         this->controlFlags = (this->controlFlags ^ ShowAbout);
      }
   }

   //Free Camera Mode?
   ITERATOR iter = this->cameraCollection.find(this->getCurrentCameraID().id);
   if (((controlFlags & FreeCamera) == FreeCamera) || (iter == this->cameraCollection.end())){

      //Rotations due to mouse?
      if ((controlFlags & FreeMouse) != FreeMouse){
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
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
         translation = glm::translate(glm::mat4(), glm::vec3(0, 0, MOVE_SPEED)) * translation;
      }
      // Move backward
      else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
         translation = glm::translate(glm::mat4(), glm::vec3(0, 0, -MOVE_SPEED)) * translation;
      }
      // Strafe right
      else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
         translation = glm::translate(glm::mat4(), glm::vec3(-MOVE_SPEED, 0, 0)) * translation;
      }
      // Strafe left
      else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
         translation = glm::translate(glm::mat4(), glm::vec3(MOVE_SPEED, 0, 0)) * translation;
      }
      
      //Update the ViewMatrix
      this->ViewMatrix = translation * rotation * this->ViewMatrix;
   }
   else{ //Camera Mode
      //Get the updated viewMatrix
      Camera* camT = dynamic_cast<Camera*>(iter->second);
      if (camT == NULL){
         fprintf(stderr, "Warning! Corruption in the Camera Collection\n");
         //Turn on freeCam mode
         this->controlFlags = (this->controlFlags ^ FreeCamera);
      }
      //Update Camera
      camT->update();
      //Get ViewMatrix
      this->ViewMatrix = camT->getViewMatrix();
   }

   //Go through user-defined controls
   for (UINT16 i = 0; i < this->controlInputs.generalKeyInputs.size(); i++){
      UINT16 key = this->controlInputs.generalKeyInputs.at(i).keyPress;
      UINT8 mods = this->controlInputs.generalKeyInputs.at(i).modifiers;
      //Is this key binded?
      ITERATOR iter = this->inputControlCollection.find(ControlBind::getStringHash(this->controlInputs.generalKeyInputs.at(i)).id);
      if (iter != this->inputControlCollection.end()){
         //Attempt to dynamic_cast
         ControlBind* cbT = dynamic_cast<ControlBind*>(iter->second);
         if (cbT == NULL)
            fprintf(stderr, "Warning! Corruption in ControlBind Collection\n");
         else
            cbT->runCallBack();
      }
   }

   //Clear the input vectors
   this->controlInputs.generalKeyInputs.clear();
   this->controlInputs.systemKeyInputs.clear();

   // For the next frame, the "last time" will be "now"
   lastTime = currentTime;

}
//**************************************************************************
void Simulator::update(void){
   //Update the background
   if (this->background != NULL)
      this->background->update();

   //Update the stage
   if (this->stage != NULL)
      this->stage->update();

   //Update the instances
   for (UINT16 i = 0; i < this->instanceCollection.size(); i++){
      for (auto kv : this->instanceCollection) {
         Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
         if (objPtr != NULL){
            objPtr->update();
         }
         else{
            fprintf(stderr, "Warning, corruption in Instance Collection.\n");
         }
      }
   }

   //Updates from inputs
   this->handleKeyEvents();

   //Update the lights
   for (UINT16 i = 0; i < this->lightCollection.size(); i++){
      for (auto kv : this->lightCollection) {
         Light* liPtr = dynamic_cast<Light*>(kv.second);
         if (liPtr != NULL){
            liPtr->update();
         }
         else{
            fprintf(stderr, "Warning, corruption in Light Collection.\n");
         }
      }
   }
}
//**************************************************************************
void Simulator::draw(void){
   /*Rendering will be in 2 steps. The first step will take into account 
   what the scene looks like from the point of view of the light sources,
   and the appropriate FrameBuffers will be filled. Only the objects that 
   block light will be rendered in this first group. The second rendering
   will take into account all the objects. Each object can link to the 
   appropriate FrameBuffer via its light collection.*/

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   //Set the viewport
   glViewport(0, 0, this->windowX, this->windowY);

   //Reset some values
   glCullFace(GL_BACK);
   glClearColor(0,0,1.0,0);

   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //Create a list of objects to draw later b/c transparent
   std::vector<Object3D*> transparentList;
   //Render all non-transparent objects
   {
      if (this->background != NULL)
         if (this->background->isTransparent())
            transparentList.push_back(this->background);
         else
            this->background->draw();
      if (this->stage != NULL)
         if (this->stage->isTransparent())
            transparentList.push_back(this->stage);
         else
            this->stage->draw();
      //Draw the instances
      for (auto kv : this->instanceCollection) {
         //Attempt to cast to Object
         Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
         if (objPtr == NULL){
            fprintf(stderr, "Warning! Corruption in Instance Collection\n");
            continue;
         }
         if (objPtr->isTransparent())
            transparentList.push_back(objPtr);
         else
            objPtr->draw();
      }
      //Draw transparent instances
      for (UINT16 i = 0; i < transparentList.size(); i++){
         transparentList.at(i)->draw();
      }
   }
   //Draw the Text2D
   {
      if ((this->controlFlags & HideText2D) != HideText2D){
         //Draw Collection
         this->text2DCollection->printAll();
         //Draw FPS
         if ((this->controlFlags & ShowFPS) == ShowFPS){
            char fpsT[10];
            sprintf_s(fpsT, "%u", (UINT16)this->fps);
            this->text2DCollection->printText2D(fpsT, 2, 2, 15);
         }
         //Draw About
         if ((this->controlFlags & ShowAbout) == ShowAbout){
            this->text2DCollection->printText2D("Created By: Jared Fowler, June 2015", 50, 2, 15);
         }
      }
   }

}
//**************************************************************************
void Simulator::runSimulation(void){

   double lastLoop = glfwGetTime() * 1000;
   double currentLoop = 0;
   double deltaTime = 0;
   this->fps = 0;

   do{
      currentLoop = glfwGetTime() * 1000;
      deltaTime = currentLoop - lastLoop;
      this->fps = (UINT16)(1000 / deltaTime);

      //If MAXUPS_FPS flag is set...
      if ((this->controlFlags & MaxUPS_FPS) != MaxUPS_FPS){
         if (deltaTime < 16.666){
            continue;
         }
      }

      //Update
      this->update();

      //Draw
      this->draw();

      // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();

      lastLoop = currentLoop;


   } // Check if the ESC key was pressed or the window was closed
   while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
   glfwWindowShouldClose(window) == 0);

}



//**************************************************************************
//**************************************************************************
//** ELEMENT ***************************************************************
//**************************************************************************
//**************************************************************************
Element::Element(const char* _id, Simulator* _simPtr){
   if (_id == NULL){
      fprintf(stderr, "ID cannot be NULL\n");
      exit(NULL_ID);
   }
   if (_simPtr == NULL){
      fprintf(stderr, "SimPtr cannot be NULL\n");
      exit(NULL_ID);
   }
   strncpy(this->id.id, _id, MAX_ID_LEN - 1);
   refCount = 0;
   this->simPtr = _simPtr;
}
//**************************************************************************
Element::Element(Element& e)
   :Element(e.getID().id, e.simPtr)
{
   fprintf(stderr, "Warning, attempting to make copy of Element Object\n");
}
//**************************************************************************
Element::~Element(void){
   //Void
}
//**************************************************************************
UINT32 Element::getReferneceCount(void){
   return this->refCount;
}
//**************************************************************************
ID_GL Element::getID(void){
   return this->id;
}
//**************************************************************************
bool Element::incReferenceCount(void){
   this->refCount++;
   if (this->refCount == 0)
      return false;
   return true;
}
//**************************************************************************
bool Element::decReferenceCount(void){
   this->refCount--;
   if (this->refCount == -1)
      return false;
   return true;
}



//**************************************************************************
//**************************************************************************
//** MODEL *****************************************************************
//**************************************************************************
//**************************************************************************
Model::Model(const char* _id, Simulator* _simPtr, const char* _filePathIBO)
   :Element(_id, _simPtr)
{
   if ( _filePathIBO == NULL){
      fprintf(stderr, "Model file path cannot be NULL\n");
      exit(MODEL_FAIL);
   }
   strncpy(this->file.file, _filePathIBO, MAX_FILE_LEN - 1);

   //Load indexed vertices from .ibo file
   std::vector<UINT32> indices;              //Indexing used to save space
   std::vector<glm::vec3> indexed_vertices;  //Vertices in indexed form
   std::vector<glm::vec2> indexed_texture;   //Texture in indexed form
   std::vector<glm::vec3> indexed_normals;   //Normal vertices in indexed form

   if (!this->loadIBO(_filePathIBO, &indices, &indexed_vertices, &indexed_texture, &indexed_normals)){
      fprintf(stderr, "Failed to load Model %s", this->getID().id);
      exit(MODEL_FAIL);
   }

   //Save the number of indices -- Used for drawing later
   this->indexCount = indices.size();

   //Generate and Bind a new Vertex Array Object
   glGenVertexArrays(1, &(this->vao));
   glBindVertexArray(this->vao);

   //Generate GPU vertex buffers, get handles, and upload values
   //Load vertex info into a VBO
   glGenBuffers(1, &(this->vbo));
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &(indexed_vertices[0]), GL_STATIC_DRAW);
   //Load normal info into a VBO
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
}
//**************************************************************************
Model::Model(Model& m)
   :Model(m.getID().id, m.simPtr, m.file.file)
{
   fprintf(stderr, "Warning, attempting to copy Model Object\n");
}
//**************************************************************************
Model::~Model(void){
   //Bind the VAO
   glBindVertexArray(this->vao);

   //Unload the vertex buffers
   glDeleteBuffers(1, &(this->vbo));
   glDeleteBuffers(1, &(this->vno));
   glDeleteBuffers(1, &(this->vto));
   glDeleteBuffers(1, &(this->veo));

   //Unbind the VAO and delete it
   glBindVertexArray(0);
   glDeleteVertexArrays(1, &(this->vao));
}
//**************************************************************************
GLuint Model::getVAO(void){
   return this->vao;
}
//**************************************************************************
GLuint Model::getVBO(void){
   return this->vbo;
}
//**************************************************************************
GLuint Model::getVTO(void){
   return this->vto;
}
//**************************************************************************
GLuint Model::getVNO(void){
   return this->vno;
}
//**************************************************************************
GLuint Model::getVEO(void){
   return this->veo;
}
//**************************************************************************
UINT32 Model::getIndexCount(void){
   return this->indexCount;
}
//**************************************************************************
bool Model::loadIBO(const char* fileName, std::vector<unsigned int>* indices, std::vector<glm::vec3>* v, std::vector<glm::vec2>* uv, std::vector<glm::vec3>* n){
   //Attempt to open the file
   FILE *fp = fopen(fileName, "rb+");

   //Read the number of indices
   unsigned int size;
   if (!fread(&size, sizeof(unsigned int), 1, fp)){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Read the indices
   unsigned int* intPtr = new unsigned int[size];
   if (fread(intPtr, sizeof(unsigned int), size, fp) != size){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Assign these values to the vector
   for (unsigned int i = 0; i < size; i++){
      indices->push_back(intPtr[i]);
   }
   delete intPtr;
   intPtr = NULL;

   //Read the number of vertices
   if (!fread(&size, sizeof(unsigned int), 1, fp)){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Read the indexed vertices
   float* floatPtr = new float[size * 3];
   if (fread(floatPtr, sizeof(float), size * 3, fp) != size * 3){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Assign these values to the vector
   for (unsigned int i = 0; i < size * 3; i += 3){
      v->push_back(glm::vec3(floatPtr[i], floatPtr[i + 1], floatPtr[i + 2]));
   }

   //Read the indexed UV's
   if (fread(floatPtr, sizeof(float), size * 2, fp) != size * 2){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Assign these values to the vector
   for (unsigned int i = 0; i < size * 2; i += 2){
      uv->push_back(glm::vec2(floatPtr[i], floatPtr[i + 1]));
   }

   //Read the indexed normals
   if (fread(floatPtr, sizeof(float), size * 3, fp) != size * 3){
      fprintf(stderr, "Read Fail: %s\n", fileName);
      return false;
   }
   //Assign these values to the vector
   for (unsigned int i = 0; i < size * 3; i += 3){
      n->push_back(glm::vec3(floatPtr[i], floatPtr[i + 1], floatPtr[i + 2]));
   }
   delete floatPtr;
   floatPtr = NULL;

   fclose(fp);

   return true;
}



//**************************************************************************
//**************************************************************************
//** AUDIO *****************************************************************
//**************************************************************************
//**************************************************************************
Audio::Audio(const char* _id, Simulator* _simPtr, const char* _filePath)
   :Element(_id, _simPtr)
{
   this->engine = irrklang::createIrrKlangDevice();
   if (this->engine == NULL){
      fprintf(stderr, "Failed to initiate Audio Engine\n");
      exit(AUDIO_FAIL);
   }
   if (_filePath == NULL){
      fprintf(stderr, "FILE cannot be NULL\n");
      exit(AUDIO_FAIL);
   }
   strncpy(this->file.file, _filePath, MAX_FILE_LEN - 1);
}
//**************************************************************************
Audio::Audio(Audio& a)
   :Audio(a.getID().id, a.simPtr, a.file.file)
{
   fprintf(stderr, "Warning, attempting to make copy of Audio Object\n");
}
//**************************************************************************
Audio::~Audio(void){
   this->engine->drop();
}
//**************************************************************************
FILE_GL Audio::getFile(void){
   return this->file; 
}
//**************************************************************************
void Audio::play(void){
   if (!this->engine->isCurrentlyPlaying(this->file.file))
      this->engine->play2D(this->file.file);
}
//**************************************************************************
void Audio::stop(void){
   if (this->engine->isCurrentlyPlaying(this->getFile().file))
      this->engine->stopAllSounds();
}
//**************************************************************************
void Audio::setVolume(float v){
   if (v < 0)
      v = 0;
   else if (v > 1)
      v = 1;
   this->engine->setSoundVolume(v);
}



//**************************************************************************
//**************************************************************************
//** SHADER ****************************************************************
//**************************************************************************
//**************************************************************************
Shader::Shader(const char* _id, Simulator* _simPtr, const char* _vertex, const char* _fragment)
   :Element(_id, _simPtr)
{
   if (_vertex == NULL){
      fprintf(stderr, "vShaderPath cannot be NULL\n");
      exit(SHADER_FAIL);
   }
   if (_fragment == NULL){
      fprintf(stderr, "fShaderPath cannot be NULL\n");
      exit(SHADER_FAIL);
   }

   strncpy(this->vertexShader.file, _vertex, MAX_FILE_LEN - 1);
   strncpy(this->fragmentShader.file, _fragment, MAX_FILE_LEN - 1);

   //Load Shaders and get handle
   this->shaderID = this->LoadShaders(_vertex, _fragment);
   glUseProgram(this->shaderID);

   //Attempt to get shader uniform handles. If not exist, write NA
   //Note: glGetUniformLocation returns -1 if value isn't found
   if ((this->mMatrixID = glGetUniformLocation(this->shaderID, M_MATRIX)) == -1)
      this->mMatrixID = NA;
   if ((this->vMatrixID = glGetUniformLocation(this->shaderID, V_MATRIX)) == -1)
      this->vMatrixID = NA;
   if ((this->mvMatrixID = glGetUniformLocation(this->shaderID, MV_MATRIX)) == -1)
      this->mvMatrixID = NA;
   if ((this->mvpMatrixID = glGetUniformLocation(this->shaderID, MVP_MATRIX)) == -1)
      this->mvpMatrixID = NA;

   //Get handles for the textures
   char uniformName[50] = { 0 };
   GLuint texTemp = NA;
   //Regular textures
   for (UINT8 i = 0; i < MAX_TEXTURES; i++){
      sprintf(uniformName, TEXTURE, i);
      texTemp = glGetUniformLocation(this->shaderID, uniformName);
      if (texTemp == -1){
         texTemp = NA;
         break;
      }
      this->textureID.push_back(texTemp);
   }
   //Get handles for light uniforms
   for (UINT8 j = 0; j < MAX_LIGHTS; j++){
      sprintf(uniformName, LIGHT, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1){
         texTemp = NA;
         break;
      }
      this->lightID.push_back(texTemp);
   }
   //Get handles for Shadow Textures
   for (UINT8 j = 0; j < MAX_LIGHTS; j++){
      sprintf(uniformName, DEPTH_TEXTURE, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1){
         texTemp = NA;
         break;
      }
      this->depthTextureID.push_back(texTemp);
   }
   //Get handles for DepthBiasMVPmatrices
   for (UINT8 j = 0; j < MAX_LIGHTS; j++){
      sprintf(uniformName, DEPTH_BIAS_MVP, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1){
         texTemp = NA;
         break;
      }
      this->dbmvpID.push_back(texTemp);
   }
   //Get handles for RandomFloats
   for (UINT8 j = 0; j < MAX_RAND_FLOATS; j++){
      sprintf(uniformName, RANDOM_FLOAT, j);
      if ((texTemp = glGetUniformLocation(this->shaderID, uniformName)) == -1){
         texTemp = NA;
         break;
      }
      this->randomFloatID.push_back(texTemp);
   }
}
//**************************************************************************
Shader::Shader(Shader& s)
   :Shader(s.getID().id, s.simPtr, s.vertexShader.file, s.fragmentShader.file)
{
   fprintf(stderr, "Warning, attempting to make copy of Shader Object\n");
}
//**************************************************************************
Shader::~Shader(void){
   //Unload shader
   glDeleteProgram(this->shaderID);
   this->lightID.clear();
   this->textureID.clear();
}
//**************************************************************************
GLuint Shader::getShaderID(void){
   return this->shaderID;
}
//**************************************************************************
GLuint Shader::getmMatrixID(void){
   return this->mMatrixID;
}
//**************************************************************************
GLuint Shader::getvMatrixID(void){
   return this->vMatrixID;
}
//**************************************************************************
GLuint Shader::getmvMatrixID(void){
   return this->mvMatrixID;
}
//**************************************************************************
GLuint Shader::getmvpMatrixID(void){
   return this->mvpMatrixID;
}
//**************************************************************************
GLuint Shader::getRenderTextureID(void){
   return this->renderTextureID;
}
//**************************************************************************
std::vector<GLuint> Shader::getDepthTextureID(void){
   return this->depthTextureID;
}
//**************************************************************************
std::vector<GLuint> Shader::getDepthMVPID(void){
   return this->dbmvpID;
}
//**************************************************************************
std::vector<GLuint> Shader::getLightID(void){
   return this->lightID;
}
//**************************************************************************
std::vector<GLuint> Shader::getTextureID(void){
   return this->textureID;
}
//**************************************************************************
std::vector<GLuint> Shader::getRandomFloatID(void){
   return this->randomFloatID;
}
//**************************************************************************
GLuint Shader::LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
   // Read the Vertex Shader code from the file
   std::string VertexShaderCode;
   std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
   if (VertexShaderStream.is_open()){
      std::string Line = "";
      while (getline(VertexShaderStream, Line))
         VertexShaderCode += "\n" + Line;
      VertexShaderStream.close();
   }
   else{
      printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
      getchar();
      return 0;
   }

   // Read the Fragment Shader code from the file
   std::string FragmentShaderCode;
   std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
   if (FragmentShaderStream.is_open()){
      std::string Line = "";
      while (getline(FragmentShaderStream, Line))
         FragmentShaderCode += "\n" + Line;
      FragmentShaderStream.close();
   }

   return Shader::LoadShaders(VertexShaderCode, FragmentShaderCode);
}
//**************************************************************************
GLuint Shader::LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode){
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
   if (InfoLogLength > 0){
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
   if (InfoLogLength > 0){
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
   if (InfoLogLength > 0){
      std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
      glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
      printf("%s", &ProgramErrorMessage[0]);
   }

   glDeleteShader(VertexShaderID);
   glDeleteShader(FragmentShaderID);

   return ProgramID;
}


//**************************************************************************
//**************************************************************************
//** TEXTURE ***************************************************************
//**************************************************************************
//**************************************************************************
Texture::Texture(const char* _id, Simulator* _simPtr, const char* _filePathDDS)
   :Element(_id, _simPtr)
{
   if (_filePathDDS == NULL){
      fprintf(stderr, "TexturePath cannot be NULL\n");
      exit(TEXTURE_FAIL);
   }
   strncpy(this->file.file, _filePathDDS, MAX_FILE_LEN - 1);

   if ((this->textureID = loadDDS(_filePathDDS)) == 0){
      this->textureID = NA;
      fprintf(stderr, "Failed to load texture: %s\r\n", _filePathDDS);
      exit(TEXTURE_FAIL);
   }
}
//**************************************************************************
Texture::Texture(Texture& t)
   : Texture((const char*)t.getID().id, t.simPtr, (const char*)t.file.file)
{
   fprintf(stderr, "Warning, attempting to make copy of Texture Object\n");
}
//**************************************************************************
Texture::~Texture(void){
   //Unload texture
   glDeleteTextures(1, &(this->textureID));
}
//**************************************************************************
GLuint Texture::getTextureID(void){
   return this->textureID;
}
//**************************************************************************
GLuint Texture::loadDDS(const char * imagepath){
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

   unsigned char header[124];

   FILE *fp;

   /* try to open the file */
   fp = fopen(imagepath, "rb");
   if (fp == NULL){
      printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
      return 0;
   }

   /* verify the type of file */
   char filecode[4];
   fread(filecode, 1, 4, fp);
   if (strncmp(filecode, "DDS ", 4) != 0) {
      fclose(fp);
      return 0;
   }

   /* get the surface desc */
   fread(&header, 124, 1, fp);

   unsigned int height = *(unsigned int*)&(header[8]);
   unsigned int width = *(unsigned int*)&(header[12]);
   unsigned int linearSize = *(unsigned int*)&(header[16]);
   unsigned int mipMapCount = *(unsigned int*)&(header[24]);
   unsigned int fourCC = *(unsigned int*)&(header[80]);


   unsigned char * buffer;
   unsigned int bufsize;
   /* how big is it going to be including all mipmaps? */
   bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
   buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
   fread(buffer, 1, bufsize, fp);
   /* close the file pointer */
   fclose(fp);

   unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
   unsigned int format;
   switch (fourCC)
   {
   case FOURCC_DXT1:
      format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      break;
   case FOURCC_DXT3:
      format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      break;
   case FOURCC_DXT5:
      format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      break;
   default:
      free(buffer);
      return 0;
   }

   // Create one OpenGL texture
   GLuint textureID;
   glGenTextures(1, &textureID);

   // "Bind" the newly created texture : all future texture functions will modify this texture
   glBindTexture(GL_TEXTURE_2D, textureID);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
   unsigned int offset = 0;

   /* load the mipmaps */
   for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
   {
      unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
      glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
         0, size, buffer + offset);

      offset += size;
      width /= 2;
      height /= 2;

      // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
      if (width < 1) width = 1;
      if (height < 1) height = 1;

   }

   free(buffer);

   return textureID;
}



//**************************************************************************
//**************************************************************************
//** TEXT2D ****************************************************************
//**************************************************************************
//**************************************************************************
Text2D::Text2D(const char* _id, Simulator* _simPtr, const char* _filePathDDS, const char* _vertex, const char* _fragment)
   :Element(_id, _simPtr)
{
   if (_filePathDDS == NULL || _vertex == NULL || _fragment == NULL){
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

   //Unbind VAO
   glBindVertexArray(0);
}
//**************************************************************************
Text2D::Text2D(Text2D& t)
   :Text2D(t.getID().id, t.simPtr, t.texture.file, t.vertex.file, t.fragment.file)
{
   fprintf(stderr, "Warning, attempting to make copy of Text2D Object\n");
   this->text = t.text;
}
//**************************************************************************
Text2D::~Text2D(void){
   //Bind VAO
   glBindVertexArray(Text2DVAO);

   // Delete buffers
   glDeleteBuffers(1, &Text2DVertexBufferID);
   glDeleteBuffers(1, &Text2DUVBufferID);

   //Unbind VAO and delete
   glBindVertexArray(0);
   glDeleteVertexArrays(1, &(Text2DVAO));

   // Delete texture
   glDeleteTextures(1, &Text2DTextureID);

   // Delete shader
   glDeleteProgram(Text2DShaderID);

   //Clear vector
   this->text.clear();
}
//**************************************************************************
bool Text2D::addText2D(const char* _id, UINT16 _x, UINT16 _y, UINT8 _size, const char* _message){
   //Input validation
   if (_id == NULL || _message == NULL){
      fprintf(stderr, "Arguments cannot be NULL.\n");
      return false;
   }
   //Unique ID?
   for (UINT16 i = 0; i < this->text.size(); i++){
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0){
         fprintf(stderr, "TextID already in use.\n");
         return false;
      }
   }
   //Create and add to vector
   Text2DInfo t;
   strncpy(t.id, _id, MAX_ID_LEN - 1);
   strncpy(t.message, _message, MAX_MESSAGE_LEN - 1);
   t.x = _x;
   t.y = _y;
   t.size = _size;
   this->text.push_back(t);

   return true;
}
//**************************************************************************
bool Text2D::removeText2D(const char* _id){
   //Does the id exist?
   if (_id == NULL)
      return false;
   for (UINT16 i = 0; i < this->text.size(); i++){
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0){
         this->text.erase(this->text.begin() + i);
         return true;
      }
   }
   return false;
}
//**************************************************************************
bool Text2D::removeAll(void){
   this->text.clear();
   return true;
}
//**************************************************************************
bool Text2D::printText2D(const char* _id){
   //Does the id exist?
   if (_id == NULL)
      return false;
   for (UINT16 i = 0; i < this->text.size(); i++){
      if (strncmp(_id, this->text.at(i).id, MAX_ID_LEN) == 0){
         this->printText2D(this->text.at(i).message, this->text.at(i).x, 
            this->text.at(i).y, this->text.at(i).size);
         return true;
      }
   }
   return false;
}
//**************************************************************************
bool Text2D::printAll(void){
   for (UINT16 i = 0; i < this->text.size(); i++){
      this->printText2D(this->text.at(i).message, this->text.at(i).x,
         this->text.at(i).y, this->text.at(i).size);
   }
   return true;
}
//**************************************************************************
void Text2D::printText2D(const char * text, int x, int y, int size){
   unsigned int length = strlen(text);

   //Bind VAO
   glBindVertexArray(this->Text2DVAO);

   // Fill buffers
   std::vector<glm::vec2> vertices;
   std::vector<glm::vec2> UVs;
   for (unsigned int i = 0; i<length; i++){

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

   //Unbind
   glBindVertexArray(0);
}



//**************************************************************************
//**************************************************************************
//** LIGHT *****************************************************************
//**************************************************************************
//**************************************************************************
Light::Light(const char* _id, Simulator* _simPtr, LightInfoBasic _li, const char* _sourceID)
   :Element(_id, _simPtr)
{ 
   _li.color = this->verifyColor(_li.color);
   _li.intensity = this->verifyIntensity(_li.intensity);
   this->info = _li;
   this->setSourceID(_sourceID);
}
//**************************************************************************
Light::Light(Light& l)
   : Light(l.getID().id, l.simPtr, l.info, l.getSourceID().id)
{
   fprintf(stderr, "Warning, attempting to make copy of Light Object\n");
}
//**************************************************************************
Light::~Light(void){
   //Void
}
//**************************************************************************
glm::vec3 Light::verifyColor(glm::vec3 _v){
   if (_v.x < 0.0f) _v.x = 0.0f;
   if (_v.x > 1.0f) _v.x = 1.0f;
   if (_v.x < 0.0f) _v.y = 0.0f;
   if (_v.x > 1.0f) _v.y = 1.0f;
   if (_v.x < 0.0f) _v.z = 0.0f;
   if (_v.x > 1.0f) _v.z = 1.0f;
   return _v;
}
//**************************************************************************
float Light::verifyIntensity(float _i){
   if (_i < 0) _i = 0;
   return _i;
}
//**************************************************************************
void Light::setSourceID(const char* _source){
   if (_source == NULL) //AKA, this light is stationary, independent
      strncpy(this->sourceID.id, "", MAX_ID_LEN - 1);
   else
      strncpy(this->sourceID.id, _source, MAX_ID_LEN - 1);
}
//**************************************************************************
Light::LightInfoBasic Light::getLightStruct(void){
   return this->info;
}
//**************************************************************************
glm::mat4 Light::getLightShaderInfo(void){
   return glm::mat4(glm::vec4(this->info.position, this->info.intensity), 
      glm::vec4(this->info.color, this->info.turnedOn),
      glm::vec4(this->info.direction, NA),
      glm::vec4(NA, NA, NA, NA));
}
//**************************************************************************
ID_GL Light::getSourceID(void){
   return this->sourceID;
}
//**************************************************************************
std::vector<GLuint> Light::getShadowTextureIDs(void){
   return std::vector<GLuint>();
}
//**************************************************************************
std::vector<glm::mat4> Light::getDepthBiasVPMatrices(void){
   return std::vector<glm::mat4>();
}
//**************************************************************************
bool Light::update(void){
   //Does this light belong to an object?
   if (strncmp(this->getSourceID().id, "", MAX_ID_LEN) == 0){
      return false;
   }
   //Attempt to get object
   Object3D* objPtr = this->simPtr->getInstance(this->getSourceID().id);
   if (objPtr == NULL){
      return false;
   }
   //Call the lightUpdate function
   LightInfoBasic _li = objPtr->updateLight(this->getLightStruct());
   _li.color = this->verifyColor(_li.color);
   _li.intensity = this->verifyIntensity(_li.intensity);
   this->info = _li;
   return true;
}
//**************************************************************************
DirectionalLight::DirectionalLight(const char* _id, Simulator* _simPtr, LightInfoBasic _li, OrthoBounds _ob, const char* _sourceID, const char* _depthShaderID)
   :Light(_id, _simPtr, _li, _sourceID)
{
   //Back up the ID of the shader to use
   if (_depthShaderID == NULL){
      fprintf(stderr, "DepthShader cannot be NULL\n");
      exit(LIGHT_FAIL);
   }
   strncpy(this->depthShaderID.id, _depthShaderID, MAX_ID_LEN);

   //Set up the orthoProjection Matrix
   this->orthoProjection = glm::ortho<float>(_ob.minX, _ob.maxX, _ob.minY, _ob.maxY, _ob.nearClip, _ob.farClip);

   //Initialize Shadow Map Buffers
   //Allocate a new frame buffer
   glGenFramebuffers(1, &(this->depthFrameBuffer));
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);

   //Allocate space for depth texture and set properties
   glGenTextures(1, &(this->depthTexture));
   glBindTexture(GL_TEXTURE_2D, this->depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

   //Attach depth texutre to the frame buffer
   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthTexture, 0);
   // No color output in the bound framebuffer, only depth.
   glDrawBuffer(GL_NONE);

   //Did everything initialize good?
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      exit(1);

   //Unbind this frame buffer -- VERY IMPORTANT!
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//**************************************************************************
DirectionalLight::DirectionalLight(DirectionalLight& t)
   : DirectionalLight(t.getID().id, t.simPtr, t.getLightStruct(), OrthoBounds(), t.getSourceID().id, t.getDepthShaderID().id)
{
   //Set the correct orthoProjectionMatrix
   this->orthoProjection = t.orthoProjection;
   fprintf(stderr, "Warning, attempting to make copy of DirecionalLight Object\n");
}
//**************************************************************************
DirectionalLight::~DirectionalLight(void){
   //Delete the frameBuffers and textures
   glDeleteTextures(1, &(this->depthTexture));
   glDeleteFramebuffers(1, &(this->depthFrameBuffer));
}
//**************************************************************************
ID_GL DirectionalLight::getDepthShaderID(void){
   return this->depthShaderID;
}
//**************************************************************************
std::vector<glm::mat4> DirectionalLight::getDepthBiasVPMatrices(void){
   return std::vector<glm::mat4>({ this->depthBias_vpMatrix });
}
//**************************************************************************
std::vector<GLuint> DirectionalLight::getShadowTextureIDs(void){
   std::vector<GLuint> temp;
   temp.push_back(this->depthTexture);
   return temp;
}
//**************************************************************************
bool DirectionalLight::update(void){
   //Call base class version
   if (!Light::update())
      return false;

   //Is shadow rendering disabled?
   if ((this->simPtr->getControlFlags() & Simulator::ControlFlags::DisableShadows) == Simulator::ControlFlags::DisableShadows)
      return true;

   //Get a copy of the light structure
   LightInfoBasic lightInfo = this->getLightStruct();

   //Only proceed if the light is turned on
   if (!lightInfo.turnedOn)
      return true;

   //Get the ViewMatrix
   glm::mat4 depth_ViewMatrix = glm::lookAt(lightInfo.position, lightInfo.position + lightInfo.direction, glm::cross(lightInfo.position + lightInfo.direction, lightInfo.position + lightInfo.direction + glm::vec3(1, 2, 3)));
   //Get the ViewProjectionMatrix
   glm::mat4 depth_vpMatrix = this->orthoProjection * depth_ViewMatrix;

   //Update shadow framebuffers
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);
   glViewport(0, 0, 1024, 1024);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //Link the shader
   Shader* shader = this->simPtr->getShader(this->depthShaderID.id);
   if (shader == NULL){
      fprintf(stderr, "Warning, failed to link shader in light ops.\n");
      glBindFramebuffer(0, this->depthFrameBuffer);
      return false;
   }
   glUseProgram(shader->getShaderID());

   //Get a handle to the instance collection
   std::unordered_map<std::string, Element*>* ptr = this->simPtr->getCollection(Simulator::Collections::C_INSTANCE);
   
   for (auto kv : *ptr) {
      //Attempt to convert the element pointer into an Object3D*
      Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
      if (objPtr == NULL){
         fprintf(stderr, "Warning, corruption in instance collection\n");
         continue;
      }
      //If the object is transparent, don't draw it
      if (objPtr->isTransparent())
         continue;

      //Attempt to get and then convert the corresponding model
      Model* model = this->simPtr->getModel(objPtr->getModelIDs().at(0));
      if (model == NULL){
         fprintf(stderr, "Warning, corruption in model/instance collection\n");
         continue;
      }

      //Is this object the light source? Don't render it if it is
      if (strncmp(objPtr->getID().id, this->getSourceID().id, MAX_ID_LEN) == 0)
         continue;

      //Calculate the MVP
      glm::mat4 depth_mvpMatrix = depth_vpMatrix * objPtr->getLastModelMatrix();

      //Load this value into the depth shader
      if (shader->getmvpMatrixID() != NA)
         glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(depth_mvpMatrix[0][0]));
      if (shader->getmMatrixID() != NA)
         glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(objPtr->getLastModelMatrix()[0][0]));
      if (shader->getvMatrixID() != NA)
         glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &(depth_ViewMatrix[0][0]));
      if (shader->getmvMatrixID() != NA)
         glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &((depth_ViewMatrix * objPtr->getLastModelMatrix())[0][0]));

      //Bind VAO and then draw values
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

      //Bind the index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

      //Draw!
      glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

      //Disable linked vertex arrays
      glDisableVertexAttribArray(0);

   }

   //Return the frameBuffer to the normal one
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   //Update the depthBias matrix
   //Takes the current vp matrix and muliplies it by a depthBias matrix.
   //The purpose of depth bias is to change texture coords from -1,1 to 0,1
   const glm::mat4 biasMatrix = glm::mat4(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
      );
   this->depthBias_vpMatrix = (biasMatrix * depth_vpMatrix);

   return true;
}
//**************************************************************************
SpotLight::SpotLight(const char* _id, Simulator* _simPtr, LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID)
   :Light(_id, _simPtr, _li, _sourceID)
{
   //Back up the ID of the shader to use
   if (_depthShaderID == NULL){
      fprintf(stderr, "DepthShader cannot be NULL\n");
      exit(LIGHT_FAIL);
   }
   strncpy(this->depthShaderID.id, _depthShaderID, MAX_ID_LEN);

   //Set up the Projection Matrix
   if (_pb.nearClip <= 0 || _pb.farClip <= 0){
      fprintf(stderr, "Warning, possible error with Spotlight projection bounds\n");
   }
   this->projection = glm::perspective(_pb.fov, _pb.aspectRatio, _pb.nearClip, _pb.farClip);

   //Initialize Shadow Map Buffers
   //Allocate a new frame buffer
   glGenFramebuffers(1, &(this->depthFrameBuffer));
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);

   //Allocate space for depth texture and set properties
   glGenTextures(1, &(this->depthTexture));
   glBindTexture(GL_TEXTURE_2D, this->depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

   //Attach depth texutre to the frame buffer
   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthTexture, 0);
   // No color output in the bound framebuffer, only depth.
   glDrawBuffer(GL_NONE);

   //Did everything initialize good?
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
      fprintf(stderr, "Failed to initialize depth framebuffer\n");
      exit(1);
   }

   //Unbind this frame buffer -- VERY IMPORTANT!
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//**************************************************************************
SpotLight::SpotLight(SpotLight& t)
   :SpotLight(t.getID().id, t.simPtr, t.getLightStruct(), ProjectionBounds(), t.getSourceID().id, t.getDepthShaderID().id)
{
   //Set the correct projection
   this->projection = t.projection;
   fprintf(stderr, "Warning, attempting to make a copy of SpotLight Object\n");
}
//**************************************************************************
SpotLight::~SpotLight(void){
   //Delete the frameBuffers and textures
   glDeleteTextures(1, &(this->depthTexture));
   glDeleteFramebuffers(1, &(this->depthFrameBuffer));
}
//**************************************************************************
ID_GL SpotLight::getDepthShaderID(void){
   return this->depthShaderID;
}
//**************************************************************************
std::vector<glm::mat4> SpotLight::getDepthBiasVPMatrices(void){
   return std::vector<glm::mat4>({ this->depthBias_vpMatrix });
}
//**************************************************************************
std::vector<GLuint> SpotLight::getShadowTextureIDs(void){
   std::vector<GLuint> temp;
   temp.push_back(this->depthTexture);
   return temp;
}
//**************************************************************************
bool SpotLight::update(void){
   //Call base class version
   if (!Light::update())
      return false;

   //Is shadow rendering disabled?
   if ((this->simPtr->getControlFlags() & Simulator::ControlFlags::DisableShadows) == Simulator::ControlFlags::DisableShadows)
      return true;

   //Get a copy of the light structure
   LightInfoBasic lightInfo = this->getLightStruct();

   //Only proceed if the light is turned on
   if (!lightInfo.turnedOn)
      return true;

   //Get the ViewMatrix
   glm::mat4 depth_ViewMatrix = glm::lookAt(lightInfo.position, lightInfo.position + lightInfo.direction, glm::cross(lightInfo.position + lightInfo.direction, lightInfo.position + lightInfo.direction + glm::vec3(1, 2, 3)));
   //Get the ViewProjectionMatrix
   glm::mat4 depth_vpMatrix = this->projection * depth_ViewMatrix;

   //Update shadow framebuffers
   glBindFramebuffer(GL_FRAMEBUFFER, this->depthFrameBuffer);
   glViewport(0, 0, 1024, 1024);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //Link the shader
   Shader* shader = this->simPtr->getShader(this->depthShaderID.id);
   if (shader == NULL){
      fprintf(stderr, "Warning, failed to link shader in light ops.\n");
      glBindFramebuffer(0, this->depthFrameBuffer);
      return false;
   }
   glUseProgram(shader->getShaderID());

   //Get a handle to the instance collection
   std::unordered_map<std::string, Element*>* ptr = this->simPtr->getCollection(Simulator::Collections::C_INSTANCE);

   for (auto kv : *ptr) {
      //Attempt to convert the element pointer into an Object3D*
      Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
      if (objPtr == NULL){
         fprintf(stderr, "Warning, corruption in instance collection\n");
         continue;
      }

      //If the object is transparent, don't draw it
      if (objPtr->isTransparent())
         continue;

      //Attempt to get and then convert the corresponding model
      Model* model = this->simPtr->getModel(objPtr->getModelIDs().at(0));
      if (model == NULL){
         fprintf(stderr, "Warning, corruption in model/instance collection\n");
         continue;
      }

      //Is this object the light source? Don't render it if it is
      if (strncmp(objPtr->getID().id, this->getSourceID().id, MAX_ID_LEN) == 0)
         continue;

      //Calculate the MVP
      glm::mat4 depth_mvpMatrix = depth_vpMatrix * objPtr->getLastModelMatrix();

      //Load this value into the depth shader
      if (shader->getmvpMatrixID() != NA)
         glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(depth_mvpMatrix[0][0]));
      if (shader->getmMatrixID() != NA)
         glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(objPtr->getLastModelMatrix()[0][0]));
      if (shader->getvMatrixID() != NA)
         glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &(depth_ViewMatrix[0][0]));
      if (shader->getmvMatrixID() != NA)
         glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &((depth_ViewMatrix * objPtr->getLastModelMatrix())[0][0]));

      //Bind VAO and then draw values
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

      //Bind the index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

      //Draw!
      glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

      //Disable linked vertex arrays
      glDisableVertexAttribArray(0);

   }

   //Return the frameBuffer to the normal one
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   //Update the depthBias matrix
   //Takes the current vp matrix and muliplies it by a depthBias matrix.
   //The purpose of depth bias is to change texture coords from -1,1 to 0,1
   const glm::mat4 biasMatrix = glm::mat4(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
      );
   this->depthBias_vpMatrix = (biasMatrix * depth_vpMatrix);

   return true;
}
//**************************************************************************
PointLight::PointLight(const char* _id, Simulator* _simPtr, Light::LightInfoBasic _li, Light::ProjectionBounds _pb, const char* _sourceID, const char* _depthShaderID)
   :Light(_id, _simPtr, _li, _sourceID)
{
   //Back up the ID of the shader to use
   if (_depthShaderID == NULL){
      fprintf(stderr, "DepthShader cannot be NULL\n");
      exit(LIGHT_FAIL);
   }
   strncpy(this->depthShaderID.id, _depthShaderID, MAX_ID_LEN);

   //Set up projection matrix
   if (_pb.farClip <= 0 || _pb.nearClip <= 0 || _pb.farClip < _pb.nearClip){
      fprintf(stderr, "Warning, probable error with depth projection.\n");
   }
   if (_pb.fov != 90.0f){
      fprintf(stderr, "Warning, point light fov should normally be 90.0f\n");
   }
   this->projection = glm::perspective<float>(_pb.fov, _pb.aspectRatio, _pb.nearClip, _pb.farClip);

#if(0)
   //Set up CameraDirection Array
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

   for (UINT8 i = 0; i < 6; i++) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R16F, 1024, 1024, 0, GL_RED, GL_FLOAT, NULL);
   }

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

   // Disable writes to the color buffer
   glDrawBuffer(GL_NONE);
   // Disable reads from the color buffer
   glReadBuffer(GL_NONE);

   GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (Status != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "Failed to initialize depth framebuffer\n");
      exit(1);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//**************************************************************************
PointLight::PointLight(PointLight& t)
   :PointLight(t.getID().id, t.simPtr, t.getLightStruct(), Light::ProjectionBounds(), t.getSourceID().id, t.getDepthShaderID().id)
{
   this->projection = t.projection;
   fprintf(stderr, "Warning, attempting to make a copy of SpotLight Object\n");
}
//**************************************************************************
PointLight::~PointLight(void){
   //Delete the frameBuffers and textures
   glDeleteTextures(1, &(this->depthTexture));
   glDeleteTextures(1, &(this->depthShadowMap));
   glDeleteFramebuffers(1, &(this->depthFrameBuffer));
}
//**************************************************************************
const PointLight::CameraDirection PointLight::gCameraDirections[] = {
   { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
   { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
};
//**************************************************************************
ID_GL PointLight::getDepthShaderID(void){
   return this->depthShaderID;
}
//**************************************************************************
std::vector<GLuint> PointLight::getShadowTextureIDs(void){
   std::vector<GLuint> temp;
   temp.push_back(this->depthShadowMap);
   return temp;
}
//**************************************************************************
bool PointLight::update(void){
   //Call base class version
   if (!Light::update())
      return false;

   //Is shadow rendering disabled?
   if ((this->simPtr->getControlFlags() & Simulator::ControlFlags::DisableShadows) == Simulator::ControlFlags::DisableShadows)
      return true;

   //Get a copy of the light structure
   LightInfoBasic lightInfo = this->getLightStruct();

   //Only proceed if the light is turned on
   if (!lightInfo.turnedOn)
      return true;

   //Link the shader
   Shader* shader = this->simPtr->getShader(this->depthShaderID.id);
   if (shader == NULL){
      fprintf(stderr, "Warning, failed to link shader in light ops.\n");
      glBindFramebuffer(0, this->depthFrameBuffer);
      return false;
   }
   glUseProgram(shader->getShaderID());

   //Load in shader values that are the same for every cube face
   //Light Position
   std::vector<GLuint> lightIDs = shader->getLightID();
   if (lightIDs.size() >= 1 && lightIDs[0] != NA){
      glUniformMatrix4fv(lightIDs[0], 1, GL_FALSE, &(this->getLightShaderInfo()[0][0]));
   }

   //Get a handle to the instance collection
   std::unordered_map<std::string, Element*>* ptr = this->simPtr->getCollection(Simulator::Collections::C_INSTANCE);

   glCullFace(GL_FRONT);
   glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

   //Bind the FrameBuffer
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->depthFrameBuffer);
   glViewport(0, 0, 1024, 1024);

   for (UINT8 i = 0; i < 6; i++) {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gCameraDirections[i].CubemapFace, this->depthShadowMap, 0);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

      //Get the ViewMatrix
      glm::mat4 depth_ViewMatrix = glm::lookAt(lightInfo.position, this->gCameraDirections[i].Target, this->gCameraDirections[i].Up);
      //Get the ViewProjectionMatrix
      glm::mat4 depth_vpMatrix = this->projection * depth_ViewMatrix;

      //Load in shader values that are same for all models
      if (shader->getvMatrixID() != NA)
         glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &(depth_ViewMatrix[0][0]));

      for (auto kv : *ptr) {
         //Attempt to convert the element pointer into an Object3D*
         Object3D* objPtr = dynamic_cast<Object3D*>(kv.second);
         if (objPtr == NULL){
            fprintf(stderr, "Warning, corruption in instance collection\n");
            continue;
         }

         //If the object is transparent, don't draw it
         if (objPtr->isTransparent())
            continue;

         //Attempt to get and then convert the corresponding model
         Model* model = this->simPtr->getModel(objPtr->getModelIDs().at(0));
         if (model == NULL){
            fprintf(stderr, "Warning, corruption in model/instance collection\n");
            continue;
         }

         //Is this object the light source? Don't render it if it is
         if (strncmp(objPtr->getID().id, this->getSourceID().id, MAX_ID_LEN) == 0)
            continue;

         //Calculate the MVP
         glm::mat4 depth_mvpMatrix = depth_vpMatrix * objPtr->getLastModelMatrix();

         //Load this value into the depth shader
         if (shader->getmvpMatrixID() != NA)
            glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(depth_mvpMatrix[0][0]));
         if (shader->getmMatrixID() != NA)
            glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(objPtr->getLastModelMatrix()[0][0]));
         if (shader->getmvMatrixID() != NA)
            glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &((depth_ViewMatrix * objPtr->getLastModelMatrix())[0][0]));

         //Bind VAO and then draw values
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

         //Bind the index buffer
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

         //Draw!
         glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

         //Disable linked vertex arrays
         glDisableVertexAttribArray(0);

      }
   }

   return true;
}



//**************************************************************************
//**************************************************************************
//** CONTROLBIND ***********************************************************
//**************************************************************************
//**************************************************************************
ControlBind::ControlBind(const char* _id, Simulator* _simPtr, KeyPressInfo _k, bool(*_callBack)(UINT16, UINT8))
   :Element(_id, _simPtr)
{ 
   if (_callBack == NULL){
      fprintf(stderr, "Callback cannot be NULL\n");
      exit(CONTROL_FAIL);
   }
   this->kpInfo = _k;
   this->callBack = _callBack;
}
//**************************************************************************
ControlBind::ControlBind(ControlBind& c)
   : ControlBind(c.getID().id, c.simPtr, c.getKeyPressInfo(), c.callBack)
{
   fprintf(stderr, "Warning, attempting to make copy of ControlBind Object\n");
}
//**************************************************************************
ControlBind::~ControlBind(void){
   this->callBack = NULL;
}
//**************************************************************************
ControlBind::KeyPressInfo ControlBind::getKeyPressInfo(void){
   return this->kpInfo;
}
//**************************************************************************
bool ControlBind::runCallBack(void){
   return this->callBack(this->kpInfo.keyPress, this->kpInfo.modifiers);
}
//**************************************************************************
ID_GL ControlBind::getStringHash(ControlBind::KeyPressInfo _kpi){
   //Very simply tranlates the keyinfo into a string for hashing.
   if (sizeof(ControlBind::KeyPressInfo) > MAX_ID_LEN - 1){
      fprintf(stderr, "ControlBind Hash Fail. Need to increase MAX_ID_LEN\n");
      exit(CONTROL_FAIL);
   }
   ID_GL str;
   BYTE* ptr = (BYTE*)(&_kpi);
   sprintf_s(str.id, "%X::%X", _kpi.keyPress, _kpi.modifiers);
   return str;
}



//**************************************************************************
//**************************************************************************
//** CAMERA ****************************************************************
//**************************************************************************
//**************************************************************************
Camera::Camera(const char* _id, Simulator* _simPtr, glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up)
   :Element(_id, _simPtr)
{
   this->eye = _eye;
   this->at = _at;
   this->up = _up;
}
//**************************************************************************
Camera::Camera(Camera& c)
   :Camera(c.getID().id, c.simPtr, c.eye, c.at, c.up)
{
   fprintf(stderr, "Warning, attempting to make copy of Camera Object\n");
}
//**************************************************************************
Camera::~Camera(void){
   //Void
}
//**************************************************************************
void Camera::setUp(glm::vec3 v){
   this->up = v;
}
//**************************************************************************
void Camera::setAt(glm::vec3 v){
   this->at = v;
}
//**************************************************************************
void Camera::setEye(glm::vec3 v){
   this->eye = v;
}
//**************************************************************************
glm::mat4 Camera::getViewMatrix(void){
   return glm::lookAt(this->eye, this->at, this->up);
}
//**************************************************************************
bool Camera::update(void){
   return true;
}
//**************************************************************************
VectorCamera::VectorCamera(const char* _id, Simulator* _simPtr, float _distZ, float _distY, const char* _obj1, const char* _obj2)
   :Camera(_id, _simPtr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0,1,0))
{
   this->distZ = _distZ;
   this->distY = _distY;
   if (_obj1 == NULL || _obj2 == NULL){
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }
   strncpy(this->objA.id, _obj1, MAX_ID_LEN - 1);
   strncpy(this->objB.id, _obj2, MAX_ID_LEN - 1);
}
//**************************************************************************
VectorCamera::VectorCamera(VectorCamera& c)
   :VectorCamera(c.getID().id, c.simPtr, c.distZ, c.distY, c.getObjA().id, c.getObjB().id)
{
   fprintf(stderr, "Warning, attempting to make copy of VectorCamera Object\n");
}
//**************************************************************************
VectorCamera::~VectorCamera(void){
   //Void
}
//**************************************************************************
glm::mat4 VectorCamera::getViewMatrix(void){
   //Uses parent function. Relys upon the update function being called
   return Camera::getViewMatrix();
}
//**************************************************************************
ID_GL VectorCamera::getObjA(void){
   return this->objA;
}
//**************************************************************************
ID_GL VectorCamera::getObjB(void){
      return this->objB;
}
//**************************************************************************
bool VectorCamera::update(void){
   //Attempt to get object_1
   Object3D* _obj1 = this->simPtr->getInstance(this->getObjA().id);
   if (_obj1 == NULL){
      return false;
   }
   //Attempt to get object_2
   Object3D* _obj2 = this->simPtr->getInstance(this->getObjB().id);
   if (_obj2 == NULL){
      return false;
   }

   //UP is the exact same as obj1's up
   this->setUp(_obj1->getLMM_Up());

   //AT is the far objects position
   this->setAt(_obj2->getLMM_Translation());

   //EYE is the combination of _obj1's position, mixed with the distZ and distY offsets
   glm::vec3 yOffset = Simulator::get3D_ScaledVector(_obj1->getLMM_Up(), this->distY);
   glm::vec3 zOffset = Simulator::get3D_ScaledVector((_obj1->getLMM_Translation() - _obj2->getLMM_Translation()), this->distZ);
   this->setEye(_obj1->getLMM_Translation() + yOffset + zOffset);

   return true;
}
//**************************************************************************
ChaseCamera::ChaseCamera(const char* _id, Simulator* _simPtr, glm::vec3 _eyeOffset, glm::vec3 _atOffset, const char* _objID)
   :Camera(_id, _simPtr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))
{
   if (_objID == NULL){
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }
   strncpy(this->objID.id, _objID, MAX_ID_LEN - 1);

   this->eyeOffset = _eyeOffset;
   this->atOffset = _atOffset;
}
//**************************************************************************
ChaseCamera::ChaseCamera(ChaseCamera& c)
   :ChaseCamera(c.getID().id, c.simPtr, c.eyeOffset, c.atOffset, c.objID.id)
{
   fprintf(stderr, "Warning, attempting to make copy of ChaseCamera Object\n");
}
//**************************************************************************
ChaseCamera::~ChaseCamera(void){
   //Void
}
//**************************************************************************
glm::mat4 ChaseCamera::getViewMatrix(void){
   //Uses parent function. Relys upon the update function being called
   return Camera::getViewMatrix();
}
//**************************************************************************
ID_GL ChaseCamera::getObjID(void){
   return this->objID;
}
//**************************************************************************
glm::vec3 ChaseCamera::getEyeOffset(void){
   return this->eyeOffset;
}
//**************************************************************************
glm::vec3 ChaseCamera::getAtOffset(void){
   return this->atOffset;
}
//**************************************************************************
bool ChaseCamera::update(void){
   //Attempt to get object
   Object3D* objPtr = this->simPtr->getInstance(this->getObjID().id);
   if (objPtr == NULL){
      return false;
   }

   //UP is the exact same as obj's up
   this->setUp(objPtr->getLMM_Up());

   //We need to translate the offsets to match the model's point of view
   glm::vec3 offX = Simulator::get3D_ScaledVector(objPtr->getLMM_Right(), this->eyeOffset.x);
   glm::vec3 offY = Simulator::get3D_ScaledVector(objPtr->getLMM_Up(), this->eyeOffset.y);
   glm::vec3 offZ = Simulator::get3D_ScaledVector(objPtr->getLMM_At(), this->eyeOffset.z);
   glm::vec3 offset = offX + offY + offZ;
   this->setEye(offset + objPtr->getLMM_Translation());

   offX = Simulator::get3D_ScaledVector(objPtr->getLMM_Right(), this->atOffset.x);
   offY = Simulator::get3D_ScaledVector(objPtr->getLMM_Up(), this->atOffset.y);
   offZ = Simulator::get3D_ScaledVector(objPtr->getLMM_At(), this->atOffset.z);
   offset = offX + offY + offZ;
   this->setAt(offset + objPtr->getLMM_Translation());

   return true;
}
//**************************************************************************
FollowCamera::FollowCamera(const char* _id, Simulator* _simPtr, glm::vec3 _eyeOffset, const char* _objID)
   :Camera(_id, _simPtr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))
{
   if (_objID == NULL){
      fprintf(stderr, "Cannot assign object to NULL\n");
      exit(CAMERA_FAIL);
   }
   strncpy(this->objID.id, _objID, MAX_ID_LEN - 1);

   this->eyeOffset = _eyeOffset;
}
//**************************************************************************
FollowCamera::FollowCamera(FollowCamera& c)
   :FollowCamera(c.getID().id, c.simPtr, c.eyeOffset, c.objID.id)
{
   fprintf(stderr, "Warning, attempting to make copy of FollowCamera Object\n");
}
//**************************************************************************
FollowCamera::~FollowCamera(void){
   //Void
}
//**************************************************************************
glm::mat4 FollowCamera::getViewMatrix(void){
   //Uses parent function. Relys upon the update function being called
   return Camera::getViewMatrix();
}
//**************************************************************************
ID_GL FollowCamera::getObjID(void){
   return this->objID;
}
//**************************************************************************
glm::vec3 FollowCamera::getEyeOffset(void){
   return this->eyeOffset;
}
//**************************************************************************
bool FollowCamera::update(void){
   //Attempt to get object
   Object3D* objPtr = this->simPtr->getInstance(this->getObjID().id);
   if (objPtr == NULL){
      return false;
   }

   //UP is the exact same as obj's up
   this->setUp(objPtr->getLMM_Up());

   //Eye position found in world_view terms, not models
   this->setEye(this->eyeOffset + objPtr->getLMM_Translation());

   //Camera will be directed at the model.
   this->setAt(objPtr->getLMM_Translation());

   return true;
}



//**************************************************************************
//**************************************************************************
//** OBJECT3D **************************************************************
//**************************************************************************
//**************************************************************************
Object3D::Object3D(const char* _id,
   Simulator* _simPtr,
   std::vector<char*> _textureIDs,
   std::vector<char*> _modelIDs,
   std::vector<char*> _lightIDs,
   const char* _colorShaderID,
   bool _isTransparent,
   glm::mat4 _scaleMatrix = glm::mat4(),
   glm::mat4 _translationMatrix = glm::mat4(),
   glm::mat4 _rotationMatrix = glm::mat4())

   :Element(_id, _simPtr)
{
   this->setRotationMatrix(_rotationMatrix);
   this->setScaleMatrix(_scaleMatrix);
   this->setTranslationMatrix(_translationMatrix);
   this->lastModelMatrix = getModelMatrix();

   this->setTransparent(_isTransparent);

   //Do the textures exist in Simulator?
   for (size_t i = 0; i < _textureIDs.size(); i++){
      if (this->simPtr->getTexture(_textureIDs.at(i)) == NULL){
         fprintf(stderr, "Texture with id: %s doesn't exist\n", _textureIDs.at(i));
         exit(OBJECT_FAIL);
      }
   }
   this->textureIDs = _textureIDs;

   //Does the Shader exist in Simulator?
   if (_colorShaderID == NULL || this->simPtr->getShader(_colorShaderID) == NULL){
      fprintf(stderr, "Shader with id: %s doesn't exist\n", _colorShaderID);
      exit(OBJECT_FAIL);
   }
   strncpy(colorShaderID.id, _colorShaderID, MAX_ID_LEN - 1);

   //Do the models exist in Simulator?
   for (size_t i = 0; i < _modelIDs.size(); i++){
      if (this->simPtr->getModel(_modelIDs.at(i)) == NULL){
         fprintf(stderr, "Model with id: %s doesn't exist\n", _modelIDs.at(i));
         exit(OBJECT_FAIL);
      }
   }
   this->modelIDs = _modelIDs;

   //Do the lights exist in Simulator?
   for (size_t i = 0; i < _lightIDs.size(); i++){
      if (this->simPtr->getLight(_lightIDs.at(i)) == NULL){
         fprintf(stderr, "Light with id: %s doesn't exist\n", _lightIDs.at(i));
         //This will not fail, but may have strange results
      }
   }
   this->lightIDs = _lightIDs;
}
//**************************************************************************
Object3D::~Object3D(){
   this->simPtr = NULL;
}
//**************************************************************************
bool Object3D::isTransparent(void){
   return this->transparent;
}
//**************************************************************************
void Object3D::setTransparent(bool b){
   this->transparent = b;
}
//**************************************************************************
glm::mat4 Object3D::getRotationMatrix(void){
   return this->rotationMatrix;
}
//**************************************************************************
bool Object3D::setRotationMatrix(glm::mat4 rm){
   this->rotationMatrix = rm; return true;
}
//**************************************************************************
glm::mat4 Object3D::getScaleMatrix(void){
   return this->scaleMatrix;
}
//**************************************************************************
bool Object3D::setScaleMatrix(glm::mat4 rm){
   this->scaleMatrix = rm; return true;
}
//**************************************************************************
glm::mat4 Object3D::getTranslationMatrix(void){
   return this->translationMatrix;
}
//**************************************************************************
bool Object3D::setTranslationMatrix(glm::mat4 rm){
   this->translationMatrix = rm; return true;
}
//**************************************************************************
glm::mat4 Object3D::getLastModelMatrix(void){
   return this->lastModelMatrix;
}
//**************************************************************************
glm::vec3 Object3D::getLMM_Translation(void){
   return glm::vec3(this->lastModelMatrix[3][0], this->lastModelMatrix[3][1], this->lastModelMatrix[3][2]);
}
//**************************************************************************
glm::vec3 Object3D::getLMM_Right(void){
   return glm::vec3(this->lastModelMatrix[0][0], this->lastModelMatrix[0][1], this->lastModelMatrix[0][2]);
}
//**************************************************************************
glm::vec3 Object3D::getLMM_Up(void){
   return glm::vec3(this->lastModelMatrix[1][0], this->lastModelMatrix[1][1], this->lastModelMatrix[1][2]);
}
//**************************************************************************
glm::vec3 Object3D::getLMM_At(void){
   return glm::vec3(this->lastModelMatrix[2][0], this->lastModelMatrix[2][1], this->lastModelMatrix[2][2]);
}
//**************************************************************************
glm::mat4 Object3D::getModelMatrix(void){
   this->lastModelMatrix = (this->getTranslationMatrix() * 
      this->getRotationMatrix() * this->getScaleMatrix());
   return this->lastModelMatrix;
}
//**************************************************************************
void Object3D::setLastModelMatrix(glm::mat4 _m){
   this->lastModelMatrix = _m;
}
//**************************************************************************
bool Object3D::update(void){
   return true;
}
//**************************************************************************
bool Object3D::draw(void){
   //Local Vars
   Simulator* sim = this->simPtr;
   Model* model = sim->getModel(this->modelIDs.at(0));
   Shader* shader = sim->getShader(this->colorShaderID.id);
   //Valid values?
   if (sim == NULL || model == NULL || shader == NULL)
      return false;

   //Bind VAO and Shaders
   glBindVertexArray(model->getVAO());
   glUseProgram(shader->getShaderID());
   glEnable(GL_DEPTH_TEST);

   //Is this object transparent?
   if (this->isTransparent()){
      // Enable blending
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else{
      glDisable(GL_BLEND);
   }
   

   //Create temporary matrices and bind them to the shaders
   glm::mat4 mvMatrix = sim->getViewMatrix() * this->getLastModelMatrix();
   glm::mat4 mvpMatrix = sim->getProjectionMatrix() * mvMatrix;

   //Set matrix shader uniforms. NA indicates that they don't exist
   //Matrices
   if (shader->getmvpMatrixID() != NA)
      glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(mvpMatrix[0][0]));
   if (shader->getmMatrixID() != NA)
      glUniformMatrix4fv(shader->getmMatrixID(), 1, GL_FALSE, &(getLastModelMatrix()[0][0]));
   if (shader->getvMatrixID() != NA)
      glUniformMatrix4fv(shader->getvMatrixID(), 1, GL_FALSE, &((sim->getViewMatrix())[0][0]));
   if (shader->getmvMatrixID() != NA)
      glUniformMatrix4fv(shader->getmvMatrixID(), 1, GL_FALSE, &(mvMatrix[0][0]));
   
   //Light Positions
   std::vector<GLuint> tempVec = shader->getLightID();
   UINT16 maxIts = std::min(this->lightIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++){
      if (tempVec.at(j) != NA){
         Light* t = sim->getLight(this->lightIDs.at(j));
         if (t == NULL) continue;
         glm::mat4 lsi = t->getLightShaderInfo();
         glUniformMatrix4fv(tempVec.at(j), 1, GL_FALSE, &(lsi[0][0]));
      }
      else
         break;
   }

   //Shadow Textures
   tempVec = shader->getDepthTextureID();
   maxIts = std::min(this->lightIDs.size(), tempVec.size());
   UINT16 textureOffset = 0;
   for (UINT16 j = 0; j < maxIts; j++){
      if (tempVec.at(j) != NA){
         Light* t = sim->getLight(this->lightIDs.at(j));
         if (t == NULL) continue;
         //Get shadow texture list
         std::vector<GLuint> shadowList = t->getShadowTextureIDs();
         for (UINT16 k = 0; k < shadowList.size(); k++){
            glActiveTexture(GL_TEXTURE0 + textureOffset);
            glBindTexture(GL_TEXTURE_2D, shadowList.at(k));
            //Link this texture to the shader
            glUniform1i(tempVec.at(j), textureOffset);
            //Update the texture offset
            textureOffset++;
         }
      }
      else
         break;
   }

   //Shadow Matrices
   tempVec = shader->getDepthMVPID();
   maxIts = std::min(this->lightIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++){
      if (tempVec.at(j) != NA){
         Light* t = sim->getLight(this->lightIDs.at(j));
         if (t == NULL) continue;
         //Get shadow matrix list
         std::vector<glm::mat4> shadowList = t->getDepthBiasVPMatrices();
         for (UINT16 k = 0; k < shadowList.size(); k++){
            glm::mat4 dbmvpTemp = shadowList.at(k) * this->getLastModelMatrix();
            glUniformMatrix4fv(tempVec.at(j), 1, GL_FALSE, &(dbmvpTemp[0][0]));
         }
      }
      else
         break;
   }

   //Bind Textures
   tempVec = shader->getTextureID();
   maxIts = std::min(this->textureIDs.size(), tempVec.size());
   for (UINT16 j = 0; j < maxIts; j++){
      if (tempVec.at(j) != NA){
         Texture* t = sim->getTexture(this->textureIDs.at(j));
         if (t == NULL) continue;
         glActiveTexture(GL_TEXTURE0 + textureOffset);
         glBindTexture(GL_TEXTURE_2D, t->getTextureID());
         //Link this texture to the shader
         glUniform1i(tempVec.at(j), textureOffset);
         //Update the texture offset
         textureOffset++;
      }
   }

   //Bind random floats if they exist
   tempVec = shader->getRandomFloatID();
   for (UINT16 j = 0; j < tempVec.size(); j++){
      //Generate random numbers
      float randVal = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
      glUniform1f(tempVec.at(j), randVal);
   }

   //Link vertices to shader
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

   //Link texture coords to shader
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, model->getVTO());
   glVertexAttribPointer(
      1,                                // attribute
      2,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
      );

   //Link normals to shader
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, model->getVNO());
   glVertexAttribPointer(
      2,                                // attribute
      3,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
      );

   //Bind the index buffer
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getVEO());

   //Draw!
   glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, (void*)0);

   //Disable linked vertex arrays
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);

   //Unbind the vertex array
   glBindVertexArray(0);

   return true;
}
//**************************************************************************
Light::LightInfoBasic Object3D::updateLight(Light::LightInfoBasic _li){
   //Congradulations, you've been classified as a light.. just return
   //your current position...and arbritrary values
   _li.position = this->getLMM_Translation();
   _li.direction = this->getLMM_At();
   return _li;
}
//**************************************************************************
inline std::vector<char*> Object3D::getModelIDs(void){
   return this->modelIDs;
}
//**************************************************************************
inline std::vector<char*> Object3D::getTextureIDs(void){
   return this->textureIDs;
}
//**************************************************************************
inline std::vector<char*> Object3D::getLightIDs(void){
   return this->lightIDs;
}
//**************************************************************************
inline ID_GL Object3D::getColorShaderID(void){
   return this->colorShaderID;
}



//**************************************************************************
//**************************************************************************
//** BACKGROUND ************************************************************
//**************************************************************************
//**************************************************************************
SkyBox::SkyBox(const char* _id,
   Simulator* _simPtr,
   const char* _colorShaderID,
   float _planeLength,
   const char* _texPosY,
   const char* _texNegY,
   const char* _texPosX,
   const char* _texNegX,
   const char* _texPosZ,
   const char* _texNegZ)
   
   //Call the parent constructor
   : Object3D(_id, _simPtr, std::vector<char*>(), std::vector<char*>(), std::vector<char*>(), _colorShaderID, FALSE,
   glm::mat4(), glm::mat4(), glm::mat4())
{
   float length = (_planeLength / 2);

   //Vertices that make up a plane of lenght <planeLength> on the x,z plane
   std::vector<glm::vec3> indexed_vertices = std::vector<glm::vec3>({ glm::vec3(-length, 0.0, -length), glm::vec3(-length, 0.0, length),
      glm::vec3(length, 0.0, -length), glm::vec3(length, 0.0, length) });
   std::vector<glm::vec2> indexed_texture = std::vector<glm::vec2>({ glm::vec2(0.0, -1.0), glm::vec2(0.0, 0.0),
      glm::vec2(1.0, -1.0), glm::vec2(1.0, 1.0) });
   std::vector<glm::vec3> indexed_normals = std::vector<glm::vec3>({ glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0) });
   std::vector<UINT32> indices = std::vector<UINT32>({ 0, 1, 2, 3, 2, 1 });

   //Generate and Bind a new Vertex Array Object
   glGenVertexArrays(1, &(this->vao));
   glBindVertexArray(this->vao);

   //Generate GPU vertex buffers, get handles, and upload values
   //Load vertex info into a VBO
   glGenBuffers(1, &(this->vbo));
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &(indexed_vertices[0]), GL_STATIC_DRAW);
   //Load normal info into a VBO
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

   this->skyPlane[0].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, length, 0)) * glm::rotate(glm::mat4(), 180.0f, glm::vec3(1, 0, 0));
   this->skyPlane[1].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, -length, 0));
   this->skyPlane[2].modelMatrix = glm::translate(glm::mat4(), glm::vec3(length, 0, 0)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(0, 0, 1));
   this->skyPlane[3].modelMatrix = glm::translate(glm::mat4(), glm::vec3(-length, 0, 0)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(0, 0, -1));
   this->skyPlane[4].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, length)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(-1, 0, 0));
   this->skyPlane[5].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -length)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(1, 0, 0));
}
//**************************************************************************
SkyBox::~SkyBox(void){
   //void
}
//**************************************************************************
bool SkyBox::draw(void){
   //Local Vars
   Simulator* sim = this->simPtr;
   Shader* shader = sim->getShader(this->getColorShaderID().id);

   //Valid values?
   if (sim == NULL || shader == NULL)
      return false;

   //Get a copy of the current viewMatrix
   glm::mat4 viewMatrix = sim->getViewMatrix();
   //Elminate the translation aspect of the view matrix
   viewMatrix[3][0] = viewMatrix[3][1] = viewMatrix[3][2] = 0;
   viewMatrix[3][3] = 1;

   viewMatrix = sim->getProjectionMatrix() * viewMatrix;

   //Bind VAO and Shaders
   glBindVertexArray(this->vao);
   glUseProgram(shader->getShaderID());
   //Disable Depth testing
   glDisable(GL_DEPTH_TEST);

   //Draw each of the planes
   for (UINT8 i = 0; i < 6; i++){
      //Get the modelView Matrix
      glm::mat4 mvpMatrix = viewMatrix * this->skyPlane[i].modelMatrix;

      //Set the shader uniform
      if (shader->getmvpMatrixID() != NA)
         glUniformMatrix4fv(shader->getmvpMatrixID(), 1, GL_FALSE, &(mvpMatrix[0][0]));

      //Bind the texture
      glActiveTexture(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, sim->getTexture(this->skyPlane[i].textureID.id)->getTextureID());
      //Link this texture to the shader
      glUniform1i(shader->getTextureID().at(0), 0);

      //Link vertices to shader
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
      glVertexAttribPointer(
         0,                  // attribute
         3,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,                  // stride
         (void*)0            // array buffer offset
         );

      //Link texture coords to shader
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, this->vto);
      glVertexAttribPointer(
         1,                                // attribute
         2,                                // size
         GL_FLOAT,                         // type
         GL_FALSE,                         // normalized?
         0,                                // stride
         (void*)0                          // array buffer offset
         );

      //Link normals to shader
      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, this->vno);
      glVertexAttribPointer(
         2,                                // attribute
         3,                                // size
         GL_FLOAT,                         // type
         GL_FALSE,                         // normalized?
         0,                                // stride
         (void*)0                          // array buffer offset
         );

      //Bind the index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->veo);

      //Draw!
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

      //Disable linked vertex arrays
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);

   }

   //Unbind the vertex array
   glBindVertexArray(0);

   return true;
}




