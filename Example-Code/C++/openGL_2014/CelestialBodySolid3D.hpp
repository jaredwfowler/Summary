/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 11, 2014
   Comp 465 "Warbird Simulation"
   Professor Barnes


ABOUT:

Child class of Object3D.  Includes functionality such as 
rotation, orbits, etc.  Includes functionality to transition
origin from natural origin to parent location origin.

This class referrs to objects such as planets, moons, or basic
satelites.

*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef CELESTIALBODYSOLID3D_HPP
#define CELESTIALBODYSOLID3D_HPP

#include "Object3D.hpp"


class CelestialBodySolid3D : public Object3D
{

private:
    // Constants #####################################################
    //################################################################
        //Void

    // Variables #####################################################
    //################################################################
    float radiansRot  = 0;     //What is the rotational speed? Radians/Frame
    float radiansSpin = 0;     //What is the spinning speed?
    bool  orbital     = false; //Is the object orbiting a point of space?
    bool  spinning    = false; //Is the object spinning about an axis?

    float gravityVector = 0;

    float currentSpin     = 0;  //Used as a helper variable in the update function
    float currentRotation = 0;  //Used as a helper variable in the update function

    glm::vec3 rotationAxis;
    glm::vec3 spinAxis;
    glm::mat4 spinningMatrix;


    CelestialBodySolid3D* cb3D = NULL;  //Pointer to associated Model (Parent)

public:
    // Constructor(s) ################################################
    //################################################################
    CelestialBodySolid3D(char* objName, char* fileName, UINT32 vertexCount, float modelSize, bool active = true, 
                         CelestialBodySolid3D* cb3D = NULL, float radiansRot = 0, float radiansSpin = 0, bool orbital = false, bool spinning = false,
                         glm::mat4 spinningMatrix = glm::mat4(),
                         glm::vec3 spinAxis = glm::vec3(0,1,0),
                         glm::vec3 rotationAxis = glm::vec3(0, 1, 0),
                         glm::mat4 scaleMatrix = glm::mat4(),
                         glm::mat4 translationMatrix = glm::mat4(),
                         glm::mat4 rotationalMatrix = glm::mat4(),
                         glm::vec3 ambientMin = glm::vec3(.3, .3, .3),
                         glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
                         glm::vec3 specularMax = glm::vec3(.1, .1, .1))

        //Call the parent constructor
        : Object3D(objName, fileName, vertexCount, modelSize, active, scaleMatrix,
                   translationMatrix, rotationalMatrix, ambientMin, diffuseMin, specularMax)
    {
        setRadiansRot(radiansRot);
        setRadiansSpin(radiansSpin);
        setOrbital(orbital);
        setSpinning(spinning);

        setSpinningMatrix(spinningMatrix);
        setRotationAxis(rotationAxis);
        setSpinAxis(spinAxis);

        setParentObj(cb3D);

        //Invoke the objects first update to get it situated
        this->getModelMatrix();
    }

    // gets and sets #################################################
    //################################################################
    inline float getRadiansRot(void){ return this->radiansRot; }
    inline bool setRadiansRot(float r){ this->radiansRot = r; return true; }

    inline float getRadiansSpin(void){ return this->radiansSpin; }
    inline bool setRadiansSpin(float r){ this->radiansSpin = r; return true; }

    inline bool isOrbital(void){ return this->orbital; }
    inline void setOrbital(bool b){ this->orbital = b; }

    inline bool isSpinning(void){ return this->spinning; }
    inline void setSpinning(bool b){ this->spinning = b; }

    inline float getGravity(void){ return this->gravityVector; }
    inline bool setGravity(float f){ this->gravityVector = f; return true; }

    inline glm::vec3 getRotationAxis(void){ return this->rotationAxis; }
    inline bool setRotationAxis(glm::vec3 ra){ this->rotationAxis = ra; return true; }

    inline glm::vec3 getSpinAxis(void){ return this->spinAxis; }
    inline bool setSpinAxis(glm::vec3 s){ this->spinAxis = s; return true; }

