/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
October 07, 2014
Comp 465 "Warbird Simulation"
Professor Barnes


ABOUT:

Child class of Object3D, this class is made exclusively for a
missle site on a CelestialBodySolid3D.  As such, it will hold a set 
number of missles, along with a pointer to a CBS3D object.  Missle
sites are buildings, and are therefore stationary upon the ground in 
which they are placed.

The inherited translation matrix and rotation matrix will reflect
the offset of the missle site. For example, is we are situated 
on a planet then the translation will most likely be the radius of
the planet in order to bring the site to the surface.  Rotation 
will signify the sites rotation on its axis.

*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef MISSLESITE_HPP
#define MISSLESITE_HPP

#include "Object3D.hpp"
#include "TypeDefinesAndHelpers.hpp"


class MissleSite : public Object3D
{

private:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    UINT32 missleCount = 0;               //Number of missles this object holds
    CelestialBodySolid3D* cbs3d = NULL;  //Ponter to object which this missle site rests on


public:
    // Constructor(s) ################################################
    //################################################################
    MissleSite(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
        bool active = true, UINT32 missleCount = 0, CelestialBodySolid3D* obj = NULL,
        glm::mat4 scaleMatrix = glm::mat4(), 
        glm::mat4 translationMatrix = glm::mat4(),
        glm::mat4 rotationalMatrix = glm::mat4(),
        glm::vec3 ambientMin = glm::vec3(.6, .6, .6),
        glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
        glm::vec3 specularMax = glm::vec3(.8, .8, .8))

        //Call the parent constructor
        : Object3D(objName, fileName, vertexCount, modelSize, active, scaleMatrix,
        translationMatrix, rotationalMatrix, ambientMin, diffuseMin, specularMax)
    {
        setMissleCount(missleCount);
        setCBS3D(obj);

        //Invoke the objects first update to get it situated
        this->getModelMatrix();
    }

    // gets and sets #################################################
    //################################################################
    inline bool setMissleCount(UINT32 mc){ this->missleCount = mc; return true; }
    inline UINT32 getMissleCount(void){ return this->missleCount; }

    inline bool setCBS3D(CelestialBodySolid3D* c){ this->cbs3d = c; return true; }
    inline CelestialBodySolid3D* getCBS3D(void){ return this->cbs3d; }

    /*Gets the current modelMatrix of the object and at the same time updates the lastModelMatrix.*/
    virtual glm::mat4 getModelMatrix(void);

    /*Update will run the getModelMatrix in order to update the lastModelMatrix*/
    virtual void update(bool updateIfInactive);

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 MissleSite::getModelMatrix(void){

    /*This objects should have some sort of parent. If it doesn't, we will use the natural origin and scale, spin, translate,
    and rotate from the origin.  If a parent exists, we will treat the parent origin as the new origin.  We want to stay in a 
    constant location on the parent. Therefore, the ordering of application would be as such:  scale, translate, rotate (spin of object), 
    translate, rotate*/

    //Do we know any celestial object that we are located on?
    if (this->cbs3d == NULL){
        //Use inherited information to transform off true origin.
        this->setLastModelMatrix(this->getRotationMatrix() * this->getTranslationMatrix() * this->getScaleMatrix());
        return this->lastModelMatrix;
    }

    glm::mat4 returnM;

    /*Note: Multiplication steps are reverse of the application steps.. I will therefore number backwards.*/

    //Step 4: Translate and rotate out to celestial body location
    returnM *= glm::translate(glm::mat4(), MathHelperJWF::getVector(cbs3d->getLastModelMatrix(), 3));

    //Step3: Spin missle site to align with celestial body spin
    returnM *= cbs3d->getCurrentSpin();
    returnM *= this->getTranslationMatrix();

    //Step 2: Spin missle site
    returnM *= this->getRotationMatrix();

    //Step 1: Scale
    returnM *= this->getScaleMatrix();

    //Back up our hard work
    this->setLastModelMatrix(returnM);

    //Return!
    return this->lastModelMatrix;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void MissleSite::update(bool updateIfInactive){

    //Update the lastModelMatrix
    this->getModelMatrix();

}


#endif