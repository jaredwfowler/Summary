/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Camera will be the base class with 
** limited functionality, but sufficient enough for simple cameras. Simulator
** will mantain a collection of Camera pointers, thus, taking advantage of
** polymorphism.
*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Core/JWFs_openGL/Simulator.hpp"

class Simulator;

//CLASS :: Camera***********************************************************
//**************************************************************************
class Camera : public Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Camera(const char* _id, Simulator* _simPtr, glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
   Camera(Camera& c);
   ~Camera(void);

   // gets and sets #################################################
   //################################################################
   virtual glm::mat4 getViewMatrix(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

protected:
   inline void setUp(glm::vec3 v);
   inline void setAt(glm::vec3 v);
   inline void setEye(glm::vec3 v);

private:
   glm::vec3 eye;
   glm::vec3 at;
   glm::vec3 up;
};

//CLASS :: VectorCamera*****************************************************
//**************************************************************************
class VectorCamera : public Camera
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   VectorCamera(const char* _id, Simulator* _simPtr, float _distZ, float _distY, const char* _obj1, const char* _obj2);
   VectorCamera(VectorCamera& c);
   ~VectorCamera(void);

   // gets and sets #################################################
   //################################################################
   virtual glm::mat4 getViewMatrix(void);
   ID_GL getObjA(void);
   ID_GL getObjB(void);
   
   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   float distZ;
   float distY;
   ID_GL objA;
   ID_GL objB;
};

//CLASS :: ChaseCamera******************************************************
//**************************************************************************
class ChaseCamera : public Camera
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   ChaseCamera(const char* _id, Simulator* _simPtr, glm::vec3 _eyeOffset, glm::vec3 _atOffset, const char* _objID);
   ChaseCamera(ChaseCamera& c);
   ~ChaseCamera(void);

   // gets and sets #################################################
   //################################################################
   virtual glm::mat4 getViewMatrix(void);
   ID_GL getObjID(void);
   inline glm::vec3 getEyeOffset(void);
   inline glm::vec3 getAtOffset(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   ID_GL objID;
   glm::vec3 eyeOffset;
   glm::vec3 atOffset;
};

//CLASS :: FollowCamera*****************************************************
//**************************************************************************
class FollowCamera : public Camera
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   FollowCamera(const char* _id, Simulator* _simPtr, glm::vec3 _eyeOffset, const char* _objID);
   FollowCamera(FollowCamera& c);
   ~FollowCamera(void);

   // gets and sets #################################################
   //################################################################
   virtual glm::mat4 getViewMatrix(void);
   ID_GL getObjID(void);
   inline glm::vec3 getEyeOffset(void);

   // Auxilary Functions ############################################
   //################################################################
   virtual bool update(void);

private:
   ID_GL objID;
   glm::vec3 eyeOffset;
};

#endif