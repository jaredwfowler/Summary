/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
  September 15, 2014

  "TypeDefinesAndHelpers.hpp"

  A simple set of typedefs and defines.
  Also includes helper funcitons which are used by
  the various classes.
*/


# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef TYPEDEFINESANDHELPERS_H
#define TYPEDEFINESANDHELPERS_H

typedef unsigned char            BYTE;
typedef unsigned int             UINT32;
typedef signed int               INT32;
typedef unsigned long long int   UINT64;


#define TRI 3  //3 vertices in a .tri model


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
class MathHelperJWF{

    /*General Function Object that just has some useful math operations*/

public:

    static glm::vec3 get3D_ScaledVector(glm::vec3 directionalVector, float distance){

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
        float sum = (float)( (pow(allPositive[0], 2)) + (pow(allPositive[1], 2)) + (pow(allPositive[2], 2)) );

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

    static glm::vec3 getVector(glm::mat4 m, BYTE c){

        /*Returns the vector that makes up the c comumn of
        matrix m.*/

        if (c < 0) c = 0;
        else if (c > 3) c = 3;

        return glm::vec3(m[c][0], m[c][1], m[c][2]);
    }

    static bool areEquivalent(glm::vec3 a, glm::vec3 b, float errorAllowed){

        /*Determines if two vectors are equivalent*/

        //Use the scale vector equation on both and then compare
        a = get3D_ScaledVector(a, 10);
        b = get3D_ScaledVector(b, 10);

        a = a - b;

        if (abs(a[0]) <= errorAllowed && abs(a[1]) <= errorAllowed && abs(a[2]) <= errorAllowed)
            return true;
        else
            return false;
    }

    static bool zeroVec3(glm::vec3 v){

        /*Determines if all fields are zero*/
        if (v[0] == 0 && v[1] == 0 && v[2] == 0)
            return true;
        else
            return false;

    }

};

#endif