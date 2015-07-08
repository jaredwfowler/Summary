/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 11, 2014
   Comp 465 "Warbird Simulation"
   Professor Barnes


ABOUT:

General 3D objects that will be used along with the openGL library.
Each object will keep track of its fileName, scale, translation,
and rotation.

*/



# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "TypeDefinesAndHelpers.hpp"
#include <iostream>

class Object3D
{

protected:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    char objName[25];             //Name of the Model (probably same as fileName)
    char fileName[25];            //Name of the Model file (.tri)
    UINT32 vertexCount = 0;       //Number of vertices in .tri Model file
    float modelSize    = 0;       //Size of the model (Relative to everything else?)
    bool active        = true;    //Do we want to utilize/update/etc this model?

    /*The following vec3's relate to how the object reacts to light.*/
    glm::vec3 ambient;    //Minimum value for the objects ambient light
    glm::vec3 specular;   //Max specular light added to object
    glm::vec3 diffuse;    //Minimum value diffuse can be over an infinite distance

    glm::mat4 rotationMatrix;
    glm::mat4 scaleMatrix;
    glm::mat4 translationMatrix;

    glm::mat4 lastModelMatrix;   //Backs up the last model matrix we worked hard to find

public:
    // Constructor(s) ################################################
    //################################################################
    Object3D(char* objName, char* fileName, UINT32 vertexCount, float modelSize, bool active = true,
             glm::mat4 scaleMatrix       = glm::mat4(),
             glm::mat4 translationMatrix = glm::mat4(),
             glm::mat4 rotationalMatrix  = glm::mat4(),
             glm::vec3 ambientMin = glm::vec3(.1,.1,.1),
             glm::vec3 diffuseMin = glm::vec3(.1,.1,.1),
             glm::vec3 specularMax = glm::vec3(.5,.5,.5)){

        setObjName(objName);
        setFileName(fileName);
        setVertexCount(vertexCount);
        setModelSize(modelSize);
        setActive(active);

        setAmbientMin(ambientMin);
        setDiffuseMin(diffuseMin);
        setSpecularMax(specularMax);

        //Unless default function parameters are overwritten,
        //we will write identity matrices to the matrices
        setRotationMatrix(rotationalMatrix);
        setScaleMatrix(scaleMatrix);
        setTranslationMatrix(translationMatrix);

    }

    // gets and sets #################################################
    //################################################################
    inline char* getObjName(void){ return this->objName; }
    inline bool setObjName(char* objName){ strcpy(this->objName, objName); return true; }

    inline char* getFileName(void){ return this->fileName; }
    inline bool setFileName(char* fileName){ strcpy(this->fileName, fileName); return true; }

    inline UINT32 getVertexCount(void){ return this->vertexCount; }
    inline bool setVertexCount(UINT32 vc){ this->vertexCount = vc; return true; }

    inline float getModelSize(void){ return this->modelSize; }
    inline bool setModelSize(float ms){ this->modelSize = ms; return true; }

    inline bool isActive(void){ return this->active; }
    inline void setActive(bool b){ this->active = b; }

    inline glm::mat4 getRotationMatrix(void){ return this->rotationMatrix; }
    inline bool setRotationMatrix(glm::mat4 rm){ this->rotationMatrix = rm; return true; }

    inline glm::mat4 getScaleMatrix(void){ return this->scaleMatrix; }
    inline bool setScaleMatrix(glm::mat4 rm){ this->scaleMatrix = rm; return true; }

    inline glm::mat4 getTranslationMatrix(void){ return this->translationMatrix; }
    inline bool setTranslationMatrix(glm::mat4 rm){ this->translationMatrix = rm; return true; }

    inline glm::mat4 getLastModelMatrix(void){ return this->lastModelMatrix; }
    inline bool setLastModelMatrix(glm::mat4 m){ this->lastModelMatrix = m; return true; }

    virtual glm::mat4 getModelMatrix(void){/*Let future children impliment this*/
        return glm::mat4();
    }

    virtual void update(bool updateIfInactive){/*Let future children impliment this*/
        return;
    }

    inline glm::vec3 getAmbientMin(void){ return this->ambient; }
    inline bool setAmbientMin(glm::vec3 a){ this->ambient = a; return true; }

    inline glm::vec3 getDiffuseMin(void){ return this->diffuse; }
    inline bool setDiffuseMin(glm::vec3 d){ this->diffuse = d; return true; }

    inline glm::vec3 getSpecularMax(void){ return this->specular; }
    inline bool setSpecularMax(glm::vec3 s){ this->specular = s; return true; }

};

#endif