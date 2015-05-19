/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
September 27, 2014
Comp 465 "Warbird Simulation"
Professor Barnes


ABOUT:

Child class of Object3D. Includes functionality such
as adjust pitch, roll, direction, position, etc.  Will
also hold missles.

*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef SPACESHIP_HPP
#define SPACESHIP_HPP

#include "Object3D.hpp"
#include "TypeDefinesAndHelpers.hpp"

class SpaceShip : public Object3D
{

private:
    // Constants #####################################################
    //################################################################
        //Void

    // Variables #####################################################
    //################################################################
    UINT32 missleCount = 0;   //Number of missles this object holds

public:
    // Constructor(s) ################################################
    //################################################################
    SpaceShip(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
        bool active = true, UINT32 missleCount = 0,
        glm::mat4 scaleMatrix = glm::mat4(),
        glm::mat4 translationMatrix = glm::mat4(),
        glm::mat4 rotationalMatrix = glm::mat4(),
        glm::vec3 ambientMin = glm::vec3(.5, .5, .5),
        glm::vec3 diffuseMin = glm::vec3(.2, .2, .2),
        glm::vec3 specularMax = glm::vec3(.8, .8, .8))

        //Call the parent constructor
        : Object3D(objName, fileName, vertexCount, modelSize, active, scaleMatrix, 
        translationMatrix, rotationalMatrix, ambientMin, diffuseMin, specularMax)
    {
        setMissleCount(missleCount);

        //Invoke the objects first update to get it situated
        this->getModelMatrix();
    }

    // gets and sets #################################################
    //################################################################
    inline bool setMissleCount(UINT32 mc){ this->missleCount = mc; return true; }
    inline UINT32 getMissleCount(void){ return this->missleCount; }

    /*Using the values for yaw, pitch, and roll, along with the translation and scale Matrices, 
    returns an appropiate model matrix. This returned matrix will probably be multiplied with 
    the view and projection matrices in order to get the final coordinates for the model.*/
    virtual glm::mat4 getModelMatrix(void);

    /*Updates the objects modelMatrix and lastModelMatrix*/
    virtual void update(bool updateIfInactive);

    /*Moves the space ship forward/backwards based upon its current relative z axis.*/
    void moveAdjust(float distance);

    /*Turns the space ship right/left. Axis of rotation is the current y vector of the space ship*/
    void yawAdjust(float radians);

    /*Causes the space ship to look up/down. Axis of rotation is the current x vector of the space ship*/
    void pitchAdjust(float radians);

    /*Causes the space ship to roll left/right. Axis of rotation is the current z vector of the space ship*/
    void rollAdjust(float radians);

    /*Causes the space ship to warp to the specified spot with the specified rotation*/
    void warp(glm::mat4 translation, glm::mat4 rotation){ this->translationMatrix = translation; this->rotationMatrix = rotation; }

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 SpaceShip::getModelMatrix(void){

    /*The model matrix is dependent upon scale, translation, and yaw, pitch, roll.  It is important to
    note that the order of these multiplications is very important.  Yaw, Pitch, and Roll should be 
    multiplied in this same order.*/

    this->setLastModelMatrix(this->translationMatrix * this->rotationMatrix * this->scaleMatrix);
    return this->lastModelMatrix;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SpaceShip::update(bool updateIfInactive){

    //Update the lastModelMatrix
    this->getModelMatrix();

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SpaceShip::moveAdjust(float distance){

    /*The object's model matrix should contain the following values:
        
        | right  up  at  position |
        |                         |
        |     values go here      |
        |   0     0   0     1     |

        We can utilze this matrix to update the objects current position.  We simply move along the 'at' vector,
        adding the value to the position vector.  We of course want to go a specified distance, so we do some 
        algebra beforehand to find an appropriatly scaled vector which matches the distance.
    */

    //Get a copy of the model matrix
    glm::mat4 mMatrix = this->lastModelMatrix;

    //Pull the 'at' vector from the model matrix
    glm::vec3 at(mMatrix[2][0], mMatrix[2][1], mMatrix[2][2]);

    //Scale the 'at' vector to match the distance
    at = MathHelperJWF::get3D_ScaledVector(at, distance);

    //Add 'at' vector to the ships translation
    if (distance < 0){  //Move ship back
        this->setTranslationMatrix(glm::translate(glm::mat4(), glm::vec3(mMatrix[3][0] - at[0], mMatrix[3][1] - at[1], mMatrix[3][2] - at[2])));
    }
    else{  //Move ship forward
        this->setTranslationMatrix(glm::translate(glm::mat4(), glm::vec3(mMatrix[3][0] + at[0], mMatrix[3][1] + at[1], mMatrix[3][2] + at[2])));
    }

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
/*The below functions which alter the ships yaw, pitch, and roll are simply written but hard to understand at first.
 We start with an understanding that by multiplying matrices we can modify the ship's scale, rotation, and translation.
 We note that the order in which we multiply the matrices are opposite in the way which they are applied to
 the model.  Therefore, the order of rotation needs to be applied in the same way.  One final note is that 
 the glm::rotate function should utilze the up,at,or right vector of the previously attained rotation matrix.*/

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SpaceShip::yawAdjust(float radians){

    //Multiply this yaw rotation with the rotationMatrix in this order.
    this->setRotationMatrix(glm::rotate(glm::mat4(), radians, MathHelperJWF::getVector(this->rotationMatrix, 1)) * this->rotationMatrix);

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SpaceShip::pitchAdjust(float radians){

    //Multiply this pitch rotation with the rotationMatrix in this order.
    this->setRotationMatrix(glm::rotate(glm::mat4(), radians, MathHelperJWF::getVector(this->rotationMatrix, 0)) * this->rotationMatrix);
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SpaceShip::rollAdjust(float radians){

    //Multiply this roll rotation with the rotationMatrix in this order.
    this->setRotationMatrix(glm::rotate(glm::mat4(), radians, MathHelperJWF::getVector(this->rotationMatrix, 2)) * this->rotationMatrix);

}


#endif