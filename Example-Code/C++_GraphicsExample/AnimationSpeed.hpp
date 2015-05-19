/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 11, 2014
   Comp 465 "Warbird Simulation"
   Professor Barnes


ABOUT:

Very basic class that holds information about animation speed and
an ID to go along with it.

*/



# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef ANIMATIONSPEED_HPP
#define ANIMATIONSPEED_HPP

#include "TypeDefinesAndHelpers.hpp"

class AnimationSpeed
{
private:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    UINT32 speed = 0;  //Animation speed
    char id[20];       //Name of this speed

public:
    // Constructor(s) ################################################
    //################################################################
    AnimationSpeed(UINT32 speed = 0, char* id = "Unknown"){
        setSpeed(speed);
        setID(id);
    }

    // gets and sets #################################################
    //################################################################
    inline UINT32 getSpeed(void){ return this->speed; }
    inline bool setSpeed(UINT32 s) { this->speed = s; return true; }

    inline char* getID(void){ return this->id; }
    inline bool setID(char* c){ strcpy(this->id, c); return true; }

};

#endif