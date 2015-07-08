/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
October 08, 2014
Comp 465 "Warbird Simulation"
Professor Barnes


ABOUT:

Child class of Object3D.  Smart missle is given a target to follow and 
a speed at which to follow such object.  The user should also assign
an initial direction along with how far the missle should travel before
its smart capabilities turn on.

*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef SMARTMISSLE_HPP
#define SMARTMISSLE_HPP

#include "Object3D.hpp"
#include "TypeDefinesAndHelpers.hpp"


class SmartMissle : public Object3D
{

private:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    Object3D* obj          = NULL;   //What object are we trying to blow up?
    Object3D* obj_d        = NULL;   //What object shot the missle?
    float speed            = 10;     //What is the speed of the missle?
    float distanceTilSmart = 0;      //How far should the missle travel before becomes smart?
    bool smartMissleOn     = false;  //Smart Capabilities activated?
    float currentDist      = 0;      //Current distance traveled
    float maxDistTilDead   = 0;      //Max distance until missle will de-activate


public:
    // Public Vars ###################################################
    //################################################################

    enum Missle_Head{  //Describes how the missle flies. (which way is forward)
        Top = 0,   //Head is at top
        Bottom,
        Right,
        Left,
        Front,
        Back,
        NOT_VALID
    } missle_head;

    // Constructor(s) ################################################
    //################################################################
    SmartMissle(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
                bool active = true, Object3D* objToFollow = NULL, Object3D* deployer = NULL, 
                float speed = 5, BYTE head_direction = 0, float distanceTilSmart = 245, 
                float maxDistanceTilDead = 15000,
                glm::mat4 scaleMatrix = glm::mat4(),
                glm::mat4 translationMatrix = glm::mat4(), 
                glm::mat4 rotationalMatrix = glm::mat4(),
                glm::vec3 ambientMin = glm::vec3(.3, .3, .3),
                glm::vec3 diffuseMin = glm::vec3(.2, .2, .2),
                glm::vec3 specularMax = glm::vec3(.8, .8, .8))

               //Call the parent constructor
               : Object3D(objName, fileName, vertexCount, modelSize, active, scaleMatrix, 
               translationMatrix, rotationalMatrix, ambientMin, diffuseMin, specularMax)
    {
        setObjectToFollow(objToFollow);
        setDeployer(deployer);
        setSpeed(speed);
        setDirection(head_direction);
        setDistanceTilSmart(distanceTilSmart);
        setMaxDistTilDead(maxDistanceTilDead);

    }

    // gets and sets #################################################
    //################################################################
    inline bool setObjectToFollow(Object3D* o) { this->obj = o; return true; }
    inline Object3D* getObjectToFollow(void) { return this->obj; }

    inline bool setDeployer(Object3D* o) { this->obj_d = o; return true; }
    inline Object3D* getDeployer(void) { return this->obj_d; }

    inline bool setSpeed(float s){ this->speed = s; return true; }
    inline float getSpeed(void){ return this->speed; }

    inline bool setDirection(BYTE d){ 
        if (d >= NOT_VALID)
            missle_head = Top;
        else
            missle_head = (Missle_Head)d;
        return true; }

    inline BYTE getDirection(void){ return this->missle_head; }

    inline bool setDistanceTilSmart(float d){ this->distanceTilSmart = d; return true; }
    inline float getDistanceTilSmart(void){ return this->distanceTilSmart; }

    inline bool setSmartMissleOn(bool f){ this->smartMissleOn = f; return true; }
    inline bool getSmartMissleOn(void){ return this->smartMissleOn; }

    inline bool setCurrentDistance(float f){ this->currentDist = f; return true; }
    inline float getCurrentDistance(void){ return this->currentDist; }

    inline bool setMaxDistTilDead(float d){ this->maxDistTilDead = d; return true; }
    inline float getMaxDistTilDead(void){ return this->maxDistTilDead; }

    /*Gets the current modelMatrix of the object and at the same time updates the lastModelMatrix.*/
    virtual glm::mat4 getModelMatrix(void);

