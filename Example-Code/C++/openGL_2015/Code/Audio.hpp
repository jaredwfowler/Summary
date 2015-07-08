/*Created By: Jared Fowler
**yrolg4tseuq@sbcglobal.net
**June 2015
**
**ABOUT:
** Built to be used by the class "Simulator". Simulator is a collection of
** models, textures, cameras, etc.. Audio uses the irrKlang library. This 
** class essentially servers only as a shell interface. 
*/

#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "Core/JWFs_openGL/Simulator.hpp"
#include "Core/irrKlang/irrKlang.h"
#include "conio.h"

class Simulator;

//CLASS :: Audio ***********************************************************
//**************************************************************************
class Audio : public Element
{
public:
   // Constructor(s) / Destructor ###################################
   //################################################################
   Audio(const char* _id, Simulator* _simPtr, const char* _filePath);
   Audio(Audio& a);
   ~Audio(void);

   // gets and sets #################################################
   //################################################################
   FILE_GL getFile(void);

   // Auxiliary Functions ###########################################
   //################################################################
   void play(void);
   void stop(void);
   void setVolume(float v);

private:
   irrklang::ISoundEngine* engine;
   FILE_GL file;
};

#endif