    inline glm::mat4 getSpinningMatrix(void){ return this->spinningMatrix; }
    inline bool setSpinningMatrix(glm::mat4 rm){ this->spinningMatrix = rm; return true; }

    inline bool setParentObj(CelestialBodySolid3D* p){ this->cb3D = p; return true; }
    inline CelestialBodySolid3D* getParentObj(void){ return this->cb3D; }

    /*Helper functions to get the current spin and rotation matrices of the celestial body.*/
    glm::mat4 getCurrentRotation(void){ return glm::rotate(glm::mat4(), this->currentRotation, this->rotationAxis); }
    glm::mat4 getCurrentSpin(void)    { return glm::rotate(glm::mat4(), this->currentSpin, this->spinAxis); }

    /*Using the rotation, translation, and scale Matrices, returns an appropiate model matrix.
    This returned matrix will probably be multiplied with the view and projection matrices in
    order to get the final coordinates for the model.*/
    glm::mat4 getModelMatrix(void);

    /*This object has the ability to update itself based upon variable values
    that we set such as radians, translation, etc. The user can choose to update all
    objects whether active or not (true), or just to update the active ojbects (false)*/
    void update(bool updateIfInactive);

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 CelestialBodySolid3D::getModelMatrix(void){

    /*NOTES: By applying a series of matrix multiplications we can scale, spin, translate, rotate, etc., our
    model. The strange part is how these operations are brought to pass.  The multiplication order is backwards
    from the actual application order.  For example, we multipy the scale matrix last, but it's this very matrix
    which is the first to be applied to the model!  Order matters!  The neat thing about this class is that it
    has a pointer to a parent object.  If no parent is pointed to, we assume the parent location to be the true
    origin of the window.  The general idea is that if the model has a parent, we will be treating the parent's 
    location as the new origin.*/

    //Final return value initialized to identity matrix
    glm::mat4 returnM(1.0f);

    //Step 1: Translate out from natural origin to parent origin (need parent's model matrix)
    if (this->getParentObj() != NULL){
        //Get parents Model Matrix
        glm::mat4 mV = this->getParentObj()->getLastModelMatrix();
        //Place this objects origin at the parents center
        returnM *= glm::translate(glm::mat4(), glm::vec3(mV[3][0], mV[3][1], mV[3][2]));
    }
    
    //Step 2: Is the object orbiting?
    if (this->orbital)
        returnM *= this->getCurrentRotation();

    //Step 3: Place this object relative to its parent's origin or true origin.
    returnM *= this->getTranslationMatrix();

    //Step 4: Is the object spinning?
    if (this->spinning)
        returnM *= this->getCurrentSpin();

    //Step 5: Scale
    returnM *= this->scaleMatrix;

    //Step 6: Back up our work
    this->setLastModelMatrix(returnM);

    //Return the Model Matrix
    return returnM;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void CelestialBodySolid3D::update(bool updateIfInactive) {

    //Is the fucntion active or is user by-passing the check?
    if (!(updateIfInactive || this->active))
        return;

    if (this->orbital){

        //Update the current Rotation
        this->currentRotation += this->radiansRot;
        //Is currentRotataion greater than 2PI?
        if ((currentRotation <= -(2 * PI)) || (currentRotation >= 2 * PI))
        {
            //How many times does 2PI divide it?
            INT32 div = (INT32)(currentRotation / (2 * PI));
            currentRotation -= (div * 2 * PI);
        }

    }

    if (this->spinning){

        //Update the current Rotation
        this->currentSpin += this->radiansSpin;
        //Is currentRotataion greater than 2PI?
        if ((currentSpin <= -(2 * PI)) || (currentSpin >= 2 * PI))
        {
            //How many times does 2PI divide it?
            INT32 div = (INT32)(currentSpin / (2 * PI));
            currentSpin -= (div * 2 * PI);
        }

    }

    //Update the lastModelMatrix
    this->getModelMatrix();

}


#endif