    /*Update will run the getModelMatrix in order to update the lastModelMatrix*/
    virtual void update(bool updateIfInactive);

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 SmartMissle::getModelMatrix(void){

    //Update the last model matrix and return it
    this->lastModelMatrix = this->getTranslationMatrix() * this->getRotationMatrix() * this->getScaleMatrix();
    return this->lastModelMatrix;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void SmartMissle::update(bool updateIfInactive){

    //Only update if object is active, or user wishes to override this check
    if (updateIfInactive || this->isActive()){

        /*The travel orientation of the missle will depend on which side is the "head" of the missle.  This will depend
        entirely on the model. Currently, it can be either up, right, left, front, back, or down.*/

        glm::vec3 dirVec;

        //Up or down will utilize the missles 'up' vector
        if (this->missle_head == Top)
            dirVec = glm::vec3 (MathHelperJWF::getVector(this->lastModelMatrix, 1));
        else if (this->missle_head == Bottom)
            dirVec = glm::vec3(-MathHelperJWF::getVector(this->lastModelMatrix, 1));

        //Right or left will utilize the missles 'right' vector
        else if (this->missle_head == Right)
            dirVec = glm::vec3(MathHelperJWF::getVector(this->lastModelMatrix, 0));
        else if (this->missle_head == Left)
            dirVec = glm::vec3(-MathHelperJWF::getVector(this->lastModelMatrix, 0));

        //Front and back will utilize the missles 'at' vector
        else if (this->missle_head == Front)
            dirVec = glm::vec3(MathHelperJWF::getVector(this->lastModelMatrix, 2));
        else if (this->missle_head == Back)
            dirVec = glm::vec3(-MathHelperJWF::getVector(this->lastModelMatrix, 2));



        //The missle is to travel forward until it passes its distance til start and continues as such if there is no object to follow
        if (!smartMissleOn || this->obj == NULL /*No object to follow*/){

            //Scale the dirVec vector to match the distance
            dirVec = MathHelperJWF::get3D_ScaledVector(dirVec, (float)this->speed);

            //Add dirVec vector to the missles translation to move it forward
            this->setTranslationMatrix(glm::translate(glm::mat4(), MathHelperJWF::getVector(this->lastModelMatrix, 3) + dirVec));

            //Are we ready to activate smart capabilities?
            if (currentDist >= this->distanceTilSmart)
                smartMissleOn = true;

        }
        else{  /*Else, move towards our target*/

            //Get a looking at vector from the missles current location to where the object is located (obj_location - missle_location)
            glm::vec3 at(MathHelperJWF::getVector(this->getObjectToFollow()->getLastModelMatrix(), 3) - MathHelperJWF::getVector(this->getLastModelMatrix(), 3));

            /*If the missles dirVec is equivalent to the 'at' between missle and object, simply move the missle in that dirction, else,
            we must first rotate the missle to face the obj.*/

            if (!(MathHelperJWF::areEquivalent(dirVec, at, 0.1f) || MathHelperJWF::zeroVec3(at))){  //We must first rotate the missle towards the target and then move it

                /*Helpful site:
                    http://www.mvps.org/DirectX/articles/math/dot/index.htm

                    Note:  dot(A, B) = |A| * |B| * cos(Θ)         ---->      arc-cos(dot(A,B)/(|A|*|B|)) = Θ
                */

                //We need to get the angle between the missles current 'at' vector and the look at between missle and object
                float angle = acos((glm::dot(dirVec, at) / (glm::distance(dirVec, glm::vec3(0, 0, 0)) * glm::distance(at, glm::vec3(0, 0, 0)))));

                //We also need to get the axis of rotation
                glm::vec3 axis = glm::cross(dirVec, at);

                //Apply the rotation to the missle
                this->rotationMatrix = glm::rotate(glm::mat4(), angle, axis) * this->rotationMatrix;

            }

            //Now, move the missle forward along the 'at' vector

            //Scale the 'at' vector to match the distance
            at = MathHelperJWF::get3D_ScaledVector(at, (float)this->speed);

            //Add 'at' vector to the ships translation
            this->setTranslationMatrix(glm::translate(glm::mat4(), glm::vec3(this->lastModelMatrix[3][0] + at[0], this->lastModelMatrix[3][1] + at[1], this->lastModelMatrix[3][2] + at[2])));

        }

        //Update the current distance traveled
        currentDist += speed;

        //Has the missle gone its max distance?
        if (currentDist >= maxDistTilDead){
            this->setActive(false);
            this->setObjectToFollow(NULL);
        }


        //Finally, update the last model matrix
        this->getModelMatrix();

    }

}

#endif